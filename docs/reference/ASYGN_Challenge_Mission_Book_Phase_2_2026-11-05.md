# ASYGN Challenge Mission Book — Phase 2 (2026-11-05)

> Searchable text extraction from the user-provided PDF reference. Page labels and extracted text are retained where available; figures and page layout may not be represented. Consult the original PDF for diagrams and exact visual formatting.

---

<PARSED TEXT FOR PAGE: 1 / 19>
ASYGN                              CARNET DE MISSION – PHASE 2
CHALLENGE AIDGE                                                  SMART VIDEO SENSOR




   CARNET DE MISSION
   PHASE 2
   05/11/2026


Information       Contenu
Date              05/11/2026
Description       Ce document contient les informations concernant la deuxième phase
                  du Challenge Etudiant AIDGE. Il est à destination des étudiants inscrits
                  sur le sujet Détection et Comptage de Personnes avec la Puce
                  ColibryNPU.




                                                                                      1/19
<PARSED TEXT FOR PAGE: 2 / 19>
 ASYGN                                                                CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                                                                     SMART VIDEO SENSOR

Contents
1.    Bienvenue dans l’univers AIDGE et ColibryNPU ....................................................................................2
2.    Glossaire.................................................................................................................................................3
3.    Prérequis ................................................................................................................................................6
4.    Objectifs de la deuxième phase .............................................................................................................7
5.    Ressources matérielles ..........................................................................................................................9
6.    Ressources logicielles et documentation ............................................................................................12
7.    Feuille de route ....................................................................................................................................14
8.    Exemple de firmware : comptage de vélos .........................................................................................15
9.    Mesure de consommation ...................................................................................................................16
10. Conseils pour la phase 2 ......................................................................................................................18




                                                                                                                                                       2/19
<PARSED TEXT FOR PAGE: 3 / 19>
 ASYGN                                          CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                               SMART VIDEO SENSOR

1. Bienvenue dans l’univers AIDGE et ColibryNPU

Cher participant au Challenge AIDGE,

La première phase du Challenge est terminée et j’espère qu’elle s’est bien passée. Lors de ces premières
semaines, tu as découvert la plateforme AIDGE et développé ton premier modèle quantifié pour
ColibryNPU, capable de détecter et de localiser des personnes dans une image. Tu l’as entraîné,
quantifié, puis exporté pour ColibryNPU en estimant sa précision, son empreinte mémoire, son nombre
de cycles et son énergie par inférence.

Si tu as réussi à réaliser toutes ces étapes et que les différents concepts abordés te sont désormais clairs,
alors BRAVO ! Tu peux passer sereinement à la phase 2. Si certains points restent flous, pas d’inquiétude :
prends le temps d’en parler avec ton encadrant, car ces notions te seront précieuses pour la suite.

Place maintenant à l’embarqué : tu vas déployer ton modèle sur la carte Colibry Tiny Evalboard et
concevoir un firmware capable de détecter et de compter les personnes qui passent devant la caméra.
Ton encadrant a reçu un kit d’évaluation qui contient la carte Colibry Tiny Evalboard (avec son module
caméra), la carte Colibry Probe et un câble USB-C pour programmer ColibryNPU. Ce carnet te donne tes
objectifs, l’essentiel des informations et nos conseils pour démarrer. Le manuel utilisateur et la datasheet
seront à ta disposition pour les informations détaillées.

Avec toute mon équipe, nous te souhaitons, à toi et à tes équipiers, une nouvelle fois bonne chance et la
meilleure expérience possible avec AIDGE et ColibryNPU !

Adrien




                                                                                                      3/19
<PARSED TEXT FOR PAGE: 4 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                              SMART VIDEO SENSOR

2. Glossaire
Tu trouveras ci-dessous une liste de définitions à connaître pour ce challenge :

Architecture de ColibryNPU

    •   ColibryNPU : puce constituée d’un microcontrôleur RISC-V, d’un accélérateur neuronal (NNPA)
        et des périphériques matériels (ISP, DMA, interfaces).
    •   RISC-V : processeur 32 bits de ColibryNPU. Il exécute le firmware : configuration et orchestration
        des périphériques, exécution des algorithmes de pré/post processing, communication avec
        l’utilisateur.
    •   NNPA (Neural Network Processing Accelerator) : accélérateur matériel dédié à l’inférence des
        réseaux de neurones. Il exécute le modèle uint8 compilé, sans solliciter le RISC-V.
    •   ISP (Image Signal Processor) : périphérique qui acquiert les images du module caméra et les
        transmet directement au NNPA.
    •   DMA (Direct Memory Access) : périphérique qui transfère des données entre périphériques et
        mémoires sans passer par le RISC-V.
    •   IRQ (Interrupt Request) : signal envoyé par un périphérique pour interrompre le RISC-V et
        déclencher un traitement. Exemple : le NNPA signale la fin d’une inférence.

