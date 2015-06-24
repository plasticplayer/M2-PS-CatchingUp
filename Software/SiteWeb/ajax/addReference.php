<?php 
	$text=$_POST['textData'];
	$idChapter=$_POST['idChap'];
	$time=$_POST['time'];
	require("../config.php");
	$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
	if (!$link) { 
		die('Could not connect to MySQL: ' . mysqli_error()); 
	} 
	$connection=mysqli_select_db($link,$databaseName);
	$query = " INSERT INTO Reference(TextReference, IdChapter, TimeCode, Relevance) VALUES ('$text',$idChapter,$time,1) "; 
	$result = mysqli_query($link,$query) or die(mysqli_error()); 
	echo "OK";
?>