# Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite
Implémentation d'un capteur de déformation/jauge de contrainte à base de graphite 3H à 3B. La mesure de résistance de ce type de crayon nécessite une amplification nous avons donc crée un Shield Arduino pour le traitement du signal et son affichage.

## Livrables
## Hardware utilisé

# Simulation du circuit amplificateur sur LTspice
Un circuit de transimpédance est utilisé pour elever le faible signal du capteur (100nA) à un signal lisible pour le microcontroleur de l'Arduino.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/Circuicuit%20et%20etages.PNG)

Differents étages de filtres passes-bas permettent de couper les hautes fréquences et le 50 Hz et ainsi diminuer le bruit sur la mesure.
  - Fréquence de coupure de l'étage 1 : 16 Hz
  - Fréquence de coupure de l'étage 2 : 1.6 Hz
  - Fréquence de coupure de l'étage 3 : 1.6 kHz

La simulation permet de verifier qu'à 50 Hz le signal est atténué de -40 dB, le bruit du secteur seras alors grandement diminié et ne perturberas pas la mesure. 

# Design du PCB sur KiCAD et fabrication du shield

Le PCB inclu l'amplificateur de transimpédance et différents modules permettant l'affichage ou le traitment des données. En partant d’un template d’une carte Arduino Uno, il a fallu :
  - Reproduire la schématique du circuit transimpédance et créer de nouveaux symboles pour les modules spécifiques à notre projet.
  - Concevoir les empreintes réelles de ces modules en prenant en compte leurs caractéristiques physiques (nombres de pins, distance entre les pins, géométrie des empreintes…)
  - Effectuer le routage du circuit à partir du chevelu
  - Poser le plan de masse pour référencer les pistes au GND

## Schema et empreintes des modules

Module Bluetooth HC 05 : ce module permet la communication des données vers une application androide. L'implemantation d'un échange entre une application et le microcontrolleur permet le stockage de donnée que l'arduino ne permettrais pas. Il permet aussi d'avoir une vue d'ensemble sur les mesures que le petit écran ne permttrais pas.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/Bluetooth.jpg)

L'écran OLED : permet l'affichage des mesures et la selection du gain de l'amplificateur.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/ecran%20OLED.jpg)

L'encodeur rotatoire : permet la navigation dans les menus. Le debouncing de l'encodeur a été réalisé en ajoutant des capacités sur les pins.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/Rotary%20Sensor.jpg)

L'amplificateur de transimpédance avec le capteur graphite.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/PCB%20ampli.jpg)

La résistance variable digitale MCP41050 : elle permet de changer la résistance R2 de l'amplificateur pour changer le gain.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/MCP.jpg)



## Schema du PCB complet

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/PCB.PNG.jpg)

## Fabrication du shield

Le shield est réalisé sur des plaques d'époxy recouvertes d'une couche de cuivre. Une résine photosensible est appliquée sur la plaque avec par dessus, le calque du circuit. Le tout est ensuit placé sous lumière UV pour faire polymeriser la résine au endroits non protégé par le calque. La plque d'époxy est ensuite plongé dans du perchlorure de fer pour retirer la couche de cuivre qui n'est pas protégé par la résine, faisant ainsi apparaître les circuits.

L'étape suivante est de percer les trous pour faire passer les pins de la carte et les composants. Une fois les trous percés, on soude les composants et les headers à la plaque.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/Photo%20shield.jpg)

# Application android

Le petit écran OLED ne permet pas d'afficher un graph et la petite mémoire de l'arduino ne permet pas de conserver les mesures. Le rôle de l'application android est de pouvoir faciliter l'affichage et le stockage des valeures de déformation.
Pour utiliser l'appliication il faut, dans un premier temps, appairer le telephone au module Bleutooth grâce au menu déroulant de l'app (ne pas oublier d'autoriser l'application à utiliser le bluetooth).
Si le télephone est connecté, l'application recevras les données en continu et les reporteras sur le graphique déroulant. En parrallèle, l'application envoi aussi toutes les mesures sur une page google sheets pour le stockage.

![](https://github.com/GaetanDelpech/Projet-MOSH-INSA-Toulouse-2023-2024-Capteur-Graphite/blob/main/images/App%20front.PNG)