Mémoires

    •   SRAM : mémoire interne, rapide et volatile (son contenu est perdu hors tension). ColibryNPU en
        possède deux : celle du RISC-V (données et exécution du firmware) et celle du NNPA (poids et
        activations du modèle).
    •   Mémoire Flash : mémoire externe non volatile, sur la carte Colibry Tiny Evalboard. Elle conserve
        le firmware et le modèle hors tension, et ils sont chargés dans ColibryNPU au démarrage.
    •   Boot : phase de démarrage pendant laquelle le firmware et le modèle sont chargés depuis la
        Flash vers ColibryNPU.

Interfaces de communication

    •   FTDI : composant de la carte Colibry Probe qui convertit l’USB du PC en interfaces JTAG, SPI,
        UART et GPIO.
    •   SPI (Serial Peripheral Interface) : liaison série synchrone maître/esclave, rapide. Elle sert à
        programmer la Flash et à récupérer le flux d’images sur le PC.
    •   UART (Universal Asynchronous Receiver Transmitter) : liaison série asynchrone. Elle sert de
        console texte (messages, logs) entre le firmware et le PC.
    •   JTAG : interface de test et de debug qui donne accès au processeur pendant l’exécution, utilisée
        avec GDB.
    •   GPIO (General Purpose Input/Output) : broche numérique configurable en entrée ou en sortie
        (LED, reset, sélection…).




                                                                                                    4/19
<PARSED TEXT FOR PAGE: 5 / 19>
 ASYGN                                       CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                            SMART VIDEO SENSOR

   •   I2C : bus série deux fils pour les capteurs et la configuration des composants (par exemple le
       module caméra).
   •   BLE (Bluetooth Low Energy) : liaison sans fil basse consommation, assurée par une puce dédiée
       de la carte. Elle sert à transmettre les résultats de comptage.

Développement et programmation

   •   Firmware : programme embarqué, écrit en C/C++, exécuté par le RISC-V.
   •   ColibrySDK (Software Development Kit) : librairie de ColibryNPU qui fournit les drivers des
       périphériques et des composants de la carte, ainsi que les outils de programmation.
   •   Compilateur RISC-V : chaîne de compilation croisée qui transforme le code C/C++ du firmware
       en binaire exécutable par le RISC-V.
   •   Compilateur ColibryNPU : package Python qui transforme un modèle quantifié exporté par
       AIDGE en fichier .nnpa exécutable par le NNPA.
   •   Programmation de ColibryNPU : écriture du firmware et du modèle dans la mémoire Flash, via
       la carte Colibry Probe (USB → SPI). Ce sont deux étapes distinctes.
   •   Debug de ColibryNPU : analyse du firmware en cours d’exécution via JTAG et GDB (points
       d’arrêt, pas à pas, lecture des registres et de la mémoire), complétée par les messages envoyés
       sur l’UART.
   •   GDB : débogueur utilisé pour le debug via JTAG.

IA embarquée et traitement d’image

   •   Quantification : conversion des poids et activations d’un modèle float32 en entiers 8 bits non
       signés (uint8), pour réduire la mémoire et l’énergie consommées.
   •   Inférence : exécution du modèle sur une image pour obtenir une prédiction.
   •   Pré-processing : traitement de l’image avant l’inférence (recadrage, redimensionnement,
       normalisation).
   •   Post-processing : traitement des sorties du modèle (filtrage, tracking, comptage).
   •   Bounding box : rectangle qui localise un objet détecté dans l’image.
   •   Seuil de confiance : score minimal sous lequel une détection est ignorée.
   •   Tracking : suivi d’un même objet d’une image à l’autre.
   •   Ligne virtuelle : ligne définie dans l’image dont le franchissement déclenche le comptage.
   •   QQVGA : résolution d’image de 160 × 120 pixels.
   •   GOPS (Giga Operations Per Second) : nombre de milliards d’opérations par seconde, mesure de
       la puissance de calcul.

Mesure de consommation

   •   Power Profiler Kit II (PPK2) : appareil Nordic qui alimente la carte et mesure son courant au
       cours du temps.




                                                                                                   5/19
