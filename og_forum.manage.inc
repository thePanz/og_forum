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
 
