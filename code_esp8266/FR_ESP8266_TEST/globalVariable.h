/**
 * Variables globales.
 */
/* BALANCE *********************************************************************************** */
// lignes connectées à la balance
#define BALANCE_CLK  2
#define BALANCE_DAT  10

// variables à étalonner avant la production
int GLOBAL_nb_echantillons_tarage = 30;
int GLOBAL_nb_echantillons_mesure = 30;
float GLOBAL_precision_calibration = 0.3;



 /* Cablage des LED *************************************************************************** */
const int RED_LED_PIN = 0;   // pin sur laquelle est connecté la LED rouge
int redLedState       = LOW;  // L'état courant de la LED ROUGE





/* BATTERIE *********************************************************************************** */
//ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)
#define pin_batt A0 





/* Pour faire clignoter la LED en fonction de l'erreur */
int delaiClignottementLED = 0;


/* Déclaration d'une structure allant contenir la configuration du programme ****************** */
typedef struct {
  String ssid = "";        // Cette valeur sera lue à partir du fichier de config WIFI
  String password = "";     // Cette valeur sera lue à partir du fichier de config WIFI
  String IDPoubelle = "";   // Cette valeur sera lue à partir du fichier de config WIFI
  boolean InitialisationUsine = false;  // Cette valeur sera lue à partir du fichier des settings.
  boolean AfficheTraceDebug = false;    // Cette valeur sera lue à partir du fichier des settings.
  int calibrationFactor = 0;      // Cette valeur sera lue à partir du fichier des settings.
  float valeurDeTarage = 0;       // Cette valeur sera lue à partir du fichier des settings.
} Configuration;
Configuration configLocale; // Initialisation de la configuration locale


/* TOCIO *************************************************************************************** */
// URL pour stocker le smesures dans le Back Office de TOCIO.
const String url  = "/cad/foodrest/backoffice/mesure/add/";
