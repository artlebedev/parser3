#!c:\progra~1\perl\bin\perl

print "content-type: text/html\n\n";

print '(<pre>', `dir`, '</pre>)<BR>';
print '(<pre>', `set`, '</pre>)<BR>';
while (($Key,$Value)=each %ENV) {
	print "$Key=$Value<BR>";
}
print "EOE";

warn "test";