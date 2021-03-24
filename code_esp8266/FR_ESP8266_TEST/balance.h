/**
   Function pour la balance.
*/
//*********************************************************************
//       Mise au point de la cablibration de la balance
//       Version : 1
//       auteur : Laurent Marchal
//       date : 17/03/2021
//
//        calibration d'un capteur de poids (0/20 kg) associé
//        à une carte sparkfun load Cell Amp basée sur le
//        circuit HX711.
//
//        Utilisation d'une carte FireBeetle ESP8266 pour l'essai
//
//*********************************************************************
//
//        Nécessite la bibliothèque HX711.h
//
//*********************************************************************
//


// bibliothèque pour la balance
#include "HX711.h"
HX711 balance;




// ---------------------------------------------------------------------------------------------------
/**
   Renvoie le poid mesuré en grammes.
*/
float pesee_balance() {
  // Adjust to this calibration factor
  balance.set_scale(configLocale.calibrationFactor);
  return balance.get_units(GLOBAL_nb_echantillons_mesure);
}




// ---------------------------------------------------------------------------------------------------
void mesure_du_poids_sur_la_balance() {
  //Adjust to this calibration factor
  balance.set_scale(configLocale.calibrationFactor);
  Serial.println("");
  Serial.print("Poids mesuré : ");
  Serial.print(pesee_balance(), 1);
  Serial.println(" g");
  Serial.println("");
}




// ---------------------------------------------------------------------------------------------------
/**
   Calibraiton de la balance.
*/
void CZL635_setup() {
  // début de la procédure de calibration usine
  Serial.println("");
  Serial.println("");
  Serial.println("*************************************************");
  Serial.println("*     FOOD REST : calibration de la balance     *");
  Serial.println("*************************************************");
  Serial.println("");
  Serial.println("");
  Serial.println("lorsque le programme vous demandera de presser une touche");
  Serial.println("       Pressez une touche puis la touche [Enter].");
  Serial.println("");
  Serial.println("");

  // Tarage de la poubelle
  Serial.println("**************************************************");
  Serial.println("*     ETAPE 1/5 : tarage de la balance           *");
  Serial.println("**************************************************");
  Serial.println("");
  Serial.println("1 - Assurez-vous que la poubelle est compètement vide");
  Serial.println("2 - Equipez la poubelle d'un sac");
  Serial.println("3 - puis pressez une touche");
  while (!Serial.available());
  while (Serial.available()) Serial.read();
  Serial.print("Tarage en cours... ");
  balance.tare(GLOBAL_nb_echantillons_tarage);
  Serial.println("OK");
  Serial.println("");
  Serial.println("");

  // Etalonnage de la balance
  Serial.println("**************************************************");
  Serial.println("*     ETAPE 2/5 : calibration de la balance      *");
  Serial.println("**************************************************");
  Serial.println("");
  Serial.println("1 - Placez un poids connu dans la poubelle,");
  Serial.println("2 - Refermez le couvercle et ");
  Serial.println("3 - Saisissez le poids inséré dans la balance.");
  Serial.println("         le poids doit être exprimé en grammes et sans unités ni décimale");
  Serial.println("          ex. : 1965  pour 1965 grammes");
  Serial.println("");
  int nb_essais = 1;

  while (!Serial.available());
  String xx = Serial.readString();
  float poids_a_atteindre = xx.toFloat();
  bool fin_calcul = false;

  while (!fin_calcul) {
    float diff = pesee_balance() - poids_a_atteindre;
    Serial.print("Différence mesurée " + String(nb_essais) + "/" + String(GLOBAL_nb_essais_calibration) + " : ");
    Serial.println(diff);
    if (abs(diff) < GLOBAL_precision_calibration) {
      break;
    } else {
      configLocale.calibrationFactor = configLocale.calibrationFactor + GLOBAL_increment_fc * (diff / abs(diff));
    }
    if (nb_essais ++ > GLOBAL_nb_essais_calibration) {
      Serial.println("Echec de la procédure de calibration");
      Serial.println("Pressez une touche pour recommencer.");
      while (!Serial.available());
      while (Serial.available()) Serial.read();
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("Calibration terminée");
  Serial.println("");
  Serial.println("");

  // enregistrement des informations
  Serial.println("***************************************************");
  Serial.println("*     ETAPE 3/5 : enregistrement des informations *");
  Serial.println("***************************************************");


  Serial.print("La nouvelle valeur du facteur de calibration est : ");
  Serial.println(configLocale.calibrationFactor);
  Serial.println("");
  Serial.print("Le poids actuellement dans la balance est de : ");
  Serial.print(pesee_balance());
  Serial.println("g");
  Serial.println("");


  // recalcul la tare
  Serial.println("");
  Serial.println("Enlevez le poids mis précédemment dans la balance.");
  Serial.println("Puis pressez une touche.");
  while (!Serial.available());
  if (Serial.available()) Serial.read();
  balance.set_scale(configLocale.calibrationFactor);
  balance.tare(GLOBAL_nb_echantillons_tarage);
  configLocale.valeurDeTarage = balance.get_tare();
  Serial.println("");
  Serial.println("La tare a été ajustée.");
  Serial.println("");
  Serial.println("Sauvegarde de ces deux paramètres sur la carte SD :");
  Serial.print("     Facteur de calibration : ");
  Serial.println(configLocale.calibrationFactor, 1);
  Serial.print("                       Tare : ");
  Serial.println(configLocale.valeurDeTarage);


  // Sauvegarde du configurationFactor
  //      configLocale.calibrationFactor
  //      configLocale.valeurDeTarage
  configLocale.InitialisationUsine = false; // Pour ne pas refaire la configuration d'usine.
  SD_EraseSettings();
  SD_WriteSettings( configLocale );


  Serial.println("OK");
  Serial.println("");
  Serial.println("Fin de la procédure de calibration");

  Serial.println("");
  Serial.println("");
  Serial.println("***************************************************");
  Serial.println("*     ETAPE 4/5 : vérification des mesures        *");
  Serial.println("***************************************************");
  Serial.println("La balance va peser en continue pour que vous puissiez vérifier l'étalonnage.");
  Serial.println("");
  Serial.println("Pressez 'x' pour quitter le programme détalonnage.");
  while (true) {
    mesure_du_poids_sur_la_balance();
    if (Serial.available()) {
      char cc = Serial.read();
      if (cc == 'x' or cc == 'X') {
        break;
      }
    }
  }

  Serial.println("");
  Serial.println("");
  Serial.println("***************************************************");
  Serial.println("*     ETAPE 5/5 : redémarrage de la balance       *");
  Serial.println("***************************************************");
  Serial.println("La balance va redémarrer puis");
  Serial.println("réaliser une mesure et se rendormir.");
  Serial.println("");
  Serial.println("Pressez une touche pour lancer la première mesure.");
  Serial.println("Ne touchez pas la balance pendant la mesure.");
  while (!Serial.available());
  while (Serial.available()) Serial.read();
  ESP.restart();
}
