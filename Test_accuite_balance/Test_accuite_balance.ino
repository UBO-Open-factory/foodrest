/**
   Gestion des mesures de la poubelle connectée.
*/
/*
// Variables globales
#include "globalVariable.h"


// Librairie de gestion des erreurs
#include "GestionErreur.h"

// Librairies pour la lecture de la configuration
#include "Config.h"

// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairie de gestion de la balance
#include "balance.h"

// Librairie de gestion de la balance
#include "RTC.h"

// Librairie de gestion de la saisie de l'ID de la balance
#include "IDpoubelle.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"

*/


// Variables globales
#include "globalVariable.h"


// Librairie de gestion de la balance
#include "balance.h"



float t_scale =0;
float t_tare=0;


// ************************************************************************************
void setup() {
  // initialisation de la liaison série (pour le moniteur)
  Serial.begin(115200);
  delay(200);  // attente pour l'init de la liaison serie

  // Orientation des E/S
  pinMode(GND_C_EN, OUTPUT);

  // Prépositionement des E/S
  digitalWrite(GND_C_EN, HIGH);         // alimentation des périphériques (RTC, balance carte SD)


  //---------------------------------------------------------------------------------------------------
  // Initialisation de la balance
  balance.begin(BALANCE_DAT, BALANCE_CLK);
  balance.set_scale();

  //---------------------------------------------------------------------------------------------------
  // Calibration de la balance
  //---------------------------------------------------------------------------------------------------
  Serial.println("**************************************************");
  Serial.println("*     Tarage de la balance                       *");
  Serial.println("**************************************************");
  Serial.println("Assurez-vous que la plateau est vide");
  Serial.println("puis pressez une touche");
  while (!Serial.available());
  String yy = Serial.readString();
  Serial.print("Tarage en cours. Cette opération peut prendre plusieurs secondes... ");
  float t_tare = BALANCE_setTare();
  Serial.println("OK");
  Serial.println("");
  Serial.println("");

  Serial.println ("Résultats :");
  Serial.print ("                   Tare : ");
  Serial.println (t_tare);
  Serial.println("");
  Serial.println("");




  
  Serial.println("**************************************************");
  Serial.println("*     Calibration de la balance                  *");
  Serial.println("**************************************************");
  Serial.println("1 - Placez un poids connu dans la poubelle,");
  Serial.println("2 - Refermez le couvercle et ");
  Serial.println("3 - Saisissez le poids inséré dans la balance.");
  Serial.println("         le poids doit être exprimé en grammes et sans unités ni décimale");
  Serial.println("          ex. : 1965  pour 1965 grammes");
  Serial.println("");
  int nb_essais = 1;
  while (!Serial.available());
  Serial.print ("Etalonnage de la balance ......");
  String xx = Serial.readString();
  float poids_a_atteindre = xx.toFloat();
  balance.callibrate_scale(poids_a_atteindre, BALANCE_NB_ECHANTILLONS_TARAGE);  
  t_scale = balance.get_scale();
  Serial.println ("OK");
  Serial.println("");
  Serial.println("");
  Serial.println ("Résultats :");
  Serial.print ("                   Tare : ");
  Serial.println (t_tare);
  Serial.print ("                  Scale : ");
  Serial.println (t_scale);
  Serial.println("");
  Serial.println("Calibration terminée");
  Serial.println("");
  Serial.println("");
  Serial.println ("Pressez une touche pour lancer une mesure.");


}



void loop() {
  //---------------------------------------------------------------------------------------------------
  // Mesures en continu des poids posés dans la balance
  //---------------------------------------------------------------------------------------------------
  //while (!Serial.available());
   //String xx = Serial.readString();
  Serial.print (" Mesure... ");
  balance.set_scale(t_scale);
  float poidNew = BALANCE_pesee_balance() + (t_tare / t_scale);
  Serial.print ("> ");
  Serial.println (poidNew);
 

}
