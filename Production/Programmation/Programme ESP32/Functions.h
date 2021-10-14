#include <Wire.h>



// -------------------------------------------------------------------------------------
/**
   Connnection WIFI.
   Renvoie true si la connection est possible (false sinon)
*/
boolean connectionWifi() {
  // LED rouge ________________________ ROUGE ON
  digitalWrite(LED_PESEE_PIN, HIGH);

  // station mode: the ESP32 connects to an access point
  WiFi.mode(WIFI_STA);
  WiFi.begin(configLocale.ssid, configLocale.password);

  Serial.println("Connection au réseau WIFI en cours ");
  int compteur = 1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (compteur % 10 == 0 && compteur != 0) {
      Serial.println("+");
    } else {
      Serial.print (".");
    }

    // Si la connection WIFI échoue au bout 20 secondes, on Affiche l'erreur.
    if ( compteur++ > 61) {
      AfficheErreur("ERR (connectionWifi)> Connection au WIFI impossible avec les paramétres :");
      AfficheErreur(configLocale.ssid);
      AfficheErreur(configLocale.password);

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
  int nv_batt = analogRead(A0);
  return nv_batt;
}



// ---------------------------------------------------------------------------------------------------
/**
   Permet de calibrer les capteurs.
*/
void calibrageUsine() {
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

  // Saisie de l'identifiant de la poubelle
  IDPoubelle_setup();

  // Calibration de la RTC (si on est connecté au Réseau WIFI )
  if (  connectionWifi() ) {
    rtc_setup_pcf8523();
  }

  // Calibration de la ballance
  BALANCE_setup();
}
