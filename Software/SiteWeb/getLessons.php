<?php 
	$c=$_POST['var'];
	//echo"value is $c";
	//echo "<script>alert(\"la variable est nulle\")</script>";
						$link = mysqli_connect('localhost','root',''); 
						if (!$link) { 
							die('Could not connect to MySQL: ' . mysqli_error()); 
						} 
						//echo 'Connection OK'; //mysql_close($link); 
						$connection=mysqli_select_db($link,'catchingup');

						$query = " SELECT NameLesson,DateLesson FROM `lesson` WHERE IdCategory = $c "; 
						$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						
						while ($row=mysqli_fetch_array($result)) 
						{ 
							echo"<tr><td id='titleLesson'>$row[0]</td><td>$row[1]</td></tr>"; 
						} 
						
?>