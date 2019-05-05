#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H
#include "MovementVector.h"
MovementVector gravity = MovementVector(0,-0.004);

//TEXTURES
GLuint scroob_normal = 0;
GLuint scroob_silly = 0;
GLuint scroob_enemy = 0;

GLuint waterfall1 = 0;
GLuint waterfall2 = 0;
GLuint waterfall3 = 0;
GLuint waterfall4 = 0;
GLuint waterfall5 = 0;
GLuint waterfall6 = 0;
GLuint waterfall7 = 0;
GLuint waterfall8 = 0;
GLuint waterfall9 = 0;
GLuint waterfall10 = 0;
GLuint waterfall11 = 0;
GLuint waterfall12 = 0;
GLuint waterfall13 = 0;
GLuint waterfall14 = 0;

GLuint grass_block = 0;
GLuint grass_platform = 0;
GLuint grass_floor = 0;
GLuint level1_sky = 0;
GLuint level1_farhills = 0;
GLuint level1_closehills = 0;
GLuint level1_detailedhills = 0;
GLuint level1_clouds = 0;
GLuint safe = 0;



bool paused = false;
bool debug = false;
int gameTime = 0;
int points = 0;
bool gameOver = false;
int dt = 0;
#endif