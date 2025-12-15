#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include "Enemi.h"

struct LevelData {
    int levelNumber;
    float enemySpawnRate;
    float enemySpeed;
    int enemyHealth;
    int enemiesRequired;
    std::string name;
};

class LevelManager {
private:
    std::vector<LevelData> levels;
    int currentLevelIndex;
    int enemiesKilledInLevel;
    bool levelCompleted;
    float transitionTimer;
    bool inTransition;

public:
    LevelManager();

    bool loadLevelsFromFile(const std::string& filename);
    void update(float deltaTime, EnemyManager& enemyManager);
    void render(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    int getCurrentLevel() const;
    bool isInTransition() const { return inTransition; }
    bool hasMoreLevels() const { return currentLevelIndex < levels.size(); }

    // Paramètres du niveau actuel
    float getEnemySpawnRate() const;
    float getEnemySpeed() const;
    int getEnemyHealth() const;
    int getEnemiesRequired() const;
    std::string getLevelName() const;

    void onEnemyKilled();

private:
    void startTransition();
    void nextLevel();
};

#endif // LEVEL_MANAGER_H