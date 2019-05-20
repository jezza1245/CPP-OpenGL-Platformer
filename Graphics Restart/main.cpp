
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


int	mouse_x=0, mouse_y=0; //Current mouse x,y coordinates
int screenWidth=1280, screenHeight=720; 
int old_t; //Used to calcuate dt

//Arrow Keys
bool LeftPressed = false;
bool LeftArrowPressed = false;
bool UpArrowPressed = false;
bool RightArrowPressed = false;
bool DownArrowPressed = false;

//Normal Keys
char keys[256];

//Text on opening screen
float textscale = 0.8;
bool textShrinking = false;

//If game is restarting, used to pause functions
bool restarting = false;


Player* player; //PLAYER ENTITY
Camera* camera; //CAMERA ENTITY

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
	int timeSinceCreation = 0; //used for animation
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
		if(animated = animate) //only one animated version (waterfall)
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
	void update() //Update scenery
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
			if (parallax == -1) { //auto moving
				if (!paused) {
					autoMove += 0.1 * dt; //move sideways
					if (autoMove > 1400) //reset
					{
						autoMove = 0;
					}
				}
				glTranslatef(autoMove, 0, 0); //apply translation
			}
			else if(parallax>0)// PARALLAX - Translate scenery by its translation factor
			{
				glTranslatef((camera->x) / parallax * 0.1, 0, 0);
			}
		

			//Draw Scenery
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
			glEnable(GL_BLEND);
			glBegin(GL_POLYGON);
			//slightly smaller texcoords than full texture so avoid artifacts
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



//Player, Camera and platforms are global and restricted to what already exists
	//good idea to make platforms similiar to other entities and create dynamically
MovingPlatform* platform1; 
MovingPlatform* platform2;
MovingPlatform* platform3;
MovingPlatform* platform4;

//Collection of scroob entities, cleared and filled dynamically by level
std::vector<Scroob*> scroobs;
//Collection of platform entities
std::vector<MovingPlatform*> platforms;


//OPENGL FUNCTION PROTOTYPES
void display();										//display function
void reshape(int width, int height);				//called when the window is resized
void loadAssets();									//loads static game files such as images
void init();										//called in winmain when the program starts.
void processKeys();									//process main user keys
void keyPressed(unsigned char c, int x, int y);		//called automatically if regular key is pressed
void keyReleased(unsigned char c, int x, int y);	//called automatically if regular key is released
void specialPressed(int key, int x, int y);			//called automatically if special key is pressed
void specialReleased(int key, int x, int y);		//called automatically if special key is released
void writeText(int x, int y, float red, float green, float blue, std::string text); //write bitmap text at x,y, with colour r,g,b
void special(int key, int x, int y);				//process arrow keys
void update();										//called in winmain to update variables


//Load PNG files (From lab example)
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

//Writes bitmap text to screen
void writeText(int x, int y, float red, float green, float blue, std::string text)
{
	glColor3f(red,green,blue); //Set colour
	glRasterPos2f(x, y); //Set position
	for(int i=0; i< text.length(); i++) //Draw every character
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	}
	glColor3f(1, 1, 1); //Return colour for consistency (was having issue with bleeding colour)
}

