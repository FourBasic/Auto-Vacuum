/*
  This is the HTML file. You can take a HTML file, paste it here in quotes,
  add \ at the end of each line, escape the internal quotes with \, and you're done
*/

#define INDEXHTML "<!DOCTYPE html> \
<html> \
<head> \
  <link rel=\"stylesheet\" href=\"styles.css\"> \
  <script src=\"script.js\"></script> \
  <title>AutoVacuum</title> \
</head> \
<body style=\"background-color:#1c87c9;\"> \
    <span class=\"name\">Watchdog: </span><span class=\"value\" id=\"watch\"></span><span class=\"units\">usec<br></span> \
    <span class=\"name\">Watchdog High: </span><span class=\"value\" id=\"watchH\"></span><span class=\"units\">usec</span><br> \
    <span class=\"name\">Watchdog Low: </span><span class=\"value\" id=\"watchL\"></span><span class=\"units\">usec</span><br> \
    <span class=\"name\">Objective: </span><span class=\"value\" id=\"objective\"></span><br> \
    <span class=\"name\">Map Action: </span><span class=\"value\" id=\"mapAction\"></span><br> \
    <span class=\"name\">US Action: </span><span class=\"value\" id=\"usAction\"></span><br> \
    <span class=\"name\">Drive Action: </span><span class=\"value\" id=\"driveAction\"></span><br> \
    <span class=\"name\">Heading: </span><span class=\"value\" id=\"heading\"></span><span class=\"units\">deg</span><br> \
</body> \
<canvas id=\'myCanvas\' width=\"1500\" height=\"1500\"></canvas> \ 
</html>"