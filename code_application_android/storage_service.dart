import 'dart:io';
import 'dart:convert';
import 'package:path_provider/path_provider.dart';
import 'airplane_profile.dart';

class StorageService {
  Future<String> get _localPath async {
    final directory = await getApplicationDocumentsDirectory();
    return directory.path;
  }

  Future<File> get _localFile async {
    final path = await _localPath;
    return File('$path/airplane_profiles.json');
  }

  Future<File> writeProfiles(List<AirplaneProfile> profiles) async {
    final file = await _localFile;
    String jsonProfiles = json.encode(profiles.map((p) => p.toJson()).toList());
    return file.writeAsString(jsonProfiles);
  }

  Future<List<AirplaneProfile>> readProfiles() async {
    try {
      final file = await _localFile;
      String contents = await file.readAsString();
      List<dynamic> jsonResponse = json.decode(contents);
      List<AirplaneProfile> profiles = jsonResponse.map((data) => AirplaneProfile.fromJson(data)).toList();
      // Trier les profils du plus récent au plus ancien
      profiles.sort((a, b) => b.timestamp.compareTo(a.timestamp));
      return profiles;
    } catch (e) {
      return [];
    }
  }
Future<void> deleteProfile(String id) async {
  List<AirplaneProfile> profiles = await readProfiles();
  // Trouver le profil à supprimer pour obtenir le chemin de son image
  AirplaneProfile? profileToDelete = profiles.firstWhere(
    (profile) => profile.id == id, orElse: () => AirplaneProfile(name: '', id: '', timestamp: '', imagePath: '', parameter1: 0, parameter2: 0));

  // Supprimer le profil de la liste

  profiles.removeWhere((profile) => profile.id == id);

  // Supprimer l'image du profil si elle n'est pas une image par défaut
  if (!profileToDelete.imagePath.startsWith('assets/')) {
    File imageFile = File(profileToDelete.imagePath);
    if (await imageFile.exists()) {
      await imageFile.delete();
    }
  }

  // Sauvegarder la nouvelle liste des profils sans le profil supprimé
  await writeProfiles(profiles);
}

  Future<AirplaneProfile?> getProfileById(String? id) async {
      List<AirplaneProfile> profiles = await readProfiles();
      return profiles.firstWhere((profile) => profile.id == id, orElse: () => AirplaneProfile(name: '', id: '', timestamp: '', imagePath: '', parameter1: 0, parameter2: 0));

    


  }
}
