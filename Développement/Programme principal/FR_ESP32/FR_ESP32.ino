/*
 * 
 * Programme  : FOODREST programme principal
 * 
 * Auteurs    : Laurent MARCHAL (UOF), ALexandre PERETJATKO (UOF) Mathieu CARIOU (UOF) Tomo MUROVEC (UOF) Arthur PIGNALET (UOF)
 * 
 * Licence    : CC-BY-SA
 * 
 * Version    : 1.0
 * 
 * Date       :  14/10/2021
 */



/*
   Gestion des mesures de la poubelle connectée.
*/

// Variables globales
#include "globalVariable.h"


// Librairie de gestion des erreurs
#include "GestionErreur.h"

// Librairies pour la lecture de la configuration
#include "Config.h"

// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairie de gestion de la balance
#include "balance.h"

// Librairie de gestion de la balance
#include "RTC.h"

// Librairie de gestion de la saisie de l'ID de la balance
#include "IDpoubelle.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"




// ************************************************************************************
void setup() {
  // initialisation de la liaison série (pour le moniteur)
  Serial.begin(115200);
  delay(200);  // attente pour l'init de la liaison serie

  // Orientation des E/S
  pinMode(LED_PESEE_PIN, OUTPUT);
  pinMode(GND_C_EN, OUTPUT);
  pinMode(MASQUE_RESET, OUTPUT);

  // Prépositionement des E/S
  digitalWrite(LED_PESEE_PIN, HIGH);    // allumage led "pesée en cours"
  digitalWrite(GND_C_EN, HIGH);         // alimentation des périphériques (RTC, balance carte SD)
  digitalWrite(MASQUE_RESET, LOW);      // interdiction du RESET lorsque le capot est ouvert

  //Initialisation du code erreur en mode normal
  code_erreur_normal = ERREUR_NO_ERREUR;

  // Initialisaiton de la balance ---------------------------------------------------------------------
  balance.begin(BALANCE_DAT, BALANCE_CLK);
  balance.power_up();


  // Initialisation de la carte SD ---------------------------------------------------------------------
  // Si on n'arrive pas à initialiser la carte SD
  if ( ! SD_initCard() ) {
    AfficheErreur("ERR (main)> Lecture carte SD impossible!");
    AfficheErreur("Veuillez insérer une carte SD et relancer le programme.");
    digitalWrite(GND_C_EN, LOW);      // coupe l'alimentation des périphériques
    code_erreur_normal = ERREUR_CARTE_SD;
    affichage_erreurs_mode_normale();

  } else {
    // Lecture de la config à partir du fichier sur la carte SD ---------------------------------------
    // ( renseigne le ssid, password, poid, IDPoubelle, etc... )
    configLocale = CONF_lectureConfigurationFromSD();
    TraceDebug("valeurDeTarage");
    TraceDebug(configLocale.valeurDeTarage);

    TraceDebug("laValeurDeTarageInitiale");
    TraceDebug(configLocale.laValeurDeTarageInitiale);


    // si un problème a été détecté sur la carte SD => erreur
    if ( code_erreur_normal == ERREUR_CARTE_SD) {
      affichage_erreurs_mode_normale();
    }



    // On fait un calibrage usine car la variable est positionnée à TRUE dans le fichier --------------
    // de settings
    if ( configLocale.InitialisationUsine ) {


      calibrageUsine();



      // Lecture des mesures ---------------------------------------------------------------------------
      // (on est en mode normal, pas de paramétrage usine à faire)
    } else {

      String Mesures = "";

      // TIMESTAMP
      String timeStamp = rtc_getTimestamp();
      Mesures.concat(timeStamp);

      // Pause de 4 sec pour attendre que la poubelle se stabilise avant de faire une mesure.
      delay(4000);

      // POIDS
      // pesée de la poubelle (valeur calculée)
      balance.set_scale(configLocale.calibrationFactor);
      // float poidNew = BALANCE_getPeseeBalance(configLocale.valeurDeTarage) + (configLocale.valeurDeTarage / configLocale.calibrationFactor);
      float poidNew = BALANCE_getPeseeBalance(configLocale.laValeurDeTarageInitiale);
      float deltaPesee = poidNew - configLocale.poidOld;
      configLocale.poidOld = poidNew;

      // supppression des cas très négatifs (lorsque l'on soulève la poubelle pendant une pesée)
      if (!(poidNew < -(BALANCE_POIDS_SAC + BALANCE_MARGE_POIDS_SAC))) {
  
        if (deltaPesee <0 && abs(poidNew+BALANCE_POIDS_SAC) < BALANCE_MARGE_POIDS_SAC) {
          deltaPesee=0;
          configLocale.poidOld = 0;
        }

        // arrondi des mesures pour supprimer les décimales
        deltaPesee = round(deltaPesee);
        float poidBrute = poidNew;
        poidBrute = round(poidBrute);
        
  
        // Formattage des poids pour TOCIO
        Mesures.concat(formatString(deltaPesee, "-5.0"));
        Mesures.concat(formatString(poidBrute, "-5.0"));
  
        // FORCE DU WIFI
        int rssi = 0; // Sera mis à jour lorsque la connection WIFI sera faite
        String retourTOCIO = "";
  
        // BATTERIE
        int niveauBatteri = niveau_battrie();
        if (niveauBatteri < SEUIL_LOW_BAT) {
          TraceDebug("code_erreur_normal = ERREUR_LOW_BAT");
          code_erreur_normal = ERREUR_LOW_BAT;
        }
        Mesures.concat(formatString(niveauBatteri, "4.0"));
  
        // Connection au WIFI ----------------------------------------------------------------------------
        // On a réussi à se connecter au WIFI
        if ( connectionWifi() ) {
  
          TraceDebug("Voila, c'est fait.");
          TraceDebug("IP : " + WiFi.localIP().toString() );
          TraceDebug("MAC address: " + WiFi.macAddress() );
  
          // Force du signal WIFI
          rssi = WiFi.RSSI();
  
          // Envoie des données vers TOCIO ------------------------------------------------------------
          retourTOCIO = sendDataInHTTPSRequest( Mesures, configLocale );
  
          if ( retourTOCIO != "ok") {
            AfficheErreur("ERR (main)> ERREUR lors de l'envoie vers TOCIO. L'erreur renvoyée est :");
            TraceDebug(retourTOCIO);
  
          } else  {
            TraceDebug("Envoie réussi");
          }
  
          // Pas de connection au Wifi --------------------------------------------------------------------
        } else {
          TraceDebug("Pas de Wifi... code_erreur_normal = ERREUR_WIFI");
          code_erreur_normal = ERREUR_WIFI;
        }
  
        // Ecriture dans le fichier CSV ----------------------------------------------------------------------
        Mesures = configLocale.IDPoubelle + "," + rtc_getDate() + "," + String(deltaPesee, 0) + "," + String(poidBrute, 0) + "," + String(niveauBatteri) + "," + String(rssi) + "," + retourTOCIO;
        SD_writeMesure(configLocale.IDPoubelle, Mesures);
        TraceDebug("Ecriture dans le fichier CSV");
        TraceDebug("Mesures: " + Mesures);
  
        // sauvegarde des settings
        // (pour mémoriser le poid mesuré et éventuellement la nouvelle valeur de tarage)
        SD_EraseSettings();
        SD_WriteSettings(configLocale);
      }
 
    }
    
  }

  
  // Sortie "propre"
  digitalWrite(MASQUE_RESET, HIGH); // autorise le RESET lorsque le capot est ouvert
  digitalWrite(GND_C_EN, LOW);      // coupe l'alimentation des périphériques
  digitalWrite(LED_PESEE_PIN, LOW); // extinction de la led "pesée en cours"
  balance.power_down();             // Put the ADC in Sleep mode

  // Affichage des erreurs
  affichage_erreurs_mode_normale();

  // Passage en deep sleep
  esp_deep_sleep_start();
}



void loop() {
}
