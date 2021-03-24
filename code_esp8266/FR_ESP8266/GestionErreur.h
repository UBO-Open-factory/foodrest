/**
   Ce fichier contient toutes les fonctions pour la gestion des erreurs.
*/



/**
   Affiche un message d'erreur
*/
void AfficheErreur(String chaine) {
  Serial.println(chaine);
}

/**
   Affiche une ligne pour le debug
   @see : http://www.pinon-hebert.fr/Knowledge/index.php/ARDUINO_predefined_constants

  void TraceDebug(String chaine) {
  if ( configLocale.AfficheTraceDebug ) {
    Serial.print(__FILE__); Serial.print(" ");
    Serial.print(__DATE__); Serial.print(" ");
    Serial.print(__TIME__); Serial.print(" ");
    Serial.print(__func__); Serial.print(" ");
    Serial.print(__LINE__); Serial.print(":");
    Serial.println( chaine );
  }
  }
*/
#define TraceDebug(message) Serial.print("DEBUG("); Serial.print(__FILE__); Serial.print(" "); Serial.print(__func__); Serial.print(","); Serial.print(__LINE__); Serial.print(")>"); Serial.println(message);

