// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"



typedef struct {
  char* ssid = "congres";
  char* password = "sufca!2019!dsiun";
  char* IDPoubelle = "FR_0001";
  int poidVide = 0;
} Config;




const int pin_batt = A0; //ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)





// *********************************************************************************
void setup() {
  // initialisation de la liason série (pour le moniteur)
  Serial.begin(115200);
  Serial.println("OK, let's go");

  // Connection WIFI -----------------------------------------------------------------------
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connection au réseau WIFI en cours ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Voila, c'est fait.");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
  Serial.print("MAC address: "); Serial.println(WiFi.macAddress());
}




// *********************************************************************************
void loop() {
  // Concatenation des mesures .............................
  String Mesures = "";

  // timestamp0 is the 'Timestamp' value from your sensor 'RTC_Timestamp' (as float) 
  Mesures.concat(formatString(rtc_timestamp(), "10.0"));

  // poid1 is the 'Poid' value from your sensor 'CZL635-20' (as float)
  Mesures.concat(formatString( mesure_poid(), "-5.0"));

  // valeur analogique(0 a 1023)2 is the 'Valeur analogique(0 à 1023)' value from your sensor 'CAN (1024)' (as float)
  Mesures.concat(formatString(niveau_battrie(), "4.0"));

  // Envoie des données vers TOCIO .........................
  sendDataInHTTPSRequest(Mesures);

  // Pause .................................................
  delay(60 * 1000); // 1 minutes
}






// --------------------------------------------------------------------------------
/**
 * Renvoie le timestamp actuel sous forme d'integer.
 * @return integer
 */
int rtc_timestamp() {
  return 1615220068;
}

// --------------------------------------------------------------------------------
/**
 * Renvoie le poind en gramme sous forme d'intger.
 * @return integer
 */
int mesure_poid() {
  int poid = 5000; //poid en grame ici = 5kg capteur de 0 a 20kg 

  // code CZL635-20
  return poid;
}


// --------------------------------------------------------------------------------
/**
 * Renvoie le niveau de la batterie (de 0 à 1024).
 * @return integer
 */
int niveau_battrie() {
  // Lecture de la valeur analogique sur la pin de la batterie
  int nv_batt = analogRead(pin_batt);
  
  Serial.print("niveau de la battrie = ");
  Serial.print(nv_batt);

  return nv_batt;
}
