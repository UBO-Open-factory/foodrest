#include <SPI.h>
#include <SD.h>
#include <SDConfigFile.h>

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
boolean init_SDCard() {

  // La pin CS de la carte doit être branchée sur le 3.3 V de la carte, peut importe la
  // valeur qui lui est attribuée ici.
  if (!SD.begin(0)) { // Fausse valeur, pour que la fonction SD.bgin() puisse passer
    AfficheErreur("ERR (init_SDCard)> Initialisation carte SD impossible.");
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------------------------------
/**
   Lecture du fichier de configuraiton de l'application.
   Si le fichier n'est pas présent sur la carte, on en cré un vierge sous un nom différent. Cela évite d'écraser
   par erreur le fichier en cas de mauvaise lecture.
   Renvoie true si la config est lue, false sinon.
*/
boolean SD_Read_Config(Configuration &myConfig) {

  // Le fichier de configuration
  SDConfigFile cfg;

  //Si le fichier existe on le lit
  if (cfg.begin("settings.txt", CONFIG_LINE_LENGTH)) {

    // Lecture de la config du fichier
    while (cfg.readNextSetting()) {

      // Lecture de l'ID de la poubelle
      if (cfg.nameIs("IDPoubelle")) {
        myConfig.IDPoubelle = cfg.copyValue();      // Get String

        // Lecture du fait si on doit faire un calibrage usine du programme
      } else if (cfg.nameIs("InitialisationUsine")) {
        myConfig.InitialisationUsine = cfg.getBooleanValue();    // Get boolean

        // Lecture si on doit afficher les trace dans le terminal serie
      } else if (cfg.nameIs("AfficheTraceDebug")) {
        myConfig.AfficheTraceDebug = cfg.getBooleanValue();    // Get boolean
        
        // Lecture de la valeur de calibration de la ballance
      } else if (cfg.nameIs("calibrationFactor")) {
        myConfig.calibrationFactor = cfg.getIntValue(); // Get integer
      }
    }


    // le fichier n'existe pas, on le créé à vide
  } else {

    AfficheErreur("ERR SD_Read_Config > Fichier settings.txt introuvable. Creation d'un vierge qu'il faut initialiser.");

    // Ouverture du fichier en écriture
    File myFile = SD.open("settings_tempo.txt", FILE_WRITE);
    myFile.println("// Ceci est le fichier de configuration pour l'application.");
    myFile.println("// Il faut renommer ce fichier avec le nom settings.txt");

    myFile.println();
    myFile.println("// Identifiant de la poubelle tel que défini dans le Back Office TOCIO.");
    myFile.println("IDPoubelle=TEST2");
    myFile.println();

    myFile.println("// Pour pouvoir faire la calibrage d'usine ce paramètre doit être à true (false par défault).");
    myFile.println("InitialisationUsine=false");
    myFile.println();
    
    myFile.println("// Pour Afficher les traces du programme dans un terminal lorsque la carte est branché à un PC.");
    myFile.println("AfficheTraceDebug=true");
    myFile.println();

    myFile.println("// Calibrage de la ballance (doit être une valeur entière positive ou négative).");
    myFile.println("calibrationFactor=0000");

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
boolean SD_Read_Wifi(Configuration &myConfig) {


  // Le fichier de configuration
  SDConfigFile cfg;

  //Si le fichier existe on le lit
  if (cfg.begin("wifi.txt", CONFIG_LINE_LENGTH)) {

    // Lecture de la config du fichier
    while (cfg.readNextSetting()) {

      // Lecture du SSID
      if (cfg.nameIs("ssid")) {

        // Get value as string
        myConfig.ssid = cfg.copyValue();

        // Lecture du Mot de passe
      } else if (cfg.nameIs("mdp")) {

        // Dynamically allocate a copy of the string.
        myConfig.password = cfg.copyValue();

      }
    }


    // le fichier n'existe pas, on le créé à vide
  } else {

    AfficheErreur("ERR SD_Read_Wifi > Fichier wifi.txt introuvable. Creation d'un vierge qu'il faut initialiser.");

    // Ouverture du fichier en écriture
    File myFile = SD.open("wifi_tempo.txt", FILE_WRITE);
    myFile.println("// Ceci est le fichier de configuration pour les accès WIFI.");
    myFile.println("// Il faut renommer ce fichier avec le nom wifi.txt");

    myFile.println();
    myFile.println("ssid=LeNomDuReseauWifi");
    myFile.println("mdp=MotDePassWifi");

    return false;
  }

  // clean up
  cfg.end();
  return true;
}
