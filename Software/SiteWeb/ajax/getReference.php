<?php
$idChapter=(isset($_POST['idChap']))?$_POST['idChap']:0;
require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
$query = "SELECT TextReference,TimeCode FROM Reference WHERE idChapter=$idChapter ORDER BY TimeCode ASC"; 
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
