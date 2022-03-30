#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	void deleteDeadActors();
	void addActor(Actor* actor);
	bool checkForOverlap(double x, double y, bool checkingSpawnOverlap); // algorithm
	void validSpawnSpot(double& width, double& height, bool DirtException); // algorithm
	void spawnObjects(int imageID, int limit); // algorithm
	void spawnGoodiesAndFungus();
	bool canSpawnGoodieOrFungus(int rand_value, double& x, double& y); // algorithm
	bool overlapWithFoodOrDirt(double x, double y, bool TestingFood); // algorithm
	bool overlap_condition(double x, double y, Actor* a, int limit); // algorithm
	bool SocratesOverlap(double x, double y);
	void SocratesImpactHP(int hp);
	void SocratesReplenishFlamethrowerCharges();
	void newCoordFromCenter(double& x, double& y, double range);
	bool isNotValidRangeFromObject(double x, double y, double range, double object_x, double object_y, double& distance); // algorithm
	bool ExactRangeFromObject(double x, double y, double range, double object_x, double object_y);
	Actor* findFoodObjectInRange(double x, double y); // algorithm
	Actor* findSocrates(double x, double y, double range); // algoirthm
	void increaseBacterium(int howmany);
	void increasePitsThatSpawnedAllBacterium();

private:
	vector<Actor*> actors;
	Actor* socrates;
	int bacterium_alive;
	int pits_that_spawned_all_bacterium;
};

#endif // STUDENTWORLD_H_
