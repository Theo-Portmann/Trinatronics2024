import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:path_provider/path_provider.dart';

import 'storage_service.dart';
import 'airplane_profile.dart';
import 'animation_plane.dart';
import 'create_profile_page.dart'; // Assurez-vous d'importer le fichier de la nouvelle page
//import 'main_controlling_page.dart';
import 'initializing_connection.dart';
// Assurez-vous d'importer le fichier de la nouvelle page
import 'dart:io';

void main() {
  //debugPaintPointersEnabled = true;  // Activez cette ligne pour voir la visualisation des touchers
  runApp(const MyApp());
}


class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      home: HomeScreen(),
    );
  }
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  HomeScreenState createState() => HomeScreenState();
}

class HomeScreenState extends State<HomeScreen> {
  final StorageService storage = StorageService();

  Future<List<AirplaneProfile>>? _futureProfiles;

  @override
  void initState() {
    super.initState();
    _futureProfiles = storage.readProfiles();
    
    clearCache();
  }

  Future<void> clearCache() async {
    try {
      final cacheDir = await getTemporaryDirectory();
      if (cacheDir.existsSync()) {
        cacheDir.deleteSync(recursive: true);
        if (kDebugMode) {
          if (kDebugMode) {
            print("Cache cleared!");
          }
        }
      }
    } catch (e) {
      if (kDebugMode) {
        print("Error clearing cache: $e");
      }
    }
  }
  void reloadProfiles() {
    
    setState(() {
      _futureProfiles = storage.readProfiles();

    });
  }


  String formatTimestamp(String timestamp) {
    final dateTime = DateTime.fromMillisecondsSinceEpoch(int.parse(timestamp));
    return "${dateTime.day}/${dateTime.month}/${dateTime.year} à ${dateTime.hour}:${dateTime.minute.toString().padLeft(2, '0')}";
  }
Future<bool> showDeleteConfirmationDialog(BuildContext context) {
  return showDialog<bool>(
    context: context,
    builder: (BuildContext context) {
      return AlertDialog(
        title: const Text('Confirmer'),
        content: const Text('Voulez-vous vraiment supprimer ce profil?'),
        actions: <Widget>[
          TextButton(
            child: const Text('Annuler'),
            onPressed: () => Navigator.of(context).pop(false),
          ),
          TextButton(
            child: const Text('Supprimer'),
            onPressed: () => Navigator.of(context).pop(true),
          ),
        ],
      );
    },
  ).then((value) => value ?? false);  // S'assurer que la valeur n'est jamais null
}

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Airplane Profiles'),
        backgroundColor: Colors.purple,

      ),
      body: FutureBuilder<List<AirplaneProfile>>(
        future: _futureProfiles,
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.done) {
            if (snapshot.hasError) {
              return Text("Error: ${snapshot.error}");
            }

            final profiles = snapshot.data ?? [];
            if (profiles.isEmpty) {
            return const Center(
              child: Column(
                mainAxisSize: MainAxisSize.min, // Réduit la taille de la colonne aux éléments qu'elle contient
                mainAxisAlignment: MainAxisAlignment.center, // Centre les éléments verticalement
                children: <Widget>[
                  PaperPlaneAnimation(), // Widget de l'animation d'avion en papier
                  SizedBox(height: 40), // Espacement vertical entre l'animation et le texte
                  Text(
                    "Bienvenue sur l'application ! Commencez par créer un nouveau profil d'avion.",
                    style: TextStyle(color: Color.fromARGB(255, 41, 36, 36), fontSize: 20),
                    textAlign: TextAlign.center,
                  ),
                ],
              ),
            );


            }

          return ListView.builder(
            itemCount: profiles.length,
            itemBuilder: (context, index) {
              final profile = profiles[index];
              return Dismissible(
                key: Key(profile.id),  // Assurez-vous que chaque élément a une clé unique
                background: Container(
                  color: Colors.blue,
                  alignment: Alignment.centerLeft,
                  child: const Padding(
                    padding: EdgeInsets.only(left: 20.0),
                    child: Icon(Icons.edit, color: Colors.white),
                  ),
                ),
                secondaryBackground: Container(
                  color: Colors.red,
                  alignment: Alignment.centerRight,
                  child: const Padding(
                    padding: EdgeInsets.only(right: 20.0),
                    child: Icon(Icons.delete, color: Colors.white),
                  ),
                ),
                confirmDismiss: (direction) async {
                  if (direction == DismissDirection.endToStart) {
                    // Demander confirmation avant suppression
                    return await showDeleteConfirmationDialog(context);
                  } else {
                    // Naviguer vers une page d'édition
                    Navigator.push(
                      context,
                      MaterialPageRoute(builder: (context) => CreateProfilePage(
                        onProfileUpdated: reloadProfiles,
                        id : profile.id,
                      )),
                    );
                    // Ne pas supprimer après édition
                    return false;
                  }
                },
                onDismissed: (direction) {
                  if (direction == DismissDirection.endToStart) {
                    // Supprimer le profil de la liste et de la source de données
                    setState(() {
                      profiles.removeAt(index);
                      storage.deleteProfile(profile.id);  // Implémentez cette méthode dans StorageService
                    });
                  }
                },
                child: ListTile(
                  leading: CircleAvatar(
                    backgroundImage: profile.imagePath.startsWith('assets')
                        ? AssetImage(profile.imagePath)
                        : FileImage(File(profile.imagePath)) as ImageProvider,
                    backgroundColor: Colors.transparent,
                  ),
                  title: Text(profile.name, style: const TextStyle(fontWeight: FontWeight.bold)),
                  subtitle: Text(
                    "Dernière modification le ${formatTimestamp(profile.timestamp)}",
                    style: TextStyle(color: Colors.grey.shade600, fontSize: 12),
                  ),
                  selected: profile.isSelected,
                  onTap: () {
                    Navigator.push(
                      context,
                      MaterialPageRoute(builder: (context) => const ConnectionPage()),
                    );
                  },
                ));
            },
          );

          } else {
            return const CircularProgressIndicator();
          }
        },
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          Navigator.push(
            context,
            MaterialPageRoute(
              builder: (context) => CreateProfilePage(
                onProfileUpdated: reloadProfiles,
              ),
            ),
          );
        },
        backgroundColor: Colors.purple,
        child: const Icon(Icons.add),
      ),
    );
  }
  

}
