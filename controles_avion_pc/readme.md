# Installer python avant toute chose	




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

  - Sur macOS/Linux :

    ```bash
    source nom_de_l_environnement/bin/activate
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


# Exécution du code python

Pour exécuter le code python, exécutez le fichier `run.bat` en double-cliquant dessus.
