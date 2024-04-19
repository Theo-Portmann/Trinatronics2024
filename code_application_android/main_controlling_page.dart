import 'package:flutter/material.dart';
import 'controller_page.dart';
import 'orientation_info_page.dart';


class MainControllingPage extends StatelessWidget {
  const MainControllingPage({super.key});

  @override
  Widget build(BuildContext context) {
    return OrientationBuilder(
      builder: (context, orientation) {
        // Affiche ControllerPage en mode paysage
        if (orientation == Orientation.landscape) {
          return const ControllerPage();
        }
        // Affiche OrientationInfoPage en mode portrait
        else {

          return const OrientationInfoPage();
        }
      },
    );
  }
}
