<?php
// $Id$;

/**
 * Implementation of hook_settings().
 */
function og_forum_admin_form() {
  $form = array();
  
  $form['og_forum_display_group_forum_link'] = array (
    '#type' => 'checkbox',
    '#title' => t('Display "Group Forum" link in the group block'),
    '#default_value' => variable_get('og_forum_display_group_forum_link', 1),
    '#description' => t('Display "Group Forum" link in the group block? Disable to avoid duplication in tabs and block links.'),
  );

  $form['update'] = array(
    '#type' => 'fieldset',
    '#title' => t('Retroactively update old groups'),
    '#description' => t('Pressing this button will create forums for any groups that do not already have them.'),
    '#collapsible' => TRUE,
  );
  $form['update']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Update old groups'),
  );

  $form['forum_default_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Default forum name'),
    '#description' => t('When this module creates a container and default forum for a new group, this will be the name given to that group.'),
    '#default_value' => variable_get('forum_default_name', 'General discussion'),
  );
  $form['container'] = array(
    '#type' => 'fieldset',
    '#title' => t('Group forum container'),
    '#collapsible' => TRUE,
  );
  $form['container']['forum_default_container_yn'] = array(
    '#type' => 'checkbox',
    '#title' => t('Would you like all group forums to be placed in the same container?'),
    '#description' => t('If you would like your forums to all be under one container, check this box and select a container in the drop down box.'),
    '#default_value' => variable_get('forum_default_container_yn', 0),
  );

  $all_container_tids = variable_get('forum_containers', array());
  $sql = "SELECT name FROM {term_data} WHERE tid = %d";
  $containers = array();
  $containers[-1] = t('none');
  foreach ($all_container_tids as $key => $tid) {
    $result = db_fetch_object(db_query($sql, $tid));
    $containers[$tid] = $result->name;
  }
  $form['container']['forum_default_container'] = array(
    '#type' => 'select',
    '#title' => t('Select container'),
    '#description' => t('You need to have setup the container before hand.'),
    '#default_value' => variable_get('forum_default_container', 0),
    '#options' => $containers,
  );
  $form['public_forums'] = array(
    '#type' => 'fieldset',
    '#title' => t('Forum publicity administration'),
    '#description' => t('Using these features is probably expensive in terms of SQL queries.'),
    '#collapsible' => TRUE,
  );

  $form['public_forums']['forum_auto_public'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatic forum publicity'),
    '#description' => t('If you would like to make the publicity, or public-ness, of forums automatic based on whether there are any public posts inside
    of them, then check this box.'),
    '#default_value' => variable_get('forum_auto_public', 0),
  );

  $form['public_forums']['forum_allow_public'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow public choice'),
    '#description' => t('If you would like to allow group owners the ability to decide which, if any, of their forums are publicly viewable, check the box.
    Those with the <em>make forums public</em> permission can always make forums and containers public. If auto mode is checked, above, then this will
    allow group owners to override auto settings on a case by case basis. The auto mode respects group owner-set values.'),
    '#default_value' => variable_get('forum_allow_public', 0),
  );
  
  $form['public_forums']['forum_all_public'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make all forums public'),
    '#description' => t('This is only a display option and will not affect database entries. If you have both boxes unchecked above and no users assigned
    to the <em>make forums public</em> permission, no database entries will be made as to the publicity of forums, but all will be visible and traversable.
    User 1 can still affect the database through the normal "make public" and "make private" functionality, but such changes won\'t affect the forums\'
    display until such time as this feature is disabled. Individual posts will be public or private based on their own settings, just like if the above
    features are used. This is an easy way to get all forums visible to all users so they can see what posts they are missing out on (in the case where posts
    are private themselves). With this setting, no forums can be private. You could use this feature during a promotion period to temporarily open the whole
    site to guests.'),
    '#default_value' => variable_get('forum_all_public', 0),
  );
  $form['public_forums']['pb'] = array(
    '#type' => 'fieldset',
    '#title' => t('Manage publicity of old groups'),
    '#description' => t('If you check the "Automatic forum publicity" box, this will go through all existing forums and set them accordingly.
    Otherwise, a public forum topic will need to be created or resaved in each forum. You don\'t need to click this if you haven\'t checked that box.
    <strong>You should use this only once - usually the first time you upgrade to a version with this feature as it will not respect settings made by
    group owners.</strong>'),
  );
  $form['public_forums']['pb']['public_button'] = array(
    '#type' => 'submit',
    '#value' => t('Publicize old groups'),
  );
  $form['public_forums']['rp'] = array(
    '#type' => 'fieldset',
    '#title' => t('Switch to automatic publicity'),
    '#description' => t('This will switch all group admin-set publicity values to automatic values. PUBLIC_BY_GROUP_OWNER -> PUBLIC_AUTO, PRIVATE_BY_GROUP_OWNER -> PRIVATE_DEFAULT. This will allow you to switch a site from
    group admin-controlled publicity to automatic publicity of forums.'),
  );
  $form['public_forums']['rp']['switch_auto'] = array(
    '#type' => 'submit',
    '#value' => t('Switch to auto mode'),
  );
  $form['forum_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Limit number of forums per group'),
    '#size' => 3,
    '#maxlength' => 3,
    '#description' => t('Set the number of forums a container can have. 0 means no limit. 1 means groups can only have the default "General Discussion" forum.'),
    '#default_value' => variable_get('forum_limit', 0),
  );
  
  // Adding custom submit handler
  $form['#submit'][] = 'og_forum_admin_form_submit';
  return system_settings_form($form);
}


/**
 * Custom submit handler for group update
 */
function og_forum_admin_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  if ($form_values['op'] == t('Update old groups')) {
    og_forum_retroactively_apply();
  }
  elseif ($form_values['op'] == t('Publicize old groups')) {
    $auto_public = variable_get('forum_auto_public', 0);
    if ($auto_public) {
      //go through all existing forums and where a node has been made public, set the forum's publicity to PUBLIC_AUTO
      $sql = "SELECT DISTINCT tn.tid, oa.group_nid FROM {node} n INNER JOIN {og_ancestry} oa ON oa.nid = n.nid INNER JOIN {og_access_post} oap ON oap.nid = oa.nid INNER JOIN {term_node} tn ON tn.nid = oa.nid  WHERE n.type = 'forum' AND oap.og_public = 1";
      $results = db_query($sql);
      while ($result = db_fetch_object($results)) {
        $gid = $result->group_nid;
        $container = og_forum_get_forum_container($gid);
        $sql2 = "UPDATE {og_term} SET public = %d WHERE tid = %d and nid = %d";
        db_query($sql2, PUBLIC_AUTO, $result->tid, $gid);
        db_query($sql2, PUBLIC_AUTO, $container, $gid);
      }
    }
    else {
      drupal_set_message(t('You need to select "Automatic forum publicity" for "Publicize old groups" to work.'), 'error');
    }
  }
  elseif ($form_values['op'] == t('Switch to auto mode')) {
    $sql = "UPDATE {og_term} SET public = %d WHERE public = %d";
    db_query($sql, PUBLIC_AUTO, PUBLIC_BY_GROUP_OWNER);
    db_query($sql, PRIVATE_DEFAULT, PRIVATE_BY_GROUP_OWNER);
  }
}