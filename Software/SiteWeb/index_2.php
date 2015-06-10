<?php
require("config.php");
$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
if (!$link) { 
	die('Could not connect to MySQL: ' . mysqli_error()); 
} 
//echo 'Connection OK'; //mysql_close($link); 
$connection=mysqli_select_db($link,$databaseName);
					$idChapter=$_GET['var'];
					echo "<script>alert($idChapter)</script>";
					//echo"<source src='$pathVideoBoard'/>";
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
			  <li class="active">
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
			  </li>
			</ul>
		  </div>
		  <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
			<ol class="breadcrumb">
			  <li><a href="#">Home</a></li>
			  <li><a href="#">Library</a></li>
			  <li class="active">Data</li>
			</ol>
			<h1 class="page-header">Dashboard</h1>
			
			<h2 class="sub-header">Section title</h2>
			  <video preload="auto" id="a" muted>
				<?php
					echo"<script>alert($idChapter)</script>";
					

						$query = " SELECT NameFileLesson FROM `fileLesson` WHERE fileLesson.idChapter = $idChapter and  fileLesson.typeFile = 'VIDEO_TRACKING'"; 
						$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						
						$row=mysqli_fetch_array($result);
					echo"<source src='$row[0]'/>";
				?>
			  </video>    
			  <video preload="auto" id="b" muted>
				<?php
				$query = " SELECT NameFileLesson FROM `fileLesson` WHERE fileLesson.idChapter = $idChapter and  fileLesson.typeFile = 'VIDEO_BOARD'"; 
						$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
						
						$row=mysqli_fetch_array($result);
					echo"<source src='$row[0]'/>";
				?>
			  </video>
			  <input type="range" value="0" id="scrub" />
			  <button type="submit" id="playvideo" class="btn btn-default">
					<span class="glyphicon glyphicon-play" aria-hidden="true"></span>
				</button>
				 <button type="submit" id="addreference" class="btn btn-default">
					<span class="glyphicon glyphicon-plus" aria-hidden="true"></span>
				</button>
				<div id = "footnotediv">
				
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
	  <script>
	  document.addEventListener( "DOMContentLoaded", function() {
	
			var videos = {
				a: Popcorn("#a"),
				b: Popcorn("#b"),  
			};
			
		videos.a.footnote({
             start: 2,
             end: 5,
			 text: "Pop dsfifhf  fh!",
             target: "footnotediv"
          });
		  
		  videos.a.footnote({
             start: 10,
             end: 15,
			 text: "Pop dsfifhf  fh!",
             target: "footnotediv"
          });
		    
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
					scrub.attr("max", this.duration() );

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
				}
				else
				{
					playbnt.find(".glyphicon").removeClass("glyphicon-pause").addClass("glyphicon-play");
					videos.a.pause();
					videos.b.pause();
					alert(videos.a.currentTime());
					//todo si admin
					
				}
			});
			
			addref.bind("click",function(){
				alert("yata");
				var newTextBoxDiv = $(document.createElement('div')).attr("id", 'TextBoxDiv');
 
				newTextBoxDiv.after().html(
				'<textarea  name="textarea" id="textArea" rows="4" cols="150" > laisser un commentaire </textarea><br><br><button type="submit" id="validatereference" class="btn btn-default"> Valider </button>');
				//todo bouton annuler
				newTextBoxDiv.appendTo("#footnotediv");
				validatereference = $("#validatereference");
				validatereference.bind("click",function(){
					var text = $("#textArea").val();
					var time = Math.round(videos.a.currentTime());
					//var idChapter = "<?php echo $idChapter ?>";
					alert(text);
					alert(time);
					//var time =
					/*$('#validatereference').load("addReference.php",{var:text},
						function() {
							// alert('Got the data.');
						}
					);*/
					
					$('#validatereference').load(
      "addReference.php.php",{var:text},
      function() {
           // alert('Got the data.');
      }
	  );
				
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

</script>
  </body>
</html>
