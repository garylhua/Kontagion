#include "Actor.h"
#include "StudentWorld.h"
#include <math.h>
#include <list>
using namespace std;

//Actor Implementations
const double PI = 4 * atan(1);

Actor::Actor(int imID, double startX, double startY, Direction startDir, int depth, StudentWorld* currSW)
	: GraphObject(imID,startX, startY, startDir, depth )
{
	m_active = true;
	m_StudentWorld = currSW;
}
Actor::~Actor()
{
}
StudentWorld* Actor::getWorld()
{
	return m_StudentWorld;
}
void Actor::setDead()
{
	m_active = false;
}
bool Actor::isActive()
{
	return m_active;
}
bool Actor::damage(int d)
{
	return false;
}
bool Actor::checkOverlapAndDoSth(double x, double y, int d, int w)
{
	return getWorld()->doSth(x, y, d, w);
}
bool Actor::canBeEaten()
{
	return false;
}
bool Actor::canBlock()
{
	return false;
}
bool Actor::isEnemy()
{
	return false;
}



//AliveActor Implemetations
AliveActor::AliveActor(int imID, double startX, double startY, Direction startDir, int depth, StudentWorld* currSW)
	: Actor(imID, startX, startY, startDir, depth, currSW)
{
}
AliveActor::~AliveActor()
{
}
int AliveActor::currHP()
{
	return m_hp;
}
bool AliveActor::damage(int d)
{
	if (isActive() == false)
	{
		return false;
	}
	m_hp += d;
	if (d < 0 && m_hp > 0)
	{
		playDamageSound();
	}
	else if (d < 0 && m_hp <= 0)
	{
		setDead();
		playDeathSound();
	}
	if (m_hp > 100)
	{
		m_hp = 100;
	}
	return true;
}
void AliveActor::setHP(int hp)
{
	m_hp = hp;
}



