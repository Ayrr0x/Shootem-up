#include "Enemi.h"
#include "Player.h"
#include <algorithm>
#include <cstdlib>

EnemyManager::EnemyManager(float enemySpawnRate)
	: spawnRate(enemySpawnRate), timeSinceLastSpawn(0.0f), score(0) {
}

void EnemyManager::update(float deltaTime, int screenWidth, int screenHeight) {
	// Générer des ennemis
	timeSinceLastSpawn += deltaTime;
	if (timeSinceLastSpawn >= spawnRate) {
		spawnEnemy(screenHeight);
		timeSinceLastSpawn = 0.0f;
	}

	// Mettre à jour les ennemis
	for (auto& enemy : enemies) {
		if (enemy.active) {
			enemy.x += enemy.velocityX * deltaTime;

			// Désactiver si hors de l'écran
			if (enemy.x < -enemy.size) {
				enemy.active = false;
			}
		}
	}
}

void EnemyManager::spawnEnemy(int screenHeight) {
	EnemyData enemy;
	enemy.x = 640.0f;
	enemy.y = static_cast<float>(rand() % (screenHeight - 30));
	enemy.velocityX = -150.0f;
	enemy.velocityY = 0.0f;
	enemy.size = 30.0f;
	enemy.health = 2;
	enemy.active = true;
	enemies.push_back(enemy);
}

void EnemyManager::setSpawnRate(int deltaTime)
{
	EnemyData enemy;
	enemy.spawnrate = 25.0f;
}
void EnemyManager::setEnemySpeed(int velocityX) {
	EnemyData enemy;
	 enemy.velocityX = -150.0f;
}
void EnemyManager::setEnemyHealth(int health) {
	EnemyData enemy;
	enemy.health = 2;
}
void EnemyManager::render(SDL_Renderer* renderer) {
	// Dessiner les ennemis (rouge)
	SDL_SetRenderDrawColorFloat(renderer, 1.0f, 0.0f, 0.0f, 1.0f);
	for (const auto& enemy : enemies) {
		if (enemy.active) {
			SDL_FRect enemyRect = { enemy.x, enemy.y, enemy.size, enemy.size };
			SDL_RenderFillRect(renderer, &enemyRect);
		}
	}

	// Afficher le score (simple indicateur visuel avec des carrés verts)
	SDL_SetRenderDrawColorFloat(renderer, 0.0f, 1.0f, 0.0f, 1.0f);
	for (int i = 0; i < score / 100 && i < 20; i++) {
		SDL_FRect scoreRect = { 10.0f + i * 15.0f, 10.0f, 10.0f, 10.0f };
		SDL_RenderFillRect(renderer, &scoreRect);
	}
}

int EnemyManager::checkCollisions(std::vector<Projectile>& projectiles, float projectileSize) {
	for (auto& proj : projectiles) {
		if (!proj.active) continue;

		for (auto& enemy : enemies) {
			if (!enemy.active) continue;

			// Collision simple rectangulaire
			if (proj.x < enemy.x + enemy.size &&
				proj.x + projectileSize > enemy.x &&
				proj.y < enemy.y + enemy.size &&
				proj.y + projectileSize > enemy.y) {

				proj.active = false;
				enemy.health--;

				if (enemy.health <= 0) {
					enemy.active = false;
					score += 100;
				}
				break;
			}
			else
				return 0;
		}
	}
}