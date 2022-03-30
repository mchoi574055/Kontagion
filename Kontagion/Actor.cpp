#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include "GameConstants.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

const double Pi = 2 * asin(1);
// USE FOR PI
																			

double convertToRadian(int angle)
{
	// RADIAN
	return angle* 1.0 * Pi / 180;
}

double convertToDegree(int radian)
{
	// DEGREE
	return radian * 1.0 * 180 / Pi;
}

																		// ACTOR
Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, Direction startDirection, int depth, int hp, bool can_take_damage, bool blocks, bool edible)
	: GraphObject(imageID, startX, startY, startDirection, depth), this_world(world), Alive(true), hitpoints(hp), can_be_damaged(can_take_damage), blocks(blocks), is_edible(edible)
{

}

int Actor::HP()
{
	// Access HP
	return hitpoints;
}

void Actor::doSomething()
{
	// Leave this empty for Dirt and Food objects doSomething
}

StudentWorld* Actor::getAPointerToMyStudentWorld()
{
	return this_world;
	// Keep Track of world
}

void Actor::setDead()
{
	Alive = false;
}

bool Actor::isAlive()
{
	return Alive;
}

bool Actor::damageable()
{
	return can_be_damaged;
}

bool Actor::edible()
{
	return is_edible;
}

void Actor::playHurtSound()
{
	// No sound played unless overriden
}

void Actor::playDieSound()
{
	// No sound played unless overriden
}

void Actor::impacted(int impact_damage)
{
	// Do this for dirt and flame objects
	if (hitpoints == 0)
	{
		setDead();
	}

	else
	{
		// Negative impact damage is a heal
		hitpoints -= impact_damage;

		// If Socrates overheal
		if (hitpoints >= 100)
		{
			hitpoints = 100;
			return;
		}

		if (hitpoints <= 0)
		{
			setDead();
			playDieSound();
		}

		// Hurt, not dead
		else if (hitpoints > 0)
		{
			playHurtSound();
		}
	}
}

bool Actor::blocksObjects()
{
	// Does this object block others?
	return blocks;
}
																			// SOCRATES

Socrates::Socrates(StudentWorld* world)
	: Actor(world, 0, 0, 128, 0, 0, 100, true, false, false), positional_angle(180), spray_charges(20), flamethrower_charges(5)
{

}

int Socrates::FlamethrowerCharges()
{
	return flamethrower_charges;
}

int Socrates::SprayCharges()
{
	return spray_charges;
}

void Socrates::PlayerMove(double angle_change)
{
	// Scale socrates appropriately accross the dish
	positional_angle += angle_change;

	moveTo(VIEW_WIDTH / 2 + (VIEW_RADIUS) * cos(convertToRadian(positional_angle)),
		VIEW_HEIGHT / 2 + (VIEW_RADIUS) * sin(convertToRadian(positional_angle)));

	// Set the new direction
	setDirection(positional_angle + 180);

}

void Socrates::getProjectileTrajectory(double& X, double& Y, Direction angle)
{
	// Get where the projectile is going
	X = getX();
	Y = getY();

	getPositionInThisDirection(angle, SPRITE_WIDTH, X, Y);
}

void Socrates::replenishFlameThrower()
{
	// Recharge flamethrower charges
	flamethrower_charges += 5;
}

