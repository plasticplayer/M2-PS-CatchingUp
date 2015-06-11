<?php 
	$text=$_POST['var'];
	$idChapter=$_POST['var2'];
	$time=$_POST['var3'];
	require("config.php");
	$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
	if (!$link) { 
		die('Could not connect to MySQL: ' . mysqli_error()); 
	} 

	$connection=mysqli_select_db($link,$databaseName);
	$query = " INSERT INTO `reference`(`TextReference`, `IdChapter`, `TimeCode`, `Relevance`) VALUES ('$text',$idChapter,$time,1) "; 
	$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						
?>