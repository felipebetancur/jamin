<?php
 
 $local_prefix = '';

 function get_header($title, $prefix) {
  global $local_prefix, $clean;
  $local_prefix = $prefix; // this is a hack so i don't have to pass prefix to get_footer
  
  $doctype = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">';

  if ($clean==1)
    return "$doctype\n<html><head><title>$title</title></head><body>\n";
  
  $menu = get_menu($prefix);
  $ret = <<<EOF
$doctype
<html>
 <head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  <title>$title</title>
  <style type="text/css"><!--
  /* bad netscape four, no stylesheet for you */
  @import url($prefix/base.css);
  --></style>
 </head>
 <body>
  <div class="body">
   <table class="body" cellspacing="0" cellpadding="0" width="100%">
    <tr>
     <td class="top left">&nbsp;<a href="$prefix/">jam</a>&nbsp;</td>
     <td width="100%">
$menu
     </td>
     <td class="top right">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
    </tr>
    <tr>     
     <td class="margin left" id="margin-td">
      <div id="margin">
       <br><br><br><br><br><br><br>
       <br><br><br><br><br><br><br>
       <br><br><br><br><br><br><br>
       <br><br><br><br><br><br><br>
      </div>
     </td>
     <td class="center text" valign="top">
      <div id="text">
       <br>
EOF;
  return $ret;
 }
 
 function get_menu ($prefix) {
  $menu_items = array(
   faq => "$prefix/docs/faq.php",
   download => "$prefix/download/",
   documentation => "$prefix/docs/",
//   news => "$prefix/news/", no news page while we don't have old news
   developers => "$prefix/dev/",
//   applications => "$prefix/apps/",
   lists => "$prefix/lists/"
  );
  
  $ret = <<<EOF
  <table cellspacing="0" class="menu" width="100%">
   <tr>
EOF;
  foreach($menu_items as $name => $path) {
      $ret .= "    <td class=\"top\" align=\"center\">&nbsp;<a href=\"$path\">$name</a>&nbsp;</td>\n";
  }
  $ret .= <<<EOF
   </tr>
  </table>
EOF;
  return $ret;
 }

 function get_footer() {
  global $local_prefix, $clean;
  $prefix = $local_prefix;
  
  if ($clean==1)
    return "</body></html>\n";
  
  $ret = <<<EOF
      </div>
     </td>
     <td class="margin right">&nbsp;</td>
    </tr>
   </table>
   <div style="text-align:right">
    <a href="http://sourceforge.net/projects/jamin/"><img src="http://sourceforge.net/sflogo.php?group_id=39687&amp;type=1" width="88" height="31" border="0" alt="SourceForge.net Project Page"></a><br>
EOF;
  $ret .= "    <a href=\"http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/jamin/www/";
  $file_dir = dirname($GLOBALS['SCRIPT_NAME']);
  $file_name = basename($GLOBALS['SCRIPT_NAME']);
  $dirs = explode ("/", $file_dir);
  array_shift ($dirs); /* the leading slash */
  for ($i=0; $i<substr_count($file_dir, "/") - substr_count($prefix, ".."); $i++) {
      array_shift ($dirs);
  }
  while ($dirs) {
      $ret .= array_shift($dirs);
      $ret .= "/";
  }
  $ret .= "$file_name\">Document History</a>\n";
  $ret .= <<<EOF
   </div
  </div>
 </body>
</html>
EOF;
  return $ret;
 }

?>
