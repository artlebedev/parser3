#	Parser: covertor of UNICODE casing data
#		http://www.unicode.org/Public/UNIDATA/CaseFolding.txt
#		to parser source code
#
#	Copyright (c) 2003 ArtLebedev Group (http://www.artlebedev.com)
#	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

# $Id: simple_folding.pl,v 1.1 2003/09/29 09:42:12 paf Exp $

#  usage:
#    download CaseFolding.txt
#    run simple_folding.pl < CaseFolding.txt
#

while(<>){
	chomp;
	# <code>; <status>; <mapping>; # <name>
	#1FAF; F; 1F67 03B9; # GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
	my($code, $status, $mapping)=split /;\s*/;

	#we need only
	next if 
		$code eq ""
		|| $code=~/^#/
		|| $status eq 'F'
		|| length($code)>4
		|| length($mapping)>4;
#	print "$_\n";
	$toLower{$code}=$mapping unless defined $toLower{$code};
	$toUpper{$mapping}=$code unless defined $toUpper{$mapping};;
}

output(\%toLower, 'utf8-to-lower.inc');
output(\%toUpper, 'utf8-to-upper.inc');

sub output{
	my($tableRef, $name)=@_;
	open F, ">../$name";
	my($first)=1;
	my($counter)=0;
	foreach $key(sort {$a cmp $b} keys %$tableRef) {
		if(!$first){
			print F ', ';
		}
		undef $first;
		if(++$counter>4) {
			print F "\n";
			$counter=1;
		}
		print F "{0x", $key, ", 0x", $tableRef->{$key}, "}";
	}
	print F "\n";
	close F;	
}
