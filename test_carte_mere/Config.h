/**
   Librairies pour la lecture du fichier de configuration.
   Le fichier doit être sous la forme JSON, c'est à dire.
*/

// Librairie pour les entrées/sorties sur la carte SD
#include "SDCard.h";



// -------------------------------------------------------------------------------------
/**
   Lecture des fichiers de configuration à partir de la carte SD.
   wifi.txt -> Les paramètres de connection au réseau WIFI.
   params.txt -> Les paramètres de config de la carte.
*/
Configuration CONF_lectureConfigurationFromSD() {
  // Définition d'une structure pour la configuration
  Configuration conf;

  // Lecture du fichier de configuration du programme
  if ( !SD_Read_Config( conf )) {
    AfficheErreur("ERREUR : Impossible de lire les settings.");
  }
  
  // Lecture du fichier de paramétrage Wifi
  // et initialisation du ssid et du password WIFI
  if ( !SD_Read_Wifi( conf )) {
    AfficheErreur("ERREUR : Impossible de lire la conf WIFI.");
  }

  return conf;
}
