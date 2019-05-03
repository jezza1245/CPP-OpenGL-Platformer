#pragma once
#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H
#include "MovementVector.h"


class MovingPlatform : public Entity
{
public:
	GLuint texture = 0;
	MovementVector vector = MovementVector(0.1, 0.0);
	float x = 0, y = 0, height = 30, width = 150, lowerBound, upperBound;
	bool horizontallyMoving; //true = horizontal false=vertical
	MovingPlatform(bool paxis, float startx, float starty, float lowerBoundry, float upperBoundry) : Entity()
	{
		horizontallyMoving = paxis;
		x = startx;
		y = starty;
		lowerBound = lowerBoundry;
		upperBound = upperBoundry;
	}
	void update()
	{
		if(horizontallyMoving)
		{
			if (x + width / 2 > upperBound || x - width / 2 < lowerBound)
			{
				if (vector.xPart > 0) x = upperBound - width / 2;
				else x = lowerBound + width / 2;
				vector.xPart = -vector.xPart;
			}
		}else
		{
			if (y + height / 2 > upperBound || y - height / 2 < lowerBound)
			{
				if(vector.yPart > 0) y = upperBound - height / 2;
				else y = lowerBound + height / 2;
				vector.yPart = -vector.yPart;
			}
		}
		x += vector.xPart * dt;
		y += vector.yPart * dt;
	}
	void draw() override
	{
		glLoadIdentity();
		glPushMatrix();
		glTranslatef(x,y,0);
		glColor3f(1.0,0.5,0.0);
		glBegin(GL_POLYGON);
			glVertex2f(-width/2,-height/2);
			glVertex2f(-width / 2, height / 2);
			glVertex2f(width / 2, height / 2);
			glVertex2f(width / 2, -height / 2);
		glEnd();
		glPopMatrix();
	}

};

#endif