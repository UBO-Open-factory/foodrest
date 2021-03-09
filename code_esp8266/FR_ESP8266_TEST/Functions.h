#include <Wire.h>
#include <SPI.h>

/**
 * capteur CZL635_20 [poids(kg/lbs)]
*/
#include "HX711.h"
#define DOUT  D3
#define CLK  D2
HX711 scale;
float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup <-- tuto
//---------------------------------------------------------------------------------------

#define led_r D4 //led indicateur visuel rouge
#define led_v D5 //led indicateur visuel vert
#define pin_batt A0 //ESP8266 analog PIN = A0, ici niveau_battrie(pin_batt)

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
//---------------------------------------------------------------------------------------

/**
 * Renvoie le timestamp actuel sous forme d'integer.
 * @return integer
 */
int rtc_timestamp() {
  return 1615220068;
}
// --------------------------------------------------------------------------------------

/**
 * mesure capteur de poid
 * balence CZL635_20
 */
int mesure_poid() {
  //int poid = 5000; //poid en grame ici = 5kg capteur de 0 a 20kg 
  //return poid;
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
}
//---------------------------------------------------------------------------------------

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
//---------------------------------------------------------------------------------------