<PARSED TEXT FOR PAGE: 6 / 19>
ASYGN                                       CARNET DE MISSION – PHASE 2
CHALLENGE AIDGE                                                            SMART VIDEO SENSOR

 •   Résistance de shunt : résistance de faible valeur placée en série sur l’alimentation. La chute de
     tension à ses bornes est proportionnelle au courant.
 •   Énergie par inférence : énergie consommée pour une inférence, E = P_moy × durée, en µJ.




                                                                                                 6/19
<PARSED TEXT FOR PAGE: 7 / 19>
 ASYGN                                        CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                               SMART VIDEO SENSOR

3. Prérequis

Pour démarrer la phase 2 dans de bonnes conditions, tu dois avoir réussi à :

1.   Installer la plateforme AIDGE et parcourir ses tutoriels ;
2.   Lancer et analyser les exemples ASYGN ;
3.   Constituer ton jeu de données (entraînement et validation) ;
4.   Entraîner avec AIDGE un modèle float32 qui n’utilise que des opérateurs supportés par ColibryNPU ;
5.   Optimiser puis quantifier ce modèle en uint8 avec AIDGE ;
6.   Exporter ce modèle pour ColibryNPU et estimer quatre critères :
         a. Sa précision, qui doit rester proche de celle du modèle float32 ;
         b. Son empreinte mémoire : poids + pic d’activations ≤ 640 kB, soit la taille de la SRAM du
              NNPA ;
         c. Son nombre de cycles d’exécution ;
         d. Son énergie par inférence.

Remarque importante

Pour chaque personne détectée, le modèle doit fournir une classe, un score de confiance et une
bounding box. La bounding box est indispensable à l’algorithme de tracking de la phase 2.




                                                                                                 7/19
<PARSED TEXT FOR PAGE: 8 / 19>
 ASYGN                                           CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                               SMART VIDEO SENSOR

4. Objectifs de la deuxième phase

Ton objectif pour la phase 2 est d’utiliser la carte Colibry Tiny Evalboard pour détecter et compter les
personnes qui passent devant la caméra, comme l’illustre la figure ci-dessous.




Pour cela, tu dois concevoir un firmware qui enchaîne les étapes suivantes :

    1. Acquérir les images du module caméra, mises en forme par l’ISP si nécessaire ;
    2. Exécuter l’inférence du modèle sur le NNPA ;
    3. Appliquer le post-processing aux sorties du modèle : filtrage par seuil de confiance, tracking,
       détection du franchissement de la ligne virtuelle, comptage ;
    4. Transmettre le comptage via BLE.

Tu pourras aussi utiliser la carte pour acquérir de nouvelles images et enrichir ton jeu de données. Ces
images, prises par la caméra réelle, réduisent l’écart entre ton jeu d’entraînement et les conditions
terrain. Il faudra ensuite réentraîner, quantifier et exporter à nouveau ton modèle avec AIDGE.

Enfin, tu mesureras la consommation de la carte pour en déduire l’énergie par inférence, et tu la
compareras aux estimations obtenues lors de l’export du modèle (voir la remarque en section 9).

Tu trouveras en section 7 plus d’informations sur la démarche conseillée à suivre.

À la fin de cette phase, tu seras capable de :

    •   Programmer ColibryNPU avec ton firmware et ton modèle ;
    •   Récupérer sur ton PC le flux d’images de la carte ;
    •   Concevoir une chaîne embarquée complète : acquisition, pré-processing, inférence, post-
        processing, communication ;
    •   Mesurer la consommation de la carte et en déduire l’énergie par inférence.

Le tableau ci-dessous résume la phase 2.

 Phase 2 – Prise en main du kit d’évaluation ColibryNPU et premiers tests terrain




                                                                                                     8/19
<PARSED TEXT FOR PAGE: 9 / 19>
ASYGN                                      CARNET DE MISSION – PHASE 2
CHALLENGE AIDGE                                                           SMART VIDEO SENSOR

Durée                                         Du 05/11/2026 au 03/12/2026
Accès aux ressources                         Carnet de mission de la phase 2.

                                      Cartes Colibry Tiny Evalboard et Colibry Probe.

                                   Manuel utilisateur de la carte Colibry Tiny Evalboard.

                                               Datasheet de la puce ColibryNPU
