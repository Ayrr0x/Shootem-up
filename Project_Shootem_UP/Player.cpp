#include "Player.h"
#include <cmath>
#include <algorithm>

Player::Player(float startX, float startY, float playerSize)
    : x(startX), y(startY), size(playerSize), speed(250.0f),
    fireRate(0.15f), timeSinceLastShot(0.15f),
    projectileSpeed(500.0f), projectileSize(10.0f) {
}

void Player::update(float deltaTime, const bool* keys, int screenWidth, int screenHeight) {
    handleMovement(deltaTime, keys, screenWidth, screenHeight);
    handleShooting(deltaTime, keys);
    updateProjectiles(deltaTime, screenWidth, screenHeight);
}

void Player::handleMovement(float deltaTime, const bool* keys, int screenWidth, int screenHeight) {
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        moveY = -speed;
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        moveY = speed;
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        moveX = -speed;
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        moveX = speed;

    // Normaliser la vitesse en diagonale
    if (moveX != 0.0f && moveY != 0.0f) {
        float magnitude = std::sqrt(moveX * moveX + moveY * moveY);
        moveX = (moveX / magnitude) * speed;
        moveY = (moveY / magnitude) * speed;
    }

    // Appliquer le mouvement
    x += moveX * deltaTime;
    y += moveY * deltaTime;

    // Limiter le joueur à l'écran
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + size > screenWidth) x = screenWidth - size;
    if (y + size > screenHeight) y = screenHeight - size;
}

void Player::handleShooting(float deltaTime, const bool* keys) {
    timeSinceLastShot += deltaTime;

    if (keys[SDL_SCANCODE_SPACE] && timeSinceLastShot >= fireRate) {
        Projectile proj;
        proj.x = x + size;
        proj.y = y + size / 2.0f - projectileSize / 2.0f;
        proj.velocityX = projectileSpeed;
        proj.velocityY = 0;
        proj.active = true;
        projectiles.push_back(proj);
        timeSinceLastShot = 0.0f;
    }
}

void Player::updateProjectiles(float deltaTime, int screenWidth, int screenHeight) {
    // Mettre à jour les projectiles
    for (auto& proj : projectiles) {
        if (proj.active) {
            proj.x += proj.velocityX * deltaTime;
            proj.y += proj.velocityY * deltaTime;

            // Désactiver si hors de l'écran
            if (proj.x > screenWidth || proj.x < -projectileSize ||
                proj.y > screenHeight || proj.y < -projectileSize) {
                proj.active = false;
            }
        }
    }

    // Nettoyer les projectiles inactifs
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.active; }),
        projectiles.end()
    );
}

void Player::render(SDL_Renderer* renderer) {
    // Dessiner le joueur (blanc)
    SDL_FRect rect = { x, y, size, size };
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 1.0f, 1.0f);
    SDL_RenderFillRect(renderer, &rect);

    // Dessiner les projectiles (jaune)
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 0.0f, 1.0f);
    for (const auto& proj : projectiles) {
        if (proj.active) {
            SDL_FRect projRect = { proj.x, proj.y, projectileSize, projectileSize };
            SDL_RenderFillRect(renderer, &projRect);
        }
    }
}