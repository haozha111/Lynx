<?php 
	$docID = $_POST["keyword"];
	//$docID = $_POST["tokens"];
	//$docID = '1234';
	// Create the socket and connect 
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP); 
	$connection = socket_connect($socket,'127.0.0.1', 7863); 

	//Ð´Êý¾Ýµ½socket»º´æ   json_encode($_POST) 		
	if(!socket_write($socket, $docID)){  
		printf("Write failed");  
	}  

	while($temp = socket_read($socket, 1024, PHP_BINARY_READ)) { 
		$buffer .= $temp;
	}

	socket_shutdown($socket, 2);
	//echo mysql_escape_string($buffer);
	
	$wordArr = explode("<#>",$buffer);
	$re = array(title=>$wordArr[0],content=>$wordArr[1],img=>$wordArr[2]);
	echo json_encode($re);
?>