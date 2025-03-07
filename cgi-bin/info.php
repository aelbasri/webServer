#!/usr/bin/php
<?php
echo "Content-Type: text/html\n\n";
echo "<h1>Server Information</h1>";
echo "<p>PHP Version: " . phpversion() . "</p>";
echo "<p>Request Method: " . $_SERVER['REQUEST_METHOD'] . "</p>";
echo "<p>Query String: " . $_SERVER['QUERY_STRING'] . "</p>";
echo "<p>Query String: " . $_SERVER['PATH_INFO'] . "</p>";

?>
