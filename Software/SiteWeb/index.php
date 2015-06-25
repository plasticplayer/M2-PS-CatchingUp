<?php
// On prolonge la session
session_start();
require("config.php");
$adminActiv = false;
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
//echo 'Connection OK'; //mysql_close($link); 
$connection=mysqli_select_db($link,$databaseName);
mysqli_query($link,"SET NAMES UTF8");

// On teste si la variable de session existe et contient une valeur
if(empty($_SESSION['login'])) 
{
	$connexion = 'Connexion';
  // Si inexistante ou nulle, on redirige vers le formulaire de login
  //header('Location: http://localhost:8080/authentification.php');
  //exit();
}
else{
	$query = " SELECT FirstName,LastName FROM User WHERE idUser = '".$_SESSION['login']."' "; 
	$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
	$row=mysqli_fetch_array($result);
	$connexion = "Bienvenue ".$row['FirstName']. " ".$row['LastName']." ";
	
	
	$query = "SELECT count(*) as count FROM RecorderUser WHERE idUser = ".$_SESSION['login'].";";
	$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
	$row =mysqli_fetch_assoc($result);
	if($row['count'] == '1')
	{
		$adminActiv = true;
	}
}

?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="generator"
    content="HTML Tidy for HTML5 (experimental) for Windows https://github.com/w3c/tidy-html5/tree/c63cc39" />
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta charset="utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <meta name="description" content="" />
    <meta name="author" content="" />
    <!--<link rel="icon" href="http://getbootstrap.com/favicon.ico">-->
    <title>Catching UP</title>
    <!-- Bootstrap core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet" />
    <!-- Custom styles for this template -->
    <link href="css/dashboard.css" rel="stylesheet" />
    <!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
    <!--[if lt IE 9]>
                  <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
                  <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
                <![endif]-->
				<style>
video {
  float: left;
  width: 40%;
}

#controls {
  clear: left;
}
</style>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
  </head>
  <body>
	  <nav class="navbar navbar-default navbar-fixed-top navbar-inverse">
		<div class="container-fluid">
		  <!-- Brand and toggle get grouped for better mobile display -->
		  <div class="navbar-header">
			<!-- Bouton utilisé pour le mode mobile pour affiché le menu -->
			<a class="navbar-brand" href="#">Catching UP</a>
		  </div>
		  <!-- Collect the nav links, forms, and other content for toggling -->
		  <div class="collapse navbar-collapse" id="id-navbar-collapse">
			<ul class="nav navbar-nav navbar-right">
			  <li>
				<a href="inscription.php">
				  <b>Inscription</b>
				</a>
			  </li>
			  <?php
				if($adminActiv)
					echo '<li><a href="admin.php"><b>Administration</b></a></li>';
			  ?>
			  <li>
				<a href="authentification.php">
				  <b> <?php echo $connexion ?></b>
				</a>
			  </li>
			</ul>
			
		  </div>
		  <!-- /.navbar-collapse -->
		</div>
		<!-- /.container-fluid -->
	  </nav>
	  <div class="container-fluid">
		<div class="row">
		  <div class="col-sm-3 col-md-2 sidebar">
			<ul id="listcategorie" class="nav nav-sidebar">
			 <li class="header">
				<a href="#">
				Catégories
				</a>
			  </li>
			  <?php 
						
						$query = "SELECT IdCategory,NameCategory FROM Category"; 
						$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
						while ($row=mysqli_fetch_array($result)) 
						{ 
							echo'<li><a href="javascript:showLessons('.$row["IdCategory"].');" >'.$row["NameCategory"].'</a></li>'; 
						} 
			 ?>
			</ul>
			</div>
		  <div class="col-md-10 col-md-offset-2 main">
		  <h2>Selectionner une catégorie de cours à afficher</h2><br/>
		  <form class="form-inline">
			  <div class="form-group">
				<label for="exampleInputName2">Catégorie : </label>
				<select name='exampleInputName2' id='selectCategory' class="form-control"> 
					<option value=-1>-- Selectionner --</option>
					<?php 
					$query = "SELECT IdCategory,NameCategory FROM Category"; 
					$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
					while ($row=mysqli_fetch_array($result)) 
					{ 
						echo"<option value='".$row["IdCategory"]."'>".$row["NameCategory"]."</option>"; 
					} 
					?>
				</select> 
			  </div>
			  
			  <button type="button" id="btnGetLessonByCat" class="btn btn-default">rechercher</button>
			</form>
			<br/>
			<table id="listAffCours" style="display:none;" class="table table-striped table-bordered" cellspacing="0" width="100%">
				<thead>
				  <tr>
					<th>Nom du cours</th>
					<th>Date de Création</th>
				  </tr>
				</thead>
			    <tbody >
				</tbody>
			</table>
			<table id="listAffChap" style="display:none;" class="table table-striped table-bordered" cellspacing="0" width="100%">
				<thead>
				  <tr>
					<th>Professeur</th>
					<th>Date de Création</th>
				  </tr>
				</thead>
			    <tbody >
				</tbody>
			</table>
		  </div>
		</div>
	  </div>
  </body>
	<script>
    function showRecordings(idLesson)
	{
		if(idLesson != -1)
	   {
		   $.post('ajax/getChapters.php', {"idLesson":idLesson}, function(data, textStatus) {
			   $("#listAffChap > tbody").empty();
			   $("#listAffChap").show();
			    $("#listAffCours").hide();
				$.each(data, function(index,jsonObject){
					
					$("#listAffChap").append('<tr><td><a href="index_2.php?idChap='+jsonObject['IdChapter']+'" >Cour enregistré par ' + jsonObject['FirstName'] + " " + jsonObject['LastName'] + ' en salle '+ jsonObject['RoomName'] + '</a></td><td>' + jsonObject['DateChapter'] + '</td></tr>' );
					/*$.each(jsonObject, function(key,val){
						console.log("key : "+key+" ; value : "+val);
					});*/
				});
			}, "json");
		}
	}
	function showLessons(idCat)
	{
		if(idCat != -1)
	   {
		   $.post('ajax/getLessons.php', {"idCat":idCat}, function(data, textStatus) {
			   $("#listAffCours > tbody").empty();
			   $("#listAffCours").show();
			   $("#listAffChap").hide();
				$.each(data, function(index,jsonObject){
					
					$("#listAffCours").append('<tr><td><a href="javascript:showRecordings('+jsonObject['IdLesson']+');" >' + jsonObject['NameLesson'] + '</a></td><td>' + jsonObject['DateLesson'] + '</td></tr>' );
					/*$.each(jsonObject, function(key,val){
						console.log("key : "+key+" ; value : "+val);
					});*/
				});
			}, "json");
		}
	}
	$("#btnGetLessonByCat").click(function()
	{
	   var idCat = $("#selectCategory").val();
	   showLessons(idCat);
	});
	</script>
</html>