void Socrates::doSomething()
{
	if (!isAlive())
		return;

	int ch;
	if (getAPointerToMyStudentWorld()->getKey(ch))
	{
		// user hit a key during this tick!
		switch (ch)
		{
			default:
			break;
		// Depending on key pressed, counterclockwise or clockwise
		case KEY_PRESS_LEFT:
			PlayerMove(5);
			break;
		case KEY_PRESS_RIGHT:
			PlayerMove(-5);
			break;
		case KEY_PRESS_SPACE:
			// Only attack if spray charges are greater than 0
			if (spray_charges > 0)
			{
				spray_charges--;
				
				double x_coord = 0, y_coord = 0;
				getProjectileTrajectory(x_coord, y_coord, getDirection());
				getAPointerToMyStudentWorld()->addActor(new Spray(x_coord, y_coord, getDirection(), getAPointerToMyStudentWorld()));
				getAPointerToMyStudentWorld()->playSound(SOUND_PLAYER_SPRAY);
			
			}
			break;
		case KEY_PRESS_ENTER:
			/// Pressed flamethrower
			{
			if (flamethrower_charges > 0)
			{
				flamethrower_charges--;
				double angle_increment = 0;
				double x_coord = 0, y_coord = 0;

				while (angle_increment <= 15.0 * 22)
				{
					getProjectileTrajectory(x_coord, y_coord, getDirection() + angle_increment);
					getAPointerToMyStudentWorld()->addActor(new Flame(x_coord, y_coord, getDirection() + angle_increment, getAPointerToMyStudentWorld()));
					getAPointerToMyStudentWorld()->playSound(SOUND_PLAYER_FIRE);
					angle_increment += 22;
					// adjust the flames to be in 22 degree angles, 16 projectiles in total
				}
			}
				break;
			}
		}
	}
	else
		// No key pressed, or invalied charges, so just recharge spray_charges
		if (spray_charges < 20)
			spray_charges++;
}

void Socrates::playHurtSound()
{
	// Socrates sound
	getAPointerToMyStudentWorld()->playSound(SOUND_PLAYER_HURT);
}

void Socrates::playDieSound()
{
	// Socrates die sound
	getAPointerToMyStudentWorld()->playSound(SOUND_PLAYER_DIE);
}

																			// DIRT

Dirt::Dirt(StudentWorld* world, double startX, double startY)
	: Actor(world, IID_DIRT, startX, startY, 0, 1, 0, true, true, false)
{

}

																			// PROJECTILE

Projectile::Projectile(StudentWorld* world, int imageID, double startX, double startY, Direction dir, double max_distance)
	: Actor(world, imageID, startX, startY, dir, 1, 0, false, false, false), distance_traveled(SPRITE_WIDTH * -1.0), max_travel(max_distance)
{

}

void Projectile::doSomething()
{
	if (!isAlive())
		return;

	bool overlap = getAPointerToMyStudentWorld()->checkForOverlap(getX(), getY(), false);
	if (overlap == true)
	{
		// Bullet hit something, so return
		impacted(0);
		return;
	}

	distance_traveled += SPRITE_WIDTH;
	// Traveled some distance

	if (distance_traveled == 0)
		return;


	moveAngle(getDirection(), SPRITE_WIDTH);


	if (distance_traveled >= max_travel)
	{
		impacted(0);
		return;
	}
}

																	// SPRAY

Spray::Spray(double startX, double startY, Direction dir, StudentWorld* world)
	: Projectile(world, IID_SPRAY,startX, startY, dir, 112)
{

}

																	// FLAME
Flame::Flame(double startX, double startY, Direction dir, StudentWorld* world)
	: Projectile(world, IID_FLAME, startX, startY, dir, 32)
{

}

// ABOVE BOTH ARE PROJECTILES

																	// FOOD
Food::Food(StudentWorld* world, double startX, double startY)
	: Actor(world, IID_FOOD, startX, startY, 90, 1, 0, false, false, true)
{

}

																	// GOODIE
Goodie::Goodie(double startX, double startY, StudentWorld* world, int imageID, int points, int soundID)
	: Actor(world, imageID, startX, startY, 0, 1, 0, true, false, false), lifetime(max(randInt(0, 300 - 10 * world->getLevel() - 1), 50)), points(points), sound_to_play(soundID)
{

}

bool Goodie::steppedOnGoodie()
{
	if (getAPointerToMyStudentWorld()->SocratesOverlap(getX(), getY()))
	{
		getAPointerToMyStudentWorld()->increaseScore(points);
		impacted(1);
		// Set the goodie to be gone
		getAPointerToMyStudentWorld()->playSound(sound_to_play);
		return true;
	}
	return false;

}

