// $Id$

Drupal.ogAttach = function() {
/*  Disable the public checkbox if no groups are selected in in Audience, unless audience is required and set in hidden field*/
  
  var hidden_set;
  if ( $('.og-audience-forum').size() > 0) {
    hidden_set = $('input.og-audience-forum').val();
  }
  if (hidden_set) {
    $('#edit-og-public').removeAttr("disabled");
  }
  else {
    $('#edit-og-public').attr("disabled", "disabled");
  }
}

if (Drupal.jsEnabled) {
  $(document).ready(Drupal.ogAttach);
}