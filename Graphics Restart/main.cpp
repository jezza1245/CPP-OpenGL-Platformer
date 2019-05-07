
#include "Image_Loading/nvImage.h"
#include "GL\freeglut.h"
#include <iostream>
#include <vector>
#include "Globals.h"
#include <algorithm>
#include "Camera.h"
#include "Player.h"
#include "NPC.h"
#include "MovingPlatform.h"


int	mouse_x=0, mouse_y=0;
int screenWidth=1280, screenHeight=720;
// Timer 
// convert clock ticks to time increment suitable for simulation constants
int old_t;
int gameSpeed = 1;
int timePassed = 0;

bool LeftPressed = false;
bool LeftArrowPressed = false;
bool UpArrowPressed = false;
bool RightArrowPressed = false;
bool DownArrowPressed = false;
char keys[256];

bool restarting = false;
Player* player;
Camera* camera;
/*
	SceneryPiece:
	Square Polygon Textured with or without animation
	Non-interactible used as decoration (non-functional)

 */
class SceneryPiece
{
public:
	GLuint texture = 0; //Base texture
	bool animated = false; //Will it iterate over many textures?
		int currentSpriteIndex = 0; //current index of texture in 'sprites'
		int timeSinceCreation = 0;
	int repeats = 0; //how many texture repeats in the x axis
	float parallax = 0; //parallax offset from character position

