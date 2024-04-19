import 'package:flutter/material.dart';
import 'joystick.dart'; // Assurez-vous que JoystickView est dans un fichier séparé ou ajustez l'importation en fonction de votre structure de projet

class ControllerPage extends StatelessWidget {
  const ControllerPage({super.key});

  @override
  Widget build(BuildContext context) {
    // ignore: prefer_const_constructors
    return Scaffold(

      body: const Row(
        children: [
          Expanded(
            child: JoystickView(), // Joystick pour la partie gauche
          ),
          Expanded(
            child: JoystickView(axis: JoystickAxis.vertical), // Joystick pour la partie droite
          ),
        ],
      ),
    );
  }
}
