# Projet BotBAR - V1.0.0

1. Integration de l'ESP dans un réseaux WiFi.
2. Integration dans la memoire le code HTML.
3. Lier le code HTML au code C de L'ESP.
4. Création de la page Web dans le WiFi.

# Installation
## Récupéré le Programme

    Télécharger Via GitHub le programme [BotBar - V1.0.0](https://pages.github.com/).

    :warning: Attention : il faut avoire installer PlatformIO sur Visual Studio CODE !

## Ajouter le programme a PlatformIO

1. Ouvrire PlatfromIO sur VS CODE et faire : Open Project

2. Ajouter dans la mémoire de l'ESP le code HTML de la page Web

    ![This is an image](https://www.raspberryme.com/checklist/2022/01/1642112646_967_ESP8266-VS-Code-PlatformIO-telecharger-des-fichiers-sur-le-systeme.jpg)

3. Integration de l'ESP au WiFi personnele

    [dans src/main.cpp](src/main.cpp) remplacer les valeur du ssid et du password

    ```c
    const char* ssid = "NOM RESEAU WIFI";          // <<--- METTRE ICI VOTRE NOM RESEAU WIFI
    const char* password = "MOT DE PASSE WIFI";    // <<--- METTRE ICI VOTRE MOT DE PASSE WIFI
    ```

    Remplacer les Valeur "NOM RESEAU WIFI" par le nom de votre WiFI priver.
    Remplacer les Valeur "MOT DE PASSE WIFI" par le MDP de votre WiFI priver.

4. Téléverser le Programme avec la fleche en bas a gauche de l'écrant de VS Code.

5. Se Connecter a la page Web

    En bas a gauche de l'écrant, se trouve une prise, elle permet d'ouvrire le terminale.
    Une foie le terminale ouvert, l'ip de l'ESP doit apparaître.

## API

    Liste des requette fonctionelle :

    > /connexion
    Retourne "connexion OK"

    > /reqEtatBouton
    Retourne "En cours" puis "Terminer" en fonction des etat d'avencement de la machine