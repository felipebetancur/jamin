<?php
 include('lib/template.php'); 
 echo get_header('jack audio mastering', '.');
?>

<!-- <center><img src="images/jack-small.png" alt="jack audio connection kit"></center> -->

<h1>what is jam?</h1>

<p>
JAM is a tool for prodcuing audio masters from the mixed down multitrack source. It runs in the <a href="http://jackit.sf.net">JACK Audio Connection Kit</a>, and uses <a href="http://www.ladspa.org/">LADSPA</a> for its backend DSP work, specifically the <a href="http://plugin.org.uk/">swh plugins</a> created by Steve Harris, JAM's main author. 
</p>

Get access to the cvs <a href="http://sf.net/projects/jamin">here</a>.<p>

<h1>news</h1>

<p>17 Apr 2003 - The JAM web site is now up.</p>

<?php
 echo get_footer();
?>
