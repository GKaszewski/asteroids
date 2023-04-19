#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_BULLETS = 100;
const int MAX_SPACESHIPS = 100;
const int MAX_ASTEROIDS = 100;
const int FPS = 60;
const float INITIAL_PLAYER_SPEED = 5.0f;

const int PLAYER_WIDTH = 40;
const int PLAYER_HEIGHT = 40;

const int BULLET_WIDTH = 12;
const int BULLET_HEIGHT = 16;

const int SPACESHIP_WIDTH = 40;
const int SPACESHIP_HEIGHT = 40;

const int ASTEROID_WIDTH = 80;
const int ASTEROID_HEIGHT = 80;

const int POINTS_PER_ASTEROID = 10;
const int POINTS_PER_SPACESHIP = 20;

Mix_Music *music = NULL;
Mix_Chunk *laserSfx = NULL;
Mix_Chunk *spaceshipLaserSfx = NULL;
Mix_Chunk *lostSfx = NULL;
Mix_Chunk *hitSfx = NULL;
Mix_Chunk *explosionSfx = NULL;

TTF_Font* font = NULL;

typedef struct Player {
	float x, y;
	float velX, velY;
	float speed;
	int lives;
} Player;

typedef struct Bullet {
	float x, y;
	float velX, velY;
} Bullet;

typedef struct Spaceship {
	float x, y;
	float velX, velY;
} Spaceship;

typedef struct Asteroid {
	float x, y;
	float velX, velY;
} Asteroid;

typedef struct Game {
	Player *player;
	Bullet** bullets;
	Bullet** enemyBullets;
	Spaceship** spaceships;
	Asteroid** asteroids;
	int points;
	bool gameOver;
} Game;

typedef struct Clock {
	float lastTime;
	float currentTime;
	float deltaTime;
} Clock;

void tick(Clock* clock) {
	clock->lastTime = clock->currentTime;
	clock->currentTime = SDL_GetTicks();
	clock->deltaTime = clock->currentTime - clock->lastTime;
}

Player* createPlayer( float x, float y, float velX, float velY, float speed ) {
	Player* player = (Player*) malloc( sizeof(Player) );
	player->x = x;
	player->y = y;
	player->velX = velX;
	player->velY = velY;
	player->speed = speed;
	player->lives = 3;
	return player;
}

Bullet* createBullet( float x, float y, float velX, float velY ) {
	Bullet* bullet = (Bullet*) malloc( sizeof(Bullet) );
	bullet->x = x;
	bullet->y = y;
	bullet->velX = velX;
	bullet->velY = velY;
	return bullet;
}

Spaceship* createSpaceship( float x, float y, float velX, float velY ) {
	Spaceship* spaceship = (Spaceship*) malloc( sizeof(Spaceship) );
	spaceship->x = x;
	spaceship->y = y;
	spaceship->velX = velX;
	spaceship->velY = velY;
	return spaceship;
}

Asteroid* createAsteroid( float x, float y, float velX, float velY ) {
	Asteroid* asteroid = (Asteroid*) malloc( sizeof(Asteroid) );
	asteroid->x = x;
	asteroid->y = y;
	asteroid->velX = velX;
	asteroid->velY = velY;
	return asteroid;
}

void initializeBullets(Bullet** bullets) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i] = NULL;
	}
}

void initializeEnemyBullets(Bullet** enemyBullets) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		enemyBullets[i] = NULL;
	}
}

void initializeSpaceships(Spaceship** spaceships) {
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		spaceships[i] = NULL;
	}
}

void initializeAsteroids(Asteroid** asteroids) {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		asteroids[i] = NULL;
	}
}

