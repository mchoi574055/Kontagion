#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

void StudentWorld::addActor(Actor* actor)
{
	actors.push_back(actor); // Careful here, if called during move()
}

bool StudentWorld::overlap_condition(double x, double y, Actor* a, int limit)
{
	// Find the overlap using the passed in limit, using in other functions
	return (sqrt(((x - (a)->getX()) * (x - (a)->getX()) + (y - (a)->getY()) * (y - (a)->getY()))) <= limit); 
}

bool StudentWorld::SocratesOverlap(double x, double y)
{
	// Socrates not accessible by Actor class, use this to access
	return (overlap_condition(x, y, socrates, SPRITE_WIDTH));
}

void StudentWorld::SocratesImpactHP(int hp)
{
	// Affect Socrates HP, can heal
	socrates->impacted(hp);
}

void StudentWorld::SocratesReplenishFlamethrowerCharges()
{
	// This way can replenish FlameThrower in less steps, no need for Actor virtual class
	Socrates* s = static_cast<Socrates*> (socrates);
	s->replenishFlameThrower();
}

bool StudentWorld::overlapWithFoodOrDirt(double x, double y, bool TestingFood)
{
	bool overlap = false;
	for (vector<Actor*>::iterator p = actors.begin(); p != actors.end(); p++)
	{
		if ((TestingFood) && ((*p)->edible())) // Test for food overlap, in this case SPRITE_WIDTH
		{
			overlap = overlap_condition(x, y, *p, SPRITE_WIDTH);
			if (overlap)
			{
				(*p)->impacted(0);
				break;
			}
		}
		
		else if ((!TestingFood) && ((*p)->blocksObjects())) // Test for dirt overlap, in this case SPRITE_WIDTH/2
		{
			overlap = overlap_condition(x, y, *p, SPRITE_WIDTH / 2);
			if (overlap)
				break;
		}
	}
	return overlap;
}

