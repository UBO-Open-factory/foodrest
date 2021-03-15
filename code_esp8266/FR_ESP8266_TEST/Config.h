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


 

// -------------------------------------------------------------------------------------
/**
   Lecture du fichier de configuration à partir de la carte SD.
   @TODO
*/
Configuration lectureConfigurationFromSD() {
  
  // Définition d'une structure pour la configuration
  Configuration conf;

  // Initialisation de la structure à partir du fichier de la carte SD @todo
  conf.ssid       = "congres";
  conf.password   = "sufca!2019!dsiun";
  conf.IDPoubelle = "TEST_2";
  conf.InitialisationUsine = false;
  conf.calibrationFactor   = -7050;

  return conf;
}
