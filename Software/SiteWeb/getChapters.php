<?php 
	$c=$_POST['var'];
	//echo"value is $c";
	//echo "<script>alert(\"la variable est nulle\")</script>";
						require("config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
//echo 'Connection OK'; //mysql_close($link); 
$connection=mysqli_select_db($link,$databaseName);

						$query = " SELECT DateChapter,FirstName,IdChapter FROM User,Chapter,Lesson WHERE Lesson.NameLesson = '$c' and User.idUser = Chapter.idUser and Lesson.idLesson = Chapter.idLesson "; 
						$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
						
						while ($row=mysqli_fetch_array($result)) 
						{ 
							echo"<tr><td id='titleChapter' value='$row[2]'>cours du $row[0]</td><td>$row[1]</td></tr>"; 
						} 
						
?>