Objectifs               Utiliser les cartes Colibry Tiny Evalboard et Colibry Probe afin de récupérer
                                         un flux d'images et l’afficher sur l’ordinateur

                        Utiliser la carte Colibry Tiny Evalboard pour transmettre des informations à
                                          un PC en utilisant le module Bluetooth BLE

                           Exporter avec la plateforme AIDGE le modèle uint8 pour ColibryNPU
                            Concevoir et déployer une chaîne de traitement d’une image dans
                                ColibryNPU et intégrer l’appel du modèle dans cette chaîne.
                       (Acquisition des images -> inférence IA -> postprocessing -> transfert BLE des
                                                          résultats)
                       Caractériser la consommation énergétique de la carte Colibry Tiny Evalboard




                                                                                                 9/19
<PARSED TEXT FOR PAGE: 10 / 19>
 ASYGN                                          CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                               SMART VIDEO SENSOR

5. Ressources matérielles
Les ressources matérielles disponibles sont présentées ci-dessous.

 Colibry Tiny Evalboard               Colibry Probe                      Câble USB-C




 Carte principale de l’application.   Carte d’interface entre le PC et   à utiliser pour relier la carte
 Elle intègre ColibryNPU, le          la Colibry Tiny Evalboard : elle   Colibry Probe au PC.
 module caméra et la puce BLE.        permet la programmation, le
                                      debug et la récupération du flux
                                      d’images.



Description de la carte Colibry Tiny Evalboard

Les composants principaux de la carte Colibry Tiny Evalboard sont présentés ci-dessous.




 1.   ColibryNPU : RISC-V 32 bits (300 MHz, 128 kB de SRAM) et NNPA (9,6 GOPS, 640 kB de SRAM)
 2.   Connecteur pour pile CR927
 3.   Accéléromètre*
 4.   Microphone MEMS*
 5.   Puce BLE : transmission des résultats de comptage
 6.   Module caméra : capteur d’images QQVGA
 7.   Mémoire Flash : stockage du firmware et du modèle




                                                                                                      10/19
<PARSED TEXT FOR PAGE: 11 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                              SMART VIDEO SENSOR


 * Non utilisé dans ce challenge.




Description de la carte Colibry Probe

La carte Colibry Probe est présentée ci-dessous.




La carte Colibry Probe relie ColibryNPU au PC grâce à son FTDI, qui offre quatre interfaces :

    Interface FTDI      Description
    USB vers JTAG       Debug du firmware avec GDB.
     USB vers SPI       Programmation de la Flash et récupération du flux d’images
    USB vers UART       Console série du firmware (messages, logs de debug).
                        Pilotage du RESET de ColibryNPU, de la GPIO NRF_P0.22 de la puce BLE et du
    USB vers GPIO
                        multiplexeur SPI qui connecte le bus soit à ColibryNPU, soit à la Flash.


Remarque importante

La carte Colibry Probe est une interface de développement. En fonctionnement autonome, elle n’est pas
nécessaire : la carte Colibry Tiny Evalboard est alimentée par une pile CR927, une batterie ou une
alimentation externe.

Utilisation de la carte Colibry Probe avec la carte Colibry Tiny Evalboard

Branche la Colibry Tiny Evalboard sur la Colibry Probe comme indiqué ci-dessous, en respectant le sens
d’insertion.




                                                                                                11/19
<PARSED TEXT FOR PAGE: 12 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                               SMART VIDEO SENSOR




Une fois le câble USB-C relié au PC, la Probe alimente la carte et permet, avec les outils de
développement, de :

    •   Programmer le firmware et le modèle en Flash ;
    •   Débuguer le firmware (JTAG/GDB) et lire sa console série (UART) ;
    •   Récupérer le flux d’images ;
    •   Piloter les GPIO spécifiques (RESET, multiplexeur SPI, NRF_P0.22).

Côté PC, le FTDI apparaît sous la forme de plusieurs ports, un par interface. Le manuel utilisateur indique
leur correspondance.

Remarque importante

Quand la carte est reliée à la Colibry Probe, le câble USB-C alimente les deux cartes. Ne branche ni pile
bouton ni alimentation externe en même temps car deux sources simultanées peuvent endommager le
matériel.

6. Ressources logicielles et documentation
Les ressources logicielles, les exemples et la documentation sont disponibles sur :
https://asygn.com/colibry-resources/

L’accès se fait avec un compte qu’ASYGN a créé pour toi : tu as dû recevoir un mail d’invitation. Si ce
n’est pas le cas, vérifie tes spams ou contacte un administrateur ASYGN (voir la section Support du
carnet de mission de la phase 1).

