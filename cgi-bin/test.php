#!/usr/bin/php
<?php
// Read the raw input from stdin
$input = file_get_contents('php://stdin');

// Parse the input (e.g., name=John)
parse_str($input, $data);
$name = $data['name'] ?? 'Guest';

// Print the response
echo "Content-Type: text/html\n\n";
echo "<h1>Hello, $name!</h1>\n";
echo "<p>This is a PHP CGI script.</p>\n";
?>