	float xmin = 0, ymin = 0, height = 0, width = 0;
	float autoMove = 0; //If moving scenery, current offset from creation
	std::vector<GLuint> sprites; //collection of textures to iterate over
	SceneryPiece()
	{

	}
	SceneryPiece(float x, float y, float w, float h, GLuint tex, float offset, int reps, bool animate)
	{
		xmin = x; ymin = y; width = w; height = h;
		texture = tex;
		parallax = offset;
		repeats = reps;
		if(animated = animate) //WRITE INTO CONSTRUCTOR TO CHOOSE TEXTURES
		{
			//FOR NOW USE DEFAULTS
			tex = waterfall1;
			sprites.push_back(waterfall1);
			sprites.push_back(waterfall2);
			sprites.push_back(waterfall3);
			sprites.push_back(waterfall4);
			sprites.push_back(waterfall5);
			sprites.push_back(waterfall6);
			sprites.push_back(waterfall7);
			sprites.push_back(waterfall8);
			sprites.push_back(waterfall9);
			sprites.push_back(waterfall10);
			sprites.push_back(waterfall11);
			sprites.push_back(waterfall12);
			sprites.push_back(waterfall13);
			sprites.push_back(waterfall14);
		}
		
	}
	void update()
	{
		if(animated) //If animated scenery piece
		{
			if(glutGet(GLUT_ELAPSED_TIME) > timeSinceCreation + 150/dt) //Has enough time passed from last texture switch
			{
				timeSinceCreation = glutGet(GLUT_ELAPSED_TIME); //get current time as last update time
				currentSpriteIndex++; //get next texture index
				if (currentSpriteIndex > sprites.size() - 1) currentSpriteIndex = 0; //cyclic
				texture = sprites.at(currentSpriteIndex); //assign new texture
			}
		}
	}
	void draw()
	{
		glLoadIdentity();
		glPushMatrix();
		if(parallax == 0) //No parralax, no translation
		{
			
		}
		else if (parallax == -1) { //auto moving
			if (!paused) {
				autoMove += 0.1 * dt; //move sideways
				if (autoMove > 910) //reset
				{
					autoMove = 0;
				}
			}
			glTranslatef(autoMove, 0, 0); //apply translation
		}
		else //
		{
			glTranslatef((camera->x) / parallax * 0.1, 0, 0);
		}
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glEnable(GL_BLEND);
		glBegin(GL_POLYGON);
		glTexCoord2f(0.01, 0.01); glVertex2f(xmin, ymin);
		glTexCoord2f(0.01, 0.99); glVertex2f(xmin, ymin+height);
		glTexCoord2f(repeats+0.99, 0.99); glVertex2f(xmin+width, ymin + height);
		glTexCoord2f(repeats+0.99, 0.01); glVertex2f(xmin+width, ymin);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

};




MovingPlatform* platform1;
MovingPlatform* platform2;
MovingPlatform* platform3;
MovingPlatform* platform4;
std::vector<Scroob*> scroobs;
std::vector<MovingPlatform*> platforms;


//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void loadAssets();
void init();				//called in winmain when the program starts.
void processKeys();
void keyPressed(unsigned char c, int x, int y);
void keyReleased(unsigned char c, int x, int y);
void specialPressed(int key, int x, int y);
void specialReleased(int key, int x, int y);
//void CircleCircleCollisions();
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
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	int actualWidth = 5000, actualheight = 1250, nTilesHigh = 25, nTilesWide = 100, tileLength = 50, levelCode = 1;
	std::vector<Entity*> characters;
	std::string layout;
	SceneryPiece ground, sky, vague, medium, detailed, skyFeature;
	std::vector<SceneryPiece> extraScenery;
	int numberOfScroobs = 0;
	Level(int levelNumber)
	{
		debug = false;
		numberOfScroobs = 0;
		levelCode = levelNumber;
		switch (levelNumber)
		{
		case 1:
			//build level 1
			//         |1  |5   |10  |15  |20  |25  |30  |35  |40  |45  |50  |55  |60  |65  |70  |75  |80  |85  |90  |95  |100  	
			layout += "................................................................................####################"; //25(0)
			layout += "....................................................................................................";
			layout += ".....S..........................S...................................................................";
			layout += "..#######..............########################.....####################........####................";
			layout += "....................................................................................................";
			layout += "...............####.................................................................###########....."; //20(5)
			layout += "....................................................................................................";
			layout += "....................................S.........................S....................#................";
			layout += ".....##............................###.......................###...................#................";
			layout += ".......................................E...................E.......................#................";
			layout += "....................##................###.................###......................#........S......."; //15(10)
			layout += "..........................................S.............S..........................#....############";
			layout += "........##...............................###...........###.........................#................";
			layout += "...................................................................................#................";
			layout += "...................................................................................#................";
			layout += ".............##..........................#...............#..............................S..........."; //10(15)
			layout += ".........................................#HHHHHHHHHHHHHHH#..........................###########.....";
			layout += "......S...................................###############...........................................";
			layout += ".....###............................................................................................";
			layout += "....................................................................................................";
			layout += "...................................................................S......E....S...................."; //5(20)
			layout += "........................###############..........#############....#####################.............";
			layout += ".............................................................#HHHH#.................................";
			layout += "..................P.........................E................######.................................";
			layout += "................######HHHHHHHHHHHHHHHHHH######.....................................HHHHHHHHHHHHHHHHH"; //0 (25)

			ground = SceneryPiece(0, -200, 5000, 1080, grass_floor, 0, 2, false);
			sky = SceneryPiece(-5000, 0, 10000, 1250, level1_sky, 1.3, 5, false);
			vague = SceneryPiece(-2000, 0, 10000, 120, level1_farhills, 1.2, 5, false);
			medium = SceneryPiece(-2000, -50, 10000, 1000, level1_closehills, 1.1, 5, false);
			detailed = SceneryPiece(0, -100, 5000, 900, level1_detailedhills, 0, 2, false);
			skyFeature = SceneryPiece(-5000, 0, 10000, 1250, level1_clouds, -1, 5, false);

			extraScenery.push_back(SceneryPiece(350, 0, 200, 348, waterfall1, 0, 0, true));

			platform1 = new MovingPlatform(true, 500, 200, 400, 800, true);
			platform2 = new MovingPlatform(true, 2500, 800, 2300, 2800, true);
			platform3 = new MovingPlatform(true, 4650, 250, 4500, 4900, true);
			platform4 = new MovingPlatform(true, 3900, 250, 3800, 4100, true);
			platforms.push_back(platform1);
			platforms.push_back(platform2);
			platforms.push_back(platform3);
			platforms.push_back(platform4);

			break;
		case 2:
			//build level 1
			layout += "...................................................................................................."; //25
			layout += ".##..................S..................................................########....................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += ".........##################################.....######......############............................";
			layout += "...................................................................................................."; //20
			layout += "....................................................................................................";
			layout += "......................................S.....################........................................";
			layout += "....................S...............................................................................";
			layout += "....................................................................................................";
			layout += ".##...............................##########........................................................"; //15
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += ".........................#########..................................................................";
			layout += "....................................................................................................";
			layout += ".##................................................................................................."; //10
			layout += "................#########...........................................................................";
			layout += "....................................................................................................";
			layout += "..........................#.........................................................................";
			layout += "....................................................................................................";
			layout += ".##...........................#....................................................................."; //5
			layout += "........................#...........................................................................";
			layout += ".......#...........E....#............#..............................................................";
			layout += ".......#.S.S.S.S.S......#...........................................................................";
			layout += "P......#................#....###HHHHHHH#############.HHHHHHHHHHH..################..................";

			ground = SceneryPiece(0, -200, 5000, 1080, grass_floor, 0, 2, false);
			sky = SceneryPiece(-5000, 0, 10000, 1250, level1_sky, 1.3, 5, false);
			vague = SceneryPiece(-2000, 0, 10000, 120, level1_farhills, 1.2, 5, false);
			medium = SceneryPiece(-2000, -50, 10000, 1000, level1_closehills, 1.1, 5, false);
			detailed = SceneryPiece(0, -100, 5000, 900, level1_detailedhills, 0, 2, false);
			skyFeature = SceneryPiece(-5000, 0, 10000, 1250, level1_clouds, -1, 5, false);
			
			platform1 = new MovingPlatform(true, 500, 200, 400, 800, true);
		
			platforms.push_back(platform1);
		
			
			break;
		default:
			layout += "...................................................................................................."; //25
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "...................................................................................................."; //20
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "...................................................................................................."; //15
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "...................................................................................................."; //10
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "....................................................................................................";
			layout += "...................................................................................................."; //5
			layout += "....................................................................................................";
			layout += "#########...........................................................................................";
			layout += ".........#..........................................................................................";
			layout += "....P....#..........................................................................................";
			ground = SceneryPiece(0, -140, 1440, 810, homePage, 0, 0, false);
			paused = true;
			break;
		}
		for(int i = 0; i<scroobs.size(); i++)
		{
			scroobs.erase(scroobs.begin() + i);
		}
		
		player = new Player(25);
		
		for(int i=0; i<layout.length(); i++)  //SPAWN SCROOBS AT LOCATIONS
		{
			if(layout[i] == 'S')
			{
				NPC* newScroob = new NPC(true,40, i % nTilesWide*tileLength + 25, (nTilesHigh - 1 - i / nTilesWide)*tileLength);
				scroobs.push_back(newScroob);
				numberOfScroobs++;
			}
			else if (layout[i] == 'E')
			{ 
				NPC* newScroob = new NPC(false, 50, i % nTilesWide*tileLength + 25, (nTilesHigh - 1 - i / nTilesWide)*tileLength);
				scroobs.push_back(newScroob);
			}
			else if(layout[i] == 'P')
			{
				player->x = i % nTilesWide*tileLength + 25;
				player->y = (nTilesHigh - 1 - i / nTilesWide)*tileLength;
				//std::cout << i % nTilesHigh*tileLength+25 << "," << (nTilesHigh-1-i/nTilesWide)*tileLength << std::endl;

			}
		}
		scroobs.push_back(player);

		restarting = false;
		paused = false;
	}

