#!c:\progra~1\perl\bin\perl

print "content-type: text/html\n\n";

while (($Key,$Value)=each %ENV) {
	print "$Key=$Value<BR>";
}
print "EOE";

warn "test";