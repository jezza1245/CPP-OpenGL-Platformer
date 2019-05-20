#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
#include "Globals.h"
#include "Scroob.h"

class Player : public Scroob
{
public:
	Player(float pRadius): Scroob()
	{
		radius = pRadius;
	}
	void updateTexture() override
	{
		//If im moving close to terminal velocity, change texture
		if(std::abs(vector.xPart) < 0.9)
		{
			texture = scroob_normal;
		}
		else {
			texture = scroob_silly;
		}
	}
	void applyFriction() override
	{
		//Horizontal slow
		vector.xPart *= 0.97;
	}
	void update(std::string level, std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms, Player* pl) override
	{
		applyFriction(); //Horizontal friction
		vector += (gravity * dt); //Apply gravity

		if (radius < 10) dead = true; 

		//restrict y velocity
		if (vector.yPart < -1.5) vector.yPart = -1.5;
		if (vector.yPart > 1.5) vector.yPart = 1.5;

		//update x pos and y pos
		x += (vector.xPart * dt);
		y += (vector.yPart* dt);
		collisions(level,scroobs,platforms); //Check and resolve collisions

		updateTexture();
	}

	//Player movement functions
	void jump()
	{
		if(isOnGround) vector.yPart = 1.5;
	}
	void moveLeft() {
		if (vector.xPart > -1) {
			vector.xPart -= 0.03;
		}
	}
	void moveRight() {
		if (vector.xPart < 1) {
			vector.xPart += 0.03;
		}
	}
	void fastFall() {
		vector.yPart -= 0.08;
	}
};

#endif