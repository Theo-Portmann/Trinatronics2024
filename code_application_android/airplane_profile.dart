class AirplaneProfile {
  final String name;
  final String id;
  final String timestamp; //ajout dun parametre timestamp
  final String imagePath; // Chemin de l'image
  final double parameter1; // Valeur du premier paramètre
  final double parameter2; // Valeur du deuxième paramètre
  bool isSelected; // Ajouté pour gérer la sélection


  AirplaneProfile({
    required this.name,
    required this.id,
    required this.timestamp,
    required this.imagePath,
    required this.parameter1,
    required this.parameter2,
    this.isSelected = false, // Initialiser isSelected à false

  });

  Map<String, dynamic> toJson() => {
    'name': name,
    'id': id,
    'timestamp': timestamp,
    'imagePath': imagePath,
    'parameter1': parameter1,
    'parameter2': parameter2

  };

  factory AirplaneProfile.fromJson(Map<String, dynamic> json) {
    return AirplaneProfile(
      name: json['name'],
      id: json['id'],
      timestamp: json['timestamp'],
      imagePath: json['imagePath'],
      parameter1: json['parameter1'] as double,
      parameter2: json['parameter2'] as double
      //ajout dun parametre timestamp

    );
  }
}
