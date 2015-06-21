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
					<a class="btn btn-default" aria-label="Left Align" href="admin.php"><span class="glyphicon glyphicon-chevron-left" aria-hidden="true"></span>Retour</a><br/>
					<?php
					
						$query = "SELECT FirstName,LastName,Email FROM User WHERE User.IdUser=$idUser LIMIT 1"; 
						$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
						$row=mysqli_fetch_assoc($result);
						echo "<h3>Bonjour <b>".$row['FirstName']." ".$row['LastName']."</b>, Bienvenue dans votre interface d'administration</h3>";
						
						$query = "SELECT NameLesson FROM Lesson WHERE IdLesson=$idLesson LIMIT 1"; 
						$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
						$row=mysqli_fetch_assoc($result);
						echo "<h4>Selectionner les enregistrements à enregistrer dans le cours <b>".$row['NameLesson']."</b> :</h4>";
					?>
					<table id="listAffChap" class="table table-striped table-bordered" cellspacing="0">
						<thead>
							<tr>
								<th>Sel</th>
								<th>Status</th>
								<th>Date Enregistrement</th>
							</tr>
						</thead>
						<tbody>
							<?php
							$query = "SELECT IdChapter,StatusChapter,DateChapter FROM Chapter WHERE StatusChapter = 'TREATED' AND Chapter.idLesson = $idLesson AND Chapter.IdUser=$idUser"; 
							$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
							while(($row = mysqli_fetch_assoc($result))) {
								echo "<tr>";
								echo "<td><input type='checkbox' value='' name='chap".$row['IdChapter']."' checked='checked'></td>";
								echo "<td>".$row['StatusChapter']."</td>";
								echo "<td>".$row['DateChapter']."</td>";
								echo "</tr>";
							}
							$query = "SELECT IdChapter,StatusChapter,DateChapter FROM Chapter WHERE StatusChapter = 'TREATED' AND Chapter.idLesson is null AND Chapter.IdUser=$idUser"; 
							$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
							while(($row = mysqli_fetch_assoc($result))) {
								echo "<tr>";
								echo "<td><input type='checkbox' name='chap".$row['IdChapter']."' value=''></td>";
								echo "<td>".$row['StatusChapter']."</td>";
								echo "<td>".$row['DateChapter']."</td>";
								echo "</tr>";
							}
						?>
						</tbody>
					</table>
					<button type="button" class="btn btn-default" id="valListRec" ><span class="glyphicon glyphicon-ok" aria-hidden="true"></span>&nbsp;Valider</button>
					<br/><br/>
					<table id="listAffFiles" class="table table-striped table-bordered" cellspacing="0">
						<thead>
							<tr>
								<th>Nom du fichier</th>
								<th>Chemin</th>
								<th>Visualisation</th>
							</tr>
						</thead>
						<tbody>
						</tbody>
					</table>
					<div class="col-md-3 col-md-offset-9 main jumbotron">
						<form class="form-horizontal" method="post" action="createFiles.php" enctype="multipart/form-data" id="formFile">
							<h4>Ajout d'un fichier attaché</h4>
							<div class="form-group">
								<label for="name">Nom :</label>
								<input type="text" class="form-control" placeholder="Nom" name="name" id="name" />
							</div>
							<div class="form-group">
								<label for="file">Fichier :</label>
								<input type="file" name="file" id="file"/> 
							</div>
							<div class="form-group">
								<input type="hidden" name="lesson" value="<?php echo $idLesson; ?>" />
								<button type="button" class="btn btn-default" id="uploadbutton" ><span class="glyphicon glyphicon-plus" aria-hidden="true"></span>&nbsp;Ajouter</button>
								
							</div>
						</form>
						<div class="alert alert-success" style="display:none;" id="popupFileOk">Fichier ajouté avec succès !</div>
						<div class="alert alert-danger" style="display:none;" id="popupFileKo">Erreur pendant l'envoi des fichiers!</div>
					</div>
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
		function getFilesAttached()
		{
			$.post("ajax/getFilesAttached.php",{'idLesson':<?php echo $idLesson; ?>}, 
				function(data, textStatus) {
						$("#listAffFiles > tbody").empty();
						$.each(data, function(index,jsonObject){
							var url = jsonObject['PathAttachment'].split( '/' );
							var ligne = '<tr>'+
									'<td>' + jsonObject['NameAttachment'] + '</td>'+
									'<td>' + jsonObject['PathAttachment'] +'</td>'+
									'<td>' + '<a type="button" class="btn btn-default" aria-label="Left Align" href="attachment/' + url[url.length-2] + '/' + url[url.length-1] +'"><span class="glyphicon glyphicon-save" aria-hidden="true"></span> Télécharger</a></td>'+
									'</tr>' ;
							$("#listAffFiles").append(ligne);
						});
				
			}, "json");	
		}
		$(document).ready(function () {
			$("#uploadbutton").click(function () {
				
				var formData = new FormData($('#formFile')[0]);
				$("#popupFileOk").hide();
				$("#popupFileKo").hide();
				$.ajax({
					url: 'createFiles.php',  //Server script to process data
					type: 'POST',
					/*xhr: function() {  // Custom XMLHttpRequest
						var myXhr = $.ajaxSettings.xhr();
						if(myXhr.upload){ // Check if upload property exists
							myXhr.upload.addEventListener('progress',progressHandlingFunction, false); // For handling the progress of the upload
						}
						return myXhr;
					},*/
					//Ajax events
					//beforeSend: beforeSendHandler,
					success: function(data,textStatus)
					{
						
						if(data == "1")
						{
							//console.log(data);
							$("#file").val("");
							$("#name").val("");
							$("#popupFileOk").show();
							getFilesAttached();
						}
						else
						{
							$("#popupFileKo").show();
							getFilesAttached();
							
						}
					},
					//error: errorHandler,
					// Form data
					data: formData,
					//Options to tell jQuery not to process data or worry about content-type.
					cache: false,
					contentType: false,
					processData: false
				});
			});
		});
		
		$("#valListRec").click(function()
		{
			var datas = new Array();
			$("#listAffChap input").each(function( index ) {
			  chapName = $(this).attr('name');
			  selected = $(this).prop('checked');
			  datas[chapName] = selected;
			});
			
			$.post("ajax/updateChapters.php",{"idLess":<?php echo $idLesson; ?>,"JSON":JSON.stringify(datas)}, 
				function(data, textStatus) {
					if(data == "OK")
					{
						//console.log(data);
						location.reload();
					}
				
			});
		});
		getFilesAttached();
		</script>
	</body>
</html>