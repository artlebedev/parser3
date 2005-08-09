#!c:/perl/bin/perl

#a
print "header:value from $ARGV[0]\n";
print "Header1: lala1111\n";
print "\n";

print $ENV{QUERY_STRING};
open BUG, '>_bug.txt';
print BUG $ENV{QUERY_STRING};
print BUG $ARGV[0];
print "done";