Documentation

Les documents disponibles sont présentés ci-dessous.




                                                                                                    12/19
<PARSED TEXT FOR PAGE: 13 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                             SMART VIDEO SENSOR




 Document                                             Document
 Manuel utilisateur de la Colibry Tiny Evalboard.     Datasheet de ColibryNPU.

 Contenu                                              Contenu
 Installation des outils, exemples d’utilisation      Description de la puce et de ses périphériques
 (dont flux d’images et BLE), fonctionnalités de la   (RISC-V, NNPA, ISP, DMA, interfaces).
 carte.
                                                      Quand l’utiliser
 Quand l’utiliser                                     Pendant la conception du firmware.
 Dès le début, pour la prise en main.


Outils de programmation

Les outils de programmation disponibles sont présentés ci-dessous.




 Outil                     Outil                      Outil                      Outil
 ColibrySDK                Compilateur                Service web                Compilateur RISC-V
                           ColibryNPU                 ColibryNPU
 Rôle                                                                            Rôle
 Drivers des               Rôle                       Rôle                       Compile le firmware du
 périphériques de          Compile le modèle          Équivalent interactif du   RISC-V.
 ColibryNPU et des         uint8 exporté par          compilateur : compile
 composants de la          AIDGE en fichier .nnpa     le modèle et estime ses




                                                                                                  13/19
<PARSED TEXT FOR PAGE: 14 / 19>
 ASYGN                                          CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                                 SMART VIDEO SENSOR

 carte, exemples, outils    exécutable par le          métriques (empreinte
 de programmation du        NNPA.                      mémoire, cycles,
 firmware et du modèle                                 énergie par inférence).
 en Flash.


Le modèle et le firmware suivent deux chaînes distinctes, avec des outils différents :

    1.   Entraîner, quantifier et exporter le modèle avec AIDGE ;
    2.   Compiler le modèle avec le compilateur ColibryNPU (.nnpa) ;
    3.   Compiler le firmware avec le compilateur RISC-V ;
    4.   Programmer séparément le modèle et le firmware dans la Flash, avec les outils du SDK.

Au boot, le firmware et le modèle sont chargés depuis la Flash vers ColibryNPU.

7. Feuille de route
Cette section liste les étapes à suivre pour la phase 2. Chaque étape s’appuie sur la précédente. Ne
passe à la suivante qu’une fois le critère de validation atteint. Les commandes et procédures sont dans
le manuel utilisateur.

           Étape                     Objectif                  Validation             Ressources
 1. Prise en main           Installer les outils,      L’exemple tourne sur la    Manuel
                            compiler et                carte et ses messages
                            programmer un              s’affichent sur la
                            exemple du SDK.            console série.
 2. Flux d’images           Récupérer les images       Images affichées sur le    Manuel et exemple
                            de la caméra sur le PC     PC, nouvelles images       fourni dans le SDK
                            et enrichir ton jeu de     ajoutées au jeu de
                            données.                   données.
 3. Déploiement du          Compiler ton modèle,                                  Manuel, section 8 du
 modèle                     le programmer en                                      carnet de mission
                            Flash
 4. Post-processing         Implémenter le filtrage,   Comptage correct sur       Datasheet, section 8 du
                            le tracking et le          des passages de test,      carnet de mission
                            comptage                   vérifié via la console
                                                       série ou en allumant la
                                                       LED utilisateur (USER
                                                       LED) à chaque
                                                       franchissement
                                                       compté.
 5. Transmission BLE        Envoyer le comptage        Compteur reçu sur le       Manuel, exemple
                            au PC via la puce BLE.     PC.                        fourni
 6. Mesure de               Mesurer la                 Trace de                   Section 9 du carnet de
 consommation               consommation de la         consommation,              mission




                                                                                                   14/19
<PARSED TEXT FOR PAGE: 15 / 19>
 ASYGN                                           CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                                 SMART VIDEO SENSOR

                              carte et en déduire        comparaison avec les
                              l’énergie par inférence    estimations du
                                                         compilateur



8. Exemple de firmware : comptage de vélos
Cet exemple (code source fourni dans le SDK) montre une chaîne de détection, de tracking et de
comptage de vélos. La même architecture s’applique aux personnes, objectif du challenge.

L’illustration ci-dessous décrit le flot du firmware.




