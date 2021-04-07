// @see : https://github.com/espressif/arduino-esp32/blob/ef99cd7fe7778719c92d6f8df0f10d3f0f7aa35e/libraries/WiFiClientSecure/src/WiFiClientSecure.h
// #include "lib/WiFiClientSecure/src/WiFiClientSecure.h"
#include <WiFiClientSecure.h>


// --------------------------------------------------------------------------------
// Formattage d'une chaine de caractères "chaine" selon le format "formattage".
// @param chaine : Chainbe de caractère à formater.
// @param formattage : Formattage d'une valeur selon la règle [-]chiffreAvantLaVirgule.chiffreApresLaVirgule
String formatString(float p_valeur, String p_formattage) {
  int delimiterPosition, lenghtAvant;
  String data = "";  // The string we need to format
  String chaine = String(abs(p_valeur));

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

  String temp = "";
  for (int i = 0; i <= lenghtAvant; i++) {
    temp = "0" + temp;
  }
  temp = temp + avant;
  temp = temp.substring(temp.length() - lenghtAvant);

  // Si on a besoin d'un signe .................................
  if ( p_formattage.substring(0, 1) == "-" ) {
    if (p_valeur < 0) {
      data = "-";
    } else {
      data = "+";
    }
  }
  // ajoute la partie entière
  data = data + temp;

  // formate la partie décimale
  temp = apres;
  for (int i = 0; i <= apres.length(); i++) {
    temp = temp + "0";
  }
  temp = temp.substring( 0, lenghtApres);

  // ajoute la partie décimale
  data = data + temp;
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
    const char* host = "uboopenfactory.univ-brest.fr";


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
    client.println("GET " + request + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Accept: */*");
    client.println("User-Agent: ESP32/FireBeetle");
    client.println("Connection: close");
    client.println(); // end HTTP request header
    TraceDebug( "Envoie OK" );

    // Read server respons
    String retour = "";
    String currentLine = "";
    boolean startRecording = false;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read(); // read a byte, then
        // Serial.write(c);        // print it out the serial monitor
        if (c == '\n') {        // the byte is a newline character
          // if the current line is blank, you got
          // two newline characters in a row.
          // that's the end of the client HTTP request,
          if (currentLine.length() == 0) {
            if ( startRecording ) {
              break;
            }
            // HTTP header if finish, we can start to record respons from server
            startRecording = true;

          } else {
            // if we got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {
          // if we got anything else but a CR character,
          // add it to the end of the currentLine
          currentLine += c;
        }
        if (startRecording) {
          retour += c;
        }
      }
    }
    // Fin de la connection
    client.stop();

    // Extraction du message d'erreur dans le retour JSON
    // 'error' est toujours renvoyé, même si le message est vide
    retour = retour.substring(retour.indexOf("\"error") + 9, retour.indexOf("}") );


    // Si on trouve le mot success dans la trame, c'est que l'insertion a bien été faite.
    // 'success' est renvoyé qu'en cas de succés, s'il n'apparait pas, il faut donc renvoyer le message d'erreur
    if ( retour.indexOf("\"success") == -1 ) {
      return retour;
    }




    return "ok";

  } else {
    return "Connection WIFI impossible : WiFi.status() = " + String(WiFi.status() + "Voir : https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#check-return-codes" );
  }
}