//Socrates Implementations
Socrates::Socrates(StudentWorld* currSW)
: AliveActor(IID_PLAYER, 0, 128, 0, 0, currSW)
{
	m_sprayCharges = 20;
	m_flameCharges = 5;
	m_posAngle = 180;
	setHP(100);
}
Socrates::~Socrates()
{
}
void Socrates::doSomething() //virtual
{
	if (isActive() == false)
	{
		return;
	}
	int keypressed;
	if (getWorld()->getKey(keypressed) == true)
	{
		if (keypressed == KEY_PRESS_SPACE && m_sprayCharges >= 1)
		{
			//double X = (VIEW_RADIUS*1.0 - 1.0* SPRITE_RADIUS) * cos((m_posAngle * 1.0) / 360 * 2 * PI)+ VIEW_WIDTH/2.0;
			//double Y = (VIEW_RADIUS * 1.0 - 1.0 * SPRITE_RADIUS) * sin((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_HEIGHT / 2;
			double X, Y;
			getPositionInThisDirection(getDirection(), SPRITE_WIDTH, X, Y);
			Actor* newSpray = new Spray(X, Y, getDirection(), getWorld());
			getWorld()->addSpray(newSpray);
			m_sprayCharges--;
			getWorld()->playSound(SOUND_PLAYER_SPRAY);
		}
		if (keypressed == KEY_PRESS_ENTER && m_flameCharges >= 1)
		{
			int currDegree = 0;

			for (; currDegree < 331; currDegree += 22)
			{
				//double X = (1.0 * SPRITE_WIDTH) * cos((currDegree * 1.0) / 360 * 2 * PI) + getX();
				//double Y = (1.0 * SPRITE_WIDTH) * sin((currDegree * 1.0) / 360 * 2 * PI) + getY();
				double X, Y;
				getPositionInThisDirection(currDegree, SPRITE_WIDTH, X, Y);
				Actor* newFlame = new Flame(X, Y, currDegree, getWorld());
				getWorld()->addFlame(newFlame);
			}
			m_flameCharges--;
			getWorld()->playSound(SOUND_PLAYER_FIRE);
		}
		if (keypressed == KEY_PRESS_LEFT)
		{
			m_posAngle += 5;
			posToRange();
			move();
			setNewDirection();
		}
		if (keypressed == KEY_PRESS_RIGHT)
		{
			m_posAngle -= 5;
			posToRange();
			move();
			setNewDirection();
		}
	}
	else
	{
		if (m_sprayCharges < 20)
		{
			m_sprayCharges++;
		}
	}
}
void Socrates::playDamageSound()
{
	getWorld()->playSound(SOUND_PLAYER_HURT);
}
void Socrates::playDeathSound()
{
	getWorld()->playSound(SOUND_PLAYER_DIE);
}
void Socrates::setNewDirection()
{
	int ndirection = m_posAngle + 180;
	if (ndirection >= 360)
	{
		ndirection = ndirection - 360;
	}
	setDirection(ndirection);
}
void Socrates::posToRange()
{
	if (m_posAngle < 0)
	{
		m_posAngle = 360 + m_posAngle;
	}

	if (m_posAngle > 359)
	{
		m_posAngle = m_posAngle - 360;
	}
}
//void Socrates::moveClockwise()
//{
//	const double PI = 4 * atan(1);
//	double newX = (VIEW_RADIUS * 1.0)*cos((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_WIDTH / 2.0;
//	double newY = (VIEW_RADIUS * 1.0) * sin((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_HEIGHT / 2.0;
//	double addX = 2.0 * (1.0 * 128) * sin((5.0 * 1.0 / 360 * 2 * PI) / 2) * cos((m_posAngle * 1.0 - 87.5) / 360 * 2 * PI);
//	double addY = 2.0 * (1.0 * 128) * sin((5.0 * 1.0 / 360 * 2 * PI) / 2) * sin((m_posAngle * 1.0 - 87.5) / 360 * 2 * PI);
//	/*moveTo(getX() + addX, getY() + addY);*/
//	moveTo(newX, newY);
//}
//void Socrates::moveCounterClockwise()
//{
//	const double PI = 4 * atan(1);
//	double newX = (VIEW_RADIUS * 1.0) * cos((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_WIDTH / 2.0;
//	double newY = (VIEW_RADIUS * 1.0) * sin((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_HEIGHT / 2.0;
//	double addX = 2.0 * (1.0 * 128) * sin((5.0 * 1.0 / 360 * 2 * PI) / 2) * cos((m_posAngle * 1.0 + 87.5) / 360 * 2 * PI);
//	double addY = 2.0 * (1.0 * 128) * sin((5.0 * 1.0 / 360 * 2 * PI) / 2) * sin((m_posAngle * 1.0 + 87.5) / 360 * 2 * PI);
//	/*moveTo(getX() + addX, getY() + addY);*/
//	moveTo(newX, newY);
//}
void Socrates::move()
{
	double newX = (VIEW_RADIUS * 1.0) * cos((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_WIDTH / 2.0;
	double newY = (VIEW_RADIUS * 1.0) * sin((m_posAngle * 1.0) / 360 * 2 * PI) + VIEW_HEIGHT / 2.0;
	//double addX = 2.0 * (1.0 * 128) * sin((5.0 * 1.0 / 360 * 2 * PI) / 2) * cos((m_posAngle * 1.0 + 87.5) / 360 * 2 * PI);
	//double addY = 2.0 * (1.0 * 128) * sin((5.0 * 1.0 / 360 * 2 * PI) / 2) * sin((m_posAngle * 1.0 + 87.5) / 360 * 2 * PI);
	/*moveTo(getX() + addX, getY() + addY);*/
	moveTo(newX, newY);
}
void Socrates::addFlameCharges()
{
	m_flameCharges += 5;
}



//Dirt Implementations
Dirt::Dirt(double startX, double startY, StudentWorld* currSW)
	: Actor(IID_DIRT, startX, startY, 0, 1, currSW)
{
}
Dirt::~Dirt()
{
}
void Dirt::doSomething()
{
	return;
}
bool Dirt::damage(int d)
{
	if (isActive() == false)
	{
		return false;
	}
	setDead();
	return true;
}
bool Dirt::canBlock()
{
	return true;
}

//BPit Implementations
BPit::BPit(double StartX, double StartY, StudentWorld* currSW)
	: Actor(IID_PIT, StartX, StartY, 0, 1, currSW)
{
	m_nRSalmonella = 5;
	m_nASalmonella = 3;
	m_nEColi = 2;
}
BPit::~BPit()
{
}
void BPit::doSomething()
{
	if (m_nRSalmonella == 0 && m_nASalmonella == 0 && m_nEColi == 0)
	{
		setDead();
		return;
	}
	int s = randInt(1, 50);
	if (s == 1)
	{
		if (m_nRSalmonella != 0 && m_nASalmonella != 0 && m_nEColi != 0)
		{
			int n = randInt(1, 3);
			if (n == 1)
			{
				getWorld()->addNewRegSal(getX(), getY());
				m_nRSalmonella--;
			}
			if (n == 2)
			{
				getWorld()->addNewAgroSal(getX(), getY());
				m_nASalmonella--;
			}
			if (n == 3)
			{
				getWorld()->addEcoli(getX(), getY());
				m_nEColi--;
			}
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
		if (m_nRSalmonella != 0 && m_nASalmonella != 0)
		{
			int n = randInt(1, 2);
			if (n == 1)
			{
				getWorld()->addNewRegSal(getX(), getY());
				m_nRSalmonella--;
			}
			if (n == 2)
			{
				getWorld()->addNewAgroSal(getX(), getY());
				m_nASalmonella--;
			}
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
		if (m_nASalmonella != 0 && m_nEColi != 0)
		{
			int n = randInt(1, 2);
			if (n == 1)
			{
				getWorld()->addNewAgroSal(getX(), getY());
				m_nASalmonella--;
			}
			if (n == 2)
			{
				getWorld()->addEcoli(getX(), getY());
				m_nEColi--;
			}
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
		if (m_nRSalmonella != 0 && m_nEColi != 0)
		{
			int n = randInt(1, 2);
			if (n == 1)
			{
				getWorld()->addNewRegSal(getX(), getY());
				m_nRSalmonella--;
			}
			if (n == 2)
			{
				getWorld()->addEcoli(getX(), getY());
				m_nEColi--;
			}
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
		if (m_nRSalmonella != 0)
		{
			getWorld()->addNewRegSal(getX(), getY());
			m_nRSalmonella--;
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
		if (m_nASalmonella != 0)
		{
			getWorld()->addNewAgroSal(getX(), getY());
			m_nASalmonella--;
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
		if (m_nEColi != 0)
		{
			getWorld()->addEcoli(getX(), getY());
			m_nEColi--;
			getWorld()->playSound(SOUND_BACTERIUM_BORN);
			return;
		}
	}
}
bool BPit::isEnemy()
{
	if (isActive() == false)
	{
		return false;
	}
	return true;
}



//Food Implementations
Food::Food(double StartX, double StartY, StudentWorld* currSW)
	: Actor(IID_FOOD, StartX, StartY, 90, 1, currSW)
{
}
Food::~Food()
{

}
void Food::doSomething()
{
	return;
}
bool Food::canBeEaten()
{
	return true;
}



//Projectile Implementations
Projectile::Projectile(int imID, double startX, double startY, Direction startDir, StudentWorld* currSW)
	: Actor(imID, startX, startY, startDir, 1, currSW)
{
	m_DistTraveled = 0;
}
Projectile::~Projectile()
{
}
void Projectile::travel()
{
	m_DistTraveled += SPRITE_WIDTH;
	moveAngle(getDirection(), SPRITE_WIDTH); //check this
	if (m_DistTraveled >= m_maxD)
	{
		setDead();
	}

}
void Projectile::setMaxD(int d)
{
	m_maxD = d;
}

//Spray Implemetations
Spray::Spray(double startX, double startY, Direction startDir, StudentWorld* currSW)
	: Projectile(IID_SPRAY, startX, startY, startDir, currSW)
{
	setMaxD(120);
}
Spray::~Spray()
{
}

int Spray::AmtDamage()
{
	return -2;
}
void Spray::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(), getY(), AmtDamage(), 1) == true)
	{
		setDead();
		return;
	}
	travel();

}



//Flame Implementation
Flame::Flame(double startX, double startY, Direction startDir, StudentWorld* currSW)
	: Projectile (IID_FLAME, startX, startY, startDir, currSW)
{
	setMaxD(40);
}
Flame::~Flame()
{
	return;
}
int Flame::AmtDamage()
{
	return -5;
}
void Flame::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(), getY(), AmtDamage(), 1) == true)
	{
		setDead();
		return;
	}
	travel();
}



//Consumable Implementation
Consumable::Consumable(int imID, double startX, double startY, StudentWorld* currSW)
	: Actor(imID, startX, startY, 0, 1, currSW)
{
}
Consumable::~Consumable()
{
}
void Consumable::setLifetime(int l)
{
	m_lifetime = l;
}
void Consumable::updateLifetime()
{
	m_lifetime--;
	if (m_lifetime == 0)
	{
		setDead();
	}
}
bool Consumable::damage(int d)
{
	if (isActive() == false)
	{
		return false;
	}
	setDead();
	return true;
}



//HealthGoodie Implemenations
HealthGoodie::HealthGoodie(double startX, double startY, StudentWorld* currSW, int l)
	: Consumable(IID_RESTORE_HEALTH_GOODIE, startX, startY, currSW)
{
	setLifetime(l);
}
HealthGoodie::~HealthGoodie()
{
}
void HealthGoodie::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(), getY(), 100, 0) == true)
	{
		getWorld()->increaseScore(250);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return;
	}
	updateLifetime();
}



