#include <SDConfigFile.h>
#include "FS.h"
#include "SPI.h"


/*
    Length of the longest line expected in the config file.
    The larger this number, the more memory is used
    to read the file.
    You probably won't need to change this number.
*/
const uint8_t CONFIG_LINE_LENGTH = 127;



// ---------------------------------------------------------------------------------------------------
/**
   Initialisaiton de la carte SD.
*/
boolean SD_initCard() {
  // La pin CS de la carte doit être branchée sur le 3.3 V de la carte, peut importe la
  // valeur qui lui est attribuée ici.
  if (!SD.begin(0)) { // Fausse valeur, pour que la fonction SD.begin() puisse passer
    AfficheErreur("ERR (init_SDCard)> Initialisation carte SD impossible.");
    return false;
  }

  // Détection si on a une carte SD insérée
  if (SD.cardType() == CARD_NONE) {
    Serial.println("No SD card attached");
    AfficheErreur("ERR (init_SDCard)> Aucune Carte SD insérée.");
    return false;
  }

  return true;
}



// ---------------------------------------------------------------------------------------------------
/**
   Permet d'écrire en toute lettres TRUE ou FLASE en ffdonction de la valeur du boolean passé en paramètre.
*/
String _convertionBoolean(boolean value) {
  if ( value ) {
    return "true";
  } else {
    return "false";
  }
}


// ---------------------------------------------------------------------------------------------------
/**
   Suppression (préventive) du fichier de settings.
   Pour pouvoir sauver une valeur dans le fichier, il est nécessaire de le supprimer et de le réecrire entièrement.
*/
void SD_EraseSettings() {
  SD.remove("/settings.txt");
}
// ---------------------------------------------------------------------------------------------------
/**
   Suppression (préventive) du fichier de configuration WIFI.
   Pour pouvoir sauver une valeur dans le fichier, il est nécessaire de le supprimer et de le réecrire entièrement.
*/
void SD_EraseWifiFile() {
  SD.remove("/wifi_tempo.txt");
}

// ---------------------------------------------------------------------------------------------------
/**
   Ecriture du fichier de settings avec les valeurs de la configuration locale.

*/
void SD_WriteSettings(Configuration &myConfig) {
  const char* fileName = "/settings.txt";

  // Ouverture du fichier en écriture (et réiniitialisation)
  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    // Ecrit les données dans le fichier.
    myFile.println("// Ceci est le fichier de configuration pour l'application.");
    myFile.println();

    myFile.println("// Pour pouvoir faire la calibrage d'usine ce paramètre doit être à true (false par défault).");
    myFile.println("InitialisationUsine=" + _convertionBoolean(myConfig.InitialisationUsine) );
    myFile.println();

    myFile.println("// Pour Afficher les traces du programme dans un terminal lorsque la carte est branchée à un PC.");
    myFile.println("AfficheTraceDebug=" + _convertionBoolean(myConfig.AfficheTraceDebug) );
    myFile.println();

    myFile.println("// Valeur de tarage de la balance.");
    myFile.println("valeurDeTarage=" + String(myConfig.valeurDeTarage) );
    myFile.println();

    myFile.println("// Calibrage de la balance (doit être une valeur entière positive ou négative).");
    myFile.println("calibrationFactor=" + String(myConfig.calibrationFactor) );
    myFile.println();

    myFile.println("// Identifiant de la poubelle tel que défini dans TOCIO.");
    myFile.println("IDPoubelle=" + String(myConfig.IDPoubelle) );
    myFile.println();

    myFile.println("// Pour Afficher les traces du programme dans un terminal lorsque la carte est branchée à un PC.");
    myFile.println("poidOld=" + String(myConfig.poidOld) );
    myFile.println();

  } else {
    AfficheErreur("ERREUR : SDCard.h SD_WriteSettings> Impossible d'ouvrir le fichier '" + String(fileName) + "' sur la carte SD pour écrire dedans");
  }
}

// ---------------------------------------------------------------------------------------------------
/**
   Ecriture du fichier de paramètrage WIFI avec les valeurs de la configuration locale.

*/
void SD_WriteWifi(Configuration &myConfig) {
  const char* fileName = "/wifi_tempo.txt";

  // Ouverture du fichier en écriture (et réinitialisation)
  File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    // Ouverture du fichier en écriture
    myFile.println("// Ceci est le fichier de configuration pour les accès WIFI.");
    myFile.println("// Il faut renommer ce fichier avec le nom wifi.txt");
    myFile.println();
    myFile.println("// Configuration WIFI");
    myFile.println("ssid=LeNomDuReseauWifi");
    myFile.println("mdp=MotDePassWifi");
  } else {
    AfficheErreur("ERREUR : SDCard.h SD_WriteSettings> Impossible d'ouvrir le fichier '" + String(fileName) + "' sur la carte SD pour écrire dedans");
  }
}





