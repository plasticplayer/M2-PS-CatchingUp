<?php 
require("../config.php");
$idChapter = (isset($_POST['idChapter'])) ? $_POST['idChapter'] : '0'; // l=> list a=>add d=>delete 
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
$query = "SELECT NameLesson,NameCategory,DateChapter FROM Lesson,Category,Chapter WHERE Chapter.idChapter=$idChapter AND Chapter.idLesson = Lesson.idLesson AND Category.idCategory = Lesson.idCategory"; 
$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
$rows = array();
while(($row = mysqli_fetch_assoc($result))) {
    $rows[] = $row;
}
echo json_encode($rows);
?>
