import 'package:flutter/material.dart';
import 'dart:math' as math;

// Enum pour spécifier l'orientation du joystick
enum JoystickAxis { horizontal, vertical }

class JoystickView extends StatefulWidget {
  final JoystickAxis axis;

  const JoystickView({super.key, this.axis = JoystickAxis.horizontal});

  @override
  JoystickViewState createState() => JoystickViewState();
}

class JoystickViewState extends State<JoystickView> {
  Offset? centerPosition;
  Offset? joystickInnerPosition;

  void updatePosition(Offset localPosition) {
    setState(() {
      const double radius = 100.0; // Rayon fixe du grand cercle.
      centerPosition ??= localPosition;

      double dx = 0.0;
      double dy = 0.0;

      // Calculer le déplacement en fonction de l'axe configuré
      if (widget.axis == JoystickAxis.horizontal) {
        dx = localPosition.dx - centerPosition!.dx;
      } else {
        dy = localPosition.dy - centerPosition!.dy;
      }

      final double distance = math.sqrt(dx * dx + dy * dy);

      if (distance > radius) {
        final double x = dx * radius / distance;
        final double y = dy * radius / distance;
        joystickInnerPosition = Offset(centerPosition!.dx + x, centerPosition!.dy + y);
      } else {
        joystickInnerPosition = Offset(centerPosition!.dx + dx, centerPosition!.dy + dy);
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: LayoutBuilder(
          builder: (context, constraints) {
            return GestureDetector(
              onPanStart: (details) => updatePosition(details.localPosition),
              onPanUpdate: (details) => updatePosition(details.localPosition),
              onPanEnd: (details) {
                setState(() {
                  joystickInnerPosition = null;
                  centerPosition = null; // Réinitialiser le centre du joystick
                });
              },
              child: CustomPaint(
                size: Size(constraints.maxWidth, constraints.maxHeight),
                painter: JoystickPainter(
                  center: centerPosition,
                  joystickPosition: joystickInnerPosition,
                  radius: 100.0,
                ),
              ),
            );
          }
        ),
      ),
    );
  }
}

class JoystickPainter extends CustomPainter {
  final Offset? center;
  final Offset? joystickPosition;
  final double radius;

  JoystickPainter({this.center, this.joystickPosition, required this.radius});

  @override
  void paint(Canvas canvas, Size size) {
    if (center != null) {
      final backgroundPaint = Paint()
        ..style = PaintingStyle.fill
        ..color = Colors.grey.withOpacity(0.3);
      canvas.drawCircle(center!, radius, backgroundPaint);

      if (joystickPosition != null) {
        final joystickPaint = Paint()
          ..style = PaintingStyle.fill
          ..color = Colors.blue;
        canvas.drawCircle(joystickPosition!, 30, joystickPaint);
      }
    }
  }

  @override
  bool shouldRepaint(covariant JoystickPainter oldDelegate) {
    return center != oldDelegate.center || joystickPosition != oldDelegate.joystickPosition;
  }
}