//Flame Goodie Implementations
FlameGoodie::FlameGoodie(double startX, double startY, StudentWorld* currSW, int l)
	: Consumable(IID_FLAME_THROWER_GOODIE, startX, startY, currSW)
{
	setLifetime(l);
}
FlameGoodie::~FlameGoodie()
{
}
void FlameGoodie::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(), getY(), 100, 2) == true)
	{
		getWorld()->increaseScore(300);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		return;
	}
	updateLifetime();
}



//Extra Life Goodie Implementations
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld* currSW, int l)
	: Consumable(IID_EXTRA_LIFE_GOODIE, startX, startY, currSW)
{
	setLifetime(l);
}
ExtraLifeGoodie::~ExtraLifeGoodie()
{
}
void ExtraLifeGoodie::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(), getY(), 0, 0) == true)
	{
		getWorld()->increaseScore(500);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->incLives();
		return;
	}
	updateLifetime();
}



//Fungus Implementations
Fungus::Fungus(double startX, double startY, StudentWorld* currSW, int l)
	: Consumable(IID_FUNGUS, startX, startY, currSW)
{
	setLifetime(l);
}
Fungus::~Fungus()
{
}
void Fungus::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(),getY(),-20, 0) == true)
	{
		getWorld()->increaseScore(-50);
		setDead();
		return;
	}
	updateLifetime();
}



