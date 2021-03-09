/**
 * librairie boad firebeetle esp8266 : fichier->preference->http://download.dfrobot.top/FireBeetle/package_firebeetle8266_index.json
 */
 
// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"

/**
 * 
 */
typedef struct {
  char* ssid = "congres";
  char* password = "sufca!2019!dsiun";
  char* IDPoubelle = "FR_0001";
  int poidVide = 0;
} Config;
//---------------------------------------------------------------------------------------

/**
 * innitialisation wifi + capteur
 */
void setup() {
// initialisation de la liason série (pour le moniteur)
  Serial.begin(115200);
  Serial.println("OK, let's go");

// Connection WIFI ----------------------------------------------------------------------
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
  setup_rtc_pcf8523();
}
//--------------------------------------------------------------------------------------

/**
 * boucle de mesure et d'envoie des donnée vers le backoffice
 */
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
  delay(60 * 1000); // 1 minutes=(60 * 1000)
}
//---------------------------------------------------------------------------------------
