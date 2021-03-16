/**
   Librairies pour la lecture du fichier de configuration.
   Le fichier doit être sous la forme JSON, c'est à dire.
*/

// Déclaration d'une structure allant contenir la configuration du programme.
typedef struct {
  String ssid = "";
  String password = "";
  String IDPoubelle = "";
  boolean InitialisationUsine = false;
  int calibrationFactor;
} Configuration;



// Librairie pour les entrées/sortiues sur la carte SD
#include "SDCard.h";



// -------------------------------------------------------------------------------------
/**
   Lecture des fichiers de configuration à partir de la carte SD.
   wifi.txt -> Les paramètres de connection au réseau WIFI.
   params.txt -> Les paramètres de config de la carte.
*/
Configuration lectureConfigurationFromSD() {
  // Définition d'une structure pour la configuration
  Configuration conf;

  // Lecture du fichier de paramétrage Wifi
  // et initialisation du ssid et du password WIFI
  if ( !SD_Read_Wifi( conf )) {
    AfficheErreur("ERR lectureConfigurationFromSD> Impossible de lire la conf WIFI.");
  }


  // Lecture du fichier de configuration du programme
  if ( !SD_Read_Config( conf )) {
    AfficheErreur("ERR lectureConfigurationFromSD> Impossible de lire les settings.");
  }

  return conf;
}
