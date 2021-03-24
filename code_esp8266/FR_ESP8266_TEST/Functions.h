#include <Wire.h>
#include <SPI.h>

// capteur RTC pcf8523
// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_PCF8523 rtc;




// -------------------------------------------------------------------------------------
/**
   Connnection WIFI.
   Renvoie true si la connection est possible (false sinon)
*/
boolean connectionWifi() {
  // Extinction de la LED rouge ________________________ ROUGE ON
  digitalWrite(RED_LED_PIN, HIGH);

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



//---------------------------------------------------------------------------------------
/**
      Mise à l'heure de la RTC.
      Si elle à été reinitilisée, on l'initialise à l'heure et la date de compilation du programme.

           The PCF8523 can be calibrated for:
            - Aging adjustment
            - Temperature compensation
            - Accuracy tuning
     The offset mode to use, once every two hours or once every minute.
     The offset Offset value from -64 to +63. See the Application Note for calculation of offset values.
     https://www.nxp.com/docs/en/application-note/AN11247.pdf
     The deviation in parts per million can be calculated over a period of observation. Both the drift (which can be negative)
     and the observation period must be in seconds. For accuracy the variation should be observed over about 1 week.
     Note: any previous calibration should cancelled prior to any new observation period.
     Example - RTC gaining 43 seconds in 1 week

    float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
    float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
    float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
    float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours
    float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)
    int offset = round(deviation_ppm / drift_unit);
    rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
    rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

     Serial.print("Offset is "); Serial.println(offset); // Print to control offset
*/
void rtc_setup_pcf8523() {

  // Si la RTC n'est pas trouvée
  if (! rtc.begin()) {
    AfficheErreur("ERR (setup_rtc_pcf8523)> Couldn't find RTC");
    abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    TraceDebug("RTC is NOT initialized, let's set the time to the date & time this sketch was compiled !");

    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  rtc.start();
}

//---------------------------------------------------------------------------------------
/**
   Renvoie le timestamp actuel sous forme de chaine de 10 caractères.
   @return integer
   librairie adafruit RTClib
*/
String rtc_getTimestamp() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    Serial.println();
  */
  return String(now.unixtime());
/*
  char jour[3]; sprintf(jour, "%02i", now.day());
  char mois[3]; sprintf(mois, "%02i", now.month());
  String date = String(jour) + "-" + String(mois) + "-" + String(now.year() );


  char hour[3]; sprintf(hour, "%02i", now.hour());
  char minute[3]; sprintf(minute, "%02i", now.minute());
  char second[3]; sprintf(second, "%02i", now.second());
  String heure = String(hour) + ":" + String(minute) + ":" + String(second);

  return date + heure;
  */
}


// --------------------------------------------------------------------------------------
/**
   mesure capteur de poid
   balence CZL635_20
*/
int mesure_poid() {
  /*
    balance.set_balance(calibration_factor); //Adjust to this calibration factor

    Serial.print("Reading: ");
    Serial.print(balance.get_units(), 1);
    Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    Serial.println();

    if (Serial.available())
    {
      char temp = Serial.read();
      if (temp == '+' || temp == 'a')
        calibration_factor += 10;
      else if (temp == '-' || temp == 'z')
        calibration_factor -= 10;
    }
  */
  return random(99999);
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
  // Calibration de la RTC
  rtc_setup_pcf8523();

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
