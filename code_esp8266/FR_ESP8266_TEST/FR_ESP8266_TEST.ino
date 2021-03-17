/**
   Gestion des mesures de la poubelle connectée.
*/
/* Cablage des LED */
const int RED_LED_PIN = 0;   // pin sur laquelle est connecté la LED rouge
int redLedState       = LOW;  // L'état courant de la LED ROUGE

/* Pour savoir si on a du WIFI ou non */
boolean ConnectionWifiEnabled = true;



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
void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  // initialisation de la liaison série (pour le moniteur)
  Serial.begin(115200);
  TraceDebug("OK, let's go");

  // Extinction de la LED rouge ----------------------------------------------------------- ROUGE OFF
  digitalWrite(RED_LED_PIN, LOW);


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

  // On fait un calibrage usine car la variable est positionnée à TRUE dans le fichier ----
  // de config.
  if ( configLocale.InitialisationUsine ) {
    TraceDebug("On entre en mode calibration d'usine.");
    calibrageUsine();

    // Mode normal ------------------------------------------------------------------------
  } else {



    // Connection au WIFI
    ConnectionWifiEnabled = connectionWifi();

    // On a réussi à se connecter au WIFI
    if ( ConnectionWifiEnabled ) {
      // Extinction de la LED rouge ----------------------------------------------------------- ROUGE OFF
      digitalWrite(RED_LED_PIN, LOW);

      TraceDebug("Voila, c'est fait.");
      TraceDebug("IP : " + WiFi.localIP().toString() );
      TraceDebug("MAC address: " + WiFi.macAddress() );

    } else {
      // Allumage de la LED rouge ------------------------------------------------------------- ROUGE ON
      digitalWrite(RED_LED_PIN, HIGH);
    }

    // Initialisation de la balance
    //setup_CZL635_20();
  }
}



// ************************************************************************************
void loop() {


  // Lecture des mesures ---------------------------------------------------------------------------
  String Mesures = "";

  // TIMESTAMP
  String timeStamp = rtc_getTimestamp();
  Mesures.concat(timeStamp);


  // POID
  // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE ON
  digitalWrite(RED_LED_PIN, HIGH);

  int poid = mesure_poid();
  Mesures.concat(formatString(poid, "-5.0"));

  // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE OFF
  digitalWrite(RED_LED_PIN, LOW);


  // FORCE DU WIFI
  String rssi = String(WiFi.RSSI());


  // BATTERIE
  int niveauBatteri = niveau_battrie();
  Mesures.concat(formatString(niveauBatteri, "4.0"));


  // Envoie des données vers TOCIO ------------------------------------------------------------
  String retour;
  if ( ConnectionWifiEnabled ) {
    // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE ON
    digitalWrite(RED_LED_PIN, HIGH);

    retour = sendDataInHTTPSRequest( Mesures, configLocale );

    // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE OFF
    digitalWrite(RED_LED_PIN, LOW);

    if ( retour != "ok") {
      AfficheErreur("ERR (main)> ERREUR lors de l'envoie vers TOCIO. L'erreur renvoyée est :");
      TraceDebug(retour);

    } else  {
      TraceDebug("Envoie réussi");
    }
  }



  // Ecriture dans le fichier -----------------------------------------------------------------
  // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE ON
  digitalWrite(RED_LED_PIN, HIGH);

  Mesures = configLocale.IDPoubelle + "," + timeStamp + "," + String(poid) + "," + String(niveauBatteri) + "," + rssi + "," + retour;
  SD_write_Mesure(Mesures);
  TraceDebug("Ecriture dans le fichier");
  TraceDebug("Mesures: " + Mesures);
  
  // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE OFF
  digitalWrite(RED_LED_PIN, LOW);



  // Pause .................................................
  delay(60 * 1000); // 10 secondes
}
