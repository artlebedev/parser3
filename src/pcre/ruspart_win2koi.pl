sub code {
	my($c)=@_;
	$c=uc $c;
	$c=~s/^0x/0x/i;
	$c
}

open WIN2KOI, 'win-koi.tab';
while(<WIN2KOI>) {
	chomp;
	@from_to=split /\s/;
	$win2koi{code($from_to[0])}=code($from_to[1]);
}
close WIN2KOI;

#char	white-space	digit	hex-digit	letter	word	lowercase	unicode1	unicode2	
$header=<>;
print $header;

while(<>) {
	chomp;
	@record=split "\t";
	next unless $record[0]=$win2koi{code($record[0])};
	if($record[6]) {
		next unless $record[6]=$win2koi{code($record[6])};
	}
    print join("\t", @record), "\n";
}