Game* createGame() {
	Game* game = (Game*) malloc( sizeof(Game) );
	game->player = createPlayer( SCREEN_WIDTH / 2, SCREEN_HEIGHT - (PLAYER_HEIGHT + 30), 0, 0, INITIAL_PLAYER_SPEED );
	game->bullets = (Bullet**) malloc( sizeof(Bullet*) * MAX_BULLETS );
	initializeBullets(game->bullets);
	game->enemyBullets = (Bullet**) malloc( sizeof(Bullet*) * MAX_BULLETS );
	initializeEnemyBullets(game->enemyBullets);
	game->spaceships = (Spaceship**) malloc( sizeof(Spaceship*) * MAX_SPACESHIPS );
	initializeSpaceships(game->spaceships);
	game->asteroids = (Asteroid**) malloc( sizeof(Asteroid*) * MAX_ASTEROIDS );
	initializeAsteroids(game->asteroids);
	game->points = 0;
	game->gameOver = false;
	return game;
}

Spaceship* addSpaceship(Game* game, Spaceship* spaceship) {
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		if (!game->spaceships[i]) {
			game->spaceships[i] = spaceship;
			return game->spaceships[i];
		}
	}

	return NULL;
}

Asteroid* addAsteroid(Game* game, Asteroid* asteroid) {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (!game->asteroids[i]) {
			game->asteroids[i] = asteroid;
			return game->asteroids[i];
		}
	}

	return NULL;
}

Bullet* addBullet(Game* game, Bullet* bullet) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!game->bullets[i]) {
			game->bullets[i] = bullet;
			return game->bullets[i];
		}
	}

	return NULL;
}

Bullet* addEnemyBullet(Game* game, Bullet* bullet) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (!game->enemyBullets[i]) {
			game->enemyBullets[i] = bullet;
			return game->enemyBullets[i];
		}
	}

	return NULL;
}

void destroyBullet( Bullet* bullet ) {
	free( bullet );
}

void destroyBullets(Bullet** bullets) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i] != NULL) {
			destroyBullet(bullets[i]);
		}
	}
}

void destroyEnemyBullets(Bullet** enemyBullets) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (enemyBullets[i] != NULL) {
			destroyBullet(enemyBullets[i]);
		}
	}
}

void destroySpaceship(Spaceship* spaceship) {
	free(spaceship);
}

void destroySpaceships(Spaceship** spaceships) {
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		if (spaceships[i] != NULL) {
			destroySpaceship(spaceships[i]);
		}
	}
}

void destroyAsteroid(Asteroid* asteroid) {
	free(asteroid);
}

void destroyAsteroids(Asteroid** asteroids) {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (asteroids[i] != NULL) {
			destroyAsteroid(asteroids[i]);
		}
	}
}

void destroyPlayer( Player* player ) {
	free( player );
}

void destroyGame(Game* game) {
	destroyPlayer(game->player);
	destroyBullets(game->bullets);
	destroyEnemyBullets(game->enemyBullets);
	destroySpaceships(game->spaceships);
	destroyAsteroids(game->asteroids);
	free(game);
}

void removeSpaceship(Game* game, Spaceship* spaceship) {
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		if (game->spaceships[i] == spaceship) {
			destroySpaceship(spaceship);
			game->spaceships[i] = NULL;
			break;
		}
	}
}

void removeAsteroid(Game* game, Asteroid* asteroid) {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (game->asteroids[i] == asteroid) {
			destroyAsteroid(asteroid);
			game->asteroids[i] = NULL;
			break;
		}
	}
}

void removeBullet(Game* game, Bullet* bullet) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->bullets[i] == bullet) {
			destroyBullet(bullet);
			game->bullets[i] = NULL;
			break;
		}
	}
}

void removeEnemyBullet(Game* game, Bullet* bullet) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->enemyBullets[i] == bullet) {
			destroyBullet(bullet);
			game->enemyBullets[i] = NULL;
			break;
		}
	}
}

void movePlayer(Player* player) {
	if (!player) return;
	if (player->x+player->velX > SCREEN_WIDTH) {
		player->x = 0;
	} else if (player->x+player->velX < 0) {
		player->x = SCREEN_WIDTH;
	} else {
		player->x += player->velX;
	}

	player->y += player->velY;
}

