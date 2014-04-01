<?php 
	$keyword = $_POST["keyword"];
	//$keyword = 'a';

	// Create the socket and connect 
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP) or die("queryCom socket create failed.."); 
	
	$connection = socket_connect($socket,'192.168.1.120', 7862) or die("queryCom socket connect failed.."); 
	
	//Ð´Êý¾Ýµ½socket»º´æ   json_encode($_POST) 		
	socket_write($socket, $keyword) or die("queryCom socket write failed..");

	while($temp = socket_read($socket, 1024, PHP_BINARY_READ )) { 
		$buffer .= $temp;
	}

	socket_shutdown($socket, 2);
	//socket_close($socket) or die("queryCom socket close failed..");

	$wordArr = explode("<#>",$buffer);
	array_pop($wordArr);
	$dataVal = array(array(title=>$keyword));

	for($i=0;$i<count($wordArr);$i++)
	{
		array_push($dataVal,array(title=>$wordArr[$i]));
	}

	$re = array(data=>$dataVal);
	echo json_encode($re);
	//print_r($re);
?>