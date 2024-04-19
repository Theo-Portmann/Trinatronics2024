import 'package:flutter/material.dart';
import 'package:wifi_iot/wifi_iot.dart';
import 'main.dart';
//importer un module qui detecte la Plateforme
import 'dart:io' show Platform;

class ConnectionPage extends StatefulWidget {
  const ConnectionPage({super.key});

  @override
  State<ConnectionPage> createState() => _ConnectionPageState();
}

class _ConnectionPageState extends State<ConnectionPage> {
  final String ssid = "ESP32PlaneServer";
  final String password = "tK54841@";
  bool isConnecting = false;
  bool isConnected = false;

  @override
  void initState() {
    super.initState();
    initConnection();
  }

void initConnection() async {
  if (!await WiFiForIoTPlugin.isEnabled()) {
    if (!mounted) return; // Ajoutez cette ligne pour s'assurer que le widget est toujours monté
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('WiFi Disabled'),
        content: const Text('Please enable WiFi, and try again'),
        actions: [
          TextButton(
            onPressed: () async {
              if (!mounted) return; // Ajoutez cette ligne ici aussi
              Navigator.pushAndRemoveUntil(
                context,
                MaterialPageRoute(builder: (context) => const HomeScreen()),
                (Route<dynamic> route) => false
              );
              
            if (Platform.isAndroid) {
              await WiFiForIoTPlugin.setEnabled(true, shouldOpenSettings: true);
            }            
            
            },
            child: const Text('Enable WiFi'),
          ),
        ],
      ),
    );
  }
  try {
    String? currentSSID = await WiFiForIoTPlugin.getSSID();
    if (currentSSID != ssid) {
      if (!mounted) return; // Ajoutez cette ligne avant de changer l'état
      setState(() {
        isConnecting = true;
      });
      await WiFiForIoTPlugin.connect(
        ssid,
        password: password,
        joinOnce: true,
        security: NetworkSecurity.WPA,
        timeoutInSeconds: 10,
      );
      if (!mounted) return; // Vérifiez à nouveau avant de modifier l'état
      setState(() {
        isConnected = true;
        isConnecting = false;
      });
    } else {
      if (!mounted) return; // Encore ici avant de changer l'état
      setState(() {
        isConnected = true;
      });
    }
  } catch (e) {
    if (!mounted) return; // Vérifiez avant de montrer la boîte de dialogue
    setState(() {
      isConnecting = false;
    });
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Connection Error'),
        content: Text('Error: $e'),
        actions: [
          TextButton(
            onPressed: () {
              if (!mounted) return; // Vérifiez avant de naviguer
              Navigator.pop(context);
            },
            child: const Text('OK'),
          ),
        ],
      ),
    );
  }
}

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Connection to WiFi'),
      ),
      body: Center(
        child: isConnecting
            ? const CircularProgressIndicator()
            : isConnected
                ? const Text("Connected to WiFi successfully!")
                : const Text("Not connected to the desired WiFi."),
      ),
    );
  }
}
