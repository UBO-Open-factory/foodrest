const int pin_batt = A0; //ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)




// --------------------------------------------------------------------------------
/**
 * Renvoie le timestamp actuel sous forme de float.
 * @return integer
 */
String rtc_timestamp() {
  
  return "1234567890";
}

// --------------------------------------------------------------------------------
/**
 * Renvoie le poind en gramme sous forme d'intger.
 * @return integer
 */
int mesure_poid() {
  int poid = 12345; //poid en grame ici = 5kg capteur de 0 a 20kg 

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
  // int nv_batt = analogRead(pin_batt);
  int nv_batt = 1234;
  
  // Serial.println("niveau de la battrie = "+String(nv_batt));
  return nv_batt;
}
