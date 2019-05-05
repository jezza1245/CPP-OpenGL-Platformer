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
	float timeSinceEvent = 0;
	NPC(float pRadius): Scroob()
	{
		radius = pRadius;
	}
	NPC(bool friendly, float pRadius, float xstart, float ystart) : Scroob()
	{
		radius = pRadius;
		x = xstart;
		y = ystart;
		isFriendly = friendly;
		if(isFriendly)
		{
			texture = scroob_silly;
		}else
		{
			texture = scroob_enemy;
		}
	}

	void updateTexture() override
	{

		
	}
	void applyFriction() override
	{
		vector.xPart *= 0.99;
	}

	void update(std::string level, std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms, Player* pl)
	{
		applyFriction();
		vector.xPart *= 1.00000001;

		if(radius * 0.9999 < 1)
		{
			radius = 5;
		}
		else {
			radius *= 0.9999;
		}

		vector += gravity;
		
		x += (vector.xPart * dt);
		y += (vector.yPart * dt);

		if(!dead && isFriendly && findTile(x,y,level) == 'H')
		{
			points += int(radius);
			dead = true;
			std::cout << "+" << radius << "   (" << points << ")" << std::endl;
		}

		
		
		collisions(level,scroobs,platforms);
		if (!isFriendly && isOnGround)
		{
			timeSinceEvent++;
			if (timeSinceEvent > 800)
			{
				timeSinceEvent = 0;
				
				if (close(pl->x, pl->y,500)) {
					if (pl->x < x) vector.xPart = -0.5;
					else vector.xPart = 0.5;
					vector.yPart = 0.5;
				}
			}
		}
		updateTexture();
	}

};

#endif