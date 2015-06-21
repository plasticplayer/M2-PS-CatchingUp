<?php 
$json = (isset($_POST['JSON']))?$_POST['JSON']:"[]";
$idLess = (isset($_POST['idLess']))?$_POST['idLess']:"[]";
$decoded = json_decode ($json);

require("../config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
$idTrue = "";
$idFalse = "";
foreach($decoded as $key => $value)
{
		if(isset($value) && $value == true)
		{
			$idTrue .= str_replace("chap", "",$key) .",";
		}
		else
		{
			$idFalse .= str_replace("chap", "",$key) .",";
		}
}
$idTrue = rtrim($idTrue, ",");
$idFalse = rtrim($idFalse, ",");
$queryTrue  = "UPDATE Chapter SET IdLesson = $idLess WHERE IdChapter IN(".$idTrue.");"; 
$queryFalse = "UPDATE Chapter SET IdLesson = NULL WHERE IdChapter IN(".$idFalse.");"; 
$resultT = mysqli_query($link,$queryTrue) or die(mysqli_error());
$resultF = mysqli_query($link,$queryFalse) or die(mysqli_error()); 
echo "OK";
?>