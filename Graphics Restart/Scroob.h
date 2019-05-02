#pragma once
#ifndef SCROOB_H
#define SCROOB_H
#include "Entity.h"

class Scroob : public Entity
{
public:
	float rollAngle = 0, radius = 1;
	Scroob() = default;
	virtual void update(std::string layout, std::vector<Scroob*> scroobs) = 0;
	void draw() override
	{
		glLoadIdentity();
		glPushMatrix(); // ENTITY
			glTranslatef(x, y, 0);
			glPushMatrix(); //PUSH CHARACTER ROTATION
			rollAngle += (-vector.xPart * dt);
				glRotatef(rollAngle, 0, 0, 1); //roll
				//glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//DRAWS A CIRCLE CENTERED AT x,y WITH RADIUS radius
				float x1 = 0, y1 = 0, angle, radian;
				glColor3f(1, 1, 1);
				glBegin(GL_POLYGON);
				for (angle = 1.0; angle < 361.0; angle += 20)
				{
					radian = angle * (3.14 / 180.0);

					float xcos = (float)cos(radian);
					float ysin = (float)sin(radian);
					x1 = xcos * radius;
					y1 = ysin * radius;
					float tx = xcos * 0.5 + 0.5;
					float ty = ysin * 0.5 + 0.5;

					glTexCoord2f(tx, ty); //fine
					glVertex2f(x1, y1);
				}
				
				glDisable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);
				glEnd();
			glPopMatrix(); //POP CHARACTER TRANSLATION
		glPopMatrix(); //ENTITY

	}
	
	void applyFriction()
	{
		vector.xPart *= 0.97;
	}
	void collisions(std::string level, std::vector<Scroob*> scroobs)
	{
		bool collidedWithGround = false;
		collidedWithGround = resolveTileCollisions(level);
		resolveMapCollisions(collidedWithGround);
		CircleCircleCollisions(level,scroobs);
	}
	bool resolveTileCollisions(std::string tileMap) //return true if bottom collision
	{
		bool tl = findTile(x-radius,y+radius,tileMap)=='#';
		bool tr = findTile(x+radius, y + radius, tileMap) == '#';
		bool bl = findTile(x - radius, y - radius, tileMap) == '#';
		bool br = findTile(x + radius, y -radius, tileMap) == '#';

		if (tl || tr || br || bl) {
			//Set block dimensions for collision with
			float xmin, xmax, ymin, ymax;

			//TOP COLLISION?
			if (tl && tr) 
			{
				xmin = getXTileCoordinate(x - radius) * 50;
				ymin = getYTileCoordinate(y + radius) * 50;
				xmax = xmin + 100;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}
			//RIGHT COLLISION
			else if (tr && br) {
				xmin = getXTileCoordinate(x + radius) * 50;
				ymin = getYTileCoordinate(y - radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 100;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}
			//BOTTOM COLLISION
			else if (br && bl)
			{
				xmin = getXTileCoordinate(x - radius) * 50;
				ymin = getYTileCoordinate(y - radius) * 50;
				xmax = xmin + 100;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
				return true;
			}
			//LEFT COLLISION
			else if (bl && tl) 
			{
				xmin = getXTileCoordinate(x - radius) * 50;
				ymin = getYTileCoordinate(y - radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 100;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}
			else if(tl)
			{
				xmin = getXTileCoordinate(x - radius) * 50;
				ymin = getYTileCoordinate(y + radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}
			else if (tr)
			{
				xmin = getXTileCoordinate(x + radius) * 50;
				ymin = getYTileCoordinate(y + radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}
			else if (br)
			{
				xmin = getXTileCoordinate(x + radius) * 50;
				ymin = getYTileCoordinate(y - radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}
			else if (bl)
			{
				xmin = getXTileCoordinate(x - radius) * 50;
				ymin = getYTileCoordinate(y - radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
			}

			return false;

		}
		//BOTTOM RIGHT COLLISION?
		//RIGHT SIDE?
		//BOTTOM SIDE?
		//ELSE NORMAL

		return false;
	}
	void resolveMapCollisions(bool onGround)
	{
		if(x-radius<0) //LEFT WALL
		{
			x = radius;
			vector.xPart = (-vector.xPart * 0.8);
		}else if(x+radius > 5000) //RIGHT WALL
		{
			x = 5000 - radius;
			vector.xPart = (-vector.xPart * 0.5);
		}

		if (y - radius < 0) //BOTOM WALL
		{

			y = radius;
			if (vector.yPart < -0.4) {
				vector.yPart = (-vector.yPart * 0.7);
			}
			else
			{
				vector.yPart = 0;
			}
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
	bool CircleCircleCollisions(std::string level,std::vector<Scroob*> scroobs)
	{
		float xDist, yDist;	
		for (int j = 0; j < scroobs.size(); j++) {
			if (this == scroobs.at(j)) continue;
			Scroob* B = scroobs.at(j);
			if (abs(x-B->x)>200 && abs(y - B->y) > 200) continue;
			float axmin = x - radius;
			float axmax = x + radius;
			float aymin = y - radius;
			float aymax = y + radius;

			float bxmin = B->x - B->radius;
			float bxmax = B->x + B->radius;
			float bymin = B->y - B->radius;
			float bymax = B->y + B->radius;

			if (axmin < bxmax && axmax > bxmin && aymin < bymax && aymax > bymin) {
				xDist = x - B->x;
				yDist = y - B->y;
				float distSquared = xDist * xDist + yDist * yDist;
				//Check the squared distances
				if (distSquared <= (radius + B->radius)*(radius + B->radius)) {
					x += (-vector.xPart * dt);
					//B->x += (-B->vector.xPart * dt);
					y += (-vector.yPart * dt);
					//B->y += (-B->vector.yPart * dt);
					float xVelocity = B->vector.xPart - vector.xPart;
					float yVelocity = B->vector.yPart - vector.yPart;
					float dotProduct = xDist * xVelocity + yDist * yVelocity;
					//check if the objects move towards one another.
					if (dotProduct > 0) {
						double collisionScale = dotProduct / distSquared;
						double xCollision = xDist * collisionScale;
						double yCollision = yDist * collisionScale;

						vector.xPart += xCollision;
						vector.yPart += yCollision;
						B->vector.xPart -= xCollision;
						B->vector.yPart -= yCollision;

						B->collisions(level,scroobs);
						return true;
					}
				}
			}

		}
		return false;
	}
};

#endif