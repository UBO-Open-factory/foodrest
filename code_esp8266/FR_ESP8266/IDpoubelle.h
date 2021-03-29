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
  Serial.println("");
  Serial.println("Entrez l'identifiant de la poubelle tel que défini dans le Back Office de TOCIO");
  Serial.println("puis la touche [Enter].");
  Serial.println("");

  // Lecture de l'ID saisie au clavier
  while (!Serial.available());
  configLocale.IDPoubelle = Serial.readString();

  // Effacement préventif du fichier wifi
  SD_EraseWifiFile();

  // Ecriture du fichier
  SD_WriteWifi(configLocale);
}
