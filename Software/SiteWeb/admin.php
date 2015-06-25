<?php
// On prolonge la session
session_start();
// On teste si la variable de session existe et contient une valeur
if(empty($_SESSION['login'])) 
{
	
  // Si inexistante ou nulle, on redirige vers le formulaire de login
  header('Location: authentification.php');
  exit();
}

require("config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
			
mysqli_query($link,"SET NAMES UTF8");
$idUser = $_SESSION['login'];
$query = "SELECT count(*) as count FROM RecorderUser WHERE idUser = ".$idUser.";";
$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
$row =mysqli_fetch_assoc($result);
if($row['count'] != '1')
{
	// Si inexistante ou nulle, on redirige vers le formulaire de login
  header('Location: index.php');
  exit();
}

?><!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="generator" content="HTML Tidy for HTML5 (experimental) for Windows https://github.com/w3c/tidy-html5/tree/c63cc39" />
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
video #a{
  float: left;
  width: 20%;
}
video #b{
  float: left;
  width: 40%;
}

#controls {
  clear: left;
}
.vertBarr{
	border : 0px 0px 0px 1px;
	border-color : black;
}
</style>
  </head>
  <body>
	  <nav class="navbar navbar-default navbar-fixed-top navbar-inverse">
		<div class="container-fluid">
		  <!-- Brand and toggle get grouped for better mobile display -->
		  <div class="navbar-header">
			<!-- Bouton utilis� pour le mode mobile pour affich� le menu -->
			<a class="navbar-brand" href="index.php">Catching UP</a>
		  </div>
		  <!-- Collect the nav links, forms, and other content for toggling -->
		  <div class="collapse navbar-collapse" id="id-navbar-collapse">
			<ul class="nav navbar-nav navbar-right">
			  <li>
				<a href="#">
				  <b>Inscription</b>
				</a>
			  </li>
			  <li>
				<a href="#">
				  <b>Connexion</b>
				</a>
			  </li>
			</ul>
			<form class="navbar-form navbar-right" role="search" action="recherche.php" method="POST">
			  <div class="form-group">
				<input type="text" class="form-control" placeholder="Search" />
			<button type="submit" class="btn btn-default">
				<span class="glyphicon glyphicon-search" aria-hidden="true"></span>
			</button>
			  </div>
			</form>
		  </div>
		  <!-- /.navbar-collapse -->
		</div>
		<!-- /.container-fluid -->
	  </nav>
		<div class="container-fluid">
			<div class="row">
				<div class="col-md-12 main">
					<?php
					
						$query = "SELECT FirstName,LastName,Email FROM User WHERE User.IdUser=$idUser LIMIT 1"; 
						$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
						$row=mysqli_fetch_assoc($result);
						echo "<h3>Bonjour <b>".$row['FirstName']." ".$row['LastName']."</b>, Bienvenue dans votre interface d'administration</h3>";
					?>
					<h4>Voici les cours:</h4>
					<table id="listAffLess" class="table table-striped table-bordered" cellspacing="0" width="100%">
						<thead>
							<tr>
								<th>Nom</th>
								<th>Status</th>
								<th>Catégorie</th>
								<th>Nombre d'enregistrement</th>
								<th>Nombre de fichiers attachés</th>
							</tr>
						</thead>
						<tbody>
							<?php
							/*$query = "SELECT Lesson.IdLesson,Lesson.NameLesson,Lesson.StatusLesson,Category.NameCategory,count(idChapter) as count FROM Chapter,Lesson,Category WHERE Lesson.idLesson = Chapter.idLesson AND Category.idCategory = Lesson.idCategory AND Chapter.IdUser=$idUser"; 
							$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
							while(($row = mysqli_fetch_assoc($result))) {
								echo "<tr>".
									"<td><a href='adminLessonDetails.php?idLesson=".$row['IdLesson']."'>".$row['NameLesson']."</a></td>";

								echo "<td>".$row['StatusLesson']."</td>";
								echo "<td>".$row['NameCategory']."</td>".
									"<td>".$row['count']."</td>".
									"</tr>";
							}*/
						?>
						</tbody>
					</table>
				</div>
				<div class="col-md-3 col-md-offset-1 main jumbotron">
					
					<form class="form-horizontal">
						<h4>Ajout d'une catégorie</h4>
						<div class="form-group">
							<label for="nameCategorie">Nom :</label>
							<input type="text" class="form-control" placeholder="Nom" name="nameCategorie" id="nameCategorie"/>
						</div>
						<div class="form-group">
							<button type="button" class="btn btn-default" id="btnAddCat"><span class="glyphicon glyphicon-plus" aria-hidden="true"></span>&nbsp;Ajouter</button>
						</div>
					</form>
					<div class="alert alert-success" style="display:none;" id="popupCat" >Catégorie ajoutée avec succès !</div>
					<div class="alert alert-danger" style="display:none;" id="popupCatError" >Erreur, veuillez verifier les champs !</div>
				</div>
				<div class="col-md-3 col-md-offset-3 main jumbotron">
					<form class="form-horizontal">
						<h4>Ajout d'un cours</h4>
						<div class="form-group">
							<label for="nameLesson">Nom :</label>
							<input type="text" class="form-control" placeholder="Nom" name="nameLesson" id="nameLesson" />
						</div>
						<div class="form-group">
							<label for="exampleInputName2">Catégorie :</label>
							<select name='exampleInputName2' id='selectCategory' class="form-control"></select> 
						</div>
						<div class="form-group">
							<button type="button" class="btn btn-default" id="btnAddLesson"><span class="glyphicon glyphicon-plus" aria-hidden="true"></span>&nbsp;Ajouter</button>
						</div>
					</form>
					<div class="alert alert-success" style="display:none;" id="popupCours">Cours ajoutée avec succès !</div>
					<div class="alert alert-danger" style="display:none;" id="popupCoursError">Erreur, veuillez verifier les champs !</div>
				</div>
			</div>
		</div>	
		<!-- Bootstrap core JavaScript
		================================================== -->
		<!-- Placed at the end of the document so the pages load faster -->
		<script src="js/jquery.min.js"></script> 
		<script src="js/bootstrap.min.js"></script>
		<script src="js/h5utils.js"></script>
		<!--<script src="js/popcorn.min.js"></script>-->
		<script src="http://popcornjs.org/code/dist/popcorn-complete.min.js"></script>
		<script type="text/javascript" id="footnotes"></script>
		<script type="text/javascript" src="js/utils.js"></script>
		<script>	
		function refreshCategories()
		{
			$.post("ajax/getCategories.php",{}, 
				function(data, textStatus) {
						$("#selectCategory ").empty();
						$('#selectCategory').append($('<option>', {
							value: -1,
							text: '-- Selectionner --'
						}));					   
						$.each(data, function(index,jsonObject){
							$('#selectCategory').append($('<option>', {
								value: jsonObject['IdCategory'],
								text: jsonObject['NameCategory']
							}));
						});
				
			}, "json");
		}
		function setOnline(idLesson)
		{
			var datas = new Array();
			datas['idLesson'] = idLesson;
			$.post("ajax/adminLessons.php",{'action':'ON',"JSON":JSON.stringify(datas)}, 
				function(data, textStatus) {
					if(data == "OK")
					{
						refreshLessons();
					}
				});
		}
		function setOffline(idLesson)
		{
			var datas = new Array();
			datas['idLesson'] = idLesson;
			$.post("ajax/adminLessons.php",{'action':'OFF',"JSON":JSON.stringify(datas)}, 
				function(data, textStatus) {
					if(data == "OK")
					{
						refreshLessons();
					}
				});
		}
		function refreshLessons()
		{
			$.post("ajax/adminLessons.php",{'action':'l'}, 
				function(data, textStatus) {
						$("#listAffLess > tbody").empty();
										   
						$.each(data, function(index,jsonObject){
							var ligne = '<tr>'+
									'<td><a href="adminLessonDetails.php?idLesson='+jsonObject['IdLesson']+'" >' + jsonObject['NameLesson'] + '</a></td>'+
									'<td>';
							if(jsonObject['StatusLesson'] == 'PUBLISHED')
							{
								ligne += "Publié";
								ligne += '<button type="button" class="pull-right btn btn-default btn-warning" style="width: 100px" onclick="setOffline('+jsonObject['IdLesson']+');"><span class="glyphicon glyphicon-eye-close" aria-hidden="true"></span>&nbsp;Masquer</button>';
							}
							else if(jsonObject['StatusLesson'] == 'OFFLINE')
							{
								ligne += "Hors ligne";
								ligne += '<button type="button" class="pull-right btn btn-default btn-info" style="width: 100px" onclick="setOnline('+jsonObject['IdLesson']+');"><span class="glyphicon glyphicon-eye-open" aria-hidden="true"></span>&nbsp;Afficher</button>';
							}
							else
							{
								ligne += "Inconnu : ("+jsonObject['StatusLesson']+")"; 
							}
							ligne +=  '</td>'+
									'<td>' + jsonObject['NameCategory'] +'</td>'+
									'<td>' + jsonObject['CountChapt'] +'</td>'+
									'<td>' + jsonObject['CountAtta'] +'</td>'+
									'</tr>' ;
							$("#listAffLess").append(ligne);
						});
				
			}, "json");	
		}
		$("#btnAddCat").click(function()
		{
			
			$("#popupCat").hide();
			$("#popupCatError").hide();
			var datas = new Array();
			datas['nameCategory'] = $("#nameCategorie")[0].value;
			if(datas['nameCategory'] != "")
			{
				$.post("ajax/addCategory.php",{"JSON":JSON.stringify(datas)}, 
					function(data, textStatus) {
						if(data == "OK")
						{
							refreshCategories();
							$("#nameCategorie").val("");
							$("#popupCat").show();
						}
				});
			}
			else
				$("#popupCatError").show();
		});
		$("#btnAddLesson").click(function()
		{
			$("#popupCoursError").hide();
			$("#popupCours").hide();
			var datas = new Array();
			datas['nameLesson'] = $("#nameLesson").val();
			datas['selectCategory'] = $("#selectCategory").val();
			if(datas['nameLesson'] != "" && datas['selectCategory'] != -1)
			{
				$.post("ajax/addLesson.php",{"JSON":JSON.stringify(datas)}, 
				function(data, textStatus) {
					if(data == "OK")
					{
						$("#nameLesson").val("");
						$("#selectCategory").val(-1);
						$("#popupCours").show();
						refreshLessons();
					}
				});
				
			}
			else
			{
				$("#popupCoursError").show();
			}
			/*$.post("addCategory.php",{"JSON":JSON.stringify(datas)}, 
				function(data, textStatus) {
					if(data == "OK")
					{
						refreshCategories();
						$("#nameCategorie").val("");
						$("#popupCat").show();
					}
			});*/
		});
		refreshLessons();
		refreshCategories();
		</script>
	</body>
</html>