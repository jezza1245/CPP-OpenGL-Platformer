#pragma once
#ifndef SCROOB_H
#define SCROOB_H
#include "Entity.h"

class Scroob : public Entity
{
public:
	bool isOnGround = false;
	Scroob()
	{
	}

	void draw() override
	{
		glLoadIdentity();
		glTranslatef(x, y, 0);
		glPushMatrix();
		//DRAWS A CIRCLE CENTERED AT x,y WITH RADIUS radius
		float x1 = 0, y1 = 0, angle, radian;
		glColor3f(1, 1, 1);
		glBegin(GL_POLYGON);
		for (angle = 1.0; angle < 361.0; angle += 0.2)
		{
			radian = angle * (3.14 / 180.0);

			float xcos = (float)cos(radian);
			float ysin = (float)sin(radian);
			x1 = xcos * radius;
			y1 = ysin * radius;
			float tx = xcos * 0.5 + 0.5;
			float ty = ysin * 0.5 + 0.5;

			//glTexCoord2f(tx, ty);
			glVertex2f(x1, y1);
		}
		glDisable(GL_BLEND);
		glEnd();
		glPopMatrix();
		glPopMatrix();

	}
	
	void applyFriction()
	{
		vector.xPart *= 0.97;
	}
	bool resolveTileCollisions(std::string tileMap)
	{
		return false;
	}
	void resolveMapCollisions(bool onGround)
	{
		if(x-radius<0) //LEFT WALL
		{
			x = radius;
			vector.xPart = (-vector.xPart * 0.5);
		}else if(x+radius > 5000) //RIGHT WALL
		{
			x = 5000 - radius;
			vector.xPart = (-vector.xPart * 0.5);
		}

		if (y - radius < 0) //BOTOM WALL
		{
			y = radius;
			vector.yPart = (-vector.yPart * 0.5);
			isOnGround = true;
		}else if (y + radius > 1250) //TOP WALL
		{
			y = 1250-radius;
			vector.yPart = (-vector.yPart * 0.5);
		}else if(!onGround)
		{
			isOnGround = false;
		}
	}

};

#endif