//Bacteria Implementation
Bacteria::Bacteria(int imID, double startX, double startY, StudentWorld* currSW)
	: AliveActor(imID, startX, startY, 90, 0, currSW)
{
	m_nPizzaAte = 0;
	m_movementPlanD = 0;
}
Bacteria::~Bacteria()
{
	getWorld()->increaseScore(100);
	int r = randInt(0, 1);
	if (r == 0)
	{
		getWorld()->addFood(getX(), getY());
	}
}
int Bacteria::getMovementPlanD()
{
	return m_movementPlanD;
}
void Bacteria::setMovementPlanD(int n)
{
	m_movementPlanD = n;
}
void Bacteria::decMovementPlanD()
{
	m_movementPlanD--;
}
void Bacteria::incPizza()
{
	m_nPizzaAte++;
}
bool Bacteria::checkRepro()
{
	if (m_nPizzaAte == 3)
	{
		double newX = 0;
		if (getX() < (VIEW_WIDTH / 2.0))
		{
			newX = getX() + SPRITE_WIDTH / 2.0;
		}
		else if (getX() > (VIEW_WIDTH / 2.0))
		{
			newX = getX() - SPRITE_WIDTH / 2.0;
		}
		else if (getX() == (VIEW_WIDTH / 2.0))
		{
			newX = getX();
		}

		double newY = 0;
		if (getY() < (VIEW_HEIGHT / 2.0))
		{
			newY = getY() + SPRITE_WIDTH / 2.0;
		}
		else if (getY() > (VIEW_HEIGHT / 2.0))
		{
			newY = getY() - SPRITE_WIDTH / 2.0;
		}
		else if (getY() == (VIEW_HEIGHT / 2.0))
		{
			newY = getY();
		}
		m_nPizzaAte = 0;
		addNewBacteria(newX, newY);
		return true;
	}
	return false;
}
void Bacteria::checkFoodOverlap()
{
	if (checkOverlapAndDoSth(getX(), getY(), 200, 3) == true)
	{
		incPizza();
	}
}
bool Bacteria::checkCanMove(int a)
{
	double pX, pY;
	getPositionInThisDirection(getDirection(), a, pX, pY);
	if (checkOverlapAndDoSth(pX, pY, 200, 4) == true)
	{
		return false;
	}
	if ((getWorld()->calcEuclid(pX, pY, VIEW_WIDTH / 2, VIEW_HEIGHT / 2)) >= VIEW_RADIUS)
	{
		return false;
	}
	return true;
}
void Bacteria::resetDir()
{
	int newDir = randInt(0, 359);
	setDirection(newDir);
	setMovementPlanD(10);
}
bool Bacteria::findSoc(double d)
{
	double nx = 999;
	double ny = 999;
	getWorld()->findSocrates(getX(), getY(), nx, ny, d);
	if ((nx != 999) && (ny != 999))
	{
		double a = nx - getX();
		double b = ny - getY();
		if (a == 0 && b > 0)
		{
			setDirection(90);
		}
		if (a == 0 && b < 0)
		{
			setDirection(-90);
		}
		if (a == 0 && b == 0)
		{
			return false;
		}
		double result = (atan(b / a) * 180.0) / PI;
		if (a >= 0)
		{
			int l = 1;
			setDirection(result);
		}
		if (a < 0 && b >= 0)
		{
			int l = 1;
			setDirection(result + 180);
		}
		if (a < 0 && b < 0)
		{
			setDirection(result - 180);
		}
		return true;
	}
	return false;
}
bool Bacteria::isEnemy()
{
	if (isActive() == false)
	{
		return false;
	}
	return true;
}


