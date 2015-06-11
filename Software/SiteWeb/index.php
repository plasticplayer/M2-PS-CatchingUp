<?php
require("config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
//echo 'Connection OK'; //mysql_close($link); 
$connection=mysqli_select_db($link,$databaseName);
?><!DOCTYPE html>
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
<script>
$(document).ready(function(){
    $('.button').click(function(){
       // var clickBtnValue = $(this).val();
       // var ajaxurl = 'ajax.php',
        //data =  {'action': clickBtnValue};
       // $.post(ajaxurl, data, function (response) {
            // Response div goes here.
            //alert("action performed successfully");
			//document.append("dffhfh");
			//$("#test").html('Hello world');
			var c = window.$vars.value;
			alert(c);
			$('#ListLesson').load(
      "getLessons.php",{var:c},
      function() {
           // alert('Got the data.');
      }
);

			
       // });
    });
	/*$('#myOptions').change(function() {
    var val = $("#myOptions option:selected").text();
    alert(val);
});*/
  $("#ListLesson").on('click', 'td', function() {
	 //alert("test");
     //alert($( this ).text());
	 if ($(this).attr('id') == 'titleLesson'){
		//alert($(this).text());
		var c = $(this).text();
		alert(c);
		$('#ListLesson').load(
      "getChapters.php",{var:c},
      function() {
           // alert('Got the data.');
      }
	  );
	 }
	 	 
	if ($(this).attr('id') == 'titleChapter'){
		//alert($(this).text());
		//var c = $(this).text();
		alert("chapitre choisi");
		alert($(this).attr('value'));
		$idChapter = $(this).attr('value');
		window.location = "index_2.php?var=2";
	/*	$('#ListLesson').load(
      "getChapters.php",{var:c},
      function() {
           // alert('Got the data.');
      }
	  );*/
	 }
   });

});
</script>
<script>
	function getLessonSelected(val){
		window.$vars = {value: val};
		alert(val);
	}

</script>
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
		  <div class="col-sm-3 col-md-2 sidebar">
			<ul class="nav nav-sidebar">
			 <li class="header">
				<a href="categories.php">
				Catégories
				</a>
			  </li>
			  <?php 
						
						$query = " SELECT NameCategory FROM `category` "; 
						$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						while ($row=mysqli_fetch_array($result)) 
						{ 
							echo"<li><a href='./#'>$row[0]</a></li>"; 
						} 
			 ?>
			 <!-- <li class="active">
				<a href="./#">Mathématiques</a>
			  </li>
			  <li>
				<a href="./#">Reports</a>
			  </li>
			  <li>
				<a href="./#">Analytics</a>
			  </li>
			  <li>
				<a href="./#">Export</a>
			  </li>-->
			</ul>
		  </div>
		 <!-- SELECT NameLesson FROM lesson WHERE IdCategory = 1 -->
		  <div class="col-sm-1 col-sm-offset-12 col-md-12 col-md-offset-2 main">
			<ol class="breadcrumb">
			  <li><a href="#">Home</a></li>
			  <li><a href="#">Library</a></li>
			  <li class="active">Data</li>
			</ol>
			<h1 class="page-header">Dashboard</h1>
			
			<h2 class="sub-header">Section title</h2>
			<table>
				<tr>
					<td>
						<?php 


							//On insère des données variables grâce à une requète préparée
						//$query="INSERT INTO `User` (`IdUser`, `FirstName`, `LastName`, `Password`, `Email`) VALUES
						//(2, 'AMIR', 'ABDELAOUI', 'PASSWD', 'amir.abdelaoui@gmail.com')";
						// execution de la requète
						//mysqli_query($link,$query);

						$query = " SELECT NameCategory FROM `category` "; 
						$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						echo "
						 <table> 
						<select id='ListCategory' name='NameCategory' id='NameCategory' onchange='getLessonSelected(this.value)' >"; 
						$i=1;
						while ($row=mysqli_fetch_array($result)) 
						{ 
							echo"<option value='$i'>$row[0]</option>"; 
							$i++;
						} 
						echo"</select> 
						</table> ";
						?>
					</td>
					<td>
						<?php 
						echo "<table>
						<select name='Statut' id='Statut'>"; 
						echo"<option>Fini</option>"; 
						echo"<option>En cours</option>"; 
						echo"</select>
						</table> ";
						?>
					</td>
					<td>
						 <input id="rechercher" type="submit" class="button" name="insert" value="rechercher" />
					</td>
				</tr>
			</table>
		  </div>
		  <div id="test"class="col-sm-1 col-sm-offset-12 col-md-12 col-md-offset-2 main">
			<table id "displaytable" class="table table-striped table-bordered" cellspacing="0" width="100%">
				<thead>
				  <tr>
					<th>Titre</th>
					<th>Date</th>
				  </tr>
				</thead>
			    <tbody id="ListLesson">
					
				</tbody>
			  </table>
		  </div>
		  
		</div>
	  </div>
  </body>
</html>
