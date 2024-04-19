import 'package:flutter/material.dart';

class PaperPlaneAnimation extends StatefulWidget {
  const PaperPlaneAnimation({super.key});

  @override
  PaperPlaneAnimationState createState() => PaperPlaneAnimationState();
}

class PaperPlaneAnimationState extends State<PaperPlaneAnimation> with SingleTickerProviderStateMixin {
  AnimationController? _controller;
  Animation<double>? _animation;

  @override
  void initState() {
    super.initState();
    _controller = AnimationController(
      duration: const Duration(seconds: 1),
      vsync: this,
    )..repeat(reverse: true);

    _animation = Tween<double>(begin: 0.0, end: 30.0).animate(_controller!)
      ..addListener(() {
        setState(() {});
      });
  }

  @override
  void dispose() {
    _controller?.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Transform.translate(
      offset: Offset(0, _animation!.value),
      child: Image.asset(
            'assets/images/avion_en_papier.png',
            width: 100, // Définir la largeur souhaitée
            height: 100, // Définir la hauteur souhaitée
          ),
    );
  }
}
