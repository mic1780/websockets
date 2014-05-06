<?php
/*
 *
 *   Copyright (C) 2014  Michael Cummins
 *   License: GNUv2
 *   
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 */
require_once('include_common.php');
//$nL =	PHP_EOL;
//$nL =	"";
$defaults['dbUser'] =	'root';

if (isset($argc) && $argc > 1) {
	switch(strtolower($argv[1])) {
		case	'test':
			$sql =	array();
			$sql[] =	"" .
						" INSERT INTO tbl_users " . $nL .
						" ( " . $nL .
							" UserID " . $nL .
							" ,UserLevel " . $nL .
							" ,Name " . $nL .
						" ) " . $nL .
						" VALUES " . $nL .
						" ( " . $nL .
							" 0 " . $nL .
							" ,1 " . $nL .
							" ,'cInsert' " . $nL .
						" ) " . $nL .
						"";
			$sql[] =	"" .
						" UPDATE tbl_users " . $nL .
						" SET UserID = ID " . $nL .
						" WHERE UserID = 0 " . $nL .
						"";
			$res =	dbOpen($defaults['dbUser'], $sql);
			echo	'successful test.';
			exit(0);
			break;
		case	'sql':
			$sql =	'';
			if (substr($argv[2], 0, 4) === 'tbl_') { 
				$sql =	"" .
							" SELECT * FROM " . $argv[2] . " " .
							"";
			} else {
				$sql =	$argv[2];
			}//END IF
			
			$data =	dbOpen($defaults['dbUser'], $sql);
			$output =	tableOutput($data);
			writeToOut($output);
			return strlen($output);
			break;
		default:
			echo 'no db call made.';
			break;
	}//END SWITCH
}//END IF

if (count($sql) > 0) {
	$res =	dbOpen($defaults['dbUser'], $sql);
}//END IF

/**/
$data =	array();
$data[] =	array(
						'ID' => 1,
						'UserID' => 1,
						'Name' => 'User 1'
						);
$data[] =	array(
						'ID' => 2,
						'UserID' => 2,
						'Name' => 'User 2'
						);
$data =	dbOpen($defaults['dbUser'], "SELECT * FROM tbl_users");
$tableArray =	array();
if (count($data) > 0) {
	$output =	'';
	//tableOutput code moved from here to function
	//$output =	tableOutput($data);
	file_put_contents('output.txt', $output);
	//echo $output;
	return 0;
}//END IF
/**/

function formatValue($col, $val) {
	$leftSum =	round(($col - strlen($val)) / 2, 0, PHP_ROUND_HALF_DOWN);
	$rightSum =	round(($col - strlen($val)) / 2, 0, PHP_ROUND_HALF_UP);
	
	$leftSum =	($leftSum < 0 ? 0 : $leftSum);
	$rightSum =	($rightSum < 0 ? 0 : $rightSum);
	//$i =	($sum < 0 ? 0 : $sum);
	return str_repeat(" ", $leftSum) . $val . str_repeat(' ', $rightSum) . "|";
}//END FUNCTION

function formatColumn($str, $width) {
	$leftWidth =	round(($width - strlen($str)) / 2, 0, PHP_ROUND_HALF_DOWN);
	$rightWidth =	round(($width - strlen($str)) / 2, 0, PHP_ROUND_HALF_UP);
	
	$leftWidth =	($leftWidth < 0 ? 0 : $leftWidth);
	$rightWidth =	($rightWidth < 0 ? 0 : $rightWidth);
	return str_repeat("-", $leftWidth) . str_repeat("-", strlen($str)) . str_repeat("-", $rightWidth) . "+";
}//END FUNCTION

function writeToOut($str) {
	fwrite(STDOUT, $str, strlen($str));
	return;
}//END FUNCTION

function tableOutput($data) {
	global $nL;
	$tableArray =	array('width' => array(), 'top' => array(), 'heading' => array());
	$output =	'';
	foreach ($data as $key => $row) {
		if ($key === 0) {
			foreach ($row as $col => $unused) {
				$tableArray['width'][$col] =	strlen($col);
				$tableArray['top'][$col] =	formatColumn($col, 0);
			}//END FOREACH LOOP
			
			foreach ($row as $col => $unused) {
				$tableArray['heading'][$col] =	formatValue(0, $col);
			}//END FOREACH LOOP
			
		}//END IF
		
		foreach ($row as $col => $val) {
			if ($tableArray['width'][$col] < strlen($val)) {
				$tableArray['width'][$col] =	strlen($val);
				
				foreach ($tableArray as $rowKey => $unused) {
					if ($rowKey === 'top' || $rowKey === 'width') {
						continue;
					} else if ($rowKey === 'heading') {
						$tableArray[$rowKey][$col] =	formatValue($tableArray['width'][$col], $col);
						continue;
					} else if ($rowKey == $key) {
						break;
					}//END IF
					
					$tableArray[$rowKey][$col] =	formatValue($tableArray['width'][$col], $data[$rowKey][$col]);
				}//END FOREACH LOOP
				
				$tableArray['top'][$col] =	formatColumn($col, $tableArray['width'][$col]);
			}//END IF
			$tableArray[$key][$col] =	formatValue($tableArray['width'][$col], $val);
		}//END FOREACH LOOP
		
	}//END FOREACH LOOP
	
	foreach ($tableArray as $key => $row) {
		if ($key === 'top' || $key === 'width')
			continue;
		$output .=	'+';
		foreach ($tableArray['top'] as $col => $text) {
			$output .=	$text;
		}//END FOREACH LOOP
		$output .=	$nL . "|";
		foreach ($row as $col => $text) {
			$output .=	$text;
		}//END FOREACH LOOP
		$output .=	$nL;
	}//END FOREACH LOOP
	
	$output .=	'+';
	foreach ($tableArray['top'] as $col => $text) {
		$output .=	$text;
	}//END FOREACH LOOP
	//$output .=	"\n";
	//$output =	rtrim($output, '-');
	
	
	return $output;
}//END FUNCTION
?>