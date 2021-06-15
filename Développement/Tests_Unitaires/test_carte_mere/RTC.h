/**
   Toutes les fonctions concernant la RTC.
*/
#include <Wire.h>

// capteur RTC pcf8523
// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include <RTClib.h>
RTC_PCF8523 rtc;


// Define NTP Client to get time ******************************************************************************************************
#include <NTPClient.h>
#include <WiFiUdp.h>




//---------------------------------------------------------------------------------------
/**
      Mise à l'heure de la RTC.
      Si elle à été reinitilisée, on l'initialise avec le serveur de temps pool.ntp.org

           The PCF8523 can be calibrated for:
            - Aging gettimement
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

  // When time needs to be set on a new device, or after a power loss, the
  // following line sets the RTC to the date & time this sketch was compiled
  //    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println("");
  Serial.println("");
  Serial.println("****************************************************");
  Serial.println("*     FOOD REST : Initialisation de l'horloge      *");
  Serial.println("****************************************************");
  Serial.println("Connection au serveur de temps...");

  // On va chercher l'heure sur le serveur de temps de L'UBO
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "chronos.univ-brest.fr");
  timeClient.begin();
  timeClient.setTimeOffset(7200); // Heure de Paris GMT +2 (in secondes)
  timeClient.update();


  // Get a Time structure for days, month and year
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm    = gmtime ((time_t *)&epochTime);
  String currentDate = String(ptm->tm_mday) + "/" + String(ptm->tm_mon + 1) + "/" + String(ptm->tm_year + 1900);

  // Si on a pas reussi a lire l'heure du serveur de temps
  if ( currentDate == "1/1/1970" ) {
    AfficheErreur("ERR (setup_rtc_pcf8523)> Impossible de lire l'heure sur le serveur chronos.univ-brest.fr, on essaie sur pool.ntp.org");
    timeClient.end();
    timeClient.setPoolServerName("pool.ntp.org");

    timeClient.begin();
    timeClient.setTimeOffset(7200); // Heure de Paris GMT +2 (in secondes)
    timeClient.update();

    epochTime = timeClient.getEpochTime();
    ptm    = gmtime ((time_t *)&epochTime);
    currentDate = String(ptm->tm_mday) + "/" + String(ptm->tm_mon + 1) + "/" + String(ptm->tm_year + 1900);
  }

  String formattedTime = timeClient.getFormattedTime();

  Serial.println("Il est : " + formattedTime);
  Serial.println("Nous sommes le : " + currentDate);

  // Mise à l'heure de la RTC
  rtc.adjust(DateTime(timeClient.getEpochTime()));
  rtc.start();
}




//---------------------------------------------------------------------------------------
/**
   Renvoie le timestamp actuel sous forme de chaine de 10 caractères.
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
}


//---------------------------------------------------------------------------------------
/**
   Renvoie la date et l'heure sous forme de chaine de caractères.
   @return integer
   librairie adafruit RTClib
*/
String rtc_getDate() {
  rtc.begin();
  rtc.start();

  DateTime now = rtc.now();
  char jour[3]; sprintf(jour, "%02i", now.day());
  char mois[3]; sprintf(mois, "%02i", now.month());
  String date = String(jour) + "-" + String(mois) + "-" + String(now.year() );


  char hour[3]; sprintf(hour, "%02i", now.hour());
  char minute[3]; sprintf(minute, "%02i", now.minute());
  char second[3]; sprintf(second, "%02i", now.second());
  String heure = String(hour) + ":" + String(minute) + ":" + String(second);

  return date + " " + heure;
}
