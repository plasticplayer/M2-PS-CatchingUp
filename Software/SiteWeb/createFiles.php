<?php
require("config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);	
mysqli_query($link,"SET NAMES UTF8");


if ( ! isset($_FILES["file"]) ){
	echo "0";
	return;
}
if ( $_FILES['file']['error'] > 0 ){
	echo "0";
	return;
}

{
	if ( !isset($_POST["lesson"]) || !isset($_POST["name"]) || $_POST["name"]=="" ){
		unlink($_FILES['file']['tmp_name']);
		echo "0";
		return;
	}
	
	// Generate Directory And create it
	$dir = $attachmentsDirectory."/".$_POST["lesson"]."/";
	$toto = $dir.md5(session_id().microtime()).".".end((explode(".", $_FILES['file']['name'])));
	
	$name = $dir.$_FILES['file']['name'];
	@mkdir($dir, 0766, true);
	
	// Move files
	$resultat = move_uploaded_file($_FILES['file']['tmp_name'],$toto);
	if ($resultat){
			$query = "INSERT INTO Attachment( IdLesson, PathAttachment, NameAttachment ) values ( ". $_POST["lesson"] .", '".$toto."', '".$_POST["name"]."');" ;
			$result = mysqli_query($link,$query) or die(mysqli_error($link));
			echo "1";
			return;
	}
	else{
		echo "0";
		return;
	}
}
echo "0";

?>