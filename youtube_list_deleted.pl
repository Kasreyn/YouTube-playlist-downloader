#!/usr/bin/perl
# Written by Jonas Jensen, 120726

use strict;
use Data::Dumper;
use Archive::Extract;

my @lines;
my $line;
my %filehash = ();
my %playlisthash = ();

sub readfiles {
	opendir(DIR, ".") or die "ERROR: can not open current directory.";
	my @files = readdir(DIR);
	closedir(DIR);
	foreach my $file (@files) {
		if($file =~ m/-(\w{11,11}).flv|webm|3gpp/) {
			$filehash{ $1 } = $file;
		}
	}
#	foreach my $key (keys %filehash) {
#		print $key . "\n";
#	}
#	print Dumper(%filehash) . "\n";

}

sub read_playlist {
	open(FIL,"<$_[0]");
	@lines = <FIL>;
	close(FIL);
	foreach $line (@lines) {
		if($line =~ m/\d+\ (\w{11,11})\ :\ \[DELETED\]/) {
			chomp($line);
			$playlisthash{ $1 } = 1;
#			print $line . "\n";
		}
	}
}

sub print_deleted {
	foreach my $key (keys %playlisthash) {
		print $key . " " . $filehash{ $key } . "\n";
	}
}

if ( @ARGV < 1 ) { print "Usage: youtube_list_deleted.pl [FILE]\n\n.. where [FILE] contains lines on format [INDEX] [YouTube ID] : [TITLE]\n\nExample:    \"3 nbrTOcUnjNY : This is Einstein!\"\n          \"250 3MVQ4uGldTM : [DELETED]\"\n\n"; }

readfiles();
read_playlist($ARGV[0]);
print_deleted();


