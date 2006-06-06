// $Id$

This module does the following:

A) Upon creating an og group, it creates a forum container for that 
   group, with a single sub-forum.
B) A record is also placed in og_term to tie the organic group with 
   the forum container's term ID.
C) When viewing groups, only forum discussions for that group are 
   displayed.

The og_term table could be useful for other applications of tying 
groups to taxonomy as well.

Note: Requires Taxonomy rewrite patch (see http://drupal.org/node/16452 for 
background info) on 4.6 (this patch already exists in 4.7).
Apply taxonomy.patch to the taxonomy module.

KNOWN ISSUES:
- when viewing the forum page, the create forum link can break you out of the forum theme since it does not have og info on querystring
- the next/previous topic links can take you outside the current forum?
- NOT COMPATIBLE with regular 'everyone can see' style forums. choose one or the other for your site.

Original author: Károly Négyesi

Special thanks to:
Evan Leeson of Catalyst Creative for sponsorship of 4.7 port and 
improvements.
Gavin Mogan for http://drupal.org/node/63379 which was a huge help in 
porting the module.