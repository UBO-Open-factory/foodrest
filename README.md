# FoodRest
Code pour le Projet de poubelle connectée basée sur la solution Open Source [TOCIO](https://github.com/UBO-Open-factory/TOCIO-Back-office) permettant de collecter et afficher des données.

Ce projet présente le code Arduino pour un [ESP32](https://github.com/UBO-Open-factory/foodrest/tree/main/code_esp32) et un [ESP8266](https://github.com/UBO-Open-factory/foodrest/tree/main/code_esp8266).

# Signification de l'allumage de la LED rouge
Etat | Signification
--- |:---
Fixe | Ne pas ouvrir le couvercle, calcul en cour.
Clignottment très rapide | Carte SD manquante
Clignottment rapide | Problème de connection WIFI
Clignottment lent | (Mode debug uniquement) Simulation du Deepsleep.