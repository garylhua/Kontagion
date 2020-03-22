#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

const double PI = 4 * atan(1);

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{   
    m_nDirt = 0;
    m_nActors = 0;
    m_nFood = 0;
    m_nBPits = 0;
}
StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    //add Socrates
    m_mySocrates = new Socrates(this);


    //add Pits
    m_nBPits = getLevel();
    m_nActors += m_nBPits;
    int nBPitsAdded = 0;
    for (int i = 0; i < m_nBPits; i++)
    {
        double startX, startY;
        genStartCoord(startX, startY);
        if (nBPitsAdded == 0)
        {
            Actor* newBPit = new BPit(startX, startY, this);
            m_actors.push_back(newBPit);
            nBPitsAdded++;
            continue;
        }
        list<Actor*>::iterator myIT = m_actors.begin();
        while (checkTooClose(myIT, startX, startY) == true)
        {
            genStartCoord(startX, startY);
        }
        Actor* newBPit = new BPit(startX, startY, this);
        m_actors.push_back(newBPit);
    }

    //add Food
    m_nFood = min(5 * getLevel(), 25);
    /*m_nFood = 1;*/
    m_nActors += m_nFood;
    for (int i = 0; i < m_nFood; i++)
    {
        double startX, startY;
        genStartCoord(startX, startY);
        list<Actor*>::iterator myIT = m_actors.begin();
        while (checkTooClose(myIT, startX, startY) == true)
        {
            genStartCoord(startX, startY);
        }
        Actor* newFood = new Food(startX, startY, this);
        m_actors.push_back(newFood);
    }

    //add Dirt
    m_nDirt = max(180 - 20 * getLevel(), 20);
    /*m_nDirt = 0;*/
    m_nActors += m_nDirt;
    int total = m_nBPits + m_nFood;
    for (int i = 0; i < m_nDirt; i++)
    {
        double startX, startY;
        genStartCoord(startX, startY);
        list<Actor*>::iterator myIT = m_actors.begin();
        while (checkTooClose(myIT, total, startX, startY) == true)
        {
            genStartCoord(startX, startY);
        }
        Actor* newDirt = new Dirt(startX, startY, this);
        m_actors.push_back(newDirt);
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_mySocrates->doSomething();
    if (m_mySocrates->isActive() == false)
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    list<Actor*>::iterator myIT;
    for (myIT = m_actors.begin(); myIT != m_actors.end(); myIT++)
    {
        (*myIT)->doSomething();
        if (m_mySocrates->isActive() == false)
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    bool end = true;

    for (myIT = m_actors.begin(); myIT != m_actors.end(); myIT++)
    {
        if ((*myIT)->isEnemy() == true)
        {
            end = false;
        }
    }
    if (end == true)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }


    //Spawn Goodies
    int ChanceGoodie = max(510 - getLevel() * 10, 250);
    int realChance = randInt(0, ChanceGoodie - 1);
    if (realChance == 0)
    {
        int angle = randInt(0, 359);
        double x = VIEW_WIDTH / 2.0 + (VIEW_RADIUS * 1.0  * cos(angle * 1.0 / 360 * 2 * PI));
        double y = VIEW_HEIGHT / 2.0 + (VIEW_RADIUS * 1.0 * sin(angle * 1.0 / 360 * 2 * PI));
        int life = max(randInt(0, 300 - 10 * getLevel() - 1), 50);
        int which = randInt(1, 10);
        if (which <= 6)
        {
            Actor* newGoodie = new HealthGoodie(x, y, this, life);
            m_actors.push_back(newGoodie);
            m_numOfActors++;
        }
        if (which > 6 && which <= 9)
        {
            Actor* newGoodie = new FlameGoodie(x, y, this, life);
            m_actors.push_back(newGoodie);
            m_numOfActors++;
        }
        if (which > 9)
        {
            Actor* newGoodie = new ExtraLifeGoodie(x, y, this, life);
            m_actors.push_back(newGoodie);
            m_numOfActors++;
        }
    }
    int ChanceFungus = max(510 - getLevel() * 10, 200);
    int realChanceF = randInt(0, ChanceFungus - 1);
    if (realChanceF == 0)
    {
        int angle = randInt(0, 359);
        double x = VIEW_WIDTH / 2.0 + (VIEW_RADIUS * 1.0 * cos(angle * 1.0 / 360 * 2 * PI));
        double y = VIEW_HEIGHT / 2.0 + (VIEW_RADIUS * 1.0 * sin(angle * 1.0 / 360 * 2 * PI));
        int life = max(randInt(0, 300 - 10 * getLevel() - 1), 50);
        Actor* newFungus = new Fungus(x, y, this, life);
        m_actors.push_back(newFungus);
        m_numOfActors++;
    }

    //delete dead actors
    for (myIT = m_actors.begin(); myIT != m_actors.end();)
    {
        if ((*myIT)->isActive() == false)
        {
            delete (*myIT);
            myIT = m_actors.erase(myIT);
            continue;
        }
        else
        {
            myIT++;
        }
    }

    //update test
    ostringstream oss;
    oss << "Score: ";
    oss.fill('0');
    oss << setw(6) << getScore() << "  ";
    oss << "Level: " << getLevel() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_mySocrates->getHealth() << "  ";
    oss << "Sprays: " << m_mySocrates->getNSprays() << "  ";
    oss << "Flames: " << m_mySocrates->getNFireCharges() << "  ";
    string s = oss.str();
    setGameStatText(s);

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_mySocrates;
    list<Actor*>::iterator myIT;
    for (myIT = m_actors.begin(); myIT != m_actors.end();)
    {
        delete (*myIT);
        myIT = m_actors.erase(myIT);
    }
}
void StudentWorld::genStartCoord(double& x, double& y)
{
    double angle = 1.0 * randInt(0, 359);
    double radius = sqrt(1.0 * randInt(0, 14400));
    x = VIEW_WIDTH / 2.0 + (radius * cos(angle * 1.0 / 360 * 2 * PI));
    y = VIEW_HEIGHT / 2.0 + (radius * sin(angle * 1.0 / 360 * 2 * PI));
}
bool StudentWorld::checkTooClose(list<Actor*>::iterator myIterator, double X, double Y)
{
    if (myIterator == m_actors.end())
    {
        return false;
    }

    bool tooClose = false;
    
    double xSquared = pow((*myIterator)->getX() - X, 2);
    double ySquared = pow((*myIterator)->getY() - Y, 2);
    double cSquared = xSquared + ySquared;

    double euclidDist = sqrt(cSquared);

    if (euclidDist <= SPRITE_WIDTH)
    {
        tooClose = true;
    }

    myIterator++;
    
    return (tooClose || checkTooClose(myIterator, X, Y));

}
bool StudentWorld::checkTooClose(std::list<Actor*>::iterator myIterator, int n, double X, double Y)
{
    if (n == 0)
    {
        return false;
    }

    bool tooClose = false;

    double xSquared = pow((*myIterator)->getX() - X, 2);
    double ySquared = pow((*myIterator)->getY() - Y, 2);
    double cSquared = xSquared + ySquared;

    double euclidDist = sqrt(cSquared);

    if (euclidDist <= SPRITE_WIDTH)
    {
        tooClose = true;
    }

    myIterator++;

    return (tooClose || checkTooClose(myIterator, n-1, X, Y));

}
bool StudentWorld::doSth(double X, double Y, int d, int w)
{
    //diff cases of w lead to different situations and changes the goal of the function call
    if (w == 4) //check blockage MOVEMENT OVERLAP
    {
        list<Actor*>::iterator myIT = m_actors.begin();
        for (; myIT != m_actors.end(); myIT++)
        {
            double euclidDist = calcEuclid(X, Y, (*myIT)->getX(), (*myIT)->getY());
            if (euclidDist <= SPRITE_WIDTH/2.0)
            {
                if ((*myIT)->canBlock())
                {
                    return true;
                }
            }

        }
        return false;
    }
    if (w == 3) //bacteria eat food
    {
        list<Actor*>::iterator myIT = m_actors.begin();
        for (; myIT != m_actors.end(); myIT++)
        {
            double euclidDist = calcEuclid(X, Y, (*myIT)->getX(), (*myIT)->getY());
            if (euclidDist <= SPRITE_WIDTH)
            {
                if ((*myIT)->canBeEaten())
                {
                    (*myIT)->setDead();
                    m_nFood--;
                    return true;
                }
            }

        }
        return false;
    
    }
    if (w == 2) //Increase Fireball
    {
        double euclidDist = calcEuclid(X, Y, m_mySocrates->getX(), m_mySocrates->getY());
        if (euclidDist <= SPRITE_WIDTH)
        {
            m_mySocrates->addFlameCharges();
            return true;
        }
        return false;
    }
    if (w == 0) //Damage Socrates
    {
        //double xSquared = pow(m_mySocrates->getX()-X, 2);
        //double ySquared = pow(m_mySocrates->getY()-Y, 2);
        //double cSquared = xSquared + ySquared;

        //double euclidDist = sqrt(cSquared);

        double euclidDist = calcEuclid(X, Y, m_mySocrates->getX(), m_mySocrates->getY());

        if (euclidDist <= SPRITE_WIDTH)
        {
            m_mySocrates->damage(d);
            return true;
        }
        return false;
    }
    if (w == 1) //for Sprays/Flames on bacteria/Dirt/Consumables
    {
        list<Actor*>::iterator myIT = m_actors.begin();
        for (; myIT != m_actors.end(); myIT++)
        {
            //double xSquared = pow((*myIT)->getX() - X, 2);
            //double ySquared = pow((*myIT)->getY() - Y, 2);
            //double cSquared = xSquared + ySquared;

            //double euclidDist = sqrt(cSquared);

            double euclidDist = calcEuclid(X, Y, (*myIT)->getX(), (*myIT)->getY());

            if (euclidDist <= SPRITE_WIDTH)
            {
                if ((*myIT)->damage(d))
                {
                    return true;
                }
            }
        }
        return false;
    }
    return false; //hi
}
void StudentWorld::addSpray(Actor* newSpray)
{
    m_actors.push_back(newSpray);
    m_nActors++;
}
void StudentWorld::addFlame(Actor* newFlame)
{
    m_actors.push_back(newFlame);
    m_nActors++;
}
double StudentWorld::calcEuclid(double x1, double y1, double x2, double y2)
{
    double xSquared = pow(x1-x2, 2);
    double ySquared = pow(y1-y2, 2);
    double cSquared = xSquared + ySquared;

    return sqrt(cSquared);
}
void StudentWorld::findNearestFood(double X, double Y, double& rx, double& ry)
{
    if (m_nFood == 0)
    {
        return;
    }
    list<Actor*>::iterator myIT = m_actors.begin();
    double smallest = 999;
    int count = 0;
    int indexSmallest = 99999;
    bool first = false;
    for (; myIT != m_actors.end(); myIT++)
    {
        if ((*myIT)->canBeEaten() && (*myIT)->isActive())
        {
            double euclidDist = calcEuclid(X, Y, (*myIT)->getX(), (*myIT)->getY());
            if (first == false)
            {
                smallest = euclidDist;
                indexSmallest = count;
                first = true;
                count++;
                continue;
            }
            if (euclidDist < smallest)
            {
                smallest = euclidDist;
                indexSmallest = count;
                count++;
                continue;
            }
        }
        count++;
    }
    if (smallest <= 128)
    {
        list<Actor*>::iterator myOIT = m_actors.begin();
        for (int i = 0; myOIT != m_actors.end() && i < indexSmallest; myOIT++, i++)
        {
        }
        rx = (*myOIT)->getX();
        ry = (*myOIT)->getY();
    }
}
void StudentWorld::addNewRegSal(double x, double y)
{
    Actor* newRSal = new RegularSalmonella(x, y, this);
    m_actors.push_back(newRSal);
}
void StudentWorld::addNewAgroSal(double x, double y)
{
    Actor* newASal = new AgroSalmonella(x, y, this);
    m_actors.push_back(newASal);
}
void StudentWorld::addEcoli(double x, double y)
{
    Actor* newEcoli = new Ecoli(x, y, this);
    m_actors.push_back(newEcoli);
}
void StudentWorld::addFood(double x, double y)
{
    Actor* newFood = new Food(x, y, this);
    m_actors.push_back(newFood);
    m_nFood++;
}
void StudentWorld::findSocrates(double x, double y, double& rx, double& ry, double d)
{
    if (m_mySocrates->isActive() == false)
    {
        return;
    }
    double euclidDist = calcEuclid(x, y, m_mySocrates->getX(), m_mySocrates->getY());
    if (euclidDist <= d)
    {
        rx = m_mySocrates->getX();
        ry = m_mySocrates->getY();
    }
}
