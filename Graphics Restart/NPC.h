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


		//Check radius
		if (isFriendly) {
			if (radius * 0.99995 < 10)
			{
				dead = true;
			}
			else { //Shrink
				radius *= 0.99995;
			}
		}

		vector += gravity;
		
		x += (vector.xPart * dt);
		y += (vector.yPart * dt);
		collisions(level, scroobs, platforms);

		//If safe
		if(!dead && isFriendly && findTile(x,y,level) == 'H')
		{
			points += int(radius);
			dead = true;
			std::cout << "+" << radius << "   (" << points << ")" << std::endl;
		}

		//Enemy movement
		if (!isFriendly && isOnGround)
		{
			timeSinceEvent++;
			if (timeSinceEvent > 500)
			{
				timeSinceEvent = 0;
				
				if (close(pl->x, pl->y,500)) {
					vector = MovementVector((pl->x - x)/200, (pl->y - y)/200);
				}
			}
		}
		updateTexture();
	}

};

#endif