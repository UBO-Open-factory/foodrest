/**
 * Variables globales.
 */
#include <SPI.h>

/* BALANCE *********************************************************************************** */
// lignes connectées à la balance
#define BALANCE_CLK  26
#define BALANCE_DAT  27
#define BALANCE_NB_ECHANTILLONS_TARAGE 100   // passage de 20 à 100 => 
#define BALANCE_NB_ECHANTILLONS_PESEE 100

 /* Cablage des LED *************************************************************************** */
// pin sur laquelle est connecté la LED rouge
#define LED_PESEE_PIN 5
int redLedState     = LOW;  // L'état courant de la LED ROUGE


 /* Autorisation du reset *************************************************************************** */
#define MASQUE_RESET 13


/* BATTERIE *********************************************************************************** */
//ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)
#define pin_batt A0
#define GND_C_EN 25
#define SEUIL_LOW_BAT 1860 // equivalent à 3,35V environ.
                            // 4,2V => 2373
                            // 3,3V => 1831


/* CODES D'ERREUR  *********************************************************************************** */
#define ERREUR_NO_ERREUR  0
#define ERREUR_WIFI       1
#define ERREUR_LOW_BAT    2
#define ERREUR_CARTE_SD   3

int code_erreur_normal = ERREUR_NO_ERREUR;

int periode_led_erreur_wifi     =   200; // en ms. période pour un clignotement isophase
int periode_led_erreur_sd       =  1500; // en ms. période pour un clignotement isophase
int periode_led_erreur_low_bat =  2500; // en ms. période pour un clignotement isophase

int duree_clignotement_erreur   = 10000;  // en ms durée pendant laquelle on affiche une erreur (clignotement led pesée) avant de repasser en deep sleep


/* Déclaration d'une structure allant contenir la configuration du programme ****************** */
typedef struct {
  char* ssid = "";          // Cette valeur sera lue à partir du fichier de config WIFI
  char* password = "";      // Cette valeur sera lue à partir du fichier de config WIFI
  String IDPoubelle = "";   // Cette valeur sera lue à partir du fichier de config WIFI
  boolean InitialisationUsine = false;  // Cette valeur sera lue à partir du fichier des settings.
  boolean AfficheTraceDebug = false;    // Cette valeur sera lue à partir du fichier des settings.
  float calibrationFactor = 110;        // Cette valeur sera lue à partir du fichier des settings.
  float valeurDeTarage = 1;             // Cette valeur sera lue à partir du fichier des settings.
  float poidOld =0;                   // poids de la dernière pesée pour calculer le différentiel
} Configuration;
Configuration configLocale; // Initialisation de la configuration locale


/* TOCIO *************************************************************************************** */
// URL pour stocker le smesures dans le Back Office de TOCIO.
const String url  = "/cad/foodrest/backoffice/mesure/add/";
