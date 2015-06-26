<?php
$idLesson =(isset($_POST['idLesson']))?$_POST['idLesson']:0;
$idChapter =(isset($_POST['idChapter']))?$_POST['idChapter']:0;
require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
if($idLesson != 0)
	$query = "SELECT IdAttachment,NameAttachment,PathAttachment FROM Attachment WHERE IdLesson=$idLesson "; 
else if($idChapter != 0)
	$query = "SELECT IdAttachment,NameAttachment,PathAttachment FROM Attachment,Chapter WHERE Attachment.IdLesson=Chapter.IdLesson AND Chapter.IdChapter=$idChapter "; 
else
	die('[]');
$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
/*while ($row=mysqli_fetch_array($result)) 
{ 
	echo "videos.a.footnote({"
		 ."start: $row[1],"
		 ."end: ".($row[1]+5).","
		 ."text: '$row[0]',"
		 ."target: 'footnotediv'"
		 ."});\r\n";
} */
$rows = array();
while(($row = mysqli_fetch_assoc($result))) {
	$rows[] = $row;
}
echo json_encode($rows);
?>
