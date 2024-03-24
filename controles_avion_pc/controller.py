import tkinter as tk
import pygame
from concurrent.futures import ThreadPoolExecutor
import asyncio
import time



class JoystickApp:
    def __init__(self, master):

        manette=False
        self.x = 100
        self.y = 100
        self.master = master
        master.title("Joystick Example")

        self.joystick_size = 20  # Taille du joystick pour faciliter les calculs

        # Configuration du joystick horizontal
        self.horizontal_joystick = tk.Canvas(master, width=200, height=100, bg='lightgray')
        self.horizontal_joystick.grid(row=0, column=0)
        self.horizontal_handle = self.horizontal_joystick.create_oval(90, 40, 110, 60, outline='black', fill='yellow', width=2)


        # Configuration du joystick vertical
        self.vertical_joystick = tk.Canvas(master, width=100, height=200, bg='lightgray')
        self.vertical_joystick.grid(row=0, column=1)
        self.vertical_handle = self.vertical_joystick.create_oval(40, 90, 60, 110,outline='black', fill='yellow', width=2)

        # Ajout des événements de mouvement
        self.horizontal_joystick.bind("<B1-Motion>", self.move_horizontal)
        self.vertical_joystick.bind("<B1-Motion>", self.move_vertical)
        self.horizontal_joystick.bind("<ButtonRelease-1>", self.reset_horizontal)
        self.vertical_joystick.bind("<ButtonRelease-1>", self.reset_vertical)

        pygame.init()
        pygame.joystick.init()
        print("Info: Veuillez connecter la manette avant de lancer le programme")
        try:
            self.joystick = pygame.joystick.Joystick(0)
            self.joystick.init()
            manette=True
            print("Manette PS4 connectée")


        except pygame.error:
            print("Manette PS4 non trouvée")


        # Lancer le thread d'écoute du contrôleur
        #dabord on creer une loop
        

        if manette:                
            loop = asyncio.new_event_loop()
            asyncio.set_event_loop(loop)
                    
            executor = ThreadPoolExecutor()
            asyncio.get_event_loop().run_in_executor(executor, self.listen_to_controller)



    def move_horizontal(self, event):
        """Gère le mouvement horizontal du joystick."""
        canvas_width = self.horizontal_joystick.winfo_width()
        self.x = event.x
        if self.x < self.joystick_size // 2-10:
            self.x = self.joystick_size // 2-10
        elif self.x > canvas_width - self.joystick_size // 2+6:
            self.x = canvas_width - self.joystick_size // 2+6
        y = 50  # Y est fixe puisque le mouvement est horizontal
        
        self.horizontal_joystick.coords(self.horizontal_handle, self.x-10, y-10, self.x+10, y+10)

    def move_vertical(self, event):
        """Gère le mouvement vertical du joystick."""
        canvas_height = self.vertical_joystick.winfo_height()
        self.y = event.y
        if self.y < (self.joystick_size+1) // 2-10:
            self.y = self.joystick_size // 2 - 10
        elif self.y > canvas_height - self.joystick_size // 2 +6:
            self.y = canvas_height - self.joystick_size // 2 +6
        x = 50  # X est fixe puisque le mouvement est vertical
        self.vertical_joystick.coords(self.vertical_handle, x-10, self.y-10, x+10, self.y+10)

    def reset_horizontal(self, event):
        self.horizontal_joystick.coords(self.horizontal_handle, 90, 40, 110, 60)
        self.x = 100

    def reset_vertical(self, event):
        self.vertical_joystick.coords(self.vertical_handle, 40, 90, 60, 110)
        self.y = 100

    def listen_to_controller(self):
        """Listens to controller events and updates positions."""
        while 1:
            time.sleep(0.01)
            for event in pygame.event.get():
                if event.type == pygame.JOYAXISMOTION:
                    # Horizontal movement(update x)
                    if event.axis == 0:
                        self.x = int((event.value + 1) * 100)
                        self.horizontal_joystick.coords(self.horizontal_handle, self.x-10, 50-10, self.x+10, 50+10)
                    # Vertical movement(update y)
                    if event.axis == 3:
                        self.y = int((event.value + 1) * 100)
                        self.vertical_joystick.coords(self.vertical_handle, 50-10, self.y-10, 50+10, self.y+10)




if __name__ == "__main__":
    root = tk.Tk()
    app = JoystickApp(root)
    root.mainloop()
    

