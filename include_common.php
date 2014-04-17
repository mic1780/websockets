<?php
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