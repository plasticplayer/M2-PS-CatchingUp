<?php
        session_start();
		
  // Definition des constantes et variables
  //define('LOGIN','toto');
  //define('PASSWORD','tata');
  $errorMessage = '';
  
 
  // Test de l'envoi du formulaire
  if(!empty($_POST)) 
  {
    // Les identifiants sont transmis ?
    if(!empty($_POST['login']) && !empty($_POST['password'])) 
    {
		require("config.php");
		if(isset($_SESSION['login']))
			unset($_SESSION['login']);
		$password = addslashes(md5($_POST['password'].$grainDeSel));
		$link = mysqli_connect($databaseHost,$databaseUser,$databasePass); 
		if (!$link) { 
			die('Could not connect to MySQL: ' . mysqli_error()); 
		 } 
		 //echo 'Connection OK'; //mysql_close($link); 
		$connection=mysqli_select_db($link,$databaseName);

		$query = " SELECT WebsiteUser.IdUser,Email,Password FROM User, WebsiteUser WHERE Email ='".$_POST['login']."' and WebsiteUser.IdUser = User.IdUser "; 
		$result = mysqli_query($link,$query) or die(mysqli_error($link)); 
		$row =mysqli_fetch_assoc($result);
		$email=$row['Email'];
		$idUser=$row['IdUser'];
		$password=$row['Password'];

		$password=mysqli_fetch_array($result);
		
		if($email[0] === NULL){
			$errorMessage = 'Mauvais login !';
		}
		elseif($pass!==$password[0]){
			$errorMessage = 'Mauvais password !';
		}
        else
		{
			// On ouvre la session

			// On enregistre le login en session
			$_SESSION['login'] = $idUser;
			// On redirige vers le fichier admin.php
			header('Location: /');
			exit();
		}
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
    <title>Formulaire d'authentification</title>
  </head>
  <body>
    <form action="<?php echo htmlspecialchars($_SERVER['PHP_SELF']); ?>" method="post">
      <fieldset>
        <legend>Identifiez-vous</legend>
        <?php
          // Rencontre-t-on une erreur ?
          if(!empty($errorMessage)) 
          {
            echo '<p>', htmlspecialchars($errorMessage) ,'</p>';
          }
        ?>
      <p>
          <label for="login">Email :</label> 
          <input type="text" name="login" id="login" value="" />
        </p>
        <p>
          <label for="password">Mot de passe :</label> 
          <input type="password" name="password" id="password" value="" /> 
        </p>
		<p>
			<input type="submit" name="submit" value="Valider" />
		</p>
      </fieldset>
    </form>
  </body>
</html>