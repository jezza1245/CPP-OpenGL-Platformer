#pragma once
#pragma once
#ifndef NPC_H
#define NPC_H
#include "Scroob.h"
#include "Globals.h"
#include <string>

class NPC : public Scroob
{
public:
	bool isFriendly, baloon = false;
	NPC(float pRadius): Scroob()
	{
		radius = pRadius;
	}

	void updateTexture() override
	{
		texture = scroob_silly;
	}

	void update(std::string level, std::vector<Scroob*> scroobs) override
	{
		applyFriction();
		if (!baloon) {
			vector += gravity;
		}
		x += (vector.xPart * dt);
		y += (vector.yPart * dt);
		
		collisions(level,scroobs);
		updateTexture();
	}

};

#endif