#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
class Actor;
class Socrates;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool doSth(double X, double Y, int d, int w);
    void addSpray(Actor* newSpray);
    void addFlame(Actor* newFlame);
    double calcEuclid(double x1, double y1, double x2, double y2);
    void findNearestFood(double X, double Y, double& rx, double& ry);
    void addNewRegSal(double x, double y);
    void addNewAgroSal(double x, double y);
    void addEcoli(double x, double y);
    void addFood(double x, double y);
    void findSocrates(double x, double y, double& rx, double& ry, double d);
private:
    //Priv Functions
    bool checkTooClose(std::list<Actor*>::iterator myIterator, double X, double Y);
    bool checkTooClose(std::list<Actor*>::iterator myIterator, int n, double X, double Y);
    void genStartCoord(double& x, double &y);

    //Data Members
    std::list<Actor*> m_actors;
    int m_numOfActors;
    Socrates* m_mySocrates;
    int m_nDirt;
    int m_nActors;
    int m_nFood;
    int m_nBPits;
};

#endif // STUDENTWORLD_H_
