#!c:\progra~1\perl\bin\perl

print "content-type: text/html\n";
print "header: env-header-value\n";
print "\n";

while (($Key,$Value)=each %ENV) {
	print "$Key=$Value<BR>\n";
}
print "EOE";

warn "test";
