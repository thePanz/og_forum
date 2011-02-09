<?php
// $Id$;

/**
 * Set a forum as public, and its container as well
 *
 * @param array The form state
 * @param int $gid The group id of the forum.
 * @param int $tid The term id of the forum.
 */

function og_forum_set_public_form($form_state, $gid, $tid) {
  $term = taxonomy_get_term($tid);
  $group = node_load(array('nid' => $gid));
  $form['gid'] = array('#type' => 'value', '#value' => $gid);
  $form['tid'] = array('#type' => 'value', '#value' => $tid);
  return confirm_form(
    $form,
    t('Are you sure you want to make the forum %name of group %group public?',
      array('%name' => $term->name, '%group' => $group->title)),
    'og_forum/manage/'. $gid,
    t('Making a forum public will allow all users to use the forum.'),
    t('Make public'),
    t('Cancel')
  );
}

function og_forum_set_public_form_submit($form, &$form_state) {
  $container = og_forum_get_forum_container($form_state['values']['gid']);
   $sql = "UPDATE {og_term} SET public = %d WHERE tid = %d and nid = %d";
  db_query($sql, PUBLIC_BY_GROUP_OWNER, $form_state['values']['tid'], $form_state['values']['gid']);
  db_query($sql, PUBLIC_BY_GROUP_OWNER, $container, $form_state['values']['gid']);
  $form_state['redirect'] = 'og_forum/manage/'. $form_state['values']['gid'];
}
 
/**
 * Set forum as private, and its container if no other forums in it are public
 *
 * @param array The form state
 * @param int $gid
 * @param int $tid
 */
function og_forum_set_private_form($form_state, $gid, $tid) {
  $term = taxonomy_get_term(tid);
  $group = node_load(array('nid' => $gid));
  $form['gid'] = array('#type' => 'value', '#value' => $gid);
  $form['tid'] = array('#type' => 'value', '#value' => $tid);
  return confirm_form(
    $form,
    t('Are you sure you want to make the forum %name of group %group private?',
       array('%name' => $term->name, '%group' => $group->title)),
    'og_forum/manage/'. $gid,
    t('Making a forum private will only group members allow to use the forum.'),
    t('Make private'),
    t('Cancel')
  );
}

function og_forum_set_private_form_submit($form, &$form_state) {
  $container = og_forum_get_forum_container($form_state['values']['gid']);
  $sql = "UPDATE {og_term} SET public = %d WHERE tid = %d and nid = %d";
  db_query($sql, PRIVATE_BY_GROUP_OWNER, $form_state['values']['tid'], $form_state['values']['gid']);
  $sql2 = "SELECT COUNT(tid) FROM {og_term} WHERE nid = %d AND tid <> %d AND public > 1";
  $results = db_query($sql2, $form_state['values']['gid'], $container);//find out if any other forums are public
  $result = db_result($results);
  if (!$result) {
    //if no other forums in the group are public, go ahead and make the container private
    db_query($sql, PRIVATE_DEFAULT, $container, $form_state['values']['gid']);
  }
  $form_state['redirect'] = 'og_forum/manage/'. $form_state['values']['gid'];
}
 
/**
 * Reset forum to default of PRIVATE_DEFAULT, and its container if no other forums in it are public
 *
 * @param array The form state
 * @param int $gid
 * @param int $tid
 */
function og_forum_reset_form($form_state, $gid, $tid) {
  $term = taxonomy_get_term(tid);
  $group = node_load(array('nid' => $gid));
  $form['gid'] = array('#type' => 'value', '#value' => $gid);
  $form['tid'] = array('#type' => 'value', '#value' => $tid);
  return confirm_form(
    $form,
    t('Are you sure you want to reset the public status of forum %name in group %group?',
       array('%name' => $term->name, '%group' => $group->title)),
    'og_forum/manage/'. $gid,
    t('This will reset the public status of your forum to the default value.'),
    t('Reset publicity'),
    t('Cancel')
  );
}

function og_forum_reset_form_submit($form, &$form_state) {
  $container = og_forum_get_forum_container($form_state['values']['gid']);
  $sql = "UPDATE {og_term} SET public = %d WHERE tid = %d and nid = %d";
  db_query($sql, PRIVATE_DEFAULT, $form_state['values']['tid'], $form_state['values']['gid']);
  $sql2 = "SELECT COUNT(tid) FROM {og_term} WHERE nid = %d AND tid <> %d AND public > 1";
  $results = db_query($sql2, $form_state['values']['gid'], $container);//find out if any other forums are public
  $result = db_result($results);
  if (!$result) {
    //if no other forums in the group are public, go ahead and make the container private
    db_query($sql, PRIVATE_DEFAULT, $container, $form_state['values']['gid']);
  }
  $form_state['redirect'] = 'og_forum/manage/'. $form_state['values']['gid'];
}
 
