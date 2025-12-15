#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <SDL3/SDL.h>

struct Projectile {
    float x;
    float y;
    float velocityX;
    float velocityY;
    bool active;
};

class Player {
private:
    float x;
    float y;
    float size;
    float speed;
    float fireRate;
    float timeSinceLastShot;
    float projectileSpeed;
    float projectileSize;
    std::vector<Projectile> projectiles;

public:
    Player(float startX, float startY, float playerSize);

    void update(float deltaTime, const bool* keys, int screenWidth, int screenHeight);
    void render(SDL_Renderer* renderer);

    float getX() const { return x; }
    float getY() const { return y; }
    float getSize() const { return size; }
    std::vector<Projectile>& getProjectiles() { return projectiles; }

private:
    void handleMovement(float deltaTime, const bool* keys, int screenWidth, int screenHeight);
    void handleShooting(float deltaTime, const bool* keys);
    void updateProjectiles(float deltaTime, int screenWidth, int screenHeight);
};

#endif // PLAYER_H
