<?php
	$idChapter=$_POST['var'];
	require("config.php");
	$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
	if (!$link) { 
		die('Could not connect to MySQL: ' . mysqli_error()); 
	} 

	$connection=mysqli_select_db($link,$databaseName);
	$query = " SELECT `TextReference`, `TimeCode` FROM Reference WHERE idChapter=$idChapter "; 
	$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
	
	//echo "<script type='text/javascript'>";
	while ($row=mysqli_fetch_array($result)) 
	{ 
		
		echo "videos.a.footnote({"
             ."start: $row[1],"
             ."end: $row[1]+5,"
			 ."text: '$row[0]',"
             ."target: 'footnotediv'"
			 ."});\r\n";
	} 
	//echo "</script>";



?>
