/**
   Function pour la balance.
*/
//*********************************************************************
//       Mise au point de la cablibration de la balance
//       auteur : Laurent Marchal & Alexandre PERETJATKO
//       date : 17/03/2021
//
//        calibration d'un capteur de poids (0/20 kg) associé
//        à une carte sparkfun load Cell Amp basée sur le
//        circuit HX711.
//
//        Utilisation d'une carte FireBeetle ESP8266
//
//*********************************************************************
//
//        Nécessite la bibliothèque HX711.h disponible dans le
//        répertoire Librairies provenant de https://github.com/bogde/HX711
//
//*********************************************************************
//


// bibliothèque pour la balance
#include <HX711.h>
HX711 balance;




// ---------------------------------------------------------------------------------------------------
/**
   Renvoie le poid mesuré en grammes.

   float valeurTarage : Valeur de tarage.

   @return float : valeur en grammes.
*/
float BALANCE_getPeseeBalance(float valeurTarage) {
  balance.set_scale(configLocale.calibrationFactor);
  balance.set_offset(valeurTarage);

  return  balance.get_units(BALANCE_NB_ECHANTILLONS_PESEE);
}


/*
   Tarage de la balance.
   @return float contenant la valeur de tarage.
*/
float BALANCE_setTare() {
  balance.set_scale();
  balance.tare(BALANCE_NB_ECHANTILLONS_TARAGE);
  return balance.get_offset();
}




// ---------------------------------------------------------------------------------------------------
/**
   Calibraiton de la balance.
*/
void BALANCE_setup() {
  // début de la procédure de calibration usine de la balance
  Serial.println("");
  Serial.println("");
  Serial.println("*************************************************");
  Serial.println("*     FOOD REST : calibration de la balance     *");
  Serial.println("*************************************************");
  Serial.println("Lorsque le programme vous demandera de presser une touche, vous devez la valider par [Enter]");
  Serial.println("");
  Serial.println("");

  Serial.println("**************************************************");
  Serial.println("*     ETAPE 1/5 : tarage de la balance           *");
  Serial.println("**************************************************");
  Serial.println(" - Assurez-vous que la poubelle est complètement vide");
  Serial.println(" - Equipez la poubelle avec un sac vide");
  Serial.println(" - puis pressez une touche");
  while (!Serial.available());
  while (Serial.available()) Serial.read();
  Serial.print("Tarage en cours. Cette opération peut prendre plusieurs secondes... ");

  balance.set_scale();
  balance.tare(BALANCE_NB_ECHANTILLONS_TARAGE);                // reset the scale to 0 on 20 echantillons


  configLocale.valeurDeTarage = balance.get_offset();
  Serial.println("la valeure mesurée de tare est (pour information) : ");
  Serial.println(configLocale.valeurDeTarage);
  Serial.println("");
  Serial.println("");


  // Etalonnage de la balance
  Serial.println("**************************************************");
  Serial.println("*     ETAPE 2/5 : calibration de la balance      *");
  Serial.println("**************************************************");
  Serial.println("1 - Placez un poids connu dans la poubelle,");
  Serial.println("2 - Refermez le couvercle et ");
  Serial.println("3 - Saisissez le poids inséré dans la balance.");
  Serial.println("         le poids doit être exprimé en grammes et sans unités ni décimale");
  Serial.println("          ex. : 1965  pour 1965 grammes");

  // Lecture d'une saisie d'un  poids
  while (!Serial.available());
  String xx = Serial.readString();
  float poids_a_atteindre = xx.toFloat();
  Serial.println("Patience je calcul ... et c'est dur...");

  // Facteur de calibration
  float poids_lu = balance.get_units(BALANCE_NB_ECHANTILLONS_PESEE);
  float result = poids_lu / poids_a_atteindre;
  balance.set_scale(result);
  configLocale.calibrationFactor = balance.get_scale();

  Serial.print ("Scale : ");
  Serial.println(configLocale.calibrationFactor);
  Serial.println("Calibration terminée");
  Serial.println("");
  Serial.println("");

  // enregistrement des informations
  Serial.println("***************************************************");
  Serial.println("*     ETAPE 3/5 : enregistrement des informations *");
  Serial.println("***************************************************");
  Serial.print("Le poids actuellement dans la balance est de : ");
  Serial.print(BALANCE_getPeseeBalance(configLocale.valeurDeTarage));
  Serial.println("g");
  Serial.println("");

  // Sauvegarde de la configuration locale dans le fichier de settings
  configLocale.laValeurDeTarageInitiale  = configLocale.valeurDeTarage;
  configLocale.InitialisationUsine    = false; // Pour ne pas refaire la configuration d'usine.
  configLocale.AfficheTraceDebug      = false;
  configLocale.poidOld                = 0;   // dernière pesée pour le poids différentiel

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
    Serial.print("Poids mesuré : ");
    float pesee_tmp = BALANCE_getPeseeBalance(configLocale.valeurDeTarage);
    Serial.print(pesee_tmp, 1);
    Serial.print(" g => arrondi : ");
    pesee_tmp = round(pesee_tmp);
    Serial.println(pesee_tmp);


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


/*
   Renvoie une marge de tolerance pour considerer que la poubelle est vide
   @param float poid : dernier poids mesuré
   @return int : marge d'erreur
 * */
int BALANCE_getMargeErreurVidange(float poid) {
  //if (poid < 500) return 10; else return 20;
  return BALANCE_MARGE_POIDS_SAC;
}
