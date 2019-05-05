#pragma once
#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H
#include "MovementVector.h"


class MovingPlatform : public Entity
{
public:
	GLuint texture = grass_platform;
	MovementVector vector;
	float x = 0, y = 0, height = 30, width = 150, lowerBound, upperBound;
	bool horizontallyMoving; //true = horizontal false=vertical
	MovingPlatform(bool paxis, float startx, float starty, float lowerBoundry, float upperBoundry, bool right) : Entity()
	{
		horizontallyMoving = paxis;
		if(horizontallyMoving)
		{
			float startXVel = 0;
			if (right) startXVel = 0.1;
			else startXVel = -0.1;
			vector = MovementVector(startXVel, 0.0);
			
		}
		else
		{
			vector = MovementVector(0.0, 0.1);
		}
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
			x += vector.xPart * dt;
		}else
		{

			if (y + height / 2 > upperBound || y - height / 2 < lowerBound)
			{
				if(vector.yPart > 0) y = upperBound - height / 2;
				else y = lowerBound + height / 2;
				vector.yPart = -vector.yPart;
			}
			y += vector.yPart * dt;
		}
		
		
	}
	void draw() override
	{
		glLoadIdentity();
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glEnable(GL_BLEND);
		glTranslatef(x,y,0);
		//glColor3f(1.0,0.5,0.0);
		glBegin(GL_POLYGON);
			glTexCoord2f(0, 0); glVertex2f(-width/2,-height/2);
			glTexCoord2f(0, 1); glVertex2f(-width / 2, height / 2);
			glTexCoord2f(1, 1); glVertex2f(width / 2, height / 2);
			glTexCoord2f(1, 0); glVertex2f(width / 2, -height / 2);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		if (debug)
		{
			////Draw square
			glPushMatrix();
			glColor3f(1,1,1);
			glTranslatef(x,y,0);
			glLineWidth(3);
			glBegin(GL_LINE_LOOP);
				glVertex2f(-width / 2, -height / 2);
				glVertex2f(-width / 2, height / 2);
				glVertex2f(width / 2, height / 2);
				glVertex2f(width / 2, -height / 2);
			glEnd();
			glPopMatrix();
		}
	}

};

#endif