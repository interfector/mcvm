#!/usr/bin/perl

$state = 0;
while ($l = <STDIN>) {
	chomp($l);
	if (!$state && $l =~ /^[0-9a-f]* <.*>:/) {
		$state++;
	} elsif ($state) {
		if ($l =~ /^\s*([0-9a-f]*):\t(.*)\t(.*)$/) {
			$a = $2;
			$a =~ s/[^0-9a-f]//g;
			$a =~ s/([0-9a-f]{2})/\\x\1/g;
			printf("%s", $a);
		} elsif ($l =~ /^([0-9a-f]+) <(\S+)>:\s*$/) {
			push (@id, [$2, $1]);
		}
	}
}
