#!/usr/bin/perl

use strict;
use warnings;

# Read input from stdin
my $input_data = <STDIN>;

# Parse the input (e.g., name=John)
my %data;
foreach my $pair (split('&', $input_data)) {
    my ($key, $value) = split('=', $pair);
    $data{$key} = $value;
}
my $name = $data{name} || 'Guest';

# Print the response
print "Content-Type: text/html\n\n";
print "<h1>Hello, $name!</h1>\n";
print "<p>This is a Perl CGI script.</p>\n";