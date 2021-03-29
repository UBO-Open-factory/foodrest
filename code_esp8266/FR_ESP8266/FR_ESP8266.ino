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
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GND_C_EN, OUTPUT);
  digitalWrite(GND_C_EN, HIGH);

  // initialisation de la liaison série (pour le moniteur)
  Serial.begin(9600);
  delay(500);
  TraceDebug("OK, let's go");

  // Extinction de la LED rouge ____________________ ROUGE OFF
  digitalWrite(RED_LED_PIN, LOW);


  // Initialisaiton de la balance ---------------------------------------------------------------------
  balance.begin(BALANCE_DAT, BALANCE_CLK);
  balance.set_scale();

  


  // Initialisation de la carte SD ---------------------------------------------------------------------
  // Si on n'arrive pas à initiliser la carte SD
  if ( ! SD_initCard() ) {
    AfficheErreur("ERR (main)> Lecture carte SD impossible!");
    AfficheErreur("Veuillez insérer une carte SD et relancer le programme.");
    delaiClignottementLED = 150;  // Pour l'affichage dans la boucle principale

  } else {



    // Lecture de la config à partir du fichier sur la carte SD ---------------------------------------
    // ( renseigne le ssid, password, poid, IDPoubelle, etc... )
    configLocale = CONF_lectureConfigurationFromSD();


    // On fait un calibrage usine car la variable est positionnée à TRUE dans le fichier --------------
    // de settings
    if ( configLocale.InitialisationUsine ) {
      TraceDebug("On entre en mode calibration d'usine.");
      calibrageUsine();




      // Lecture des mesures ---------------------------------------------------------------------------
      // (on est en mode normal, pas de paramétrage usine à faire)
    } else {

      String Mesures = "";

      // TIMESTAMP
      String timeStamp = rtc_getTimestamp();
      Mesures.concat(timeStamp);


      // POID
      // Allumage/Extinction de la LED rouge ____________________ ROUGE ON
      digitalWrite(RED_LED_PIN, HIGH);

      float poid = BALANCE_pesee_balance();
      Mesures.concat(formatString(poid, "-5.0"));

      // Allumage/Extinction de la LED rouge ____________________ ROUGE OFF
      digitalWrite(RED_LED_PIN, LOW);

      // FORCE DU WIFI
      int rssi = 0; // Sera mis à jour lorsque la connecction sera faite
      String retourTOCIO = "";

      // BATTERIE
      int niveauBatteri = niveau_battrie();
      Mesures.concat(formatString(niveauBatteri, "4.0"));





      // Connection au WIFI ----------------------------------------------------------------------------
      // On a réussi à se connecter au WIFI
      if ( connectionWifi() ) {
        // Extinction de la LED rouge __________________________ ROUGE OFF
        digitalWrite(RED_LED_PIN, LOW);

        TraceDebug("Voila, c'est fait.");
        TraceDebug("IP : " + WiFi.localIP().toString() );
        TraceDebug("MAC address: " + WiFi.macAddress() );

        // Force du signal WIFI
        rssi = WiFi.RSSI();

        // Envoie des données vers TOCIO ------------------------------------------------------------
        // Allumage/Extinction de la LED rouge _______________ ROUGE ON
        digitalWrite(RED_LED_PIN, HIGH);

        retourTOCIO = sendDataInHTTPSRequest( Mesures, configLocale );

        // Allumage/Extinction de la LED rouge _______________ ROUGE OFF
        digitalWrite(RED_LED_PIN, LOW);

        if ( retourTOCIO != "ok") {
          AfficheErreur("ERR (main)> ERREUR lors de l'envoie vers TOCIO. L'erreur renvoyée est :");
          TraceDebug(retourTOCIO);

        } else  {
          TraceDebug("Envoie réussi");
        }




        // Pas de connection au Wifi --------------------------------------------------------------------
      } else {

        // Allumage de la LED rouge ___________________________ ROUGE ON
        delaiClignottementLED = 1000;
        digitalWrite(RED_LED_PIN, HIGH);
      }

      // Ecriture dans le fichier ----------------------------------------------------------------------
      // Allumage de la LED rouge _____________________________ ROUGE ON
      digitalWrite(RED_LED_PIN, HIGH);

      Mesures = configLocale.IDPoubelle + "," + timeStamp + "," + String(poid) + "," + String(niveauBatteri) + "," + String(rssi) + "," + retourTOCIO;
      SD_write_Mesure(Mesures);
      TraceDebug("Ecriture dans le fichier");
      TraceDebug("Mesures: " + Mesures);

      // Allumage/Extinction de la LED rouge _________________ ROUGE OFF
      digitalWrite(RED_LED_PIN, LOW);

      // Juste pour afficher une simulation du deepSleep sur la LED
      delaiClignottementLED = 2000;



      // Deep sleep ----------------------------------------------------------------------------------
      // On passe en deepSleep, uniquement si on est pas en mode DEBUG
      if ( configLocale.AfficheTraceDebug ) {
        TraceDebug("Passage en DeepSleep");
      } else {
        digitalWrite(GND_C_EN, LOW);
        ESP.deepSleep(0);
      }
    }
  }

}



// ************************************************************************************
/**
   Si on arrive dans cette boucle c'est qu'il y a un problème car lorsque tout se passe bien
   on fini en deep sleep.
   L'affichage du problème se fait par le clignottement de la LED plus ou moins rapide.
*/
void loop() {
  delay( delaiClignottementLED );

  // Allumage/Extinction de la LED rouge ____________________ ROUGE TOOGLE
  redLedState = !redLedState;
  digitalWrite(RED_LED_PIN, redLedState);
}