void moveBullet(Bullet* bullet) {
	if (!bullet) return;
	bullet->x += bullet->velX;
	bullet->y += bullet->velY;
}

void moveBullets(Bullet** bullets) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i] != NULL) {
			moveBullet(bullets[i]);
		}
	}
}

void moveSpaceship(Spaceship* spaceship) {
	if (!spaceship) return;
	float amplitude = 0.75f;
	float frequency = 0.1f;
	static float phase = 0.0f;
	float x = amplitude * cos(frequency * (spaceship->y + phase)) + spaceship->x;
	float speed = 1.f;

	spaceship->x = x;
	spaceship->y += speed;
	phase += 0.1f;
}

void moveSpaceships(Spaceship** spaceships) {
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		if (spaceships[i] != NULL) {
			moveSpaceship(spaceships[i]);
		}
	}
}

void moveAsteroid(Asteroid* asteroid) {
	if (!asteroid) return;
	float amplitude = 0.25f;
	float frequency = 0.05f;
	static float phase = 0.0f;
	float x = amplitude * cos(frequency * (asteroid->y + phase)) + asteroid->x;
	float speed = .25f;

	asteroid->x = x;
	asteroid->y += speed;
	phase += 0.1f;
}

void moveAsteroids(Asteroid** asteroids) {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (asteroids[i] != NULL) {
			moveAsteroid(asteroids[i]);
		}
	}
}

void moveGame(Game* game) {
	movePlayer(game->player);
	moveBullets(game->bullets);
	moveBullets(game->enemyBullets);
	moveSpaceships(game->spaceships);
	moveAsteroids(game->asteroids);
}

void drawPlayer(Player* player, SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_Rect rect = { player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void drawBullet(Bullet* bullet, SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_Rect rect = { bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void drawEnemyBullet(Bullet* bullet, SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_Rect rect = { bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void drawSpaceship(Spaceship* spaceship, SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_Rect rect = { spaceship->x, spaceship->y, SPACESHIP_WIDTH, SPACESHIP_HEIGHT };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void drawAsteroid(Asteroid* asteroid, SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_Rect rect = { asteroid->x, asteroid->y, ASTEROID_WIDTH, ASTEROID_HEIGHT };
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void drawGame(Game* game, SDL_Renderer* renderer, SDL_Texture* playerTexture, SDL_Texture* bulletTexture, SDL_Texture* spaceshipTexture, SDL_Texture* asteroidTexture) {
	drawPlayer(game->player, renderer, playerTexture);
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->bullets[i] != NULL) {
			drawBullet(game->bullets[i], renderer, bulletTexture);
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->enemyBullets[i] != NULL) {
			drawEnemyBullet(game->enemyBullets[i], renderer, bulletTexture);
		}
	}
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		if (game->spaceships[i] != NULL) {
			drawSpaceship(game->spaceships[i], renderer, spaceshipTexture);
		}
	}
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (game->asteroids[i] != NULL) {
			drawAsteroid(game->asteroids[i], renderer, asteroidTexture);
		}
	}
}

void shoot(Game* game) {
	bool created = false;
	Bullet* bullet = createBullet(game->player->x + 5, game->player->y + 5, 0, -10);
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->bullets[i] == NULL) {
			game->bullets[i] = bullet;
			created = true;
			Mix_PlayChannel(-1, laserSfx, 0);
			break;
		}
	}

	if (!created) {
		destroyBullet(game->bullets[0]);
		game->bullets[0] = bullet;
		Mix_PlayChannel(-1, laserSfx, 0);
	}
}

void spaceshipShoot(Game* game, Spaceship* spaceship, float bulletSpeed) {
	bool created = false;
	Bullet* bullet = createBullet(spaceship->x + 5, spaceship->y + 5, 0, bulletSpeed);
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->enemyBullets[i] == NULL) {
			game->enemyBullets[i] = bullet;
			created = true;
			Mix_PlayChannel(-1, spaceshipLaserSfx, 0);
			break;
		}
	}

	if (!created) {
		destroyBullet(game->enemyBullets[0]);
		game->enemyBullets[0] = bullet;
		Mix_PlayChannel(-1, spaceshipLaserSfx, 0);
	}
}

