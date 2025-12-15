#include "LevelManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

LevelManager::LevelManager()
    : currentLevelIndex(0), enemiesKilledInLevel(0),
    levelCompleted(false), transitionTimer(0.0f), inTransition(false) {
    std::cout << "LevelManager créé" << std::endl;
}

bool LevelManager::loadLevelsFromFile(const std::string& filename) {
    std::ifstream file;
    file.open(filename);

    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    std::string line;
    int lineNumber = 0;

    // Lire toutes les lignes
    while (std::getline(file, line)) {
        lineNumber++;

        // Ignorer les lignes vides et commentaires
        if (line.empty() || line[0] == '#') {
            std::cout << "Ligne " << lineNumber << " ignorée (commentaire ou vide)" << std::endl;
            continue;
        }

        std::istringstream iss(line);
        LevelData level;

        // Format: levelNumber,name,spawnRate,speed,health,enemiesRequired
        char comma;
        if (iss >> level.levelNumber >> comma) {
            // Lire le nom (peut contenir des espaces)
            std::getline(iss, level.name, ',');

            // Supprimer les espaces au début et à la fin du nom
            size_t start = level.name.find_first_not_of(" \t");
            size_t end = level.name.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                level.name = level.name.substr(start, end - start + 1);
            }

            if (iss >> level.enemySpawnRate >> comma >>
                level.enemySpeed >> comma >>
                level.enemyHealth >> comma >>
                level.enemiesRequired) {

                levels.push_back(level);
                std::cout << "Niveau " << level.levelNumber << " chargé: "
                    << level.name
                    << " (spawn:" << level.enemySpawnRate
                    << ", speed:" << level.enemySpeed
                    << ", health:" << level.enemyHealth
                    << ", required:" << level.enemiesRequired << ")" << std::endl;
            }
            else {
                std::cerr << "Erreur de parsing ligne " << lineNumber << ": " << line << std::endl;
            }
        }
        else {
            std::cerr << "Erreur de parsing ligne " << lineNumber << ": " << line << std::endl;
        }
    }

    file.close();

    if (levels.empty()) {
        std::cerr << "Erreur: Aucun niveau chargé!" << std::endl;
        return false;
    }

    std::cout << "=== Total de " << levels.size() << " niveaux chargés ===" << std::endl;
    std::cout << "Niveau actuel: " << getCurrentLevel() << std::endl;
    std::cout << "Ennemis requis: " << getEnemiesRequired() << std::endl;
    return true;
}

void LevelManager::update(float deltaTime, EnemyManager& enemyManager) {
    if (!hasMoreLevels()) {
        std::cout << "Plus de niveaux disponibles!" << std::endl;
        return;
    }

    // Gérer la transition entre les niveaux
    if (inTransition) {
        transitionTimer += deltaTime;

        if (transitionTimer >= 3.0f) {  // 3 secondes de transition
            inTransition = false;
            transitionTimer = 0.0f;
            levelCompleted = false;
            enemiesKilledInLevel = 0;

            std::cout << "Transition terminée, nouveau niveau: " << getCurrentLevel() << std::endl;

            // Mettre à jour les paramètres du gestionnaire d'ennemis
            enemyManager.setSpawnRate(getEnemySpawnRate());
        }
    }

    // Vérifier si le niveau est terminé
    if (!inTransition && enemiesKilledInLevel >= getEnemiesRequired()) {
        std::cout << "Niveau " << getCurrentLevel() << " terminé! ("
            << enemiesKilledInLevel << "/" << getEnemiesRequired() << ")" << std::endl;
        startTransition();
        nextLevel();
    }
}

void LevelManager::render(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    if (!hasMoreLevels()) return;

    // Afficher le numéro de niveau en haut à droite
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 1.0f, 1.0f);

    int levelNum = getCurrentLevel();
    int startX = screenWidth - 150;
    int startY = 10;

    // Dessiner "LVL" avec des petits carrés
    for (int i = 0; i < 3; i++) {
        SDL_FRect rect = { static_cast<float>(startX + i * 15), static_cast<float>(startY), 10.0f, 10.0f };
        SDL_RenderFillRect(renderer, &rect);
    }

    // Dessiner le chiffre du niveau
    for (int i = 0; i < levelNum && i < 10; i++) {
        SDL_FRect rect = { static_cast<float>(startX + 60 + i * 15), static_cast<float>(startY), 10.0f, 10.0f };
        SDL_RenderFillRect(renderer, &rect);
    }

    // Afficher la progression (ennemis tués / requis)
    SDL_SetRenderDrawColorFloat(renderer, 0.7f, 0.7f, 1.0f, 1.0f);
    int required = getEnemiesRequired();
    if (required > 0) {
        int progress = (enemiesKilledInLevel * 10) / required;
        for (int i = 0; i < progress; i++) {
            SDL_FRect rect = { static_cast<float>(startX + i * 12), static_cast<float>(startY + 20), 10.0f, 5.0f };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // Si en transition, afficher un message
    if (inTransition) {
        SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 0.0f, 1.0f);

        // Afficher "LEVEL COMPLETE" avec des carrés qui clignotent
        if (static_cast<int>(transitionTimer * 2) % 2 == 0) {
            int messageY = screenHeight / 2;
            for (int i = 0; i < 10; i++) {
                SDL_FRect rect = { static_cast<float>(screenWidth / 2 - 75 + i * 15), static_cast<float>(messageY), 10.0f, 10.0f };
                SDL_RenderFillRect(renderer, &rect);
            }

            // "NEXT LEVEL"
            if (currentLevelIndex < levels.size()) {
                for (int i = 0; i < 8; i++) {
                    SDL_FRect rect = { static_cast<float>(screenWidth / 2 - 60 + i * 15), static_cast<float>(messageY + 30), 10.0f, 10.0f };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
            else {
                // "GAME COMPLETE"
                for (int i = 0; i < 10; i++) {
                    SDL_FRect rect = { static_cast<float>(screenWidth / 2 - 75 + i * 15), static_cast<float>(messageY + 30), 10.0f, 10.0f };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }
}

int LevelManager::getCurrentLevel() const {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex].levelNumber;
    }
    return 0;
}

float LevelManager::getEnemySpawnRate() const {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex].enemySpawnRate;
    }
    return 1.5f;
}

float LevelManager::getEnemySpeed() const {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex].enemySpeed;
    }
    return 150.0f;
}

int LevelManager::getEnemyHealth() const {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex].enemyHealth;
    }
    return 2;
}

int LevelManager::getEnemiesRequired() const {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex].enemiesRequired;
    }
    return 5;
}

std::string LevelManager::getLevelName() const {
    if (currentLevelIndex < levels.size()) {
        return levels[currentLevelIndex].name;
    }
    return "Unknown";
}

void LevelManager::onEnemyKilled() {
    enemiesKilledInLevel++;
    std::cout << "Ennemi tué! Progression: " << enemiesKilledInLevel
        << "/" << getEnemiesRequired() << std::endl;
}

void LevelManager::startTransition() {
    inTransition = true;
    transitionTimer = 0.0f;
    levelCompleted = true;
    std::cout << "Transition de niveau commencée" << std::endl;
}

void LevelManager::nextLevel() {
    currentLevelIndex++;
    std::cout << "Passage au niveau suivant (index: " << currentLevelIndex << ")" << std::endl;
}
