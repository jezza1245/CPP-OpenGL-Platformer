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
	bool isFriendly;
	NPC(float pRadius): Scroob()
	{
		radius = pRadius;
	}

	void updateTexture() override
	{
		texture = scroob_silly;
	}

	void update(std::string level, std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms) override
	{
		applyFriction();

		if(radius * 0.9999 < 1)
		{
			radius = 1;
		}
		else {
			radius *= 0.9999;
		}

		vector += gravity;
		
		x += (vector.xPart * dt);
		y += (vector.yPart * dt);

		if(!dead && findTile(x,y,level) == 'H')
		{
			if(radius > 25)
			{
				points += int(radius);
			}
			dead = true;
			std::cout << points << std::endl;
		}

		
		
		collisions(level,scroobs,platforms);
		updateTexture();
	}

};

#endif