//Salmonella Implementations
Salmonella::Salmonella(double startX, double startY, StudentWorld* currSW)
	: Bacteria(IID_SALMONELLA, startX, startY, currSW)
{
}
Salmonella::~Salmonella()
{
}
bool Salmonella::findFood()
{
	double nx = 999;
	double ny = 999;
	getWorld()->findNearestFood(getX(), getY(), nx, ny);
	if ((nx != 999) && (ny != 999))
	{
		double a = nx - getX();
		double b = ny - getY();
		if (a == 0 && b > 0)
		{
			setDirection(90);
		}
		if (a == 0 && b < 0)
		{
			setDirection(-90);
		}
		if (a == 0 && b == 0)
		{
			return false;
		}
		double result = (atan(b / a) * 180.0) / PI;
		if (a >= 0)
		{
			int l = 1;
			setDirection(result);
		}
		if (a < 0 && b >= 0)
		{
			int l = 1;
			setDirection(result + 180);
		}
		if (a < 0 && b < 0)
		{
			setDirection(result - 180);
		}
		return true;
	}
	return false;
}
void Salmonella::playDamageSound()
{
	getWorld()->playSound(SOUND_SALMONELLA_HURT);
}
void Salmonella::playDeathSound()
{
	getWorld()->playSound(SOUND_SALMONELLA_DIE);
}
void Salmonella::doRegSal(int d)
{
	if (checkOverlapAndDoSth(getX(), getY(), d, 0) == false)
	{
		if (checkRepro() == true)
		{
			//it reproduced
		}
		else
		{
			checkFoodOverlap();
		}
	}
	if (getMovementPlanD() > 0)
	{
		decMovementPlanD();
		if (checkCanMove(3) == true)
		{
			moveAngle(getDirection(), 3);
		}
		else
		{
			resetDir();
		}
		return;
	}
	if (findFood() == true)
	{
		if (checkCanMove(3) == true)
		{
			moveAngle(getDirection(), 3);
			return;
		}
		resetDir();
		return;
	}
	resetDir();
	return;
}



