#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
	public:
		Actor(StudentWorld* world, int imageID, double startX, double startY, Direction startDirection, int depth, int hp, bool can_take_damage, bool blocks, bool edible);
		virtual void doSomething();
		StudentWorld* getAPointerToMyStudentWorld();
		bool isAlive();
		void setDead();
		bool damageable();
		void impacted(int impact_damage);
		bool edible();
		bool blocksObjects();
		virtual void playHurtSound();
		virtual void playDieSound();
		int HP();

	private:
		StudentWorld* this_world;
		bool Alive;
		int hitpoints;
		bool can_be_damaged;
		bool blocks;
		bool is_edible;

};

class Socrates : public Actor
{
	public:
		Socrates(StudentWorld* world);
		void doSomething();
		void replenishFlameThrower();
		int SprayCharges();
		int FlamethrowerCharges();
		void playHurtSound();
		void playDieSound();

	private:
		void getProjectileTrajectory(double& X, double& Y, Direction angle);
		void PlayerMove(double angle_change);
		double positional_angle;
		int spray_charges;
		int flamethrower_charges;
};

class Projectile : public Actor
{
	public: 
		Projectile(StudentWorld* world, int imageID, double startX, double startY, Direction dir, double max_distance);
		void doSomething();
		
	private:
		double distance_traveled;
		double max_travel;
};

class Spray : public Projectile
{
	public:
		Spray(double startX, double startY, Direction dir, StudentWorld* world);
};

class Flame : public Projectile
{
	public:
		Flame(double startX, double startY, Direction dir, StudentWorld* world);
};

class Dirt : public Actor
{
	public:
		Dirt(StudentWorld* world, double startX, double startY);
};

class Food : public Actor
{
	public:
		Food(StudentWorld* world, double startX, double startY);
};

class Goodie : public Actor
{
	public:
		Goodie(double startX, double startY, StudentWorld* world, int imageID, int points, int soundID);
		void doSomething();
		virtual void SpecificGoodieChecks() = 0;
	private:
		bool steppedOnGoodie();
		bool lifetime_over();
		int lifetime;
		int points;
		int sound_to_play;
};

class HealthGoodie : public Goodie
{
	public:
		HealthGoodie(double startX, double startY, StudentWorld* world);
		void SpecificGoodieChecks();
};

class FlamethrowerGoodie : public Goodie
{
	public:
		FlamethrowerGoodie(double startX, double startY, StudentWorld* world);
		void SpecificGoodieChecks();
};

class ExtraLifeGoodie : public Goodie
{
	public:
		ExtraLifeGoodie(double startX, double startY, StudentWorld* world);
		void SpecificGoodieChecks();
};

class Fungus : public Goodie
{
	public:
		Fungus(double startX, double startY, StudentWorld* world);
		void SpecificGoodieChecks();
};

class Pit : public Actor
{
public:
	Pit(double startX, double startY, StudentWorld* world);
	void doSomething();

private:
	int bacterium_in_stash();
	int reg_salm;
	int agg_salm;
	int e_col;

};

class Bacterium : public Actor
{
	public:
		Bacterium(double startX, double startY, StudentWorld* world, int imageID, int hp, int socrates_dmg);
		int foodConsumed();
		void resetFoodEaten();
		void increaseFoodEaten();
		virtual void doSomething() = 0;
		bool socratesOverlap();
		virtual void playHurtSound() = 0;
		virtual void playDieSound() = 0;
		double returnDirection(double x, double y, Actor* a);
		virtual void repopulate(double x, double y) = 0;
		bool calculateConsumed();
		void spawnFood();
		bool detectBlocking(int pixels_to_look_at);

	private:
		int foodEaten;
		int socrates_dmg;
};

class Salmonella : public Bacterium
{
	public: 
		Salmonella(double startX, double startY, StudentWorld* world, int hp, int socrates_dmg);
		int movementPlanValue();
		void setMovementPlan(int new_movement_plan);
		void playHurtSound();
		void playDieSound();
		void newMovementPlan();
		virtual	void doSomething() = 0;
		virtual void repopulate(double x, double y) = 0;

	private:
		int movement_plan;

};

class RegSalmonella : public Salmonella
{
	public:
		RegSalmonella(double startX, double startY, StudentWorld* world);
		void doSomething();
		void repopulate(double x, double y);
};

class AggressiveSalmonella : public Salmonella
{
	public:
		AggressiveSalmonella(double startX, double startY, StudentWorld* world);
		void doSomething();
		void repopulate(double x, double y);

};

class EColi : public Bacterium
{
	public:
		EColi(double startX, double startY, StudentWorld* world);
		void doSomething();
		void repopulate(double x, double y);
		void playHurtSound();
		void playDieSound();

};


#endif // ACTOR_H_