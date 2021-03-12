/**
   Gestion des mesures de la poubelle connectée.
   Principe des mesure !
   La poubelle est en mode deep sleep.
   Dès que le capteur d'ouverture (capteur ILS) détecte une ouverture, on réveil la poubelle.
     .On fait clignoter la LED rouge (tant que le couvercle est ouvert)

   Dés que le capteur d'ouverture (capteur ILS) détecte une fermeture :
     . On éteind la LED rouge et on allume la led verte
     . On fait la mesure du poid.
     . On prend l'heure (timestamp).
     . On prend le niveau de la batterie.
     . On enregistre l'ID de la poubelle + timestamp + poids + niveau batterie sur la carte SD dans un fichier CSV temporaire.
     . On éteind la led verte.
     . On endors la puobelle

   Chaque jour à 03:00 :
     . On se connecte au réseau WIFI local.
     . On charge le fichier CSV temporaire.
     . On allume la led verte.
        - Pour chacune des lignes, on l'envoie au serveur TOCIO.
          . Si tout se passe bien (retour "ok")
            - On écrit la ligne dans le fichier CSV des mesures.
            - On supprime la ligne dans le fichier CSV temporaire.
     . On éteind la LED verte.
     . On endors la poubelle.
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
  WiFi.begin(configLocale.ssid, configLocale.password);

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
  setup_CZL635_20();

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
  Mesures.concat(formatString( mesure_poid(), "-5.0"));

  // valeur analogique(0 a 1023)2 is the 'Valeur analogique(0 à 1023)' value from your sensor 'CAN (1024)' (as float)
  Mesures.concat(formatString(niveau_battrie(), "4.0"));




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
