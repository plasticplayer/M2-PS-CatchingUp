<?php
// On prolonge la session
session_start();
// On teste si la variable de session existe et contient une valeur
if(empty($_SESSION['login'])) 
{
  $connexion = 'Connexion';
  // Si inexistante ou nulle, on redirige vers le formulaire de login
  header('Location: authentification.php');
  exit();
}
require("config.php");
$adminActiv = false;
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
$connection=mysqli_select_db($link,$databaseName);
$idChapter=$_GET['idChap'];
			
mysqli_query($link,"SET NAMES UTF8");

$idUser = $_SESSION['login'];
$query = "SELECT FirstName,LastName,Email FROM User WHERE User.IdUser=$idUser LIMIT 1"; 
$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
$row=mysqli_fetch_assoc($result);
$connexion = "Bienvenue ".$row['FirstName']. " ".$row['LastName']." ";

$query = "SELECT count(*) as count FROM RecorderUser WHERE idUser = ".$_SESSION['login'].";";
$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
$row =mysqli_fetch_assoc($result);
if($row['count'] == '1')
{
	$adminActiv = true;
}
?>
<!DOCTYPE html>
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
			<!-- Bouton utilisé pour le mode mobile pour affiché le menu -->
			<a class="navbar-brand" href="index.php">Catching UP</a>
		  </div>
		  <!-- Collect the nav links, forms, and other content for toggling -->
		  <div class="collapse navbar-collapse" id="id-navbar-collapse">
			<ul class="nav navbar-nav navbar-right">
			  <li>
				<a href="deconnexion.php">
				  <b>Déconnexion</b>
				</a>
			  </li>
			  <li>
				<a href="#">
				  <b><?php echo $connexion ?></b>
				</a>
			  </li>
			</ul>
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
			<a class="btn btn-default" aria-label="Left Align" href="index.php"><span class="glyphicon glyphicon-chevron-left" aria-hidden="true"></span>Retour à l'acceuil</a><br/>
			<h3>Cours nommé : <b id="nomCours"></b> du <span id="dateCours"></span> enregistré dans la catégorie <b id="nomCategorie"></b></h3>
				<video preload="auto" id="a" muted>
				<?php
					$query = " SELECT NameFileLesson FROM FileLesson fileLesson WHERE fileLesson.idChapter = $idChapter and  fileLesson.typeFile = 'VIDEO_TRACKING'"; 
					$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
					$row=mysqli_fetch_array($result);
					$splitUrl = explode( "/", $row[0]);
					$sSize = count( $splitUrl);
					$address = "media/".$splitUrl[ $sSize - 2]."/".$splitUrl[$sSize-1];
					echo"<source src='$address' type='video/mp4'/>";
				?>
				</video>    
				<video preload="auto" id="b" muted>
				<?php
					$query = " SELECT NameFileLesson FROM FileLesson fileLesson WHERE fileLesson.idChapter = $idChapter and  fileLesson.typeFile = 'VIDEO_BOARD'"; 
					$result = mysqli_query($link,$query) or die( mysqli_error($link)); 
					$row=mysqli_fetch_array($result);
					$splitUrl = explode( "/", $row[0]);
					$sSize = count( $splitUrl);
					$address = "media/".$splitUrl[ $sSize - 2]."/".$splitUrl[$sSize-1];
					echo"<source src='$address' type='video/mp4'/>";
				?>
				</video>
				<input type="range" value="0" id="scrub" ></input>
				<button type="button" id="playvideo" class="btn btn-default">
					<span class="glyphicon glyphicon-play" aria-hidden="true"></span>
				</button>
				<?php if($adminActiv)
				echo '<button type="button" id="addreference" class="btn btn-default" >
					<span class="glyphicon glyphicon-plus" aria-hidden="true"></span>
				</button>'
				?>
				<h4>Notes additionelles :</h4>
				<div id = "footnotediv"></div>
				<div id="BoxAddNote" style="display:none;">
					<h4>Ajouter une note</h4>
					<div id = "addNote"></div>
				</div>
			</div>
			<div class="col-md-3 main">
				<h4>Liste des références</h4>
				<table id="listReferences" class="table table-striped table-bordered table-condensed" style="overflow:hidden;" cellspacing="0" width="100%" fixed-header>
				<thead>
				  <tr>
					<th>Temps</th>
					<th>Texte</th>
				  </tr>
				</thead>
			    <tbody >
				</tbody>
			</table>
			</div>
			<div class="col-md-9 main">
				<h4>Liste des fichiers joints</h4>
				<table id="listAffFiles" class="table table-striped table-bordered" cellspacing="0">
					<thead>
						<tr>
							<th>Nom du fichier</th>
							<th>Visualisation</th>
						</tr>
					</thead>
					<tbody>
					</tbody>
				</table>
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

	  <script>

		var videos;
		var lengthMax = 0;
		function setVideoTime(seconds)
		{
			videos.a.currentTime( seconds );
			videos.b.currentTime( seconds );
		}
		function getFilesAttached()
		{
			$.post("ajax/getFilesAttached.php",{'idChapter':<?php echo $idChapter; ?>}, 
				function(data, textStatus) {
						$("#listAffFiles > tbody").empty();
						$.each(data, function(index,jsonObject){
							var url = jsonObject['PathAttachment'].split( '/' );
							var ligne = '<tr>'+
									'<td>' + jsonObject['NameAttachment'] + '</td>'+
									'<td>' + '<a type="button" class="btn btn-default" aria-label="Left Align" href="attachment/' + url[url.length-2] + '/' + url[url.length-1] +'"><span class="glyphicon glyphicon-save" aria-hidden="true"></span> Télécharger</a></td>'+
									'</tr>' ;
							$("#listAffFiles").append(ligne);
						});
				
			}, "json");	
		}
		function getInfosCours()
		{
			$.post('ajax/getInfosChapter.php', {"idChapter":<?php echo $idChapter ?>}, function(data, textStatus) {
				$.each(data, function(index,jsonObject){
					$("#nomCours").text(jsonObject['NameLesson']);
					$("#dateCours").text(jsonObject['DateChapter']);
					$("#nomCategorie").text(jsonObject['NameCategory']);
				});
			}, "json");	
		}
		function refreshRefs()
		{
			$.post('ajax/getReference.php', {"idChap":<?php echo $idChapter ?>}, function(data, textStatus) {
				$("#listReferences > tbody").empty();
				$.each(data, function(index,jsonObject){
					var date = new Date(null);
					date.setSeconds(jsonObject['TimeCode'] ); // specify value for SECONDS here
					var stDate = date.toISOString().substr(11, 8);
					var ligne = '<tr>'+
									'<td><a href="javascript:setVideoTime('+jsonObject['TimeCode']+')">' + stDate  + '</a></td>'+
									'<td>' + jsonObject['TextReference'] + '</td>'+
									'</tr>' ;
							$("#listReferences").append(ligne);
					videos.a.footnote({start: jsonObject['TimeCode'],end: (jsonObject['TimeCode'] + 5),text: jsonObject['TextReference'],target: 'footnotediv'});
					//$("#listAffChap").append('<tr><td><a href="index_2.php?idChap='+jsonObject['IdChapter']+'" >Cour enregistré par ' + jsonObject['FirstName'] + " " + jsonObject['LastName'] + ' en salle '+ jsonObject['RoomName'] + '</a></td><td>' + jsonObject['DateChapter'] + '</td></tr>' );
					/*$.each(jsonObject, function(key,val){
						console.log("key : "+key+" ; value : "+val);
					});*/
				});
			}, "json");	
		}
	  document.addEventListener( "DOMContentLoaded", function() {
			videos = {
				a: Popcorn("#a"),
				b: Popcorn("#b"),  
			};
			//$('#footnotes').load("getReference.php",{"idChap":<?php echo $idChapter ?>},function(){});
			
			refreshRefs();
			
			scrub = $("#scrub"),
			playbnt = $("#playvideo"),
			addref = $("#addreference"),
			loadCount = 0, 
			events = "play pause timeupdate seeking".split(/\s+/g);
			var UpdateScrub = true;

			// iterate both media sources

			Popcorn.forEach( videos, function( media, type ) {

				

				// when each is ready... 

				media.on( "canplayall", function() {

					

					// trigger a custom "sync" event

					this.trigger("sync");

					

					// set the max value of the "scrubber"
					if(lengthMax < this.duration() )
						lengthMax = this.duration() ;
					scrub.attr("max", lengthMax );



				// Listen for the custom sync event...    

				}).on( "sync", function() {

					

					// Once both items are loaded, sync events

					if ( ++loadCount == 2 ) {

						

						// Iterate all events and trigger them on the video B

						// whenever they occur on the video A

						events.forEach(function( event ) {

							videos.a.on( event, function() {
								// Avoid overkill events, trigger timeupdate manually
								if ( event === "timeupdate" ) {
									// update scrubber
									if(UpdateScrub)
										scrub.val( this.currentTime() );
									if ( !this.media.paused ) {
										return;
									} 
									videos.b.trigger( "timeupdate" );
									return;

								}

								

								if ( event === "seeking" ) {

									videos.b.currentTime( this.currentTime() );

								}

								

								if ( event === "play" || event === "pause" ) {

									videos.b[ event ]();

									sync();

								}

							});

						});

					}

				});

			});



			scrub.bind("change", function() {

				var val = this.value;

				

				videos.a.currentTime( val );

				videos.b.currentTime( val );

			});

			scrub.bind("mousedown",function() {

				UpdateScrub = false;

			});

			scrub.bind("mouseup",function() {

				UpdateScrub = true;

			});

			playbnt.bind("click",function(){

				if(videos.a.media.paused)

				{			

					playbnt.find(".glyphicon").removeClass("glyphicon-play").addClass("glyphicon-pause");

					videos.a.play();

					videos.b.play();

					document.getElementById("addreference").style.visibility='hidden';

				}

				else

				{

					playbnt.find(".glyphicon").removeClass("glyphicon-pause").addClass("glyphicon-play");

					videos.a.pause();

					videos.b.pause();

					document.getElementById("addreference").style.visibility='visible'; // show

					

					//todo si admin

					

				}

			});

			

			addref.bind("click",function(){
				addref.hide();
				$("#BoxAddNote").show();

				$("#addNote").append('<textarea  name="textarea" id="textArea" rows="4" cols="150" >laisser un commentaire</textarea><br><br><button type="submit" id="validatereference" class="btn btn-default"> Valider </button><button type="button" id="closeRef" class="btn btn-default">Annuler</button>');

				validatereference = $("#validatereference");
				closeRef = $("#closeRef");

				validatereference.bind("click",function(){
					var text = $("#textArea").val();
					var time = Math.round(videos.a.currentTime());
					
					$.post("ajax/addReference.php",{"textData":text,"idChap":<?php echo $idChapter ?>,"time":time}, 
						function(data, textStatus) {
							if(data == "OK")
							{
								$("#addNote").empty();
								$("#BoxAddNote").hide();
								refreshRefs();
								addref.show();
							}
					});	
				});
				closeRef.bind("click",function(){
					$("#addNote").empty();
					$("#BoxAddNote").hide();
					addref.show();
	
				});
				

			});

			

			

			

			



			// With requestAnimationFrame, we can ensure that as 

			// frequently as the browser would allow, 

			// the video is resync'ed.

			function sync() {

				videos.b.currentTime( 

					videos.a.currentTime()        

				);

				requestAnimFrame( sync );

			}





			sync();

		});
getInfosCours();
getFilesAttached();



</script>

  </body>

</html>
