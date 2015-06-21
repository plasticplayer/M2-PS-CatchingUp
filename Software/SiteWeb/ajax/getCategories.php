<?php 
require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
$query = "SELECT IdCategory,NameCategory FROM Category;" ; 
$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
$rows = array();
while(($row = mysqli_fetch_assoc($result))) {
    $rows[] = $row;
}
echo json_encode($rows);
?>
