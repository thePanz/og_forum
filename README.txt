$Id$

This module does the following:

A) Upon creating an og group, it creates a forum container for that
   group, with a single sub-forum.
B) A record is also placed in og_term to tie the organic group with
   the forum container's term ID.
C) When viewing groups, only forum discussions for that group are
   displayed.

The og_term table could be useful for other applications of tying
groups to taxonomy as well.

Original author: Károly Négyesi

Special thanks to:
Evan Leeson of Catalyst Creative for sponsorship of 4.7 port and
improvements.
Gavin Mogan for http://drupal.org/node/63379 which was a huge help in
porting the module.
