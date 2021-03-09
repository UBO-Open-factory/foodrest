/**
 * projet pobelle connectée pour analyse des tandence de surconsomation de produit végétal
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "HX711.h"
/**
 * capteur CZL635_20 [poids(kg/lbs)]
*/
#define DOUT  3
#define CLK  2
HX711 scale;
float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup <-- tuto
//--------------------------------------------------------------------------------------

/**
 * configuration wifi network
 */ 
const char* ssid = "congres";
const char* password = "sufca!2019!dsiun";
//--------------------------------------------------------------------------------------

/**
 * pin mape configuration analog pin pour mesue battrie
 */ 
const int pin_batt = A0; //ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)
//--------------------------------------------------------------------------------------

/**
 * configuration server adresse
 */ 
const String host = "https://uboopenfactory.univ-brest.fr/cad/proto1/backoffice/";
const String url  = "/mesure/add/FR_TEST2";
//--------------------------------------------------------------------------------------

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

/**
 * 
 */
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
  delay(60 * 1000);
}
//--------------------------------------------------------------------------------------

/**
 * Formattage d'une chaine de caractères "chaine" selon le format "formattage".
 * @param chaine : Chainbe de caractère à formater.
 * @param formattage : Formattage d'une valeur selon la règle [-]chiffreAvantLaVirgule.chiffreApresLaVirgule
 */
String formatString(float p_valeur, String p_formattage) {
  int delimiterPosition, lenghtAvant;
  String data = "";  // The string we need to format
  String chaine = String(p_valeur);

  delimiterPosition = chaine.indexOf(".");  // Read the delimiter positon in "chaine".
  String avant = chaine.substring(0, delimiterPosition);
  String apres = chaine.substring(delimiterPosition + 1);

  delimiterPosition = p_formattage.indexOf(".");           // Read the delimiter position in "formattage".
  if ( p_formattage.substring(0, 1) == "-" ) {
    lenghtAvant = p_formattage.substring(1, delimiterPosition).toInt();
  } else {
    lenghtAvant = p_formattage.substring(0, delimiterPosition).toInt();
  }
  int lenghtApres = p_formattage.substring(delimiterPosition + 1).toInt();

  // Si on a besoin d'un signe .................................
  if ( p_formattage.substring(0, 1) == "-" ) {
    if (p_valeur < 0) {
      data.concat("-");
    } else {
      data.concat("+");
    }
  }

// Padding with 0 for the "avant" part ........................
  String temp = "";
  for (int i = 0; i <= lenghtAvant; i++) {
    // Concatenation de tout les 0
    temp.concat("0");
  }
  temp.concat(avant);
  data.concat(temp.substring( temp.length() - lenghtAvant ));

// Formattage de la partie apres la virgule ...................
  for (int i = 0; i <= apres.length(); i++) {
    apres.concat("0");
  }
  apres = apres.substring(0, lenghtApres);
  data.concat(apres);

  return data;
}
//--------------------------------------------------------------------------------------

/**
 * Envoi au serveur TOCIO les mesures ("data") passées en paramètre.
 * @param data : String contenant les mesures formatée selon la payload défini dans le Back Office de Tocio
 */
String sendDataInHTTPSRequest(String data) {

// If we are connecte to the WIFI
  if (WiFi.status() == WL_CONNECTED) {

//Create an https client
    WiFiClientSecure client;
    int port = 443;
    if (!client.connect(host, port)) {
      Serial.println("connection failed");
      return "nok";
    }

// Send data to the client with a GET method
    String request = url + "/" + data;
    client.print(String("GET ") + request + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n\r\n");

// Lecture de ce qui est renvoyée par le serveur
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    client.stop();
    return "ok";

  } else {
    return "nok";
  }
}
//--------------------------------------------------------------------------------------

/**
 * Capteur RTC Timestamp des captures de valeur
 */
int rtc_timestamp() {
  return epoch_rtc; //code RTC
}
//--------------------------------------------------------------------------------------

/**
 * mesure capteur de poid
 * balence CZL635_20
 */
int mesure_poid() {
  int poid = 5000; //poid en grame ici = 5kg capteur de 0 a 20kg 
  return poid;
/**
 * CODE TUTO CAPTEUR CZL635_20
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 1);
  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
 */

}
//--------------------------------------------------------------------------------------

/**
 * détection niveau batrie sur broche analogique A0 si nv_bat<850 recharger la battrie
 */
int niveau_battrie() {
  nv_batt = analogRead(pin_batt);//lecture de la valeur analogique
  Serial.print("niveau de la battrie = ");
  Serial.print(nv_batt);

  return nv_batt;
}
//--------------------------------------------------------------------------------------
