// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"



typedef struct {
  char* ssid = "congres";
  char* password = "sufca!2019!dsiun";
  char* IDPoubelle = "FR_0001";
  int poidVide = 0;
} Config;





/**
 *
 */
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
  setup_CZL635_20();
}
//--------------------------------------------------------------------------------------

/**
 * setup capteur CZL635_20
 */
void setup_CZL635_20() {
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.begin(DOUT, CLK); //definition des pin
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
}
//--------------------------------------------------------------------------------------




// *********************************************************************************
void loop() {
  // Concatenation des mesures .............................
  String Mesures = "";

// timestamp0 is the 'Timestamp' value from your sensor 'RTC_Timestamp' (as float)
  int Mesures.concat(formatString(rtc_timestamp(), "10.0"));

// poid1 is the 'Poid' value from your sensor 'CZL635-20' (as float)
  int Mesures.concat(formatString( mesure_poid(), "-5.0"));

// valeur analogique(0 a 1023)2 is the 'Valeur analogique(0 à 1023)' value from your sensor 'CAN (1024)' (as float)
  int Mesures.concat(formatString(niveau_battrie(), "4.0"));

// Envoie des données vers TOCIO .........................
  sendDataInHTTPSRequest(Mesures);

  // Pause .................................................
  delay(60 * 1000); // 1 minutes
}
//--------------------------------------------------------------------------------------
