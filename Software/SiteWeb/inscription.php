<?php
  // Definition des constantes et variables
  //define('LOGIN','toto');
  //define('PASSWORD','tata');
  $errorMessage = '';
  
 
  // Test de l'envoi du formulaire
  if(!empty($_POST)) 
  {
    // Les identifiants sont transmis ?
    if(!empty($_POST['nom']) && !empty($_POST['prenom']) && !empty($_POST['email']) && !empty($_POST['password'])) 
    {
		
		//TODO: interdiction d'ajouter un utilisateur avec un mot de passe déjà existant
		$errorMessage = 'Ici 3 !';
		require("config.php");
		$password = addslashes(md5($_POST['password'].$grainDeSel));
		$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
		if (!$link) { 
			die('Could not connect to MySQL: ' . mysqli_error()); 
		} 
		 //echo 'Connection OK'; //mysql_close($link); 
		$connection=mysqli_select_db($link,$databaseName);

		$query = " SELECT Email FROM `user` WHERE Email ='".$_POST['email']."' "; 
		$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
		$email=mysqli_fetch_array($result); 
		
		$query = "SELECT user.IdUser FROM `user`,`websiteuser` WHERE user.IdUser = websiteuser.IdUser and user.email = '".$_POST['email']."'";
		$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
		$id=mysqli_fetch_array($result);

		
		if($email[0] === NULL){ // aucune existence dans la base (aucune présence dans la table user et websiteuser)
			$errorMessage = 'nouvel utilisateur !';
			$query = " INSERT INTO `user`(`IdUser`, `FirstName`, `LastName`, `Password`, `Email`) VALUES (null,'".$_POST['prenom']."','".$_POST['nom']."','".$password."','".$_POST['email']."') "; 
			$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
			// TODO: mettre date d'enregistrement
			$query = " INSERT INTO `websiteuser`(`IdUser`, `DateRegistration`) VALUES ('".mysqli_insert_id($link)."','2015-05-06 10:32:14') "; 
			$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
		}
		elseif($id[0] !== NULL){ // si utilisateur présent dans user ET websiteuser
			$errorMessage ='Compte déjà existant!';
		}
		else{ //utilisateur présent uniquement dans user
			$query = " SELECT Password FROM `user` WHERE Email ='".$_POST['email']."' "; 
			$result = mysqli_query($link,$query) or die("Requete pas comprise"); 
			$password=mysqli_fetch_array($result);
			if($_POST['password'] === $password[0]){
				// 1 ligne à ajouter websiteuser
				$query = "SELECT IdUser FROM `user` WHERE email = '".$_POST['email']."'";
				$result = mysqli_query($link,$query) or die("Requete pas comprise 1"); 
				$id = mysqli_fetch_array($result);
				
				$query = " INSERT INTO `websiteuser`(`IdUser`, `DateRegistration`) VALUES ($id[0],'2015-05-06 10:32:14') "; 
				$result = mysqli_query($link,$query) or die("Requete pas comprise 2"); 
			}
			else{
				$errorMessage = 'mot de passe incorrect';
			}
			
			//TODO: vérifier que la personne qui souhaite s'enregistrer à renseigner les mêmes informations que user
		}
       

			/*// On ouvre la session
			session_start();
			// On enregistre le login en session
			$_SESSION['login'] = $_POST['email'];
			// On redirige vers le fichier admin.php
			header('Location: http://localhost:8080');
			exit();*/
		//}
    }
    else
    {
      $errorMessage = 'Veuillez inscrire vos identifiants svp !';
    }
  }
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr">
  <head>
  <link rel="stylesheet" href="css/authentification.css">
    <title>Formulaire d'inscription</title>
  </head>
  <body>
    <form action="<?php echo htmlspecialchars($_SERVER['PHP_SELF']); ?>" method="post">
      <fieldset>
        <legend>Inscrivez-vous</legend>
        <?php
          // Rencontre-t-on une erreur ?
          if(!empty($errorMessage)) 
          {
            echo '<p>', htmlspecialchars($errorMessage) ,'</p>';
          }
        ?>
       <p>
          <label for="nom">Nom :</label> 
          <input type="text" name="nom" id="nom" value="" />
        </p>
        <p>
          <label for="prenom">Prénom :</label> 
          <input type="prenom" name="prenom" id="prenom" value="" /> 
        </p>
		<p>
          <label for="email">Email :</label> 
          <input type="email" name="email" id="email" value="" /> 
        </p>
		<p>
          <label for="password">Mot de passe :</label> 
          <input type="password" name="password" id="password" value="" /> 
        </p>
		<p>
		  <input type="submit" name="submit" value="valider" />
		</p>
      </fieldset>
    </form>
  </body>
</html>