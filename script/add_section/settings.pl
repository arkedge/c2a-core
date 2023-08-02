use strict;
use warnings;
use utf8;

# package sub;

sub GetSetting {
	print "Laod Settings...\n";

	my %SETTING;

	$SETTING{'HEADER'} = '#pragma section REPRO';
	$SETTING{'FOOTER'} = '#pragma section';

	$SETTING{'ROOT_PATH'} = '../../';
	$SETTING{'FILE_ENCODING'} = 'utf8';
	$SETTING{'LOG_FILE'}  = './log.log';
	$SETTING{'SEARCH_PATH'} = [
		'applications',
		'Drivers',
		'hal',
		'Library',
		'System',
		'TlmCmd'
	];

	# スキップファイル
	$SETTING{'IGNORE_FILE'} = [
		'print.c',
		'watchdog_timer.c'
	];

	return %SETTING;
}

1;
