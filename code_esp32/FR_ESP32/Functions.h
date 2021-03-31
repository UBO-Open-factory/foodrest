#include <Wire.h>



// -------------------------------------------------------------------------------------
/**
   Connnection WIFI.
   Renvoie true si la connection est possible (false sinon)
*/
boolean connectionWifi() {
  // Extinction de la LED rouge ________________________ ROUGE ON
  digitalWrite(RED_LED_PIN, HIGH);

  // station mode: the ESP32 connects to an access point
  WiFi.mode(WIFI_STA);
  WiFi.begin(configLocale.ssid, configLocale.password);

  TraceDebug("Connection au réseau WIFI en cours ");
  int compteur = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // Allumage/Extinction de la LED rouge _______________ ROUGE TOOGLE
    redLedState = !redLedState;
    digitalWrite(RED_LED_PIN, redLedState);

    delay(500);
    Serial.print(".");

    // Si la connection WIFI échoue au bout 20 secondes, on Affiche l'erreur.
    if ( compteur++ > 200) {
      AfficheErreur("ERR (connectionWifi)> Connection au WIFI impossible avec les paramétres :");
      AfficheErreur(configLocale.ssid);
      AfficheErreur(configLocale.password);

      // Extinction de la LED rouge ________________________ ROUGE OFF
      digitalWrite(RED_LED_PIN, HIGH);
      return false;
    }
  }
  return true;
}



// ---------------------------------------------------------------------------------------------------
/**
   Renvoie le niveau de la batterie (de 0 à 1024).
   @return integer
*/
int niveau_battrie() {
  // Lecture de la valeur analogique sur la pin de la batterie
  // int nv_batt = analogRead(pin_batt);
  int nv_batt = 1234;

  return nv_batt;
}



// ---------------------------------------------------------------------------------------------------
/**
   Permet de calibrer les capteurs.
*/
void calibrageUsine() {
  // Saisie de l'identifiant de la poubelle
  IDPoubelle_setup();

  // Calibration de la RTC
  if (  connectionWifi() ) {
    rtc_setup_pcf8523();
  }

  // Calibration de la ballance
  CZL635_setup();
}


// ---------------------------------------------------------------------------------------------------
/**
   Ecriture d'une ligne dans le fichier des mesures sur la carte SD.
   chaine : La chaine de caractère à écrire dans le fichier.
*/
void SD_write_Mesure(String chaine) {
  File dataFile = SD.open(configLocale.IDPoubelle + "-mesures.csv", FILE_WRITE);

  // Si on a réussi à ouvrir le fichier en écriture
  if (dataFile) {
    // Ecriture de la ligne dans le fichier
    dataFile.println(chaine);

    // Fermeture du fichier
    dataFile.close();

  } else {
    AfficheErreur("ERR (write_Mesure)> Impossible d'ouvir le fichier en écriture sur la carte SD.");
  }
}
