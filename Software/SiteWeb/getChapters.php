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

						$query = " SELECT DateChapter,FirstName,IdChapter,LastName FROM `user`,`chapter`,`lesson` WHERE lesson.NameLesson = '$c' and user.idUser = chapter.idUser  and lesson.idLesson = chapter.idLesson "; 
						$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						
						while ($row=mysqli_fetch_array($result)) 
						{ 
							echo"<tr><td id='titleChapter' value='$row[2]'><a>cours du $row[0]</a></td><td>$row[3] $row[1] </td></tr>"; 
						} 
						
?>