/*
	Contains a tileMap, set of scenery, and a code (level number/state)

	Provided level code calls relevant constructor and everything is initialised in each overloaded instance

	Destructor deletes all entities from memory and clears the respective collections
		(player entity remains untouched)
*/
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
		debug = false; //Exit if in debug mode previously
		numberOfScroobs = 0; //Set scroob count to 0
		levelCode = levelNumber; //Save the level code
		if (levelNumber != 0) points = 0; //Keep score if entering menu level (in order to show it)
		switch (levelNumber)
		{
		case 1:
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
			layout += "......S.............................................................................................";
			layout += ".....###............................................................................................";
			layout += "....................................................................................................";
			layout += "...................................................................S......E....S...................."; //5(20)
			layout += "........................###############..........#############....#####################.............";
			layout += "..............................................................HHHH..................................";
			layout += "..................P.........................E.......................................................";
			layout += "................######HHHHHHHHHHHHHHHHHH######.....................................HHHHHHHHHHHHHHHHH"; //0 (25)

			//Initialise map textures
			ground = SceneryPiece(0, -200, 5000, 1080, grass_floor, 0, 2, false);
			sky = SceneryPiece(-5000, 0, 10000, 1250, level1_sky, 1.3, 5, false);
			vague = SceneryPiece(-2000, 0, 10000, 120, level1_farhills, 1.2, 5, false);
			medium = SceneryPiece(-2000, -50, 10000, 1000, level1_closehills, 1.1, 5, false);
			detailed = SceneryPiece(0, -100, 5000, 900, level1_detailedhills, 0, 2, false);
			skyFeature = SceneryPiece(-2000, 0, 7000, 1250, level1_clouds, -1, 4, false);

			//Extra features (waterfall)
			extraScenery.push_back(SceneryPiece(220, 0, 210, 348, waterfall1, 0, 0, true));

			//Some platforms
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
			layout += ".##...........................#.......................................S............................."; //5
			layout += "........................#....................######................######...........................";
			layout += ".......#...........E....#............#..............................................................";
			layout += ".......#.S.S.S.S.S......#...........................................................................";
			layout += "P......#................#....###HHHHHHH#############.HHHHHHHHHHH..################..................";

			//Initialise map textures
			ground = SceneryPiece(0, -200, 5000, 1080, grass_floor, 0, 2, false);
			sky = SceneryPiece(-5000, 0, 10000, 1250, level1_sky, 1.3, 5, false);
			vague = SceneryPiece(-2000, 0, 10000, 120, level1_farhills, 1.2, 5, false);
			medium = SceneryPiece(-2000, -50, 10000, 1000, level1_closehills, 1.1, 5, false);
			detailed = SceneryPiece(0, -100, 5000, 900, level1_detailedhills, 0, 2, false);
			skyFeature = SceneryPiece(-2000, 0, 7000, 1250, level1_clouds, -1, 4, false);
			
			//Some platforms
			platform1 = new MovingPlatform(true, 500, 200, 400, 800, true);
			platform2 = new MovingPlatform(true, 3000, 190, 2550, 3350, true);
			platforms.push_back(platform1);
			platforms.push_back(platform2);
			
			break;
		default:
			//MAIN MENU
			//Done as level as program requires a player entity (ideally restructured)
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

			//Set title page texture to ground object and fill screen
			ground = SceneryPiece(0, -140, 1440, 810, homePage, 0, 0, false); 
			break;
		}

		//Delete any possible scroobs left from other level
		for(int i = 0; i<scroobs.size(); i++)
		{
			scroobs.erase(scroobs.begin() + i);
		}
		
		player = new Player(25); //New player
		
		//Check tiles for entity symbols, create and add to collections
		for(int i=0; i<layout.length(); i++) 
		{
			if(layout[i] == 'S') //Friendly Scroob NPC
			{
				NPC* newScroob = new NPC(true,40, i % nTilesWide*tileLength + 25, (nTilesHigh - 1 - i / nTilesWide)*tileLength);
				scroobs.push_back(newScroob);
				numberOfScroobs++;
			}
			else if (layout[i] == 'E') //Enemy Scroob NPC
			{ 
				NPC* newScroob = new NPC(false, 50, i % nTilesWide*tileLength + 25, (nTilesHigh - 1 - i / nTilesWide)*tileLength);
				scroobs.push_back(newScroob);
			}
			else if(layout[i] == 'P') //Player Spawn
			{
				player->x = i % nTilesWide*tileLength + 25;
				player->y = (nTilesHigh - 1 - i / nTilesWide)*tileLength;

			}
		}
		scroobs.push_back(player); //Add player to scroob collection for collisions

		restarting = false;
		paused = false;
	}

	//Level destructor
	~Level()
	{
		//Delete platforms
		if (!platforms.empty()) {
			for (MovingPlatform* platform : platforms)
			{
				delete platform;
			}
			platforms.clear();
		}
		//Delete non-player scroobs
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

			//update max score?
			if(points > maxScore)
			{
				maxScore = points;
			}
		}
	}

	//Draw the tile map
	void drawTiles()
	{
		//From bottom to top
		for (int y = nTilesHigh - 1; y >= 0; y--) { 
			//From left to right
			for (int x = 0; x < nTilesWide; x++) { 
				char tile = layout[y*nTilesWide + x]; //Get character from string at x,y
				GLuint texture = 0; //Texture for tile
				bool drawTile = true; //Am i drawing a tile at this location?
				//Set texture dependant on symbol
				switch (tile) { 
				case '#':
					texture = grass_block;
					break;
				case 'H':
					texture = safe;
					break;
				default:
					drawTile = false;
					break;
				}

				//If symbol was a tile symbol, draw a tile
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
					

					//If in debug mode, draw bounding box
					if (debug)
					{
						glColor3f(1, 1, 1);
						glLineWidth(3);
						glBegin(GL_LINE_LOOP);
							glVertex2f(0, 0);
							glVertex2f(0, tileLength);
							glVertex2f(tileLength, tileLength);
							glVertex2f(tileLength, 0);
						glEnd();						
					}
					glPopMatrix();
				}

			}

		}
	}

	/*
		Draw the level
	*/
	void draw()
	{
		
		glLoadIdentity(); //Load identity matrix
		glPushMatrix();
			//If main menu
			if(levelCode == 0)
			{
				ground.draw(); //Draw main title graphic

				//Draw rules at top
				std::string rules1 = "Knock the friendly Scroobs into the safe zone, the bigger the better!";
				std::string rules2 = "Blue Scroobs are dangerous, beware!";
				std::string rules3 = "Scroobs shrink over time so get them home quick!";
				std::string rules4 = "The Game ends when you save them all or you/they die!";
				writeText(400, 650, 1.0, 0.4, 0.0, rules1);
				writeText(500, 630, 1.0, 0.4, 0.0, rules2);
				writeText(470, 610, 1.0, 0.4, 0.0, rules3);
				writeText(450, 590, 1.0, 0.4, 0.0, rules4);

				//Draw scores in middle
				std::string lastScore = "Score: ";
				lastScore.append(std::to_string(points));
				writeText(720, 360, 0.0, 0.4, 0.0, lastScore);
				std::string highScore = "High Score: ";
				highScore.append(std::to_string(maxScore));
				writeText(720, 330, 0.0, 0.8, 0.0, highScore);


			
				//Draw navigation info on right
				std::string info1 = "ESC - OVERLAY MENU ";
				writeText(1000, 360, 1.0, 0.0, 0.0, info1);
				std::string info2 = "Press ESC then choose level...";
				writeText(1000, 330, 0.0, 0.3, 0.8, info2);
				std::string info3 = "1 - Level 1";
				writeText(1000, 300, 0.0, 0.0, 0.8, info3);
				std::string info4 = "2 - Level 2";
				writeText(1000, 270, 0.0, 0.0, 0.8, info4);
				std::string info5 = "0 - Main Menu";
				writeText(1000, 240, 0.0, 0.0, 0.8, info5);
				std::string info6 = "r - Restart";
				writeText(1000, 210, 0.0, 0.0, 0.8, info6);

				//Draw in game controls on left
				std::string info13 = "In-Game Options:";
				writeText(300, 360, 0.0, 0.0, 0.8, info13);
				std::string info7 = "W - Jump";
				writeText(300, 330, 0.0, 0.0, 0.8, info7);
				std::string info8 = "A- Left";
				writeText(300, 300, 0.0, 0.0, 0.8, info8);
				std::string info9 = "S - FastFall";
				writeText(300, 270, 0.0, 0.0, 0.8, info9);
				std::string info10 = "D - Right";
				writeText(300, 240, 0.0, 0.0, 0.8, info10);
				std::string info11 = "/ - Debug Mode";
				writeText(300, 200, 0.0, 0.0, 0.8, info11);
				std::string info12 = ". - Free Camera (Debug Only)";
				writeText(300, 170, 0.0, 0.0, 0.8, info12);

				//Draw scaling 'click to start' graphic
				glPushMatrix();
					glLineWidth(5);
					glTranslatef(camera->width/2.2-textscale*360, camera->y + camera->height/5 -textscale*50, 0);
					glScalef(textscale, textscale, 1);
					glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"Click To Start");
				glPopMatrix();
				return; //Nothing more to draw, return
			} 

			/*
				Not Menu screen, draw full level
			*/

			/*
			Draw Background-------------------------------------------------------------
			*/
			sky.draw();
			vague.draw();
			medium.draw();
			detailed.draw();

			/*
			Draw midground---------------------------------------------------------------
			*/
			//Draw waterfalls
			for (auto& extra : extraScenery)
			{
				extra.draw();
			}
		

			/*
			Draw foreground -----------------------------------------------------------
			*/

			//If not in debug, draw entities here
			if (!debug) {
				for (auto& scroob : scroobs)
				{
					scroob->draw();
				}
			}

			drawTiles(); //Draw tilemap

			//Draw platforms
			for (auto& platform : platforms)
			{
				platform->draw();
			}

			ground.draw();
			skyFeature.draw();

			//If in debug mode draw map boundries
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

				//In debug, draw scroobs in front of all game objects
				for (auto& scroob : scroobs)
				{
					scroob->draw();
				}
			}

			//Draw score and scroobs remaining stat
			std::string pointsAsString = "SCORE: ";
			pointsAsString.append(std::to_string(points));
			writeText(camera->x + camera->width / 10, camera->y + camera->height - camera->height / 10, 0.0, 0.0, 0.0, pointsAsString);
			std::string scroobsLeft = "Remaining: ";
			scroobsLeft.append(std::to_string(numberOfScroobs));
			writeText(camera->x + camera->width / 10, camera->y + camera->height - camera->height / 10 - 50, 0.0, 0.0, 0.0, scroobsLeft);
			
			//Pause mode, draw controles and navigation options
			if(paused)
			{
				std::string info1 = "PAUSED";
				writeText(camera->x + camera->width /2 - 20, camera->y + camera->height/1.5, 1.0, 0.8, 0.3, info1);
			
				std::string info3 = "1 - Level 1";
				writeText(camera->x + camera->width / 2 - 20, camera->y + camera->height / 1.5 - 80, 1.0, 0.0, 0.8, info3);
				std::string info4 = "2 - Level 2";
				writeText(camera->x + camera->width / 2 - 20, camera->y + camera->height / 1.5 - 100, 1.0, 0.0, 0.8, info4);
				std::string info5 = "0 - Main Menu";
				writeText(camera->x + camera->width / 2 - 20, camera->y + camera->height / 1.5 - 120, 0.0, 0.0, 0.8, info5);
				std::string info6 = "r - Restart";
				writeText(camera->x + camera->width / 2 - 20, camera->y + camera->height / 1.5 - 140, 0.0, 0.0, 0.8, info6);
		
			
				std::string info7 = "W - Jump";
				writeText(camera->x + camera->width / 1.3, camera->y + camera->height - 50, 0.0, 0.0, 0.8, info7);
				std::string info8 = "A- Left";
				writeText(camera->x + camera->width / 1.3, camera->y + camera->height - 70, 0.0, 0.0, 0.8, info8);
				std::string info9 = "S - FastFall";
				writeText(camera->x + camera->width / 1.3, camera->y + camera->height - 90, 0.0, 0.0, 0.8, info9);
				std::string info10 = "D - Right";
				writeText(camera->x + camera->width / 1.3, camera->y + camera->height - 110, 0.0, 0.0, 0.8, info10);

				std::string info11 = "/ - Debug Mode";
				writeText(camera->x + camera->width / 1.3, camera->y + camera->height  - 130, 0.0, 0.0, 0.8, info11);
				std::string info12 = ". - Free Camera (Debug Only)";
				writeText(camera->x + camera->width / 1.3, camera->y + camera->height  - 150, 0.0, 0.0, 0.8, info12);
			}
		glPopMatrix();

	}
};
Level* currentLevel; //Pointer to current level