	~Level()
	{
		if (!platforms.empty()) {
			for (MovingPlatform* platform : platforms)
			{
				delete platform;
			}
			platforms.clear();
		}
		if (!scroobs.empty()) {
			Scroob* findPlayer;
			for (Scroob*scroob : scroobs)
			{
				if (scroob != player)
				{
					delete scroob;
				}else
				{
					findPlayer = scroob;
				}
			}
			scroobs.clear();
			scroobs.push_back(findPlayer);
		}
	}

	void drawTiles()
	{
		for (int y = nTilesHigh - 1; y >= 0; y--) { //int y = height - 1;  y >= 0; y--
			for (int x = 0; x < nTilesWide; x++) { //int x = 0; x < width; x++
				char tile = layout[y*nTilesWide + x];
				GLuint texture = 0;
				bool drawTile = true;
				switch (tile) {
				case '#':
					//set texture
					texture = grass_block;
					//glColor3f(0, 0.8, 0.6);
					break;
				case 'H':
					texture = safe;
					break;
				default:
					drawTile = false;
					//glClearColor(0.0, 0.0, 0.0, 1.0);
					break;
				}

				if (drawTile)
				{
					glPushMatrix();
					glTranslatef(x*tileLength, (nTilesHigh*tileLength) - (y*tileLength) - tileLength, 0);

					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, texture);
					glEnable(GL_BLEND);

					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glBegin(GL_POLYGON);
					glTexCoord2f(0, 0); glVertex2f(0, 0);
					glTexCoord2f(0, 1);  glVertex2f(0, tileLength);
					glTexCoord2f(1, 1);  glVertex2f(tileLength, tileLength);
					glTexCoord2f(1, 0); glVertex2f(tileLength, 0);
					glEnd();
					glDisable(GL_BLEND);
					glDisable(GL_TEXTURE_2D);
					glPopMatrix();

					if (debug)
					{
						////Draw square
						glPushMatrix();
						glColor3f(1, 1, 1);
						glTranslatef(x*tileLength, (nTilesHigh*tileLength) - (y*tileLength) - tileLength, 0);
						glLineWidth(3);
						glBegin(GL_LINE_LOOP);
						glVertex2f(0, 0);
						glVertex2f(0, tileLength);
						glVertex2f(tileLength, tileLength);
						glVertex2f(tileLength, 0);
						glEnd();
						glPopMatrix();

						//Draw Circle Hitbox
					}
				}

			}

		}
	}

	void draw()
	{
		
		glLoadIdentity();
		glPushMatrix();
		if(levelCode == 0)
		{
			ground.draw();
			glPopMatrix();
			return;
		}
		//draw background
		sky.draw();
		vague.draw();
		medium.draw();
		detailed.draw();

		

		for (auto& extra : extraScenery)
		{
			extra.draw();
		}
		
		//draw platforms
		for (auto& platform : platforms)
		{
			platform->draw();
		}
		

		//draw entities
		if (!debug) {
			for (auto& scroob : scroobs)
			{
				scroob->draw();
			}
		}
		//draw foreground

		drawTiles();

		ground.draw();
		skyFeature.draw();

		if (debug) {
			glPushMatrix();
			glColor3f(1, 1, 1);
			glLineWidth(3);
			glBegin(GL_LINE_LOOP);
			glVertex2f(0, 0);
			glVertex2f(0, actualheight);
			glVertex2f(actualWidth, actualheight);
			glVertex2f(actualWidth, 0);
			glEnd();
			glPopMatrix();

			//draw entities
			for (auto& scroob : scroobs)
			{
				scroob->draw();
			}
		}

		glPopMatrix();

	}
};
Level* currentLevel;

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()																	
{
	
	int new_t = gameTime = glutGet(GLUT_ELAPSED_TIME);
	dt = (new_t - old_t);
	if (dt < 0) {
		dt = 0;
	}if(debug)
	{
		dt = 1;
	}
	

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	
	/*
		Game Code Begin...
	*/
	if(!restarting) currentLevel->draw();
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
void loadAssets()
{
	homePage = loadPNG("assets/titlescreen.png");

	scroob_normal = loadPNG("assets/scroobs/scroob_normal.png");
	scroob_silly = loadPNG("assets/scroobs/scroob_silly.png");
	scroob_enemy = loadPNG("assets/scroobs/enemy_normal.png");

	grass_block = loadPNG("assets/level1/grass_blockt.png");
	grass_platform = loadPNG("assets/level1/platform.png");
	grass_floor = loadPNG("assets/level1/bg1e.png");

	level1_sky = loadPNG("assets/level1/bg1a.png");
	level1_clouds = loadPNG("assets/level1/bg1f.png");
	level1_closehills = loadPNG("assets/level1/bg1c.png");
	level1_farhills = loadPNG("assets/level1/bg1b.png");
	level1_detailedhills = loadPNG("assets/level1/bg1d.png");
	safe = loadPNG("assets/glass.png");

	waterfall1 = loadPNG("assets/waterfalls/waterfall_a.png");
	waterfall2 = loadPNG("assets/waterfalls/waterfall_b.png");
	waterfall3 = loadPNG("assets/waterfalls/waterfall_c.png");
	waterfall4 = loadPNG("assets/waterfalls/waterfall_d.png");
	waterfall5 = loadPNG("assets/waterfalls/waterfall_e.png");
	waterfall6 = loadPNG("assets/waterfalls/waterfall_f.png");
	waterfall7 = loadPNG("assets/waterfalls/waterfall_g.png");
	waterfall8 = loadPNG("assets/waterfalls/waterfall_h.png");
	waterfall9 = loadPNG("assets/waterfalls/waterfall_i.png");
	waterfall10 = loadPNG("assets/waterfalls/waterfall_j.png");
	waterfall11 = loadPNG("assets/waterfalls/waterfall_k.png");
	waterfall12 = loadPNG("assets/waterfalls/waterfall_l.png");
	waterfall13 = loadPNG("assets/waterfalls/waterfall_m.png");
	waterfall14 = loadPNG("assets/waterfalls/waterfall_n.png");
}
void init()
{
	//glClearColor(0.0,0.0,0.0,1.0);
	loadAssets();


	currentLevel = new Level(0);
	gameTime = glutGet(GLUT_ELAPSED_TIME);
	camera = new Camera();

	old_t = glutGet(GLUT_ELAPSED_TIME);
	timePassed = glutGet(GLUT_ELAPSED_TIME);
}

void keyPressed(unsigned char c, int x, int y) {
	keys[c] = true;

	if(c == 27) paused = !paused;
	if (c == '/') debug = !debug;
	if (debug && c == '.') {
		cameraLock = !cameraLock;
		std::cout << "CAMERA LOCK: " << cameraLock << std::endl;
	}
}
void keyReleased(unsigned char c, int x, int y) {
	keys[c] = false; 
}
void processKeys() 
{
	if (!paused) {
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
	}
	else
	{
		if (keys['r']) {
			restarting = true;
			int levelCode = currentLevel->levelCode;
			delete currentLevel;
			currentLevel = new Level(levelCode);
		}
		if (keys['0']) {
			restarting = true;
			delete currentLevel;
			currentLevel = new Level(0);
		}
		if (keys['1']) {
			restarting = true;
			delete currentLevel;
			currentLevel = new Level(1);
		}
		if (keys['2']) {
			restarting = true;
			delete currentLevel;
			currentLevel = new Level(2);
		}

	}
	if (LeftArrowPressed) camera->moveLeft();
	if (RightArrowPressed) camera->moveRight();
	if (UpArrowPressed) camera->moveUp();
	if (DownArrowPressed) camera->moveDown();
}

void specialPressed(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		LeftArrowPressed = true;
		break;
	case GLUT_KEY_RIGHT:
		RightArrowPressed = true;
		break;
	case GLUT_KEY_DOWN:
		DownArrowPressed = true;
		break;
	case GLUT_KEY_UP:
		UpArrowPressed = true;
		break;
	}

}

