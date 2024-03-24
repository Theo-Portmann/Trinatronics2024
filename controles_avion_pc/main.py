
import websockets
import subprocess
import asyncio
import os
from concurrent.futures import ThreadPoolExecutor
import tkinter as tk
from controller import JoystickApp

profile_name = "ESP32-Access-Point1"
interface_name = "Wi-Fi"
wifi_connexion=False
app=None

def connect_to_wifi(profile_name):
    global wifi_connexion
    try:
        # Construit et exécute la commande pour se connecter au profil WiFi
        # execute la commande netsh wlan connect name=profile_name
        result = subprocess.run(['netsh', 'wlan', 'connect', f'name={profile_name}', f'interface={interface_name}'])
        # Vérifie si la commande a réussi
        if result.returncode == 0:
            wifi_connexion=True
            #print(f"Connecté au profil WiFi {profile_name} avec succès.")
        else:
            print(f"Échec de la connexion au profil WiFi {profile_name}.")
    except subprocess.CalledProcessError as e:
        print(f"Une erreur s'est produite lors de la tentative de connexion au profil WiFi {profile_name}: {e}")

def disconnect_wifi():
    input("Press Enter to disconnect from the WiFi network...\n")
    
    command = 'netsh wlan disconnect'
    try:
        subprocess.run(command, check=True, shell=True)
        print("Disconnected.")
    except subprocess.CalledProcessError as e:
        print(f"Failed to disconnect: {e}")

    # Reconnect to the previous WiFi network after disconnection
    # try:
    #     #connect_to_wifi(profile_name)
    #     connect_to_wifi("hso-securenet")
    # except subprocess.CalledProcessError as e:
    #     print(f"Failed to connect to WiFi: {e}")
    # except Exception as e:
    #     #donner aussi la ligne de l'erreur
    #     print(f"Reconnect failed: {e} trying to reconnect again to the WiFi network...")

        #arreter le programme
    os._exit(1)
async def handler():
    global app
    previous_x=100
    previous_y=100
    if not wifi_connexion:
        raise Exception("No wifi connexion")
    #mettre le timeout à 1s
    async with websockets.connect("ws://192.168.4.1/ws", open_timeout=2, close_timeout=1,ping_interval=5) as websocket:
        # Logique de gestion de la connexion WebSocket
        try:
            while 1:
                #sleep 1s(asynchronous sleep)
                #await asyncio.sleep(.01)
                await asyncio.sleep(.01)

                try:
                    #envoie d'un message compris entre 0 et 100
                    send_x=app.x
                    if send_x>previous_x+2 or send_x<previous_x-2:
                        previous_x=app.x                    
                        await asyncio.wait_for(websocket.send(str(send_x)), timeout=.1)
                        print(f"Sent: {send_x}")
                    send_y=app.y
                    if send_y>previous_y+2 or send_y<previous_y-2:
                         previous_y=app.y
                         await asyncio.wait_for(websocket.send(str(send_y)), timeout=.1)
                         print(f"Sent: {send_y}")


                except asyncio.TimeoutError:
                    pass
                    #print("Timeout")
                
               
        except websockets.exceptions.ConnectionClosedError as e:
            pass

async def connect_to_websocket():

    while True:
        try:
            await handler()
        except Exception as e:
            print(f"Connection closed: {e}")
    
            connect_to_wifi(profile_name)
            await asyncio.sleep(1.5)

            #print(f"Connection closed: {e}")
            continue


def tkinter_app():
    global app
    # Création de la fenêtre principale
    root = tk.Tk()
    root.title("Joystick Controller")
    # Création de l'application JoystickApp
    app = JoystickApp(root)
    # Lancement de la boucle principale
    root.mainloop()

async def main():
    # Création d'un ThreadPoolExecutor pour exécuter des tâches bloquantes
    #ThreadPoolExecutor permet d'exécuter des tâches bloquantes dans un thread séparé tout en restant dans un contexte asynchrone
    executor = ThreadPoolExecutor()
    # Note : disconnect_wifi est appelée sans bloquer l'event loop
    asyncio.get_event_loop().run_in_executor(executor, disconnect_wifi)
    #lancer le programme joystick, qui affiche la fenetre tkinter et qui peut se connecter à la manette
    asyncio.get_event_loop().run_in_executor(executor, tkinter_app)

    # Exécution de la tâche asynchrone principale
    try:
        await connect_to_websocket()
    except Exception as e:
        print(f"An error occurred: {e}")

# Lancement du programme
asyncio.run(main())