void Goodie::doSomething()
{
	if (!isAlive())
		return;

	if (steppedOnGoodie())
	{
		SpecificGoodieChecks();
		return;
	}

	if (lifetime_over())
	{
		impacted(1);
	}
}

// Detect if lifetime is over, set dead if so
bool Goodie::lifetime_over()
{
	lifetime--;

	if (lifetime <= 0)
		return true;

	return false;
}

																	// HEALTH GOODIE
HealthGoodie::HealthGoodie(double startX, double startY, StudentWorld* world)
	: Goodie(startX, startY, world, IID_RESTORE_HEALTH_GOODIE, 250, SOUND_GOT_GOODIE)
{

}

void HealthGoodie::SpecificGoodieChecks()
{
	// Damage negatively, so heal to full
	getAPointerToMyStudentWorld()->SocratesImpactHP(-100);
}
																	// FLAMETHROWER GOODIE
FlamethrowerGoodie::FlamethrowerGoodie(double startX, double startY, StudentWorld* world)
	: Goodie(startX, startY, world, IID_FLAME_THROWER_GOODIE, 300, SOUND_GOT_GOODIE)
{

}

void FlamethrowerGoodie::SpecificGoodieChecks()
{
	// Refresh flamethrower charges
	getAPointerToMyStudentWorld()->SocratesReplenishFlamethrowerCharges();
}


																	// EXTRALIFE GOODIE
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld* world)
	: Goodie(startX, startY, world, IID_EXTRA_LIFE_GOODIE, 500, SOUND_GOT_GOODIE)
{

}

void ExtraLifeGoodie::SpecificGoodieChecks()
{
	getAPointerToMyStudentWorld()->incLives();
	// Increase lives with the extralife goodie
}

																	// FUNGUS
Fungus::Fungus(double startX, double startY, StudentWorld* world)
	: Goodie(startX, startY, world, IID_FUNGUS, -50, SOUND_NONE)
{

}

void Fungus::SpecificGoodieChecks()
{
	getAPointerToMyStudentWorld()->SocratesImpactHP(20);
	// The worst goodie, just take 20 damage
}

																	// BACTERIUM
Bacterium::Bacterium(double startX, double startY, StudentWorld* world, int imageID, int hp, int socrates_dmg)
	: Actor(world, imageID, startX, startY, 90, 0, hp, true, false, false), foodEaten(0), socrates_dmg(socrates_dmg)
{

}

// All bacteriums consume food
int Bacterium::foodConsumed()
{
	return foodEaten; // Get food
}

void Bacterium::resetFoodEaten()
{
	foodEaten = 0; // Reset food
}

void Bacterium::increaseFoodEaten()
{
	foodEaten++;
}

bool Bacterium::socratesOverlap()
{
	// Check if the bacterium overlaps with socrates, damage him if so
	if (getAPointerToMyStudentWorld()->SocratesOverlap(getX(), getY()))
	{
		getAPointerToMyStudentWorld()->SocratesImpactHP(socrates_dmg); // Different bacteriums have different damage values
		return true;
	}
	return false;
}

double Bacterium::returnDirection(double x, double y, Actor* a)
{
	// Takes an Actor and a position to return the direction of xy to the Actor
	return convertToDegree(atan2(a->getY() - y, a->getX() - x));
}

bool Bacterium::calculateConsumed()
{
	if (foodConsumed() == 3)
	{
		// Set the new values if 3 food are consumed

		double newX = 0;
		if (getX() < VIEW_WIDTH / 2)
			newX = getX() + SPRITE_WIDTH / 2;

		else if (getX() > VIEW_WIDTH / 2)
			newX = (getX() - SPRITE_WIDTH / 2);

		else if (getX() == VIEW_WIDTH / 2)
			newX = getX();

		double newY = 0;
		if (getY() < VIEW_HEIGHT / 2)
			newY = getY() + SPRITE_WIDTH / 2;

		else if (getY() > VIEW_HEIGHT / 2)
			newY = getY() - SPRITE_WIDTH / 2;

		else if (getY() == VIEW_HEIGHT / 2)
			newY = getY();

		// Tell StudentWorld another bacterium was formed
		getAPointerToMyStudentWorld()->increaseBacterium(1);

		// Spawn another bacterium based on the derived class
		repopulate(newX, newY);
		resetFoodEaten();
		return true;
	}
	return false;
}

