<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="fr" >
   <head>
       <title>Bienvenue sur mon formulaire</title>
       <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
   </head>
   <body>
   <p>
   <form method="post" action="secret.php" /> 
  Mot de passe <input type="text" name="motdepasse"/>
  <input type="submit" value="valider"/>
  
</form>
</p>
<?php
for ($i=5;$i<=21;$i++)
{
	echo 'numero: ' . $i .  '<br/>';
}
?>
 </body>
  
  
  
