#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include "Image_Loading/glew.h"
#include "Player.h"

class Camera
{
public:
	float x = 0, y = 0, width = 950, height = 540;
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

		if (player.y < height / 2 - 200) //STOP CAMERA GOING OFF MAP BOTTOM
		{
			y = -200;
		}
		else if (player.y > 1250 - height / 2) { //STOP CAMERA GOING OFF MAP TOP
			x = 1250 - height;
		}else
		{
			y = player.y - height / 2; //Aim camera at player
		}


		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(x,x+width,y,y+height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
};

#endif