void Bacterium::spawnFood()
{
	// This function is called if bacterium died

	// Increase score for killing
	getAPointerToMyStudentWorld()->increaseScore(100);

	int chanceToSpawnFood = randInt(1, 2);

	if (chanceToSpawnFood == 1)
	// 50% chance to spawn a food
	{
		getAPointerToMyStudentWorld()->addActor(new Food(getAPointerToMyStudentWorld(), getX(), getY()));
	}
}


bool Bacterium::detectBlocking(int pixels_to_look_at)
{
	double x = getX();
	double y = getY();

	bool blocked = false;
	double distance = 0;

	getPositionInThisDirection(getDirection(), pixels_to_look_at, x, y);

	// If dirt is in the way or will go out of the Petri dish bounds
	if ((getAPointerToMyStudentWorld()->overlapWithFoodOrDirt(x, y, false)) || ((getAPointerToMyStudentWorld()->isNotValidRangeFromObject(x, y, VIEW_RADIUS, VIEW_WIDTH / 2 * 1.0, VIEW_HEIGHT / 2 * 1.0, distance)) || (getAPointerToMyStudentWorld()->ExactRangeFromObject(x, y, VIEW_RADIUS * 1.0, VIEW_WIDTH / 2 * 1.0, VIEW_HEIGHT / 2 * 1.0))))
	{
		blocked = true;
	}

	// Return blocked value
	return blocked;
}

																	// SALMONELLA
Salmonella::Salmonella(double startX, double startY, StudentWorld* world, int hp, int socrates_dmg)
	: Bacterium(startX, startY, world, IID_SALMONELLA, hp, socrates_dmg), movement_plan(0)
{

}

void Salmonella::playHurtSound()
{
	// Overriden
	getAPointerToMyStudentWorld()->playSound(SOUND_SALMONELLA_HURT);
}

void Salmonella::playDieSound()
{
	// Overriden virtual
	getAPointerToMyStudentWorld()->playSound(SOUND_SALMONELLA_DIE);
	getAPointerToMyStudentWorld()->increaseBacterium(-1);

	spawnFood();
}

void Salmonella::setMovementPlan(int new_movement_plan)
{
	// Adjust movement plan
	movement_plan = new_movement_plan;
}

void Salmonella::newMovementPlan()
{
	// Randomize a new direction, reset movementplan, set direction to this direction
	int newDirection = randInt(0, 359);
	setDirection(newDirection);
	setMovementPlan(10);
}

int Salmonella::movementPlanValue()
{
	return movement_plan;
}



																	// REGULAR SALMONELLA
RegSalmonella::RegSalmonella(double startX, double startY, StudentWorld* world)
	: Salmonella(startX, startY, world, 4, 1)
{

}

void RegSalmonella::repopulate(double x, double y)
{
	// New regular salmonella
	getAPointerToMyStudentWorld()->addActor(new RegSalmonella(x, y, getAPointerToMyStudentWorld()));
}