bool checkCollision( SDL_Rect a, SDL_Rect b ) {
	int leftA = a.x;
	int rightA = a.x + a.w;
	int topA = a.y;
	int bottomA = a.y + a.h;

	int leftB = b.x;
	int rightB = b.x + b.w;
	int topB = b.y;
	int bottomB = b.y + b.h;

	if (bottomA <= topB) {
		return false;
	}

	if (topA >= bottomB) {
		return false;
	}

	if (rightA <= leftB) {
		return false;
	}

	if (leftA >= rightB) {
		return false;
	}

	return true;
}

bool checkBulletCollisionWithAsteroid(Game* game, Bullet* bullet, Asteroid* asteroid) {
	if (bullet == NULL || asteroid == NULL) {
		return false;
	}
	SDL_Rect bulletRect = { bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT };
	SDL_Rect asteroidRect = { asteroid->x, asteroid->y, ASTEROID_WIDTH, ASTEROID_HEIGHT };
	if (checkCollision(bulletRect, asteroidRect)) {
		removeAsteroid(game, asteroid);
		Mix_PlayChannel(-1, explosionSfx, 0);
		game->points += POINTS_PER_ASTEROID;
		removeBullet(game, bullet);
		return true;
	}

	return false;
}

bool checkBulletCollisionWithSpaceship(Game* game, Bullet* bullet, Spaceship* spaceship) {
	if (bullet == NULL || spaceship == NULL) {
		return false;
	}

	SDL_Rect bulletRect = { bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT };
	SDL_Rect spaceshipRect = { spaceship->x, spaceship->y, SPACESHIP_WIDTH, SPACESHIP_HEIGHT };
	if (checkCollision(bulletRect, spaceshipRect)) {
		removeSpaceship(game, spaceship);
		Mix_PlayChannel(-1, explosionSfx, 0);
		game->points += POINTS_PER_SPACESHIP;
		removeBullet(game, bullet);
		return true;
	}

	return false;
}

bool checkBulletCollisionWithPlayer(Game* game, Bullet* bullet, Player* player) {
	if (bullet == NULL || player == NULL) {
		return false;
	}
	SDL_Rect bulletRect = { bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT };
	SDL_Rect playerRect = { player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT };
	if (checkCollision(bulletRect, playerRect)) {
		Mix_PlayChannel(-1, hitSfx, 0);
		player->lives--;
		game->points -= POINTS_PER_ASTEROID + POINTS_PER_SPACESHIP;
		if (player->lives <= 0) {
			Mix_PlayChannel(-1, lostSfx, 0);
			game->gameOver = true;
		}
		removeEnemyBullet(game, bullet);
		return true;
	}

	return false;
}

bool checkAsteroidCollisionWithPlayer(Game* game, Asteroid* asteroid, Player* player) {
	if (asteroid == NULL || player == NULL) {
		return false;
	}
	SDL_Rect asteroidRect = { asteroid->x, asteroid->y, ASTEROID_WIDTH, ASTEROID_HEIGHT };
	SDL_Rect playerRect = { player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT };
	if (checkCollision(asteroidRect, playerRect)) {
		player->lives--;
		Mix_PlayChannel(-1, hitSfx, 0);
		game->points -= POINTS_PER_ASTEROID + POINTS_PER_SPACESHIP;
		if (player->lives <= 0) {
			game->gameOver = true;
		}

		removeAsteroid(game, asteroid);
		Mix_PlayChannel(-1, explosionSfx, 0);
		return true;
	}

	return false;
}

