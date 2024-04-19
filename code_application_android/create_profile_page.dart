import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/foundation.dart';
import 'package:image_picker/image_picker.dart';
import 'airplane_profile.dart';
import 'storage_service.dart'; // Import the necessary package
import 'package:path_provider/path_provider.dart'; // Import the necessary package



class CreateProfilePage extends StatefulWidget {
  final VoidCallback onProfileUpdated;
  final String? id;  // ID facultatif pour édition

  const CreateProfilePage({super.key, required this.onProfileUpdated, this.id});

  @override
  CreateProfilePageState createState() => CreateProfilePageState();
}

class CreateProfilePageState extends State<CreateProfilePage> {
  final TextEditingController _nameController = TextEditingController();
  // final TextEditingController _idController = TextEditingController();

  bool _isButtonEnabled = false;
  double _parameterValue1 = 0.0; // Valeur du premier curseur
  double _parameterValue2 = 0.0; // Valeur du deuxième curseur
  File? _image; // Variable pour stocker l'image sélectionnée

  final ImagePicker _picker = ImagePicker();

  // Fonction pour choisir une image
  Future<void> _pickImage() async {
    final XFile? pickedFile = await _picker.pickImage(source: ImageSource.gallery);
    if (pickedFile != null) {
      setState(() {
        _image = File(pickedFile.path);
      });
    }
  }
  
  @override
  void initState() {
      super.initState();
      if (widget.id != null) {
          _loadProfileData();
      }
      _nameController.addListener(_updateSaveButtonState); //ceci permet de mettre à jour le bouton d'enregistrement lorsqu'un utilisateur saisit un nom
  }
  void _updateSaveButtonState() {
    if (_nameController.text.isNotEmpty != _isButtonEnabled) {
        setState(() {
            _isButtonEnabled = _nameController.text.isNotEmpty;
        });
    }
}



  Future<void> _loadProfileData() async {
      AirplaneProfile? profile = await StorageService().getProfileById(widget.id);
        _nameController.text = profile!.name;
        _parameterValue1 = profile.parameter1;
        _parameterValue2 = profile.parameter2;
        if (profile.imagePath.isNotEmpty && !profile.imagePath.startsWith('assets')) {
            _image = File(profile.imagePath);
        }
        setState(() {});
      }

  @override
  void dispose() {
    _nameController.dispose();
    super.dispose();
  }

void _saveProfile() async {
  final directory = await getApplicationDocumentsDirectory();
  String imagePath;
  //avant de sauvegarder le profil, supprimer l'ancien profil s'il existe en utilisant deleteProfile
  if (widget.id != null) {
    await StorageService().deleteProfile(widget.id!);
  }

  // Utiliser l'image sélectionnée ou définir le chemin vers l'image par défaut
  if (_image != null) {
    final String imageName = 'Profile_${DateTime.now().millisecondsSinceEpoch}.png';
    imagePath = '${directory.path}/$imageName';
    await _image!.copy(imagePath);
  }
  else {
    imagePath = 'assets/images/default_avatar.png';
  }

  // Création de l'objet AirplaneProfile avec l'image par défaut si aucune image n'est sélectionnée
  var profile = AirplaneProfile(
    name: _nameController.text,
    id: widget.id ?? DateTime.now().millisecondsSinceEpoch.toString(), // si l'id est null, créez un nouvel identifiant
    timestamp: DateTime.now().millisecondsSinceEpoch.toString(),
    imagePath: imagePath,
    parameter1: _parameterValue1,
    parameter2: _parameterValue2
  );

  // Lecture, mise à jour et sauvegarde des profils
  List<AirplaneProfile> profiles = await StorageService().readProfiles();
  profiles.add(profile);
  await StorageService().writeProfiles(profiles);

  if (kDebugMode) {
    print('Profile saved: ${profile.toJson()}');
  }

  widget.onProfileUpdated();  // Appelle le callback
  if (mounted) {
    Navigator.pop(context);  // Retourne à l'écran précédent
  }
}


  @override
  Widget build(BuildContext context) {
    const ImageProvider defaultImage = AssetImage('assets/images/default_avatar.png');
    
    return Scaffold(
      appBar: AppBar(
        // Affiche le titre de la page Edit Profile ou Create New Profile en fonction de l'ID
        title : Text(widget.id != null ? 'Edit Profile' : 'Create New Profile'),
      ),
      body: SingleChildScrollView( // Ajoutez SingleChildScrollView ici
        child: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.start,
            children: [
              CircleAvatar(
                radius: 50,
                backgroundImage: _image != null ? FileImage(_image!) : defaultImage,
                backgroundColor: Colors.transparent,
              ),
              ElevatedButton(
                onPressed: _pickImage,
                child: const Text('Choisir une image'),
              ),
              TextField(
                controller: _nameController,
                decoration: const InputDecoration(
                  labelText: 'Nom de l\'avion',
                  border: OutlineInputBorder(),
                ),
              ),
              const SizedBox(height: 20),
              Text('Paramètre 1: ${_parameterValue1.toStringAsFixed(2)}'),
              Slider(
                value: _parameterValue1,
                min: 0.0,
                max: 100.0,
                divisions: 100,
                label: _parameterValue1.toStringAsFixed(2),
                onChanged: (double value) {
                  setState(() {
                    _parameterValue1 = value;
                  });
                },
              ),
              const SizedBox(height: 20),
              Text('Paramètre 2: ${_parameterValue2.toStringAsFixed(2)}'),
              Slider(
                value: _parameterValue2,
                min: 0.0,
                max: 100.0,
                divisions: 100,
                label: _parameterValue2.toStringAsFixed(2),
                onChanged: (double value) {
                  setState(() {
                    _parameterValue2 = value;
                  });
                },
              ),
            ],
          ),
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _isButtonEnabled ? _saveProfile : null,
        backgroundColor: _isButtonEnabled ? Colors.purple : Colors.grey,
        child: const Icon(Icons.save),
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.endFloat,
    );
  }
}