void RegSalmonella::doSomething()
{
	if (!isAlive())
		return;

	bool skipToStep5 = false;

	if (socratesOverlap())
		skipToStep5 = true;

	// If overlap, that means skip to step 5

	if (skipToStep5 == false)
	{
		skipToStep5 = calculateConsumed();
		// If 3 foods consumed, that means skip to step 5
	}

	// STEP 4
	if (skipToStep5 == false)
	{
		if (getAPointerToMyStudentWorld()->overlapWithFoodOrDirt(getX(), getY(), true)) // See if overlap with a food object
			increaseFoodEaten();
		// Overlapped with a food object
	}

	// STEP 5

	if (movementPlanValue() > 0)
	{
		// Has a movement plan
		setMovementPlan(movementPlanValue() - 1);

		bool blocked = detectBlocking(3);

		// If movementplan is blocked, select a new direction, else move in the current direction
		if (blocked == false)
		{
			moveAngle(getDirection(), 3);
		}

		else
		{
			newMovementPlan();
		}
		return;
	}

	else if (movementPlanValue() <= 0)
	{
		// No plan, so try to find closest food object
		Actor* closestFood = getAPointerToMyStudentWorld()->findFoodObjectInRange(getX(), getY());

		if (closestFood == nullptr)
		{
			// No close food
			newMovementPlan();
			return;
		}
			
		else
		{
			// Close food, try and move in this direction
			setDirection(returnDirection(getX(), getY(), closestFood));

			if (detectBlocking(3))
			{
				newMovementPlan();
			}

			else
				moveAngle(getDirection(), 3);
		}
	}
}

																	// AGGRESSIVE SALMONELLA
AggressiveSalmonella::AggressiveSalmonella(double startX, double startY, StudentWorld* world)
	: Salmonella(startX, startY, world, 10, 2)
{

}

void AggressiveSalmonella::repopulate(double x, double y)
{
	// Spawn AggressiveSalmonella
	getAPointerToMyStudentWorld()->addActor(new AggressiveSalmonella(x, y, getAPointerToMyStudentWorld()));
}

void AggressiveSalmonella::doSomething()
{
	if (!isAlive())
		return;

	Actor* potential_socrates = getAPointerToMyStudentWorld()->findSocrates(getX(), getY(), 72);

	bool onlyperformSteps3to5 = false;

	if (potential_socrates != nullptr)
	{
		// Found socrates in the range, therefore try and reach him
		setDirection(returnDirection(getX(), getY(), potential_socrates));

		bool blocked = detectBlocking(3); // If blocked, then don't do anything, might be stuck

		// If blocked
		if (!blocked)
		{
			moveAngle(getDirection(), 3); // Move towards socrates
		}

		onlyperformSteps3to5 = true; // Now only do steps 3 to 5
	}

	bool skiptoStep6 = false; 

	if (socratesOverlap())
	{
		// Socrates overlapped, don't check if overlapped with food
		if (onlyperformSteps3to5 == false)
		{
			skiptoStep6 = true;
		}
	}

	if (skiptoStep6 == false)
	{
		if (calculateConsumed()) // 3 food consumed
		{

		}
		else if (getAPointerToMyStudentWorld()->overlapWithFoodOrDirt(getX(), getY(), true)) // Check to see if overlap
		{
			increaseFoodEaten();
		}
	}

	if (onlyperformSteps3to5 == true)
		return;

	if (movementPlanValue() > 0)
	{
		// Has a movementPlanValue
		setMovementPlan(movementPlanValue() - 1);
		// Decrement

		bool blocked = detectBlocking(3);
		// See 3 blocks ahead
		if (!blocked)
		{
			moveAngle(getDirection(), 3);
			// If not blocked, move ahead
		}
		else
		{
			// New movementplan if blocked
			newMovementPlan();
		}
	}

	else
	{
		// Try to find food object
		Actor* closestFood = getAPointerToMyStudentWorld()->findFoodObjectInRange(getX(), getY());

		if (closestFood == nullptr)
		{
			// No food close by, new movementplan
			newMovementPlan();
			return;
		}

		else
		{
			// Food closes by, new movementplan
			setDirection(returnDirection(getX(), getY(), closestFood));

			if (detectBlocking(3))
			{
				// Blocked, so just have a new movementplan
				newMovementPlan();
			}
			
			else
			{
				// Not blocked, move towards the food
				moveAngle(getDirection(), 3);
			}
		}
	}
}

													// E COLI
