#pragma once
#pragma once
#ifndef NPC_H
#define NPC_H
#include "Scroob.h"
#include "Globals.h"
#include <string>

class NPC : Scroob
{
	bool isFriendly, baloon = false;
	NPC(float pRadius): Scroob()
	{
		radius = pRadius;
	}

	void update(std::string layout) override
	{
		applyFriction();
		if (!baloon) {
			vector += gravity;
		}
		x += (vector.xPart * dt);
		y += (vector.yPart * dt);
		
		resolveTileCollisions(layout);
		resolveMapCollisions();
	}

};

#endif