Le firmware orchestre les étapes suivantes. Une partie d’entre elles est exécutée de manière
asynchrone par les périphériques matériels :

       Étape              Exécutée par                                   Description
 1. Acquisition      ISP, DMA, NNPA            Le RISC-V configure l’ISP et le DMA. L’ISP acquiert les
 et inférence                                  images de la caméra et réalise un éventuel pré-processing,
                                               puis les transmet au NNPA, qui exécute l’inférence.
 2. Filtrage         RISC-V                    En fin d’inférence, une IRQ réveille le RISC-V. Il récupère les
                                               sorties du modèle et ignore les détections sous le seuil de
                                               confiance.
 3. Tracking         RISC-V                    À partir des bounding boxes, il associe les détections d’une
                                               image à l’autre et attribue un identifiant unique à chaque
                                               objet suivi.
 4. Comptage         RISC-V                    Il incrémente le compteur lorsqu’un objet suivi franchit la
                                               ligne virtuelle, dans un sens ou dans l’autre. Un même
                                               identifiant n’est compté qu’une seule fois par
                                               franchissement.




                                                                                                       15/19
<PARSED TEXT FOR PAGE: 16 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                              SMART VIDEO SENSOR




Entre deux inférences, le RISC-V peut rester en veille. C’est important pour réduire la consommation.

Remarque importante

Pour le challenge, ajoute à cette chaîne la transmission du comptage via BLE (étape 5 de la feuille de
route).



9. Mesure de consommation
La consommation de la carte Colibry Tiny Evalboard se mesure avec un PPK2 en mode Source Meter : il
alimente la carte et mesure son courant. Relie VOUT du PPK2 à EXT_VBAT et GND à GND sur le
connecteur J2, et applique 2 V.




                                                                                                   16/19
<PARSED TEXT FOR PAGE: 17 / 19>
 ASYGN                                       CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                            SMART VIDEO SENSOR




Remarque importante

Débranche la Colibry Probe pendant la mesure car le PPK2 doit être la seule source d’alimentation. Ton
firmware doit donc démarrer seul depuis la Flash.



La trace de consommation donne à la fois la puissance consommée et la durée de chaque phase du
firmware. L’exemple ci-dessous, issu d’une application audio, enchaîne le calcul d’un spectrogramme
Log-Mel puis une inférence sur le NNPA. L’énergie de l’inférence vaut P_moy × durée = 2,35 mW × 22 ms
≈ 52 µJ.




                                                                                                17/19
<PARSED TEXT FOR PAGE: 18 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                              SMART VIDEO SENSOR




Remarque importante

La mesure se fait aux bornes de la carte. Elle inclut donc ColibryNPU en entier, la caméra, la puce BLE et
les régulateurs, en activité comme en veille. Les estimations obtenues lors de l’export ne couvrent que
l’inférence sur le NNPA. Pour comparer les deux, isole la phase d’inférence sur la trace (tu peux négliger
la consommation de fond si tu as pris soin de la réduire).

Pour réduire la consommation, joue sur la veille du RISC-V entre deux inférences, la cadence
d’acquisition et la fréquence des transmissions BLE.

À défaut de PPK2, tu peux mesurer le courant avec une résistance de shunt en série sur l’alimentation,
en relevant à l’oscilloscope la chute de tension à ses bornes.




10. Conseils pour la phase 2

Format d’entrée du modèle
Ton modèle doit recevoir des images au format de la caméra (QQVGA, niveaux de gris). Si ce n’est pas le
cas, adapte-le dès l’étape 2 de la feuille de route, avec les images de la carte.

Boucle d’amélioration
Chaque enrichissement du jeu de données implique de réentraîner, quantifier, exporter et recompiler le
modèle. Prévois plusieurs itérations pour améliorer la précision de ton modèle.




                                                                                                   18/19
<PARSED TEXT FOR PAGE: 19 / 19>
 ASYGN                                         CARNET DE MISSION – PHASE 2
 CHALLENGE AIDGE                                                              SMART VIDEO SENSOR


Conditions terrain
Acquiers tes images dans des conditions proches du test final (éclairage, distance, angle de la caméra).
Elles auront de l’influence sur la précision du comptage.

Debug UART/USER LED avant BLE
Valide le post-processing sur la console série avant de passer au BLE.

RISC-V
Le post-processing tourne sur le RISC-V et doit tenir dans ses 128 kB de SRAM. Son temps d’exécution
augmente l’énergie consommée par image.

Caractérise dès que possible la consommation
Fais une première mesure de consommation dès que le firmware tourne, sans attendre la fin. Elle
guidera tes choix d’optimisation.




                                                                                                   19/19
