
import 'package:flutter/material.dart';

class OrientationInfoPage extends StatelessWidget {
  const OrientationInfoPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            const Text(
              "Veuillez tourner votre appareil en mode paysage pour utiliser le joystick.",
              textAlign: TextAlign.center,
              style: TextStyle(
                fontSize: 18,
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 20),  // Espacement entre le texte et l'image
            Image.asset(
              'assets/images/rotate-phone.gif',
              width: 300,  // Taille adaptée de l'image
              height: 300,
            ),
          ],
        ),
      ),
    );
  }
}

