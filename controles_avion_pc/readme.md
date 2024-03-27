# Installer python avant toute chose	(3.10)

# Création d'un environnement virtuel avec venv

L'environnement virtuel (venv) est un outil qui permet d'isoler les dépendances d'un projet Python. Cela permet d'éviter les conflits entre les différentes versions des bibliothèques utilisées.

## Étapes pour créer un environnement virtuel

1. Ouvrez une fenêtre de terminal.

2. Naviguez jusqu'au répertoire racine de votre projet.

3. Exécutez la commande suivante pour créer un nouvel environnement virtuel :

  ```bash
  python -m venv nom_de_l'environnement
  ```

  Remplacez `nom_de_l'environnement` par le nom que vous souhaitez donner à votre environnement virtuel. Le nom de l'environnement n'a pas d'importance particulière.

4. Activez l'environnement virtuel en exécutant la commande appropriée selon votre système d'exploitation :

  - Sur Windows :

    ```bash
    nom_de_l_environnement\Scripts\activate
    ```


5. Une fois l'environnement virtuel activé, vous pouvez installer les dépendances spécifiques à votre projet en utilisant `pip`. Vous pouvez également installer les bibliothèques répertoriées dans le fichier `requirements.txt` en exécutant la commande suivante :

  ```bash
  pip install -r requirements.txt
  ```

Vous avez maintenant créé et activé un environnement virtuel à utiliser pour votre projet Python.


6. Lorsque vous avez terminé de travailler avec l'environnement virtuel, vous pouvez le désactiver en exécutant la commande suivante :

  ```bash
  deactivate
  ```

  Cela vous ramènera à l'environnement Python global de votre système.


## Exécution du code python

Pour exécuter le code python, exécutez le fichier `run.bat` en double-cliquant dessus. La manette PS4 est facultative.
Vous pourrez (sans la manette) contrôler l'avion en utilisant l'inteface graphique qui s'affichera à l'écran.

![image info](./pictures/joystic.png)

*mini Interface graphique*


# Connecter une manette PS4 au pc via bluetooth

1. Activer le mode bluetooth de la manette PS4 en appuyant simultanément sur les boutons PS et Share jusqu'à ce que la barre lumineuse clignote.


2. Ouvrir les paramètres bluetooth de votre pc et cliquer sur "Ajouter un appareil bluetooth ou un autre appareil".

![image info](./pictures/ps4.jpg)

*activer le mode bluetooth de la manette PS4*



3. Sélectionner "Wireless Controller" dans la liste des appareils disponibles.

4. Lancer le programme en double-cliquant sur le fichier `run.bat`.


## Structure

The code file `main.py` has the following structure:

1. Dependencies:
  - websockets
  - subprocess
  - asyncio
  - os
  - concurrent.futures
  - tkinter
  - controller (a custom module)

2. Global Variables:
  - `profile_name`: The name of the WiFi profile to connect to.
  - `interface_name`: The name of the WiFi interface to use.
  - `wifi_connexion`: An indicator of the WiFi connection status.
  - `app`: An instance of the Tkinter application.

3. Functions:
  - `connect_to_wifi(profile_name)`: Attempts to connect to a WiFi network specified by `profile_name`. If the connection is successful, it updates the global variable `wifi_connexion` to `True`.
  - `disconnect_wifi()`: Disconnects from the current WiFi connection and attempts to reconnect to another WiFi network.
  - `handler()`: Handles the WebSocket connection. Sends joystick data via the WebSocket connection.
  - `connect_to_websocket()`: Attempts to connect to the WebSocket and handle the connection.
  - `tkinter_app()`: Creates and launches the Tkinter application.
  - `main()`: Main function. Creates a ThreadPoolExecutor to execute blocking tasks, launches the Tkinter application, and attempts to connect to the WebSocket.

