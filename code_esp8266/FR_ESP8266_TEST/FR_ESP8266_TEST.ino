/**
   Gestion des mesures de la poubelle connectée.
   Principe des mesure !

 A FAIRE
*/

// Librairies pour la lecture de la configuration
#include "Config.h"
Configuration configLocale;

// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"



// ************************************************************************************
void setup() {
  // initialisation de la liason série (pour le moniteur)
  Serial.begin(115200);
  Serial.println("OK, let's go");

  // Lecture de la config à partir du fichier sur la carte SD -----------------------------
  // ( renseigne le ssid, password, poid, IDPoubelle
  configLocale = lectureConfigurationFromSD();


  // On fait un calibrage usine car la variable est positionnée à TRUE dans le fichier
  // de config.
  if ( configLocale.InitialisationUsine ) {
    calibrageUsine();
  }


  // Connection WIFI ----------------------------------------------------------------------
  WiFi.mode(WIFI_STA);
  // WiFi.begin(configLocale.ssid, configLocale.password);
  WiFi.begin("congres", "sufca!2019!dsiun");
  Serial.println();
  Serial.print("Connection au réseau WIFI en cours ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Voila, c'est fait.");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Serial.print("MAC address: "); Serial.println(WiFi.macAddress());

  // Initialisation de la balance
  //setup_CZL635_20();

  // Initialisation de l'horloge
  setup_rtc_pcf8523();

}



// ************************************************************************************
void loop() {
  // Concatenation des mesures .............................
  String Mesures = "";
  Serial.println("debug1");
  // timestamp0 is the 'Timestamp' value from your sensor 'RTC_Timestamp' (as float)
  Mesures.concat(rtc_timestamp());
  Serial.println("debug2");
  // poid1 is the 'Poid' value from your sensor 'CZL635-20' (as float)
  //Mesures.concat(formatString( mesure_poid(), "-5.0"));
  Mesures.concat(mesure_poid());
  Serial.println("debug3");
  // valeur analogique(0 a 1023)2 is the 'Valeur analogique(0 à 1023)' value from your sensor 'CAN (1024)' (as float)
  Mesures.concat(formatString(niveau_battrie(), "4.0"));
  Serial.println("debug4");
  // Envoie des données vers TOCIO .........................
  String retour = sendDataInHTTPSRequest( Mesures, configLocale );
  if ( retour != "ok") {
    Serial.println("*** ERREUR lors de l'envoie de la requette *********");
    Serial.println(retour);
  } else  {
    Serial.println("Envoie réussi");
  }



  // Pause .................................................
  delay(60 * 1000); // 10 secondes
}
