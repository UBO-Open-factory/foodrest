
#include <ESP8266WiFi.h>


// @see : https://github.com/espressif/arduino-esp32/blob/ef99cd7fe7778719c92d6f8df0f10d3f0f7aa35e/libraries/WiFiClientSecure/src/WiFiClientSecure.h
#include <WiFiClientSecure.h>


// Paramètres de connection au réseau wifi
const char* ssid = "congres";
const char* password = "sufca!2019!dsiun";

const String host = "https://uboopenfactory.univ-brest.fr/cad/proto1/backoffice/";
const String url  = "/mesure/add/FR_TEST2";



// --------------------------------------------------------------------------------
// Formattage d'une chaine de caractères "chaine" selon le format "formattage".
// @param chaine : Chainbe de caractère à formater.
// @param formattage : Formattage d'une valeur selon la règle [-]chiffreAvantLaVirgule.chiffreApresLaVirgule
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

  // Formattage de la partie apres la virgule ....................
  for (int i = 0; i <= apres.length(); i++) {
    apres.concat("0");
  }
  apres = apres.substring(0, lenghtApres);
  data.concat(apres);

  return data;
}



// --------------------------------------------------------------------------------
// Envoi au serveur TOCIO les mesures ("data") passées en paramètre.
// @param data : String contenant les mesures formatée selon la payload défini dans le Back Office de Tocio
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
