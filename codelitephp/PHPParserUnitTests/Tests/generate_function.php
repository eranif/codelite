<?php

$phpdocDir = "";

function foo()
{
	global $phpdocDir;
	for ($i = 0; $i < count($argv); ++$i) {
		switch ($argv[$i]) {
		case "-nosplit":
			$splitFiles = false;
			break;

		case "-help":
			show_help();
			break;

		default:
			$phpdocDir = $argv[$i];
		}
	}
}
