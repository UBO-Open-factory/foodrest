/**
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
  balance.set_scale();

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


    // si un problème a été détecté sur la carte SD => erreur
    if ( code_erreur_normal == ERREUR_CARTE_SD) {
      affichage_erreurs_mode_normale();
    }





    // On fait un calibrage usine car la variable est positionnée à TRUE dans le fichier --------------
    // de settings
    if ( configLocale.InitialisationUsine ) {
      TraceDebug("On entre en mode calibration d'usine.");

      // Afichage d'un message d'attente pour laisser le temps au port USB de "capter" la balance
      while (true) {
        Serial.println ("Veuillez presser une touche pour entrer dans le mode usine...");
        if (Serial.available()) {
          while (Serial.available()) {
            Serial.read();
          }
          break;
        }
        delay(1000);
      }
      calibrageUsine();



      // Lecture des mesures ---------------------------------------------------------------------------
      // (on est en mode normal, pas de paramétrage usine à faire)
    } else {

      String Mesures = "";

      // TIMESTAMP
      String timeStamp = rtc_getTimestamp();
      Mesures.concat(timeStamp);

      // POIDS
      // pesée de la poubelle
      balance.set_scale(configLocale.calibrationFactor);
      float poidNew = BALANCE_pesee_balance() + (configLocale.valeurDeTarage / configLocale.calibrationFactor);
      float deltaPesee = poidNew - configLocale.poidOld;

      // La poubelle a été vidée
      if (abs(poidNew) <= BALANCE_getMargeErreurVidange(configLocale.poidOld)) {
        deltaPesee = 0;
        configLocale.poidOld = 0;
        // on retare la balance
        configLocale.valeurDeTarage = BALANCE_setTare();

        // on a enlevé une partie du contenu de la poubelle et que l'on ne l'a pas vidée complètement
      } else {
        configLocale.poidOld = poidNew;
      }

      // Formattage du poid pour TOCIO
      Mesures.concat(formatString(deltaPesee, "-5.0"));

      // FORCE DU WIFI
      int rssi = 0; // Sera mis à jour lorsque la connecction sera faite
      String retourTOCIO = "";

      // BATTERIE
      int niveauBatteri = niveau_battrie();
      Serial.print ("Niveau batterie : ");
      Serial.println (niveauBatteri);
      if (niveauBatteri < SEUIL_LOW_BAT) {
        Serial.println ("code_erreur_normal = ERREUR_LOW_BAT");
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
        Serial.println ("Pas de Wifi...");
        Serial.println ("code_erreur_normal = ERREUR_WIFI");
        code_erreur_normal = ERREUR_WIFI;
      }

      // Ecriture dans le fichier CSV ----------------------------------------------------------------------
      Mesures = configLocale.IDPoubelle + "," + rtc_getDate() + "," + String(deltaPesee, 0) + "," + String(niveauBatteri) + "," + String(rssi) + "," + retourTOCIO;
      SD_writeMesure(configLocale.IDPoubelle, Mesures);
      TraceDebug("Ecriture dans le fichier CSV");
      TraceDebug("Mesures: " + Mesures);

      // sauvegarde des settings pour mémoriser le poid mesuré
      SD_EraseSettings();
      SD_WriteSettings(configLocale);


    }
  }
  // Sortie "propre"
  digitalWrite(MASQUE_RESET, HIGH); // autorise le RESET lorsque le capot est ouvert
  digitalWrite(GND_C_EN, LOW);      // coupe l'alimentation des périphériques
  digitalWrite(LED_PESEE_PIN, LOW); // extinction de la led "pesée en cours"
  affichage_erreurs_mode_normale();

}



void loop() {



}
