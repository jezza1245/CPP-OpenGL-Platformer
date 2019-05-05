#pragma once
#ifndef SCROOB_H
#define SCROOB_H
#include "Entity.h"
#include "MovingPlatform.h"
#include "Globals.h"

class Player;

class Scroob : public Entity
{
public:
	bool dead = false, isFriendly = true;
	float rollAngle = 0, radius = 1;
	Scroob() = default;
	float r=0, g=0, b=0;
	virtual void updateTexture() = 0;
	virtual void update(std::string layout, std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms, Player* p) = 0;
	void draw() override
	{
		glLoadIdentity();
		glPushMatrix(); // ENTITY
			glTranslatef(x, y, 0);
			glPushMatrix(); //PUSH CHARACTER ROTATION
			if (!paused) {
				rollAngle += (-vector.xPart * 2 * dt);
			}
				glRotatef(rollAngle, 0, 0, 1); //roll
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texture);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//DRAWS A CIRCLE CENTERED AT x,y WITH RADIUS radius
				float x1 = 0, y1 = 0, angle, radian;
				//glColor3f(1, 1, 1);
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

			
				glEnd();

				glDisable(GL_BLEND);
				glDisable(GL_TEXTURE_2D);
			glPopMatrix(); //POP CHARACTER ROTATION
			if (debug)
			{

				//Draw square
				glPushMatrix();
				glColor3f(1,1,1);
				glLineWidth(3);
				glBegin(GL_LINE_LOOP);
					glVertex2f(-radius/2, -radius/2);
					glVertex2f(-radius/2, radius/2);
;					glVertex2f(radius/2, radius/2);
					glVertex2f(radius/2, -radius/2);
				glEnd();
				glPopMatrix();

				glPushMatrix();
				glColor3f(1.0, 0.5, 0);
				glBegin(GL_LINE_LOOP);
				
				for (angle = 1.0; angle <= 361.0; angle += 20)
				{
					radian = angle * (3.14 / 180.0);

					float xcos = (float)cos(radian);
					float ysin = (float)sin(radian);
					x1 = xcos * radius;
					y1 = ysin * radius;

					glVertex2f(x1, y1);
				}
				glEnd();
				glColor3f(1, 1, 1);
				glPopMatrix();

			}
		glPopMatrix(); //ENTITY

	}
	
	virtual void applyFriction() = 0;
	void leadingPointCollision()
	{
		//get angle ball is heading from north, clockwise
		//int angularDirection = atan2(vector.xPart, vector.yPart) * 180 / 3.14156; //Direction player is heading
		//std::cout << angularDirection << std::endl;
		//std::cout << xpos <<"," << ypos << std::endl;

		// Get Leading Point x,y
		//float newang = (ang / 180) * 3.14156; //convert to rads
		//float xpos = xTrans + (25 * sin(newang)); //find x coord
		//float ypos = yTrans + (25 * cos(newang)); //find y coord
	}
	void collisions(std::string level, std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms)
	{
		bool collidedWithGround = false;
		
		collidedWithGround = resolveTileCollisions(level);
		resolveMapCollisions(collidedWithGround);
		resolvePlatformCollisions(platforms);
		CircleCircleCollisions(level,scroobs,platforms);
		if(radius<5)
		{
			dead = true;
		}
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

			bool bottomCollision = false;


			//-------------HITTING TWO TILES AT ONCE -------------------
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


			//-------------HITTING JUST ONE TILE ------------------------
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
				return true;
			}
			else if (bl)
			{
				xmin = getXTileCoordinate(x - radius) * 50;
				ymin = getYTileCoordinate(y - radius) * 50;
				xmax = xmin + 50;
				ymax = ymin + 50;
				AABBCollision(radius, radius, xmin, xmax, ymin, ymax);
				return true;
			}

			


			return false;

		}

		return false;
	}
	void resolvePlatformCollisions(std::vector<MovingPlatform*> platforms)
	{
		for(auto& platform : platforms)
		{
			if (close(platform->x, platform->y, 500)) {

				if (AABBCollision(radius, radius, platform->x - platform->width / 2, platform->x + platform->width / 2,
					platform->y - platform->height / 2, platform->y + platform->height / 2)) {
					x += platform->vector.xPart * dt;
					y += platform->vector.yPart * dt;
					if (vector.yPart < -0.7) {
						vector.yPart = (-vector.yPart * 0.7);
					}
					else
					{
						vector.yPart = 0;
					}
				}
			}
		}
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
			if (vector.yPart < -0.4) {
				vector.yPart = (-vector.yPart * 0.7);
			}
			else
			{
				vector.yPart = 0;
			}
			y = radius;
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
	bool CircleCircleCollisions(std::string level,std::vector<Scroob*> scroobs, std::vector<MovingPlatform*> platforms)
	{
		float xDist, yDist;	
		for (int j = 0; j < scroobs.size(); j++) {
			if (this == scroobs.at(j)) continue;
			Scroob* B = scroobs.at(j);
			if (!close(B->x,B->y,200)) continue;
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

						if(!B->isFriendly)
						{
							radius -= 5;
						}
						if(!isFriendly)
						{
							B->radius -= 5;
						}

						B->collisions(level,scroobs,platforms);
						return true;
					}
				}
			}

		}
		return false;
	}
};

#endif