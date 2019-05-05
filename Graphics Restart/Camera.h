#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include "Image_Loading/glew.h"
#include "Player.h"

class Camera
{
public:
	float x = 0, y = 0, width = 950, height = 540;
	MovementVector vector = MovementVector(0.1,1.0);

	void update(Player player)
	{
		//Update based on character position

		if (player.x < width / 2) //STOP CAMERA GOING OFF MAP LEFT
		{
			x = 0;
		}
		else if (player.x > 5000 - width / 2) { //STOP CAMERA GOING OFF MAP RIGHT
			x = 5000 - width;
		}else
		{
			x = player.x - width / 2; //Aim camera at player
		}

		//CAMERA YPOS FOLLOWS PLAYER ON DELAY

		//Camera Y positioning V1.0

		y = player.y - height / 2; //Aim camera at player

		//CAMERA Y positioning V2.0, delayed camera
		//float scaleFactor = 1;
		//scaleFactor =  1 /( player.y - y + height / 2);
		//scaleFactor *= 4;

		//if(abs(player.vector.yPart) < 0.05) 
		//
		//if(player.y > y+height/2) //If player above centre of camera
		//{
		//	y += vector.yPart * dt * scaleFactor ; //Tell camera to move up my vector
		//	if(player.y > y + height / 4 * 3 && player.vector.yPart > 0) //Player is 3/4 up screen, lock
		//	{
		//		y = player.y - height / 4 * 3; 
		//	}
		//}else
		//{
		//	y += vector.yPart * dt * scaleFactor; //Tell camera to move down by vector
		//	if (player.y < y + height / 4 && player.vector.yPart < 0) //Player is 1/4 up screen, lock
		//	{
		//		y = player.y - height / 4;
		//	}
		//}
		

		//CAMERA Y positioning V3.0, when player has very low velocity, change to using vector
		
		//if(abs(player.vector.yPart) < 0.9)
		//{
		//	std::cout << "SLOW" << std::endl;
		//	vector = -player.vector;
		//	y += vector.yPart *dt;
		//	if (player.y < -140 + height / 2) //STOP CAMERA GOING OFF MAP BOTTOM
		//	{
		//		y = -140;
		//	}
		//	else if (player.y > 1250 - height / 2) { //STOP CAMERA GOING OFF MAP TOP
		//		y = 1250 - height;
		//	}

		//}
		

		




		if (y < -140) //STOP CAMERA GOING OFF MAP BOTTOM
		{
			y = -140;
		}
		else if (y>1250-height) { //STOP CAMERA GOING OFF MAP TOP
			y = 1250 - height;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(x,x+width,y,y+height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
};

#endif