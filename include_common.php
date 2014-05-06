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
//include_common.php
$defaults['dbUser'] =	'root';
$nL =	'
';

function dbOpen($user, $sql) {
	global $nL;
	
	switch(strtolower($user)) {
		case 'root':
		default:
			$db_user =	'root';
			$db_pass =	'';
			$db_name =	'test';
			break;
	}
	$mysql =	mysql_connect('localhost', $db_user, $db_pass);
	mysql_select_db($db_name, $mysql);
	
	if (is_array($sql)) {
		//insert/update
		foreach ($sql as $key => $val) {
			$res = mysql_query($val, $mysql);
			$result[$key]['res'] = $res;
			$result[$key]['num_rows'] = mysql_affected_rows();
		}

	} else {
		
		//select
		$res =	mysql_query($sql, $mysql);
		while ($row = mysql_fetch_assoc($res)) {
			$result[] = $row;
		}
	}
	mysql_close($mysql);
	return $result;
}


?>