bool checkSpaceshipCollisionWithPlayer(Game* game, Spaceship* spaceship, Player* player) {
	if (spaceship == NULL || player == NULL) {
		return false;
	}
	SDL_Rect spaceshipRect = { spaceship->x, spaceship->y, SPACESHIP_WIDTH, SPACESHIP_HEIGHT };
	SDL_Rect playerRect = { player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT };
	if (checkCollision(spaceshipRect, playerRect)) {
		Mix_PlayChannel(-1, hitSfx, 0);
		player->lives--;
		game->points -= POINTS_PER_ASTEROID + POINTS_PER_SPACESHIP;
		if (player->lives <= 0) {
			game->gameOver = true;
		}

		Mix_PlayChannel(-1, explosionSfx, 0);
		removeSpaceship(game, spaceship);
		return true;
	}

	return false;
}

void cleanUpBullet(Game* game, int index) {
	Bullet* bullet = game->bullets[index];
	if (bullet->x < 0 || bullet->x > SCREEN_WIDTH || bullet->y < 0 || bullet->y > SCREEN_HEIGHT) {
		destroyBullet(game->bullets[index]);
		game->bullets[index] = NULL;
	}
}

void cleanUpEnemyBullet(Game* game, int index) {
	Bullet* bullet = game->enemyBullets[index];
	if (bullet->x < 0 || bullet->x > SCREEN_WIDTH || bullet->y < 0 || bullet->y > SCREEN_HEIGHT) {
		destroyBullet(game->enemyBullets[index]);
		game->enemyBullets[index] = NULL;
	}
}

void cleanUpEnemyBullets(Game* game) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->enemyBullets[i] != NULL) {
			cleanUpEnemyBullet(game, i);
		}
	}
}

void cleanUpBullets(Game* game) {
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (game->bullets[i] != NULL) {
			cleanUpBullet(game, i);
		}
	}
}

void cleanUpSpaceship(Game* game, int index) {
	Spaceship* spaceship = game->spaceships[index];
	if (spaceship->y > SCREEN_HEIGHT) {
		destroySpaceship(game->spaceships[index]);
		game->spaceships[index] = NULL;
	}
}

void cleanUpSpaceships(Game* game) {
	for (int i = 0; i < MAX_SPACESHIPS; i++) {
		if (game->spaceships[i] != NULL) {
			cleanUpSpaceship(game, i);
		}
	}
}

void cleanUpAsteroid(Game* game, int index) {
	Asteroid* asteroid = game->asteroids[index];
	if (asteroid->y > SCREEN_HEIGHT) {
		destroyAsteroid(game->asteroids[index]);
		game->asteroids[index] = NULL;
	}
}

void cleanUpAsteroids(Game* game) {
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (game->asteroids[i] != NULL) {
			cleanUpAsteroid(game, i);
		}
	}
}

void drawRepeatingTexture(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect srcRect, SDL_Rect destRect) {
	int x = destRect.x;
	int y = destRect.y;
	int w = destRect.w;
	int h = destRect.h;
	while (y < SCREEN_HEIGHT) {
		while (x < SCREEN_WIDTH) {
			SDL_Rect rect = { x, y, w, h };
			SDL_RenderCopy(renderer, texture, &srcRect, &rect);
			x += w;
		}
		x = destRect.x;
		y += h;
	}
}

void drawText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const char* text, int x, int y) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect rect = { x, y, surface->w, surface->h };
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
} 

