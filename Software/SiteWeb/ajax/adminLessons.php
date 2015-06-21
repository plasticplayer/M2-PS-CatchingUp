<?php 
$action = (isset($_POST['action'])) ? $_POST['action'] : 'l'; // l=> list a=>add d=>delete 
$json = (isset($_POST['JSON']))?$_POST['JSON']:"[]";
$decoded = json_decode ($json);
require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
}
$connection=mysqli_select_db($link,$databaseName);

if($action == 'a')
{
	$query  = "INSERT INTO Lesson(IdCategory,DateLesson,StatusLesson,NameLesson) VALUES('".$decoded->selectCategory."',now(),'OFFLINE','".$decoded->nameLesson."');"; 
	$result = mysqli_query($link,$query) or die(mysqli_error($link));
	echo "OK";
}
else if($action == 'ON')
{
	$query = "UPDATE Lesson SET StatusLesson = 'PUBLISHED' ".
	"WHERE IdLesson=".$decoded->idLesson." LIMIT 1; ";
	$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
	echo "OK";
}
else if($action == 'OFF')
{
	$query = "UPDATE Lesson SET StatusLesson = 'OFFLINE' ".
	"WHERE IdLesson=".$decoded->idLesson." LIMIT 1; ";
	$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
	echo "OK";
}
else if($action == 'l')
{
	$query = "SELECT Category.IdCategory,NameCategory,Lesson.IdLesson,DateLesson,StatusLesson,NameLesson, "
	."(SELECT COUNT(idChapter) FROM Chapter WHERE Lesson.IdLesson=Chapter.idLesson ) as CountChapt, "
	."(SELECT COUNT(IdAttachment) FROM Attachment WHERE Lesson.IdLesson=Attachment.idLesson  ) as CountAtta "
	."FROM Lesson, Category "
	."WHERE Category.IdCategory = Lesson.IdCategory "
	."GROUP BY Lesson.IdLesson;" ; 
	$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
	$rows = array();
	while(($row = mysqli_fetch_assoc($result))) {
		$rows[] = $row;
	}
	echo json_encode($rows);
}
else if($action == 'd')
{
	//$query  = "INSERT INTO Lesson(IdCategory,DateLesson,StatusLesson,NameLesson) VALUES('".$decoded->selectCategory."',now(),'OFFLINE','".$decoded->nameLesson."');"; 
	//$result = mysqli_query($link,$query) or die(mysqli_error($link));
	echo "OK";
}
?>