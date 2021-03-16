/**
   Gestion des mesures de la poubelle connectée.
*/

// Librairie de gestion des erreurs
#include "GestionErreur.h"

// Librairies pour la lecture de la configuration
#include "Config.h"
Configuration configLocale;

// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"



// ************************************************************************************
boolean ConnectionWifiEnabled = true;
void setup() {
  // initialisation de la liaison série (pour le moniteur)
  Serial.begin(115200);
  TraceDebug("OK, let's go");

  // Initialisation de la carte SD --------------------------------------------------------
  init_SDCard();



  // Lecture de la config à partir du fichier sur la carte SD -----------------------------
  // ( renseigne le ssid, password, poid, IDPoubelle
  configLocale = lectureConfigurationFromSD();
  /*
    Serial.println("configLocale : ");
    Serial.println(configLocale.ssid);
    Serial.println(configLocale.password);
    Serial.println(configLocale.IDPoubelle);
    Serial.println(configLocale.calibrationFactor);
  */

  // On fait un calibrage usine car la variable est positionnée à TRUE dans le fichier
  // de config.
  if ( configLocale.InitialisationUsine ) {
    calibrageUsine();
  }



  // Connection au WIFI
  ConnectionWifiEnabled = connectionWifi();
  if ( ConnectionWifiEnabled ) {
    TraceDebug("Voila, c'est fait.");
    TraceDebug("MAC address: "+ WiFi.macAddress() );
  }

  // Initialisation de la balance
  //setup_CZL635_20();

  // Initialisation de l'horloge
  setup_rtc_pcf8523();

}



// ************************************************************************************
void loop() {
  // Concatenation des mesures .............................
  String Mesures = "";

  // timestamp0 is the 'Timestamp' value from your sensor 'RTC_Timestamp' (as float)
  Mesures.concat(rtc_timestamp());

  // poid1 is the 'Poid' value from your sensor 'CZL635-20' (as float)
  //Mesures.concat(formatString( mesure_poid(), "-5.0"));
  Mesures.concat(mesure_poid());

  // valeur analogique(0 a 1023)2 is the 'Valeur analogique(0 à 1023)' value from your sensor 'CAN (1024)' (as float)
  Mesures.concat(formatString(niveau_battrie(), "4.0"));

  // Envoie des données vers TOCIO .........................
  if ( ConnectionWifiEnabled ) {
    String retour = sendDataInHTTPSRequest( Mesures, configLocale );
    if ( retour != "ok") {
      Serial.println("*** ERREUR lors de l'envoie de la requette *********");
      Serial.println(retour);
    } else  {
      Serial.println("Envoie réussi");
    }
  }


  // Pause .................................................
  delay(60 * 1000); // 10 secondes
}
