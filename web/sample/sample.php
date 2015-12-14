<?php
header ('Content-Type: application/xml');
echo '
<daisyduck version="1.0">
  <audiobook>
   <number>1</number>
   <title>Fire Safety</title>
   <creator>Wendy Blaxland - Macmillan Education Aust Pty. Ltd.</creator>
   <category>Sample</category>
   <summary>
     This book contains full text, images and recorded audio in human voice. It has page numbers and two levels of headings.
   </summary>
   <uri>http://daisy.schroetersa.ch/fire-safety/ncc.html</uri>
  </audiobook>
  <uriparam>
    <param name="User" value="foobar"/>
  </uriparam>
</daisyduck>
';