EColi::EColi(double startX, double startY, StudentWorld* world)
	: Bacterium(startX, startY, world, IID_ECOLI, 5, 4)
{

}

void EColi::playHurtSound()
{
	getAPointerToMyStudentWorld()->playSound(SOUND_ECOLI_HURT);
}

void EColi::playDieSound()
{
	// Overriden
	getAPointerToMyStudentWorld()->playSound(SOUND_ECOLI_DIE);
	getAPointerToMyStudentWorld()->increaseBacterium(-1);

	spawnFood();
}

void EColi::repopulate(double x, double y)
{
	// Bacterium adds a new EColi
	getAPointerToMyStudentWorld()->addActor(new EColi(x, y, getAPointerToMyStudentWorld()));
	
}


void EColi::doSomething()
{
	if (!isAlive())
	{
		return;
	}

	bool skipToStep5 = false;

	if (socratesOverlap())
	{
		// If on socrates, then skip to step 5
		skipToStep5 = true;
	}

	if (skipToStep5 == false)
	{
		// Don't check if overlap socrates
		skipToStep5 = calculateConsumed();
	}

	if (skipToStep5 == false)
	{
		// If didn't consume 3 foods
		if (getAPointerToMyStudentWorld()->overlapWithFoodOrDirt(getX(), getY(), true)) // Check if overlapping with food object
		{
			increaseFoodEaten();
		}
	}

	Actor* potential_socrates = getAPointerToMyStudentWorld()->findSocrates(getX(), getY(), 256); // Try to find socrates in range of petri dish

	if (potential_socrates != nullptr)
	{
		// socrates in the range
		setDirection(returnDirection(getX(), getY(), potential_socrates));

		for (int i = 0; i < 10; i++)
		{
			// Ten attempts to move to Socrates in 2 pixel increments with 10 degree adjustments each time
			if (!detectBlocking(2))
			{
				moveAngle(getDirection(), 2);
				break;
			}

			else
			{
				setDirection(getDirection() + 10);
			}
		}
	}
}

																	// PIT
Pit::Pit(double startX, double startY, StudentWorld* world)
	: Actor(world, IID_PIT, startX, startY, 0, 1, 0, false, false, false), reg_salm(5), agg_salm(3), e_col(2)
{

}

int Pit::bacterium_in_stash()
{
	return (reg_salm + agg_salm + e_col); // 0 if no more bacterium to produce
}

void Pit::doSomething()
{
	if (bacterium_in_stash() <= 0)
	{
		impacted(0);
		// No more bacterium, set to dead and return
		getAPointerToMyStudentWorld()->increasePitsThatSpawnedAllBacterium();
		return;
	}

	int spawn_bacterium = randInt(1, 50);
	// 1 in 50 chance to spawn bacterium each tick

	if (spawn_bacterium == 1)
	{

		bool spawnedBacterium = false;

		
		while (spawnedBacterium == false)
		{
			int which_bacterium = randInt(1, 3);

			// Repeat until right bacterium is spawned

			if ((which_bacterium == 1) && (reg_salm > 0))
			{
				getAPointerToMyStudentWorld()->addActor(new RegSalmonella(getX(), getY(), getAPointerToMyStudentWorld()));
				reg_salm--;
				spawnedBacterium = true;
			}

			else if ((which_bacterium == 2) && (agg_salm > 0))
			{
				getAPointerToMyStudentWorld()->addActor(new AggressiveSalmonella(getX(), getY(), getAPointerToMyStudentWorld()));
				agg_salm--;
				spawnedBacterium = true;
			}

			else if ((which_bacterium == 3) && (e_col > 0))
			{
				getAPointerToMyStudentWorld()->addActor(new EColi(getX(), getY(), getAPointerToMyStudentWorld()));
				e_col--;
				spawnedBacterium = true;
			}

		}
		getAPointerToMyStudentWorld()->increaseBacterium(1);
	}
}
