
    
  FORUM PUBLICITY ADMINISTRATION
    This section has several controls which work together as follows:
    
   -AUTOMATIC FORUM PUBLICITY
      This turns on/off the automatic publicity of group forums. If a group forum has at
      least one post which is public, than entries will be made in the database to indicate
      that the forum and its container are also publicly browsable. If a group forum does
      not have any public posts, then that forum will not be publicly browsable except as
      noted below in conjunction with other settings.
      
      NOTE: The automatic setting uses two values for storing the forum and containers'
      states - PRIVATE_DEFAULT and PUBLIC_AUTO.
      
   -ALLOW PUBLIC CHOICE
      This allows group owners to decide which of their forums are public or private, but
      can also work with the auto setting above. First an explaination without the auto
      setting:
      
        In each forum, the group owner (or admins) will see a link in the context menu to
        'Administer group forums' which presents a table structure whereby they may edit
        forums' names or delete them, add forums to the container, make them public,
        make them private, or rest them. The publicity settings enter these values into the
        database - PRIVATE_SET_BY_OWNER, PUBLIC_SET_BY_OWNER, and PRIVATE_DEFAULT. The 'set
        by owner' values take precedence over the 'auto' and 'default' values.
        
      With the auto setting:
      
        Forums and containers will appear or not in any browsable listing per the standard
        automatic rules unless overridden by the group owner or an admin. As mentioned above,
        the 'set by owner' values take precedence over the 'auto' and 'default' values when
        determining publicity or browsability.
        
   -MAKE ALL FORUMS PUBLIC
      Checking this does not mean you can't check the options above. However, the effects of
      any settings made under the above settings will not be appearant unless this feature
      is later turned off. This is either an easy way to ensure that all forums are always
      browsable by the public, or may be used to temporarily open the site for something like
      an 'open house'. Turning it off would keep in-tact any settings made before its use.
      
   -MANAGE PUBLICITY OF OLD GROUPS
      Similar to the other retroactive control above, this one manages publicity settings in
      the database for the case where you are upgrading from a version of og_forum without
      these features. As noted on the admin page, it should only be used once.
      
   -SWITCH TO AUTOMATIC PUBLICITY
      Although this may sound like simply turning off the 'Make all forums public' check box
      and ensuring that the 'Automatic forum publicity' checkbox is set, that is NOT what
      this feature does.
      
      Should you use the 'Allow public choice' feature, but later decide that as site admin
      you would like total control of the forums' publicity, you may push this button and
      change all entries in the database to reflect a state as though they had always been
      managed by the 'Automatic forum publicity' feature.
      
      In other words, PUBLIC_SET_BY_OWNER will be changed to PUBLIC_AUTO and PRIVATE_SET_BY_
      OWNER will be changed to PRIVATE_DEFAULT in all database entries. Make sure that you
      uncheck the 'Allow public choice' checkbox, or else group owners will continue to be
      able to make their own settings.
    
  LIMIT NUMBER OF FORUMS PER GROUP
    Here you can set a three digit number (0 - 999) to limit the number of forums that may
    be created by group owners. This limit does not apply to user 1, nor to those with the
    'administer forums' permission.
    
 