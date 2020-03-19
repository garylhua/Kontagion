#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

class Actor : public GraphObject 
{
public:
	Actor(int imID, double startX, double startY, Direction startDir, int depth, StudentWorld* currSW);
	virtual ~Actor();
	virtual void doSomething() = 0;
	StudentWorld* getWorld();
	void setDead();
	bool isActive();
	virtual bool damage(int d);
	bool checkOverlapAndDoSth(double x, double y, int d, int w);
	virtual bool canBeEaten();
	virtual bool canBlock();
	virtual bool isEnemy();
private:
	bool m_active;
	StudentWorld* m_StudentWorld;
};

class AliveActor : public Actor
{
public:
	AliveActor(int imID, double startX, double startY, Direction startDir, int depth, StudentWorld* currSW);
	virtual ~AliveActor();
	int currHP();
	void setHP(int hp);
	virtual bool damage(int d);
private:
	virtual void playDamageSound() = 0;
	virtual void playDeathSound() = 0;
	int m_hp;
};

class Socrates : public AliveActor
{
public:
	Socrates(StudentWorld* currSW);
	virtual ~Socrates();
	virtual void doSomething();
	int getNSprays() { return m_sprayCharges; }
	int getNFireCharges() { return m_flameCharges; }
	int getHealth() { return currHP(); }
	void addFlameCharges();
private:
	virtual void playDamageSound();
	virtual void playDeathSound();
	void setNewDirection();
	void posToRange();
	//void moveCounterClockwise();
	//void moveClockwise();
	void move();
	int m_sprayCharges;
	int m_flameCharges;
	int m_posAngle;
};

class Dirt : public Actor
{
public:
	Dirt(double StartX, double StartY, StudentWorld* currSW);
	virtual ~Dirt();
	virtual void doSomething();
private:
	virtual bool damage(int d);
	virtual bool canBlock();
};

class BPit : public Actor 
{
public:
	BPit(double StartX, double StartY, StudentWorld* currSW);
	virtual ~BPit();
	virtual void doSomething();
private:
	virtual bool isEnemy();

	int m_nRSalmonella;
	int m_nASalmonella;
	int m_nEColi;
};

class Food : public Actor
{
public:
	Food(double StartX, double StartY, StudentWorld* currSW);
	virtual ~Food();
	virtual void doSomething();
private:
	virtual bool canBeEaten();
};

//Bacteria

class Bacteria : public AliveActor 
{
public:
	Bacteria(int imID, double startX, double startY, StudentWorld* currSW);
	virtual ~Bacteria();
	int getMovementPlanD();
	void setMovementPlanD(int n);
	void decMovementPlanD();
	void incPizza(); //also checks to see if reproduction time
	bool checkRepro();
	void checkFoodOverlap();
	bool checkCanMove(int a);
	void resetDir();
	bool findSoc(double d);
private:
	virtual bool isEnemy();
	virtual void addNewBacteria(double x, double y) = 0;

	int m_nPizzaAte;
	int m_movementPlanD;
};
class Salmonella : public Bacteria 
{
public:
	Salmonella(double startX, double startY, StudentWorld* currSW);
	virtual ~Salmonella();
	void doRegSal(int d);
private:
	virtual void playDamageSound();
	virtual void playDeathSound();
	bool findFood();
};
class RegularSalmonella : public Salmonella 
{
public:
	RegularSalmonella(double startX, double startY, StudentWorld* currSW);
	virtual ~RegularSalmonella();
	virtual void doSomething();
private:
	virtual void addNewBacteria(double x, double y);
};
class AgroSalmonella : public Salmonella 
{
public:
	AgroSalmonella(double startX, double startY, StudentWorld* currSW);
	virtual ~AgroSalmonella();
	virtual void doSomething();
private:
	virtual void addNewBacteria(double x, double y);
};
class Ecoli : public Bacteria 
{
public:
	Ecoli(double startX, double startY, StudentWorld* currSW);
	virtual ~Ecoli();
	virtual void doSomething();
private:
	virtual void addNewBacteria(double x, double y);
	virtual void playDamageSound();
	virtual void playDeathSound();
};


//Consumables
class Consumable: public Actor 
{
public:
	Consumable(int imID, double startX, double startY, StudentWorld* currSW);
	virtual ~Consumable();
	void setLifetime(int l);
	void updateLifetime();
private:
	virtual bool damage(int d);
private:
	int m_lifetime;
};

class HealthGoodie : public Consumable 
{
public:
	HealthGoodie(double startX, double startY, StudentWorld* currSW, int l);
	virtual ~HealthGoodie();
	virtual void doSomething();
private:
};

class FlameGoodie: public Consumable
{
public:
	FlameGoodie(double startX, double startY, StudentWorld* currSW, int l);
	virtual ~FlameGoodie();
	virtual void doSomething();
private:
};

class ExtraLifeGoodie : public Consumable 
{
public:
	ExtraLifeGoodie(double startX, double startY, StudentWorld* currSW, int l);
	virtual ~ExtraLifeGoodie();
	virtual void doSomething();
private:
};

class Fungus : public Consumable 
{
public:
	Fungus(double startX, double startY, StudentWorld* currSW, int l);
	virtual ~Fungus();
	virtual void doSomething();
private:
};


//Projectiles
class Projectile : public Actor
{
public:
	Projectile(int imID, double startX, double startY, Direction startDir, StudentWorld* currSW);
	virtual ~Projectile();
	void travel();
	void setMaxD(int d);
private:
	virtual int AmtDamage() = 0;
	int m_maxD;
	int m_DistTraveled;
};

class Flame : public Projectile
{
public:
	Flame(double startX, double startY, Direction startDir, StudentWorld* currSW);
	virtual ~Flame();
	virtual void doSomething();
private:
	virtual int AmtDamage();

};

class Spray : public Projectile 
{
public:
	Spray(double startX, double startY, Direction startDir, StudentWorld* currSW);
	virtual ~Spray();
	virtual void doSomething();
private:
	virtual int AmtDamage();
};


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
