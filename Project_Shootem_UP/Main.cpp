#include <iostream>
#include <algorithm>
#include <SDL3/SDL.h>
#include "Player.h"
#include "Enemi.h"
#include "LevelManager.h"

int main(int argc, char** argv)
{
	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "Erreur SDL_Init: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Set app metadata
	SDL_SetAppMetadata("SDL Test", "1.0", "games.anakata.test-sdl");

	// Create window and renderer
	SDL_Window* window;
	SDL_Renderer* renderer;
	if (!SDL_CreateWindowAndRenderer("Projet Shootem UP", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		std::cerr << "Erreur création fenêtre: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	// Activer VSync pour un rendu plus fluide
	SDL_SetRenderVSync(renderer, 1);

	// Initialiser le gestionnaire de niveaux et charger les niveaux
	LevelManager levelManager;
	if (!levelManager.loadLevelsFromFile("level.txt")) {
		std::cerr << "Erreur: Impossible de charger les niveaux!" << std::endl;
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Initialiser les objets du jeu
	Player player(320.0f, 240.0f, 25.0f);
	EnemyManager enemyManager(levelManager.getEnemySpawnRate());
  /*  Scrolling scrolling(640, 480, 50.0f);*/

	// Configurer les paramètres initiaux des ennemis
	enemyManager.setEnemySpeed(levelManager.getEnemySpeed());
	enemyManager.setEnemyHealth(levelManager.getEnemyHealth());

	std::cout << "Jeu démarré - Niveau: " << levelManager.getCurrentLevel() << std::endl;

	// Main loop
	bool keepGoing = true;
	Uint64 lastTime = SDL_GetTicks();

	// Limiter le deltaTime pour éviter les sauts
	const float maxDeltaTime = 0.05f;

	while (keepGoing) {
		// Calculer le delta time
		Uint64 currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		deltaTime = std::min(deltaTime, maxDeltaTime);
		lastTime = currentTime;

		// Handle events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT)
				keepGoing = false;
		}

		// Get keyboard state
		const bool* keys = SDL_GetKeyboardState(nullptr);

		// Ne pas spawner d'ennemis pendant la transition
		if (!levelManager.isInTransition() && levelManager.hasMoreLevels()) {
			player.update(deltaTime, keys, 640, 480);
			enemyManager.update(deltaTime, 640, 480);

			// Configurer les paramètres des ennemis selon le niveau
			enemyManager.setEnemySpeed(levelManager.getEnemySpeed());
			enemyManager.setEnemyHealth(levelManager.getEnemyHealth());

			// Vérifier les collisions et compter les ennemis tués
			int enemiesKilled = enemyManager.checkCollisions(player.getProjectiles(), 10.0f);
			for (int i = 0; i < enemiesKilled; i++) {
				levelManager.onEnemyKilled();
			}
		}
		else {
			// Pendant la transition, continuer à déplacer le joueur mais pas de tir
			player.update(deltaTime, keys, 640, 480);
		}

		// Mettre à jour le gestionnaire de niveaux
		if (levelManager.hasMoreLevels()) {
			levelManager.update(deltaTime, enemyManager);
		}

		// Clear screen (fond noir)
		SDL_SetRenderDrawColorFloat(renderer, 0.0f, 0.0f, 0.0f, 1.0f);
		SDL_RenderClear(renderer);

		// Dessiner les éléments du jeu
		/*scrolling.render(renderer);*/
		player.render(renderer);
		enemyManager.render(renderer);

		if (levelManager.hasMoreLevels()) {
			levelManager.render(renderer, 640, 480);
		}
		else {
			// Afficher "GAME COMPLETE" si tous les niveaux sont terminés
			SDL_SetRenderDrawColorFloat(renderer, 0.0f, 1.0f, 0.0f, 1.0f);
			for (int i = 0; i < 12; i++) {
				SDL_FRect rect = { 320.0f - 90.0f + i * 15.0f, 240.0f, 10.0f, 10.0f };
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		SDL_RenderPresent(renderer);
	}

	// Cleanup
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
