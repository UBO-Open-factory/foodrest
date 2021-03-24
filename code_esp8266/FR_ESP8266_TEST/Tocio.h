#include <ESP8266WiFi.h>

// @see : https://github.com/espressif/arduino-esp32/blob/ef99cd7fe7778719c92d6f8df0f10d3f0f7aa35e/libraries/WiFiClientSecure/src/WiFiClientSecure.h
// Documentaiton : https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#
#include <WiFiClientSecure.h>

/*
 Use the web site https://www.grc.com/fingerprints.htm to the fingerprint from  your web site
 const char* fingerprint = "2A:12:65:E0:C9:41:C3:77:58:23:9F:02:EA:49:7F:84:D1:90:DE:50";
*/



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
// @param data : String contenant les mesures formatées selon la payload défini dans le Back Office de Tocio
// @param config : le fichier de configuration pour lire les variables locales de connection Wifi, ID poubelle, etc...
String sendDataInHTTPSRequest(String data, Configuration configLocale) {

  // If we are connect to the WIFI
  if (WiFi.status() == WL_CONNECTED) {

    //  Create an https client
    WiFiClientSecure client;
    const String host = "uboopenfactory.univ-brest.fr";


    // Don't validate the certificat (and avoid fingerprint).
    client.setInsecure();

    const int port = 443;
    if (!client.connect(host, port)) {
      AfficheErreur("ERR (sendDataInHTTPSRequest)> connection failed");
      return "connection failed (host:" + String(host) + ", port:" + String(port) + ")";
    }

    // Send data to the client with a GET method
    String request = url + configLocale.IDPoubelle + "/" + data;
    TraceDebug( String(request) );

    // Send data to the client with a GET method
    TraceDebug("Request : " + url + "/" + data);
    client.println("GET " + request + " HTTP/1.1");
    client.println("Host: " + host);
    client.println("Accept: */*");
    client.println("User-Agent: ESP8266/NodeMCU 0.9");
    client.println("Connection: close");
    client.println(); // end HTTP request header


    // Read server respons
    String retour = "";
    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        // Serial.print(c);
        retour.concat( String (c) );
      }
    }

    // Extraction du message d'erreur dans le retour JSON
    // 'error' est toujours renvoyé, même si le message est vide
    retour = retour.substring(retour.indexOf("\"error") + 9, retour.indexOf("}") );


    // Si on trouve le mot success dans la trame, c'est que l'insertion a bien été faite.
    // 'success' est renvoyé qu'en cas de succés, s'il n'apparait pas, il faut donc renvoyer le message d'erreur
    if ( retour.indexOf("\"success") == -1 ) {
      return retour;
    }


    // Fin de la connection
    if (!client.connected()) {
      // if the server's disconnected, stop the client:
      client.stop();
    }
    return "ok";

  } else {
    return "Connection WIFI impossible : WiFi.status() = " + String(WiFi.status() + "Voir : https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#check-return-codes" );
  }
}
