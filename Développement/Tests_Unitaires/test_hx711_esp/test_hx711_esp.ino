/**
 *
 * Premier programme avec librairie pour ESP32
 * https://github.com/bogde/HX711
 *
 * L. Marchal & A.Pignalet
 * 
 * 14/04/2021
 * 
 * Version 1
 *
 *  essai avec un HX711 / capteur de force 20 kg et FireBeetle ESP32 Wroom
 *
 *
 *
**/
#include "HX711.h"


#define NB_ECHANTILLONS 20

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 26;
const int LOADCELL_SCK_PIN = 27;

HX711 balance;

void setup() {
  Serial.begin(38400);
  delay(500);
  Serial.println("HX711 Demo");
  balance.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("**************************************************");
  Serial.println("*     Tarage de la balance                       *");
  Serial.println("**************************************************");
  Serial.println("Assurez-vous que la plateau est vide");
  Serial.println("puis pressez une touche");
  while (!Serial.available());
  String yy = Serial.readString();
  Serial.println("Tarage en cours.... ");
  balance.set_scale();     

unsigned long debut = millis();
  balance.tare(NB_ECHANTILLONS);                // reset the scale to 0 on 20 echantillons
unsigned duree = (millis() - debut);


  Serial.println ("");
  Serial.println ("");

Serial.print ("Durée tarrage : ");
Serial.println (duree);
  Serial.println ("");
  Serial.println ("");
  

  Serial.println("**************************************************");
  Serial.println("*     Calibration de la balance                  *");
  Serial.println("**************************************************");
  Serial.println("1 - Placez un poids connu dans la poubelle,");
  Serial.println("2 - Refermez le couvercle et ");
  Serial.println("3 - Saisissez le poids inséré dans la balance.");
  Serial.println("         le poids doit être exprimé en grammes et sans unités ni décimale");
  Serial.println("          ex. : 1965  pour 1965 grammes");
  Serial.println("");
  while (!Serial.available());
  Serial.print ("Etalonnage de la balance ......");
  String xx = Serial.readString();
  float poids_a_atteindre = xx.toFloat();

  float poids_lu = balance.get_units(NB_ECHANTILLONS);

  float result = poids_lu/poids_a_atteindre;


  balance.set_scale(result);


  Serial.println ("");
  Serial.println ("");
  Serial.println ("Tarrage effectué.");
  Serial.println ("");
  Serial.println ("");
  
  float t_tare = balance.get_offset();
  float t_scale = balance.get_scale();

  Serial.print ("         TARE : ");
  Serial.println (t_tare,1);
  
  Serial.print ("       OFFSET : ");
  Serial.println (t_scale,1);
 Serial.println ("");
  Serial.println ("");
 
}

void loop() {
  Serial.print ("Mesure > ");
  Serial.println (balance.get_units(NB_ECHANTILLONS));
  delay(200);
   
 
}
