#ifndef ENEMY_H
#define ENEMY_H

#include <vector>
#include <SDL3/SDL.h>

struct EnemyData {
    float x;
    float y;
    float velocityX;
    float velocityY;
    float size;
    float health;
    bool active;
    float spawnrate;
};

class EnemyManager {
private:
    std::vector<EnemyData> enemies;
    float spawnRate;
    float timeSinceLastSpawn;
    int score;


public:
    EnemyManager(float enemySpawnRate);

    void update(float deltaTime, int screenWidth, int screenHeight);
    void render(SDL_Renderer* renderer);
    void spawnEnemy(int screenHeight); 
    void setSpawnRate(int deltaTime);
    void setEnemySpeed(int velocityX);
    void setEnemyHealth(int health);
    std::vector<EnemyData>& getEnemies() { return enemies; }
    int getScore() const { return score; }
    void addScore(int points) { score += points; }

    int checkCollisions(std::vector<struct Projectile>& projectiles, float projectileSize);
};


#endif // ENEMY_H

