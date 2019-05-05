#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include "MovementVector.h"

class Entity 
{
public:
	GLuint texture = 0;
	MovementVector vector = MovementVector(0,0.0);
	float x = 0, y = 0;
	bool isOnGround = false;

	virtual void draw() = 0;
	
	bool close(float otherX, float otherY, float distance)
	{
		return abs(x - otherX)<distance && abs(y - otherY)< distance;
	}

	int getXTileCoordinate(float x)
	{
		return x / 50; //50 is H and W
	}
	int getYTileCoordinate(float y)
	{
		return y / 50; //50 is H and W
	}
	char findTile(float x, float y, std::string layout)
	{
		int tilesInX = 100, tilesInY = 25;
		int xTileCoordinate = getXTileCoordinate(x);
		int yTileCoordinate = getYTileCoordinate(y);

		if(xTileCoordinate < tilesInX && xTileCoordinate >= 0 &&
			yTileCoordinate < tilesInY && yTileCoordinate >= 0)
		{
			return layout[(tilesInY - 1 - yTileCoordinate)*tilesInX + xTileCoordinate];
		}
		return NULL;

	}
	bool AABBCollision(float width, float height,
		float bxmin, float bxmax, float bymin, float bymax)
	{
		width /= 2; height /= 2;
		if (x-width < bxmax && x+width > bxmin && y-height < bymax && y+height > bymin) {
			if (vector.xPart > 0) //Travelling right
			{
				if (x - bxmin < 0) //left side
				{
					x = bxmin - width;
					vector.xPart = -vector.xPart;
				}
				else //bottom or top side 
				{
					if (vector.yPart <= 0) //top side
					{
						y = bymax + height;
						isOnGround = true;
						if(vector.yPart > -0.04)
						{
							vector.yPart = 0;
						}else
						{
							vector.yPart = -vector.yPart * 0.6;
						}
					}
					else //bottom side
					{
						y = bymin - height;
						vector.yPart = -vector.yPart * 0.6;
					}
					
				}
			}
			else //Travelling left
			{
				if (x - bxmax > 0) //right side
				{
					x = bxmax + width;
					vector.xPart = -vector.xPart;
				}
				else //top or bottom side
				{
					if (vector.yPart <= 0) //top side
					{
						y = bymax + height;
						isOnGround = true;
						if (vector.yPart > -0.04)
						{
							vector.yPart = 0;
						}
						else
						{
							vector.yPart = -vector.yPart * 0.6;
						}
					}
					else //bottom side
					{
						y = bymin - height;
						vector.yPart = -vector.yPart * 0.6;
					}
					
					
				}
			}
			return true;
		}
		return false;
	}
};

#endif