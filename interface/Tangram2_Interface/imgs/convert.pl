# Script to solve the problem of "Premature end of JPEG file"

$start = 1;

foreach $_ (`ls *.jpg`)
{

	s/\n//g;
	
	$orig =  $_;
	

	$cmd_line = "convert ".$orig." ".$orig."\n";
    system($cmd_line);	
	
}


system($cmd_line);