//RegularSalmonella Implementations
RegularSalmonella::RegularSalmonella(double startX, double startY, StudentWorld* currSW)
	: Salmonella(startX, startY, currSW)
{
	setHP(4);
}
RegularSalmonella::~RegularSalmonella()
{
}
void RegularSalmonella::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	doRegSal(-1);
}
void RegularSalmonella::addNewBacteria(double x, double y)
{
	getWorld()->addNewRegSal(x, y);
}



//Agro Salmonella Implementations
AgroSalmonella::AgroSalmonella(double startX, double startY, StudentWorld* currSW)
	: Salmonella(startX,startY,currSW)
{
	setHP(10);
}
AgroSalmonella::~AgroSalmonella()
{
}
void AgroSalmonella::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (findSoc(72.0) == true)
	{
		if (checkCanMove(3) == true)
		{
			moveAngle(getDirection(), 3);
		}
		if (checkOverlapAndDoSth(getX(), getY(), -2, 0) == false)
		{
			if (checkRepro() == true)
			{
				//it reproduced
			}
			else
			{
				checkFoodOverlap();
			}
		}
		return;
	}
	doRegSal(-2);
}
void AgroSalmonella::addNewBacteria(double x, double y)
{
	getWorld()->addNewAgroSal(x, y);
}

Ecoli::Ecoli(double startX, double startY, StudentWorld* currSW)
	: Bacteria(IID_ECOLI, startX, startY, currSW)
{
	setHP(5);
}
Ecoli::~Ecoli()
{
}
void Ecoli::doSomething()
{
	if (isActive() == false)
	{
		return;
	}
	if (checkOverlapAndDoSth(getX(), getY(), -4, 0) == false)
	{
		if (checkRepro() == true)
		{
			//it reproduced
		}
		else
		{
			checkFoodOverlap();
		}
	}
	if (findSoc(256.0) == true)
	{
		int count = 0;
		while (count < 10)
		{
			if (checkCanMove(2) == true)
			{
				moveAngle(getDirection(), 2);
				return;
			}
			int newD = getDirection() + 10;
			if (newD >= 360)
			{
				newD = newD - 360;
			}
			setDirection(newD);
			count++;
		}
	}
}
void Ecoli::addNewBacteria(double x, double y)
{
	getWorld()->addEcoli(x, y);
}
void Ecoli::playDamageSound()
{
	getWorld()->playSound(SOUND_ECOLI_HURT);
}
void Ecoli::playDeathSound()
{
	getWorld()->playSound(SOUND_ECOLI_DIE);
}
//create a list of classes and work from there, maybe finish init();
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