void specialReleased(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		LeftArrowPressed = false;
		break;
	case GLUT_KEY_RIGHT:
		RightArrowPressed = false;
		break;
	case GLUT_KEY_DOWN:
		DownArrowPressed = false;
		break;
	case GLUT_KEY_UP:
		UpArrowPressed = false;
		break;
	}
}


void special(int key, int x, int y)
{
	if (!cameraLock) {
		if(key == GLUT_KEY_LEFT)
		{
			camera->moveLeft();
		}
		if (key == GLUT_KEY_RIGHT)
		{
			camera->moveRight();
		}
		if (key == GLUT_KEY_UP)
		{
			camera->moveUp();
		}
		if (key == GLUT_KEY_DOWN)
		{
			camera->moveDown();
		}
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
	if(!debug)
	{
		cameraLock = true;
	}
	processKeys();
	if((player->dead || currentLevel->numberOfScroobs == 0 )&& currentLevel->levelCode != 0)
	{
		restarting = true;
		delete currentLevel;
		currentLevel = new Level(0);
		return;
	}
	if (currentLevel->levelCode == 0)
	{
		camera->x = 0;
		camera->y = 0;
	};
	if (!paused && !restarting) {
		if(scroobs.size() < 2)
		{
			gameOver = true;
		}
		for (int i=0; i<scroobs.size(); i++) //REMOVE DEAD/SAFE SCROOBS
		{
			if(scroobs.at(i)->dead)
			{
				currentLevel->numberOfScroobs--;
				delete scroobs.at(i);
				scroobs.erase(scroobs.begin() + i);
			};
		}

		for (auto& extra : currentLevel->extraScenery)
		{
			extra.update();
		}
		for (auto& platform : platforms) //UPDATE PLATFORM POSITIONS
		{
			platform->update();
		}

		for (auto& scroob : scroobs) //UPDATE AND PERFORM COLLISIONS FOR ALL SCROOBS
		{
			scroob->update(currentLevel->layout, scroobs,platforms, player);
		}

		//CircleCircleCollisions();

		camera->update(*player); //AIM CAMERA AT PLAYER

		
	}
	
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
	glutSpecialFunc(specialPressed);
	glutSpecialUpFunc(specialReleased);
	
	glutMainLoop();
	
	return 0;
}
