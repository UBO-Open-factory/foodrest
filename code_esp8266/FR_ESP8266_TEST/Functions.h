#include <Wire.h>
#include <SPI.h>

/**
   capteur CZL635_20 [poids(kg/lbs)]
*/
#include "HX711.h"
#define DOUT  D3
#define CLK  D2
HX711 scale;
float calibration_factor = configLocale.calibrationFactor;









//---------------------------------------------------------------------------------------
/**
   capteur RTC pcf8523
*/
// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_PCF8523 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//---------------------------------------------------------------------------------------

#define led_r D4 //led indicateur visuel rouge
#define led_v D5 //led indicateur visuel vert
#define pin_batt A0 //ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)



// -------------------------------------------------------------------------------------
/**
   Connnection WIFI.
   Renvoie true false si la connection est possible (false sinon)
*/
boolean connectionWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(configLocale.ssid, configLocale.password);

  TraceDebug("Connection au réseau WIFI en cours ");
  int compteur = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // Allumage/Extinction de la LED rouge ----------------------------------------------------------- ROUGE TOOGLE
    redLedState = !redLedState;
    digitalWrite(RED_LED_PIN, redLedState);

    delay(500);
    Serial.print(".");

    // Si la connection WIFI échoue au bout 20 secondes, on Affiche l'erreur.
    if ( compteur++ > 200) {
      AfficheErreur("ERR (connectionWifi)> Connection au WIFI impossible avec les paramétres :");
      AfficheErreur(configLocale.ssid);
      AfficheErreur(configLocale.password);
      return false;
    }
  }
  return true;
}




//---------------------------------------------------------------------------------------
/**
   setup capteur CZL635_20
*/
void setup_CZL635_20() {
  /*
    Serial.println("HX711 calibration sketch");
    Serial.println("Remove all weight from scale");
    Serial.println("After readings begin, place known weight on scale");
    Serial.println("Press + or a to increase calibration factor");
    Serial.println("Press - or z to decrease calibration factor");
  */

  scale.begin(DOUT, CLK); //definition des pin
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

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

  DateTime now = rtc.now();
  /*
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
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
}


// --------------------------------------------------------------------------------------
/**
   mesure capteur de poid
   balence CZL635_20
*/
int mesure_poid() {
  /*
    scale.set_scale(calibration_factor); //Adjust to this calibration factor

    Serial.print("Reading: ");
    Serial.print(scale.get_units(), 1);
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
  //setup_CZL635_20();
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
