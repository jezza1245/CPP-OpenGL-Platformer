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
		vector.xPart *= 0.97;
	}
	void update(std::string level, std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms, Player* pl) override
	{
		applyFriction();
		vector += (gravity * dt);

		if (vector.yPart < -1.5) vector.yPart = -1.5;
		if (vector.yPart > 1.5) vector.yPart = 1.5;

		x += (vector.xPart * dt);
		y += (vector.yPart* dt);
		collisions(level,scroobs,platforms);

		updateTexture();
	}
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