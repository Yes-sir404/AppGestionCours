#ifndef USER_H
#define USER_H

#include <QString>
#include <QVariant>

// Enum pour savoir si c'est un prof ou un étudiant
enum UserRole {
    ETUDIANT,
    PROFESSEUR
};

// ==========================================
// 1. CLASSE USER (La mère)
// ==========================================
class User {
public:
    // Constructeur
    User(int id, QString nom, QString prenom, QString email, QString password, UserRole role);
    User(); // Constructeur vide requis pour certaines opérations
    virtual ~User(); // Important pour la mémoire

    // --- Getters (Pour récupérer les infos) ---
    int getId() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getEmail() const;
    QString getPassword() const;
    UserRole getRole() const;
    QString getNomComplet() const; // Utile pour l'affichage "Bonjour M. X"

    // --- Setters (Pour modifier les infos) ---
    void setId(int id);
    void setNom(const QString &nom);
    void setPrenom(const QString &prenom);
    void setEmail(const QString &email);
    void setPassword(const QString &password);

    // --- FONCTIONS CRITIQUES (Ce dont vous avez besoin) ---

    // [Login] : Statique car on l'appelle AVANT d'avoir un objet User
    // Elle va vérifier la BDD et retourner un objet Etudiant ou Prof
    static User* login(const QString &email, const QString &password);

    // [Inscription] : Sauvegarde l'utilisateur actuel dans la BDD (table 'utilisateurs')
    bool signup();

    // [Vérification] : Vérifie si l'email existe déjà (pour éviter les doublons à l'inscription)
    static bool emailExiste(const QString &email);

    // [Validation] : Vérifie le format de l'email (doit finir par @edulink.prof.ma ou @edulink.etud.ma)
    static bool isValidEmailStructure(const QString &email);

    // [Helper] : Déduit le rôle depuis l'email
    static UserRole getRoleFromEmail(const QString &email);

    // [Validation] : Vérifie la complexité du mot de passe
    static bool isValidPasswordStructure(const QString &password);

protected:
    int m_id;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_password;
    UserRole m_role;
};

// ==========================================
// 2. CLASSE PROF (Spécifique)
// ==========================================
class Prof : public User {
public:
    // Constructeur spécifique
    Prof(int id, QString nom, QString prenom, QString email, QString password);

    // Le prof n'a pas besoin de fonctions spéciales ici pour l'instant.
    // La création de cours se fera via la classe "ClassModule", mais on utilise
    // l'ID du prof pour lier le cours.
};

// ==========================================
// 3. CLASSE ETUDIANT (Spécifique)
// ==========================================
class Etudiant : public User {
public:
    // Constructeur spécifique
    Etudiant(int id, QString nom, QString prenom, QString email, QString password);

    // --- FONCTION CLÉ DU PROJET ---

    // [Rejoindre] : L'étudiant tape le "Code Unique", et cette fonction
    // crée le lien dans la table 'inscriptions'
    bool rejoindreClasse(const QString &codeUnique);
};

#endif // USER_H
