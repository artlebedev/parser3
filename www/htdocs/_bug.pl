#!c:\perl\bin\perl

#a
print "header:value from $0\n";
print "Header1: lala1111\n";
print "\n";

print $ENV{QUERY_STRING};
open F, '>_bug.txt';
print F $ENV{QUERY_STRING};
print "done";
