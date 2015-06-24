<?php
require("config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
			
mysqli_query($link,"SET NAMES UTF8");
function enum_to_array($table, $field) {
		global $link;
    $query = "SHOW FIELDS FROM `{$table}` LIKE '{$field}'";
	$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
    $row = mysqli_fetch_assoc($result);
    preg_match('#^enum\((.*?)\)$#ism', $row['Type'], $matches);
    $enum = str_getcsv($matches[1], ",", "'");
    return $enum;
}
$listStatusLesson = enum_to_array("Lesson","StatusLesson");
$idUser = 17;
$idLesson = (isset($_GET['idLesson']))?$_GET['idLesson']:0;
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
			<!-- Bouton utilis?our le mode mobile pour affich?e menu -->
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
						
						$query = "SELECT NameLesson FROM Lesson WHERE IdLesson=$idLesson LIMIT 1"; 
						$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
						$row=mysqli_fetch_assoc($result);
						echo "<h4>Sélectionner les enregistrements à enregistrer dans le cours <b>".$row['NameLesson']."</b> :</h4>";
					?>
					
						<form method="post" action="createFiles.php" enctype="multipart/form-data">
							<label for="icone"> Sélectionner un enregistrement </label>
							<input type="text" name="name" />
							<input type="file" name="file" />
							<input type="submit" name="submit" value="Envoyer" />
						</form>
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
		
		
		$("#valListRec").click(function()
		{
			var datas = new Array();
			$("#listAffChap input").each(function( index ) {
			  chapName = $(this).attr('name');
			  selected = $(this).prop('checked');
			  datas[chapName] = selected;
			});
			
			$.post("updateChapters.php",{"idLess":<?php echo $idLesson; ?>,"JSON":JSON.stringify(datas)}, 
				function(data, textStatus) {
					if(data == "OK")
					{
						//console.log(data);
						location.reload();
					}
				
			});
		});
		</script>
	</body>
</html>