
#include "Game.h"
#include "ResourceManager.h"
#include "MusicHandler.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "GameLevel.h"
#include "Player.h"

#include <iostream>
#include <vector>
#include <time.h>
#include <string>

// Game-related state data
SpriteRenderer* renderer;

GameLevel* level;
Player* player;
bool keyActionPressed = false;

// Player move (up, down, left, right)
Move playerMove;

// Game Constructor

Game::Game(GLuint width, GLuint height)
    : WIDTH(width), HEIGHT(height), time_step(0)
{
    for (int i = 0; i < 1024; i++) {
        keys[i] = -1;
    }
}

// Game Destructor
Game::~Game() {
	delete renderer;
	delete level;
}

void Game::init() {
	// Load shaders
	ResourceManager::loadShaderFromFile("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->WIDTH), static_cast<GLfloat>(this->HEIGHT), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use().setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);

	// Load textures
	ResourceManager::loadTexture("img/pengo/pengo0.png", GL_TRUE, "pengoDown0");
	ResourceManager::loadTexture("img/pengo/pengo1.png", GL_TRUE, "pengoDown1");
	ResourceManager::loadTexture("img/pengo/pengo2.png", GL_TRUE, "pengoLeft0");
	ResourceManager::loadTexture("img/pengo/pengo3.png", GL_TRUE, "pengoLeft1");
	ResourceManager::loadTexture("img/pengo/pengo4.png", GL_TRUE, "pengoUp0");
	ResourceManager::loadTexture("img/pengo/pengo5.png", GL_TRUE, "pengoUp1");
	ResourceManager::loadTexture("img/pengo/pengo6.png", GL_TRUE, "pengoRight0");
	ResourceManager::loadTexture("img/pengo/pengo7.png", GL_TRUE, "pengoRight1");
	ResourceManager::loadTexture("img/walls/wall0.png", GL_TRUE, "wall0");
	ResourceManager::loadTexture("img/walls/wall1.png", GL_TRUE, "wall1");
	ResourceManager::loadTexture("img/iceblock/iceblock.png", GL_TRUE, "iceblock");
	ResourceManager::loadTexture("img/diamond/diamond.png", GL_TRUE, "diamond");
	ResourceManager::loadTexture("img/diamond/diamond-shiny.png", GL_TRUE, "diamond-shiny");

	ResourceManager::loadTexture("img/blocks.png", GL_TRUE, "blocks");
	ResourceManager::loadTexture("img/creatures.png", GL_TRUE, "creatures");

	// Set Render-specific contols
	Shader spriteShader = ResourceManager::getShader("sprite");
	renderer = new SpriteRenderer(spriteShader, this->WIDTH, this->HEIGHT);

	level = new GameLevel();
	level->load("levels/level1.txt");

	player = level->pengo;
	playerMove = MOVE_DOWN;

	// Play music
	MusicHandler::init();
	MusicHandler::loadSound("sounds/create_level.wav","create_level");
	MusicHandler::loadSound("sounds/init_level.wav","init_level");
	MusicHandler::loadSound("sounds/level.wav","level");
	MusicHandler::loadSound("sounds/death.wav","death");
	MusicHandler::loadSound("sounds/insert_coin.wav","insert_coin");// TODO Neceasrio?

	MusicHandler::play("create_level");

}

void Game::update() {
	player->setSprite(playerMove);
    ResourceManager::addTick();

    // Generate level
	if(this->state == GAME_GEN_LEVEL) {
		time_step += 1;
		bool end = true;
		if (time_step>0) {
			time_step = 0;
		    end = level->generate();
			if (end) {
				MusicHandler::play("init_level",1);
				this->state = GAME_START_LEVEL;
			}
		}
	} else if(this->state == GAME_START_LEVEL) {
		if (!MusicHandler::isPlaying()) {
			this->state = GAME_ACTIVE;
			MusicHandler::play("level");
		}
	}
}

static glm::vec2 nextPosRelative(Move m){
	switch(m){
		case MOVE_UP: return glm::vec2(0.0f,-1.0f);
		break;
		case MOVE_DOWN: return glm::vec2(0.0f,1.0f);
		break;
		case MOVE_RIGHT: return glm::vec2(1.0f,0.0f);
		break;
		case MOVE_LEFT: return glm::vec2(-1.0f,0.0f);
		break;
	}
}

void Game::proccessInput() {
	if(this->keys[GLFW_KEY_ESCAPE] == GLFW_PRESS) {
		if (this->state == GAME_PAUSE_MENU) {
			this->state = GAME_ACTIVE;
		}
		if (this->state == GAME_ACTIVE) {
			this->state = GAME_PAUSE_MENU;
		}
	}
	if (this->state == GAME_ACTIVE) {
		if(this->keys[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS && !player->isMoving && !keyActionPressed) {
			// Look in front of Pengo
			glm::vec2 npr = nextPosRelative(player->lastMove);
			if (level->checkCollision(player->position + npr)) {
				// If iceblock -> Slide or disintegrate
				Iceblock* block = dynamic_cast< Iceblock* >(level->getObjFromPosition(player->position + npr));
				if (block!=nullptr){
					if (!level->checkCollision(player->position + (npr + npr))) {
						// Slide
						block->slide(player->lastMove,level);
					} else {
						block->disintegrate(level);
					}
				}

				// If diamondblock -> Slide or nothing
				Diamondblock* dblock = dynamic_cast< Diamondblock* >(level->getObjFromPosition(player->position + npr));
				if (dblock!=nullptr){
					if (!level->checkCollision(player->position + (npr + npr))) {
						// Slide
						dblock->slide(player->lastMove,level);
					}
				}
			}
		}

		if(this->keys[GLFW_KEY_LEFT_CONTROL] == GLFW_PRESS && !player->isMoving) {
			keyActionPressed = true;
		}
		if (this->keys[GLFW_KEY_LEFT_CONTROL] == GLFW_RELEASE) {
			keyActionPressed = false;
		}
		// Move playerboard
		glm::vec2 newPos;
		if (this->keys[GLFW_KEY_UP] >= GLFW_PRESS) {
			player->lastMove = MOVE_UP;
			if (!player->isMoving) {
	            playerMove = MOVE_UP;
	            newPos = player->position + glm::vec2(0, -1);
	            if(!level->checkCollision(newPos)){
	            	player->isMoving = true;
	            	player->destination = newPos;
	            }
			}
		}

		if (this->keys[GLFW_KEY_DOWN] >= GLFW_PRESS) {
			player->lastMove = MOVE_DOWN;
			if (!player->isMoving) {
	            playerMove = MOVE_DOWN;
	            newPos = player->position + glm::vec2(0, 1);
	            if(!level->checkCollision(newPos)){
	            	player->isMoving = true;
	            	player->destination = newPos;
	            }
			}
		}

		if (this->keys[GLFW_KEY_LEFT] >= GLFW_PRESS) {
			player->lastMove = MOVE_LEFT;
			if (!player->isMoving) {
	            playerMove = MOVE_LEFT;
	            newPos = player->position + glm::vec2(-1, 0);
	            if(!level->checkCollision(newPos)){
	            	player->isMoving = true;
	            	player->destination = newPos;
	            }
			}
		}

		if (this->keys[GLFW_KEY_RIGHT] >= GLFW_PRESS) {
			player->lastMove = MOVE_RIGHT;
			if (!player->isMoving) {
	            playerMove = MOVE_RIGHT;
	            newPos = player->position + glm::vec2(1, 0);
	            if(!level->checkCollision(newPos)){
	            	player->isMoving = true;
	            	player->destination = newPos;
	            }
			}
		}
	}
}

void Game::render(GLfloat interpolation) {
    player->move(playerMove, interpolation);
    level->moveBlocks(interpolation);
    level->destroyBlocks(interpolation);
    if (this->state == GAME_ACTIVE) {
    	level->moveEnemies(interpolation);
    }
	if (this->state == GAME_ACTIVE || this->state == GAME_START_LEVEL) {
		level->draw(*renderer);
		player->draw(*renderer);
	} else if (this->state == GAME_GEN_LEVEL) {
		level->draw(*renderer);
		level->drawGenerating(*renderer);
	}
}
