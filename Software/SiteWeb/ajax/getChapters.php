<?php 
$c= (isset($_POST['idLesson'])) ? $_POST['idLesson'] : 0;
require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
$query = "SELECT DateChapter,FirstName,IdChapter,LastName,RoomName FROM User,Chapter,Recorder,Room WHERE User.idUser = Chapter.idUser AND Chapter.idRecorder = Recorder.idRecorder AND Recorder.idRoom = Room.idRoom AND Chapter.idLesson = ".$c." AND Chapter.StatusChapter = 'TREATED' " ; 
$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
$rows = array();
while(($row = mysqli_fetch_assoc($result))) {
    $rows[] = $row;
}
echo json_encode($rows);
?>
