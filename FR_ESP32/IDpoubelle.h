/**
   Fonctions pour l'initialisation de l'ID de la poubelle.
*/


// ---------------------------------------------------------------------------------------------------
/**
   Lecture dans le terminal Série de l'ID poubelle et sauvegarde dans le fihcier de configuration (le fichier wifi.txt).
   Cette fonction est appellée lors du paramétrage usine.
*/
void IDPoubelle_setup() {
  Serial.println("");
  Serial.println("*************************************************");
  Serial.println("*     FOOD REST : Saisie de l'ID de la poubelle *");
  Serial.println("*************************************************");
  Serial.println("Entrez l'identifiant de la poubelle tel que défini dans le Back Office de TOCIO");
  Serial.println("puis la touche [Enter].");
  Serial.println("");

  // Lecture de l'ID saisie au clavier
  while (!Serial.available());
  configLocale.IDPoubelle = Serial.readStringUntil('\n');

  // Effacement préventif du fichier des settings
  SD_EraseSettings();

  // Ecriture du fichier
  SD_WriteSettings(configLocale);

  // Création des entes du fichier CSV
  /**
    Il ne faut pas faire de suppression préventive du fichier car on pourait supprimer des
    données -> Il vaut mieux avoir plusieurs fois les entetes de colonne dans le fichier plutôt
    que de perdre des données.
  */
  String file = SD_writeEntesMesure();
  if ( file != "" ) {
    TraceDebug("Initialisation du fichier des mesures faite dans :" + file);
  } else {
    TraceDebug("Impossible d'ouvir le fichier " + file);
  }

}