int main (int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == NULL) {
		printf("SDL_CreateWindow Error: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
		return 1;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
		return 1;
	}

	if (TTF_Init() == -1) {
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(win);
		printf("SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	Game* game = createGame();
	bool shouldClose = false;
	unsigned time = SDL_GetTicks();
	
	float spawnTimer = 0.f;
	float spaceshipTime = 2.5f;
	
	float asteroidTimer = 0.f;
	float asteroidTime = 5.5f;

	float bulletTimer = 0.f;
	float bulletTime = 0.75f;
	float bulletSpeed = 10.f;
	float bulletChance = 0.25f;

	music = Mix_LoadMUS("data/music.ogg");
	laserSfx = Mix_LoadWAV("data/sfx_laser1.ogg");
	spaceshipLaserSfx = Mix_LoadWAV("data/sfx_laser2.ogg");
	lostSfx = Mix_LoadWAV("data/sfx_lose.ogg");
	hitSfx = Mix_LoadWAV("data/sfx_zap.ogg");
	explosionSfx = Mix_LoadWAV("data/sfx_twoTone.ogg");

	SDL_Texture *backgroundTexture = NULL;
	SDL_Texture* playerTexture = NULL;
	SDL_Texture* asteroidTexture = NULL;
	SDL_Texture* bulletTexture = NULL;
	SDL_Texture* spaceshipTexture = NULL;
	SDL_Texture* playerHealthTexture = NULL;
	SDL_Surface *background = IMG_Load("data/background.png");
	SDL_Surface *playerSurface = IMG_Load("data/player.png");
	SDL_Surface *asteroidSurface = IMG_Load("data/asteroid.png");
	SDL_Surface *bulletSurface = IMG_Load("data/bullet.png");
	SDL_Surface *spaceshipSurface = IMG_Load("data/spaceship.png");
	SDL_Surface *playerHealthSurface = IMG_Load("data/player_health.png");
	SDL_Rect backgroundRect = {0, 0, 256, 256};
	SDL_Rect dstBackgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_Rect healthRect = {10, 10, 32, 32};

	font = TTF_OpenFont("data/font.ttf", 16);

	SDL_Color textColor = { 255, 255, 255 };

	backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
	playerTexture = SDL_CreateTextureFromSurface(renderer, playerSurface);
	asteroidTexture = SDL_CreateTextureFromSurface(renderer, asteroidSurface);
	bulletTexture = SDL_CreateTextureFromSurface(renderer, bulletSurface);
	spaceshipTexture = SDL_CreateTextureFromSurface(renderer, spaceshipSurface);
	playerHealthTexture = SDL_CreateTextureFromSurface(renderer, playerHealthSurface);
	SDL_FreeSurface(playerSurface);
	SDL_FreeSurface(asteroidSurface);
	SDL_FreeSurface(bulletSurface);
	SDL_FreeSurface(spaceshipSurface);
	SDL_FreeSurface(background);
	SDL_FreeSurface(playerHealthSurface);

	while (!shouldClose) {
		SDL_Event e;
		unsigned now = SDL_GetTicks();
		float delta = (now - time) / 1000.0f;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				shouldClose = true;
			}

			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_UP:
						game->player->velY = -game->player->speed;
						break;
					case SDLK_DOWN:
						game->player->velY = game->player->speed;
						break;
					case SDLK_LEFT:
						game->player->velX = -game->player->speed;
						break;
					case SDLK_RIGHT:
						game->player->velX = game->player->speed;
						break;
					case SDLK_SPACE:
						shoot(game);
						break;
				}
			}

			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_UP:
						game->player->velY = 0;
						break;
					case SDLK_DOWN:
						game->player->velY = 0;
						break;
					case SDLK_LEFT:
						game->player->velX = 0;
						break;
					case SDLK_RIGHT:
						game->player->velX = 0;
						break;
				}
			}
		}

		if (!Mix_PlayingMusic()) {
			Mix_PlayMusic(music, -1);
		}

		if (!game->gameOver) {
			spawnTimer += delta;
			asteroidTimer += delta;
			bulletTimer += delta;
		}
		
		if (spawnTimer > spaceshipTime) {
			float x = rand() % SCREEN_WIDTH;
			Spaceship* spaceship = createSpaceship(x, 0, 0, 0);
			addSpaceship(game, spaceship);
			spawnTimer = 0.f;
		}

		if (asteroidTimer > asteroidTime) {
			float x = rand() % SCREEN_WIDTH;
			Asteroid* asteroid = createAsteroid(x, 0, 0, 0);
			addAsteroid(game, asteroid);
			asteroidTimer = 0.f;
		}

		if (bulletTimer > bulletTime) {
			for (int i = 0; i < MAX_SPACESHIPS; i++) {
				if (game->spaceships[i] != NULL) {
					float chance = (rand() % 100) / 100.f;
					if (chance < bulletChance) {
						spaceshipShoot(game, game->spaceships[i], bulletSpeed);
					}
				}
			}
			bulletTimer = 0.f;
		}

		cleanUpBullets(game);
		cleanUpEnemyBullets(game);
		cleanUpSpaceships(game);
		cleanUpAsteroids(game);
		if (!game->gameOver) moveGame(game);

		for (int i = 0; i < MAX_BULLETS; i++) {
			if (game->bullets[i] != NULL) {
				for (int j = 0; j < MAX_ASTEROIDS; j++) {
					if (game->asteroids[j] != NULL) {
						checkBulletCollisionWithAsteroid(game, game->bullets[i], game->asteroids[j]);
					}
				}

				for (int j = 0; j < MAX_SPACESHIPS; j++) {
					if (game->spaceships[j] != NULL) {
						checkBulletCollisionWithSpaceship(game, game->bullets[i], game->spaceships[j]);
					}
				}
			}
		}

		for (int i = 0; i < MAX_BULLETS; i++) {
			if (game->enemyBullets[i] != NULL) {
				checkBulletCollisionWithPlayer(game, game->enemyBullets[i], game->player);
			}
		}

		for (int i = 0; i < MAX_ASTEROIDS; i++) {
			if (game->asteroids[i] != NULL) {
				checkAsteroidCollisionWithPlayer(game, game->asteroids[i], game->player);
			}
		}

		for (int i = 0; i < MAX_SPACESHIPS; i++) {
			if (game->spaceships[i] != NULL) {
				checkSpaceshipCollisionWithPlayer(game, game->spaceships[i], game->player);
			}
		}

		char healthText[10];
		snprintf(healthText, 10, "Lives: %i", game->player->lives);
		char pointsText[16];
		snprintf(pointsText, 16, "Points: %i", game->points);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		drawRepeatingTexture(renderer, backgroundTexture, backgroundRect, dstBackgroundRect);
		drawGame(game, renderer, playerTexture, bulletTexture, spaceshipTexture, asteroidTexture);
		SDL_RenderCopy(renderer, playerHealthTexture, NULL, &healthRect);
		drawText(renderer, font, textColor, healthText, healthRect.x + 40, healthRect.y);
		drawText(renderer, font, textColor, pointsText, SCREEN_WIDTH / 2 - 60, healthRect.y);
		if (game->gameOver) {
			char gameOverText[32];
			snprintf(gameOverText, 32, "Game Over! Your score: %i", game->points);
			drawText(renderer, font, textColor, gameOverText, SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 20);
		}
		SDL_RenderPresent(renderer);

		time = now;
	}

	Mix_HaltMusic();

	destroyGame(game);

	SDL_DestroyTexture(backgroundTexture);
	SDL_DestroyTexture(playerTexture);
	SDL_DestroyTexture(bulletTexture);
	SDL_DestroyTexture(spaceshipTexture);
	SDL_DestroyTexture(asteroidTexture);
	SDL_DestroyTexture(playerHealthTexture);

	Mix_FreeMusic(music);
	Mix_FreeChunk(laserSfx);
	Mix_FreeChunk(spaceshipLaserSfx);
	Mix_FreeChunk(lostSfx);
	Mix_FreeChunk(hitSfx);
	Mix_FreeChunk(explosionSfx);

	Mix_CloseAudio();
	Mix_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}