bool StudentWorld::checkForOverlap(double x, double y, bool checkingSpawnOverlap)
{
	// If spawn dirt first, this function will ensure nothing is overlapping

	for (vector<Actor*>::iterator p = actors.begin(); p != actors.end(); p++)
	{
		if (overlap_condition(x, y, *p, SPRITE_WIDTH))
		{
			if (!checkingSpawnOverlap)
			{
				if ((*p)->damageable())
				{
					(*p)->impacted(2);
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

Actor* StudentWorld::findFoodObjectInRange(double x, double y)
{
	double distance = 100000.0;
	double temp = 10000000.0;
	Actor* closestPizza = nullptr;
	// Returns null if no pizza in range

	for (vector<Actor*>::iterator p = actors.begin(); p != actors.end(); p++)
	{
		if (((*p)->edible()) && (*p)->isAlive())
		{
			if (!isNotValidRangeFromObject((*p)->getX(), (*p)->getY(), 128, x, y, temp)) // Find the food object if within 128 pixels
			{
				if (distance > temp)
				{
					closestPizza = (*p);
					distance = temp;
				}
			}
		}
	}

	return closestPizza; // Returns pointer to a food object if food in range
}

Actor* StudentWorld::findSocrates(double x, double y, double range)
{
	double distance = 10000.0;
	if (!isNotValidRangeFromObject(socrates->getX(), socrates->getY(), range, x, y, distance))
	{
		// Returns socrates pointer if he is indeed in range, nullptr otherwise
		return socrates;
	}
	return nullptr;
}

void StudentWorld::deleteDeadActors()
{
	// Delete all dead actors
	for (vector<Actor*>::iterator p = actors.begin(); p != actors.end();)
	{
		if ((*p)->isAlive() == false)
		{
			delete* p;
			p = actors.erase(p);
		}
		else
			p++;
	}
}

void StudentWorld::newCoordFromCenter(double& x, double& y, double range)
{
	// Generate new coordinates
	x = randInt((VIEW_WIDTH / 2) - range, (VIEW_WIDTH / 2) + range);
	y = randInt((VIEW_HEIGHT / 2) - range, (VIEW_HEIGHT / 2) + range);
}

bool StudentWorld::isNotValidRangeFromObject(double x, double y, double range, double object_x, double object_y, double& distance)
{

	// Changes distance for overlap functions, returns whether the object is in given range
	bool notValid = (sqrt((x - object_x) * (x - object_x) + (y - object_y) * (y - object_y)) > range);
	distance = (sqrt((x - object_x) * (x - object_x) + (y - object_y) * (y - object_y)));
	return notValid;
	
}

bool StudentWorld::ExactRangeFromObject(double x, double y, double range, double object_x, double object_y)
{
	// Previous function didn't have exact range, use this for that function
	return (sqrt((x - object_x) * (x - object_x) + (y - object_y) * (y - object_y)) == range);
}

void StudentWorld::validSpawnSpot(double& x, double& y, bool DirtException)
{
	// Use recursion here to find a valid spot
	// In the end, x and y are valid spawn spots
	newCoordFromCenter(x, y, 120);

	bool overlap = checkForOverlap(x, y, true);

	if (DirtException)
		overlap = false;

	double distance = 0;

	while ((isNotValidRangeFromObject(x, y, 120, VIEW_WIDTH/2, VIEW_HEIGHT/2, distance)) || overlap)
	{
		validSpawnSpot(x, y, DirtException);
		return;
	}
}

void StudentWorld::spawnObjects(int imageID, int limit)
{

	for (int i = 0; i < limit; i++)
	{
		// Dirt blocks can overlap, so fine if they are together
		bool DirtException = false;

		if (imageID == IID_DIRT)
			DirtException = true;

		double x = 0, y = 0;
		validSpawnSpot(x, y, DirtException);

		switch (imageID)
		{
			case IID_DIRT:
				actors.push_back(new Dirt(this, x, y));
				break;

			case IID_FOOD:
				actors.push_back(new Food(this, x, y));
				break;

			case IID_PIT:
				actors.push_back(new Pit(x, y, this));
				break;

			default:
				break;
		}
	}
}

void StudentWorld::increaseBacterium(int howmany)
{
	// If this is over 0, there are still bacterium
	bacterium_alive += howmany;
}

int StudentWorld::init()
{
	// Socrates
	socrates = new Socrates(this);

	// Dirt
	int limit = max(180 - 20 * getLevel(), 20);
	spawnObjects(IID_DIRT, limit);

	// Food
	limit = min(5 * getLevel(), 25);
	spawnObjects(IID_FOOD, limit);

	// Pits
	limit = getLevel();
	spawnObjects(IID_PIT, limit);

	bacterium_alive = 0;
	pits_that_spawned_all_bacterium = 0;

    return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::canSpawnGoodieOrFungus(int rand_value, double& x, double& y)
{
	// Use this function to determine whether and which goodie to spawn

	// Fungi in my implementation are considered goodies, just not the good kind
	int spawn_or_not = (randInt(0, rand_value - 1));

	if (spawn_or_not == 0)
	{
		newCoordFromCenter(x, y, VIEW_RADIUS);
		while (!ExactRangeFromObject(x, y, VIEW_RADIUS, VIEW_WIDTH/2, VIEW_HEIGHT/2))
		{
			newCoordFromCenter(x, y, VIEW_RADIUS);
		}
		return true;
	}
	return false;
}

void StudentWorld::spawnGoodiesAndFungus()
{
	// In conjuction with the above functionto spawn and intialize the goodies for each tick
	double x = 0, y = 0;
	int ChanceFungus = max(510 - getLevel() * 10, 200);
	if (canSpawnGoodieOrFungus(ChanceFungus, x, y))
	{
		actors.push_back(new Fungus(x, y, this));
	}
	
	int ChanceGoodie = max(510 - getLevel() * 10, 250);
	
	if (canSpawnGoodieOrFungus(ChanceGoodie, x, y))
	{
		int which_goodie = randInt(1, 10);

		if (which_goodie == 1)
		{
			actors.push_back(new ExtraLifeGoodie(x, y, this)); // 10% for extra life
		}

		else if (which_goodie <= 4)
		{
			actors.push_back(new FlamethrowerGoodie(x, y, this)); // 30% for flamethrower
		}

		else
		{
			actors.push_back(new HealthGoodie(x, y, this)); // 60% for health
		}
	}
}

void StudentWorld::increasePitsThatSpawnedAllBacterium()
{
	pits_that_spawned_all_bacterium++; // use this check if all pits spawned all bacterium
}

int StudentWorld::move()
{
	// socrates do something
	socrates->doSomething();

	int length = actors.size();

	// This way don't have to worry about the vector resizing based on adding actors during call
	for (int i = 0; i < length; i++)
	{

		if (socrates->isAlive() == false)
		{
			decLives();
			return GWSTATUS_PLAYER_DIED; // calls CleanUp
		}

		if ((bacterium_alive == 0) && (pits_that_spawned_all_bacterium == getLevel()))
		{
			// continue the game
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}

		(actors.at(i))->doSomething();
	}

	deleteDeadActors();


	// SPAWN GOODIE

	spawnGoodiesAndFungus();

	int game_score = getScore();
	int level = getLevel();
	int lives = getLives();
	int health = socrates-> HP();
	int sprays = static_cast<Socrates*>(socrates)->SprayCharges();
	int flames = static_cast<Socrates*>(socrates)->FlamethrowerCharges();


	ostringstream text;

	if (game_score < 0)
	{
		text << "Score: -";
		game_score += (-game_score * 2); // Deal with negative score
		text.fill('0');
		text << setw(5) << game_score;
	}

	else 
	{
		text << "Score: ";
		text.fill('0');
		text << setw(6) << game_score;
	}

	text << "  Level: " << level; // DISPLAY LEVEL

	text << "  Lives: " << lives; // DISPLAY LIVES

	text << "  Health: " << health; //DISPLAY HEALTH

	text << "  Sprays: " << sprays; // DISPLAY SPRAYS

	text << "  Flames: " << flames; // DISPLAY FLAMES

	string StatText = text.str();

	setGameStatText(StatText); // Pass to function

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	delete socrates;
	// In case socrates gets deleted again
	socrates = nullptr;

	for (vector<Actor*>::iterator p = actors.begin(); p != actors.end();)
	{
		delete *p;
		p = actors.erase(p);
	}
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}