/**
 * Let group managers manage their forums
 */
function og_forum_manage($gid) { 
  global $user;
  $group = node_load($gid);
  $allow_public = variable_get('forum_allow_public', 0);
  og_set_theme($group->nid);
  og_set_language($group);
  og_set_group_context($group);
  // set a nice breadcrumb
  _og_forum_manage_breadcrumb($group);
  $content = '<p>' .t('This page shows the forums associated with the %group group.', array('%group' => $group->title)). '</p>';
  $og_terms = _og_forum_ogterms($group);
  $forum_limit_reached = og_forum_forum_limit_reached($group->nid);
  $header = array(t('Name'), t('Operations'));
  $sql = "SELECT public, tid FROM {og_term} WHERE nid = %d";
  $results = db_query($sql, $group->nid);
  $all_tids = array();
  while ($result = db_fetch_object($results)) {
    $all_tids[$result->tid] = $result->public;
  }
  $tree = taxonomy_get_tree(og_forum_get_vid());
  if ($tree) {
    foreach ($tree as $term) {
      if (in_array($term->tid, $og_terms)) {
        if (in_array($term->tid, variable_get('forum_containers', array()))) {//we're looking at a container
          if ($forum_limit_reached && !(user_access('administer forums'))) {//the limit for the number of forums allowed was reached
            $rows[] = array(array('data' => t('You have reached the maximum allowed number of forums for groups.'), 'colspan' => '3'));
            $rows[] = array(str_repeat('-', $term->depth). ' ' .check_plain($term->name), array('data' => '', 'colspan' => '3'));
          }
          else {//the limit on the number of forums has not been reached, or there is no limit
            $rows[] = array(str_repeat('-', $term->depth). ' ' .check_plain($term->name), array('data' => l(t('add forum'), "og_forum/manage/add/$group->nid/$term->tid"), 'colspan' => '3'));
          }
        }
        else {//we're looking at a forum, not a container
          if ((user_access('make forums public') || $allow_public) && $all_tids[$term->tid] < PUBLIC_AUTO) {//it's presently private
            $rows[] = array(str_repeat('-', $term->depth). ' ' .check_plain($term->name), l(t('edit forum'), "og_forum/manage/edit/$group->nid/$term->tid"), l(t('make public'), "og_forum/manage/public/$group->nid/$term->tid"), l(t('reset publicity'), "og_forum/manage/reset/$group->nid/$term->tid"));
          }
          elseif ((user_access('make forums public') || $allow_public) && $all_tids[$term->tid] > PRIVATE_BY_GROUP_OWNER) {//it's presently public
            $rows[] = array(str_repeat('-', $term->depth). ' ' .check_plain($term->name), l(t('edit forum'), "og_forum/manage/edit/$group->nid/$term->tid"), l(t('make private'), "og_forum/manage/private/$group->nid/$term->tid"), l(t('reset publicity'), "og_forum/manage/reset/$group->nid/$term->tid"));
          }
          else {
            $rows[] = array(str_repeat('-', $term->depth). ' ' .check_plain($term->name), l(t('edit forum'), "og_forum/manage/edit/$group->nid/$term->tid"));
          }
        }
      }
    }
  }
  return $content . theme('table', $header, $rows);
}

/**
 * Select the right form to display
 */
function og_forum_form_main($type, $group_id, $tid) {
  $edit = array('group_id' => $group_id, 'parent' => $tid);
  if ($_POST['op'] == t('Delete') || $_POST['confirm']) {
    return drupal_get_form('og_forum_confirm_delete', $tid, $group_id);
  } 
  switch ($type) {
    case 'add':
      $group_id = $edit['group_id']; 
      return drupal_get_form('og_forum_form_forum', $edit, $group_id, $edit['tid']);
    case 'edit':
      $group_id = $edit['group_id'];
      $group = node_load($group_id);
      $term = taxonomy_get_term($tid);
      $parents = taxonomy_get_parents($term->tid);
      $parent = array_pop($parents);
      $term->parent = $parent->tid;
      $term->group_id = $group_id;
      $edit = (array)$term;
      return drupal_get_form('og_forum_form_forum', $edit, $group_id);
  }
}
