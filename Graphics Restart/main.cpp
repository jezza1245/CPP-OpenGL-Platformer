
#include "Image_Loading/nvImage.h"
#include "GL\freeglut.h"
#include <iostream>
#include <vector>
#include "Globals.h"
#include <algorithm>
#include "Camera.h"
#include "Player.h"

int	mouse_x=0, mouse_y=0;
int screenWidth=1280, screenHeight=720;
// Timer 
// convert clock ticks to time increment suitable for simulation constants
int old_t;
int gameSpeed = 1;
int timePassed = 0;
bool debug = false;
bool LeftPressed = false;
char keys[256];

Camera* camera;
Player* player;
std::vector<Entity*> entities;


//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();
void keyPressed();
void keyReleased();
void special(int key, int x, int y);
void update();				//called in winmain to update variables

GLuint loadPNG(char* name)
{
	// Texture loading object
	nv::Image img;

	GLuint myTextureID;

	// Return true on success
	if (img.loadImageFromFile(name))
	{
		glGenTextures(1, &myTextureID);
		glBindTexture(GL_TEXTURE_2D, myTextureID);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, img.getInternalFormat(), img.getWidth(), img.getHeight(), 0, img.getFormat(), img.getType(), img.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	}
	else {
		std::cout << "ERROR LOADING TEXTURE";
		return 0;
	}

	return myTextureID;
}

class Level
{
public:
	int actualWidth = 5000, actualheight = 1250, nTilesHigh = 25, nTilesWide = 100, tileLength = 50;
	std::vector<Entity*> characters;
	std::string layout;
	Level(int levelNumber, std::vector<Entity*> entities)
	{
		switch (levelNumber)
		{
		case 1:
			//build level 1
			layout += "........................#..........................................................................."; //25
			layout += ".......................#............................................................................";
			layout += "......................#.............................................................................";
			layout += ".....................#..............................................................................";
			layout += "....................#...............................................................................";
			layout += "...................#................................................................................"; //20
			layout += "..................#.................................................................................";
			layout += ".................#..................................................................................";
			layout += "................#...................................................................................";
			layout += "...............#....................................................................................";
			layout += "..............#....................................................................................."; //15
			layout += ".............#......................................................................................";
			layout += "............#.......................................................................................";
			layout += "...........#........................................................................................";
			layout += "..........#.........................................................................................";
			layout += ".........#.........................................................................................."; //10
			layout += "........#...........................................................................................";
			layout += ".......###..........................................................................................";
			layout += "......#.............................................................................................";
			layout += ".....#..............................................................................................";
			layout += "....#..............................................................................................."; //5
			layout += "...#................................................................................................";
			layout += "..#.................................................................................................";
			layout += ".#..................................................................................................";
			layout += "#...................................................................................................";
			break;
		case 2:
			//build level 2
			break;
		default:
			//build level 1?
			break;
		}

		characters = entities;
	}

	void draw()
	{
		glLoadIdentity();
		glPushMatrix();
		//draw background


		//draw tiles
		for (int y = nTilesHigh - 1; y >= 0; y--) { //int y = height - 1;  y >= 0; y--
			for (int x = 0; x < nTilesWide; x++) { //int x = 0; x < width; x++
				char tile = layout[y*nTilesWide + x];
				bool drawTile = true;
				switch (tile) {
				case '#':
					//set texture
					glColor3f(0, 0.8, 0.6);
					break;
				default:
					drawTile = false;
					glClearColor(0.0, 0.0, 0.0, 1.0);
					break;
				}

				if (drawTile)
				{
					glPushMatrix();
					glTranslatef(x*tileLength, (nTilesHigh*tileLength) - (y*tileLength) - tileLength, 0);
					glBegin(GL_POLYGON);
					glVertex2f(0, 0);
					glVertex2f(0, tileLength);
					glVertex2f(tileLength, tileLength);
					glVertex2f(tileLength, 0);
					glEnd();
					glPopMatrix();
				}

			}

		}
		glPopMatrix();

		//draw entities
		for (auto& entity : entities)
		{
			entity->draw();
		}
		//draw foreground


	}
};
Level* currentLevel;

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()																	
{
	
	int new_t = glutGet(GLUT_ELAPSED_TIME);
	dt = (new_t - old_t);
	if (dt < 0) {
		dt = 0;
	}if(debug)
	{
		dt *= 0.1;
	}
	

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	
	/*
		Game Code Begin...
	*/
	/*glPushMatrix();

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 0);
	glVertex2f(50, 50);
	glVertex2f(50, 100);
	glVertex2f(100, 100);
	glVertex2f(100, 50);
	glEnd();
	glPopMatrix();*/
	std::cout << player->x << std::endl;
	currentLevel->draw();
	/*
		Game Code End
	*/

	glutPostRedisplay();
	glFlush();
	glutSwapBuffers();
	old_t = new_t;
}


void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
	// we will use these values to set the coordinate system
	
	glViewport(0,0,width,height);						
	
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									

	gluOrtho2D(0, 950, 0, 540);           // set the coordinate system for the window
	
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									
}
void init()
{
	glClearColor(0.0,0.0,0.0,1.0);

	player = new Player(25);
	player->x = 100;
	player->y = 100;
	entities.push_back(player);
	currentLevel = new Level(1, entities);
	camera = new Camera();

	old_t = glutGet(GLUT_ELAPSED_TIME);
	timePassed = glutGet(GLUT_ELAPSED_TIME);
}

void keyPressed(unsigned char c, int x, int y) {
	keys[c] = true;
}
void keyReleased(unsigned char c, int x, int y) {
	keys[c] = false;
}

void processKeys() 
{
	if (keys['w']) {
		player->jump();
	}
	if (keys['a']) {
		player->moveLeft();
	}
	if (keys['d']) {
		player->moveRight();
	}
	if (keys['s']) {
		player->fastFall();
	}
	if (keys[32]) {
		debug = true;
	}else
	{
		debug = false;
	}

}

void special(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
			
			break;
		case GLUT_KEY_RIGHT:
			
			break;
		case GLUT_KEY_UP:
			break;
		case GLUT_KEY_DOWN:
			break;
	}
}

void mouseMotion(int x, int y)
{
	mouse_y = y;
	mouse_x = x;

}

void mouse(int button, int state, int x, int y)
{
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN)
			{
				LeftPressed = true;
			}	
			else
				LeftPressed = false;
			break;
		case GLUT_RIGHT_BUTTON:
			
			break;
		default:
			break;
	}
}

void update()
{
	/*
		Update Code Start
	*/

	for (auto& entity : entities)
	{
		entity->update(currentLevel->layout);
	}

	camera->update(*player);


	processKeys();
	/*
		Update Code End
	*/
	glutPostRedisplay();
}




/**************** END OPENGL FUNCTIONS *************************/
int main(int argc, char **argv)
{

	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenWidth, screenHeight);
   	glutInitWindowPosition(100,100);
	
	glutCreateWindow("Graphics 1 Coursework - 100126866");
	
	init();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouseMotion);
	glutIdleFunc(update);
	
	//add keyboard callback.
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyReleased);
	//add callback for the function keys.
	glutSpecialFunc(special);
	
	glutMainLoop();
	
	return 0;
}