/*
Main draw function
*/
void display()																	
{
	//Perform delta time calculations
	int new_t = gameTime = glutGet(GLUT_ELAPSED_TIME);
	dt = (new_t - old_t);
	if (debug || dt <= 0) {
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

	gluOrtho2D(0, 950, 0, 540);           // set the coordinate system to camera
	
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									
}

//Load all static files such as images
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
	loadAssets();

	//Initialize at menu
	currentLevel = new Level(0);
	gameTime = glutGet(GLUT_ELAPSED_TIME); //Game time counter
	camera = new Camera(); //Initialize camera

	old_t = glutGet(GLUT_ELAPSED_TIME);
}
/*
	Keyboard functions ##########################################
*/
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
	else if(paused || (!paused && currentLevel->levelCode == 0))
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
//###############################################################
/*
	Mouse functions ############################################
*/
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
void clicked() {
	if (currentLevel->levelCode == 0){
		delete currentLevel;
		currentLevel = new Level(1);
	}
}
//##############################################################


/*
	Update game entities
*/
void update()
{
	/*
		Update Code Start
	*/
	if(!debug)
	{
		cameraLock = true;
	}
	//Process User Input
	if (LeftPressed) clicked(); 
	processKeys();

	//END GAME IF LOSE/WIN AND NOT IN MENU STATE
	if((player->dead || currentLevel->numberOfScroobs == 0 )&& currentLevel->levelCode != 0) 
	{
		restarting = true;
		delete currentLevel;
		currentLevel = new Level(0);
		return;
	}

	//Reset camera in menu
	if (currentLevel->levelCode == 0) 
	{
		if (textShrinking) {
			textscale /= 1.002;
			if (textscale < 0.4) textShrinking = false;
		}
		else {
			textscale *= 1.002;
			if (textscale > 1.1) textShrinking = true;
		}

		camera->x = 0;
		camera->y = 0;
	};

	//Main entitiy updates
	if (!paused && !restarting) {
		if(scroobs.size() < 2)
		{
			gameOver = true;
		}
		for (int i=0; i<scroobs.size(); i++) //REMOVE DEAD/SAFE SCROOBS
		{
			if(scroobs.at(i)->dead)
			{
				if (scroobs.at(i)->isFriendly) {
					currentLevel->numberOfScroobs--;
				}
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
   	glutInitWindowPosition(20,20);
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
