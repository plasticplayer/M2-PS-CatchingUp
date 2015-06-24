<?php 
$json = (isset($_POST['JSON']))?$_POST['JSON']:"[]";
$decoded = json_decode ($json);

require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
}
$connection=mysqli_select_db($link,$databaseName);
$query  = "INSERT INTO Lesson(IdCategory,DateLesson,StatusLesson,NameLesson) VALUES('".$decoded->selectCategory."',now(),'OFFLINE','".$decoded->nameLesson."');"; 
$result = mysqli_query($link,$query) or die(mysqli_error($link));
echo "OK";
?>