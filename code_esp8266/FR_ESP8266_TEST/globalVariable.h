/**
 * Variables globales.
 */

 /* Cablage des LED */
const int RED_LED_PIN = 0;   // pin sur laquelle est connecté la LED rouge
int redLedState       = LOW;  // L'état courant de la LED ROUGE


/* Pour faire clignoter la LED en fonction de l'erreur */
int delaiClignottementLED = 0;


/* Pour savoir si on a du WIFI ou non */
boolean ConnectionWifiEnabled;



// Déclaration d'une structure allant contenir la configuration du programme.
typedef struct {
  String ssid = "";
  String password = "";
  String IDPoubelle = "";
  boolean InitialisationUsine = false;  // Cette valeur sera lue à partir du fichier des settings.
  boolean AfficheTraceDebug = false;    // Cette valeur sera lue à partir du fichier des settings.
  int calibrationFactor;
} Configuration;
Configuration configLocale; // Initialisation de la configuration locale
