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
   Affiche une ligne pour le debug (si c'est autorisé dans le fichier de settings).
*/
void TraceDebug(String chaine) {
  if ( configLocale.AfficheTraceDebug ) {
    Serial.println( chaine );
  }
}
