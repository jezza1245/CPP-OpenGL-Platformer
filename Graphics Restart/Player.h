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

	void update(std::string level) override
	{
		bool collidedWithGround = false;
		applyFriction();
		vector += (gravity * dt);
		x += (vector.xPart * dt);
		y += (vector.yPart* dt);

		collidedWithGround = resolveTileCollisions(level);
		resolveMapCollisions(collidedWithGround);
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
		vector.yPart -= 0.03;
	}

};

#endif