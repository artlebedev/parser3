#!/usr/bin/perl
undef $/;

scandir("./",*action);

sub action {
	$file=@_[0];
	print "$file\n";
	
	open(FILE, $file);
	$_=<FILE>;
	close($file);

	s/\.html/.htm/sg;
#    s/(\<head\>)/$1<meta http-equiv="Content-Type" content="text\/html;charset=Windows-1251">/;
    s/(Language=)/;$1/;

	open(FILE, ">$file");
	print FILE $_;
	close($file);
}

sub scandir {
	local($Dir, *Sub) = @_;

	opendir Dir, $Dir;
	local(@Dir) = readdir Dir;
	closedir Dir;

	foreach $ProcessFiles (1, 0) {
		foreach $FileName (@Dir) {
			next if $FileName =~ /^\./;

			$FullName = $Dir.$FileName;
			
			if ($ProcessFiles) {
				$TotalCount++;
				Sub($FullName) if (!-d $FullName && $FileName =~ /(^tree\.htm|\.hh.)$/);
			} else {
				scandir("$FullName/") if -d $FullName;
			}
		}
	}
}