// ---------------------------------------------------------------------------------------------------
/**
   Lecture du fichier de configuraiton de l'application.
   Si le fichier n'est pas présent sur la carte, on en cré un vierge sous un nom différent. Cela évite d'écraser
   par erreur le fichier en cas de mauvaise lecture.
   Renvoie true si la config est lue, false sinon.
*/
boolean SD_Read_Config(Configuration &myConfig) {
  const char* fileName = "/settings.txt";

  // Le fichier de configuration
  SDConfigFile cfg;

  //Si le fichier existe on le lit
  if (cfg.begin(fileName, CONFIG_LINE_LENGTH)) {

    // Lecture de la config du fichier
    while (cfg.readNextSetting()) {

      // Lecture du fait si on doit faire un calibrage usine du programme
      if (cfg.nameIs("InitialisationUsine")) {
        myConfig.InitialisationUsine = cfg.getBooleanValue();    // Get boolean

        // Lecture si on doit afficher les trace dans le terminal serie
      } else if (cfg.nameIs("AfficheTraceDebug")) {
        myConfig.AfficheTraceDebug = cfg.getBooleanValue();    // Get boolean

        // Lecture de la valeur de calibration de la balance
      } else if (cfg.nameIs("calibrationFactor")) {
        myConfig.calibrationFactor = cfg.getIntValue(); // Get integer

        // Lecture de la valeur de tarage de la balance
      } else if (cfg.nameIs("valeurDeTarage")) {
        myConfig.valeurDeTarage = cfg.getIntValue(); // Get integer

        // lecture de l'identifiant poubelle
      } else if (cfg.nameIs("IDPoubelle")) {
        myConfig.IDPoubelle = cfg.copyValue();      // Get String

        // lecture de la dernière pesée
      } else if (cfg.nameIs("poidOld")) {
        myConfig.poidOld = cfg.getIntValue();      // Get String

      }
    }

    // le fichier n'existe pas, on le créé à vide
  } else {

    // Ecriture d'un fichier de config vierge
    AfficheErreur("ERR (SD_Read_Config)> Fichier " + String(fileName) + " introuvable. Creation d'un vierge qu'il faut initialiser.");
    SD_WriteSettings(myConfig);
    // affiche erreur sur carte Wifi
    code_erreur_normal = ERREUR_CARTE_SD;
    Serial.println ("Fichiers de configuration inexistant");


    return false;
  }

  // clean up
  cfg.end();
  return true;
}



// ---------------------------------------------------------------------------------------------------
/**
   Lecture du fichier de configuration du Wifi.
   Si le fichier n'est pas présent sur la carte, on en cré un vierge sous un nom différent. Cela évite d'écraser
   par erreur le fichier en cas de mauvaise lecture.
   Renvoie true si la conifg est lue, false sinon.
*/
boolean SD_Read_Wifi(Configuration & myConfig) {
  const char* fileName = "/wifi.txt";

  // Le fichier de configuration
  SDConfigFile cfg;

  //Si le fichier existe on le lit
  if (cfg.begin(fileName, CONFIG_LINE_LENGTH)) {

    // Lecture de la config du fichier
    while (cfg.readNextSetting()) {
      // Lecture du SSID
      if (cfg.nameIs("ssid")) {
        myConfig.ssid = cfg.copyValue();   // Get value as string

        // Lecture du Mot de passe
      } else if (cfg.nameIs("mdp")) {
        myConfig.password = cfg.copyValue();// Dynamically allocate a copy of the string.

      }
    }


    // le fichier n'existe pas, on le créé à vide
  } else {

    AfficheErreur("ERR (SD_Read_Wifi)> Fichier " + String(fileName) + " introuvable. Creation d'un vierge qu'il faut initialiser.");
    // affiche erreur sur carte Wifi
    code_erreur_normal = ERREUR_CARTE_SD;
    Serial.println ("Fichiers de configuration vide");

    // Effacement préventif du fichier wifi
    SD_EraseWifiFile();

    // Ecriture du fichier
    SD_WriteWifi(myConfig);

    return false;
  }

  // clean up
  cfg.end();
  return true;
}


// ---------------------------------------------------------------------------------------------------
/**
   Ecriture d'une ligne dans le fichier des mesures sur la carte SD.

   IDPoubelle : L'identificatn de la poubelle tel que défini dans le BO TOCIO.
   chaine : La chaine de caractère à écrire dans le fichier.
*/
void SD_writeMesure(String IDPoubelle, String chaine) {
  // Ouverture du fichier en ajout
  File dataFile = SD.open("/" + IDPoubelle + "-mesures.csv", FILE_APPEND);

  // Si on a réussi à ouvrir le fichier en écriture
  if (dataFile) {
    // Ecriture de la ligne dans le fichier
    dataFile.println(chaine);

    // Fermeture du fichier
    dataFile.close();

  } else {
    AfficheErreur("ERR (write_Mesure)> Impossible d'ouvir le fichier en écriture sur la carte SD.");
  }
}


// ---------------------------------------------------------------------------------------------------
/**
   Ecriture des entetes du fichier CSV des mesures.

   Renvoie "" si init impossible sinon le nom du fichier.
*/
String SD_writeEntesMesure() {
  String fileName = "/" + configLocale.IDPoubelle + "-mesures.csv";
  File dataFile = SD.open(fileName, FILE_APPEND);

  // Si on a réussi à ouvrir le fichier en écriture
  if (dataFile) {
    // Ecriture de la ligne dans le fichier
    dataFile.println("IDPoubelle,Date,Poids Ajouté,Niveau batterie,RSSI,Réception TOCIO");

    // Fermeture du fichier
    dataFile.close();
    return fileName;

  } else {
    AfficheErreur("ERR (write_Mesure)> Impossible d'ouvir le fichier en écriture sur la carte SD.");
    return "";
  }
}
