//Jason Song
#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <list>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

    virtual int init(); //NEVER call

    virtual int move(); //NEVER call

    virtual void cleanUp(); //NEVER call
     
    ~StudentWorld();
    
    void earthInvisible(int col, int row);
    
    bool getEarthVisibility(int col, int row);
    
    bool tmCheckFourbyFour(int x, int y);
    
    double computeDistanceToTM(Actor* actor);
    
    //Decrement level's Barrel count by one when player picks up Barrel
    void subtractBarrel();
    
    void addToList(Actor* actor);
    
    //Returns true if there is no Boulder within radius of 3.0 from (x,y)
    bool tunnelManBoulderChecker(int x, int y);
    
    //Makes visible all hidden game objects that are within radius of 12 from (x,y)
    void tunnelManSonarChecker(int x, int y);
    
    bool isBoulder(int x, int y);
    
    bool isAnnoyable(int x, int y);
    
    /*If a Squirt is within a radius of 3.0 of one or more Protesters (up to and including a distance of 3.0 squares away), it will cause 2 points of annoyance to these Protester(s), and then immediately set its state to dead, so it can be removed from the oil field at the end of the tick.*/
    void squirtAssistant(Squirt* s);
    
    //Returns pointer to a Protestor (if any) within radius of 3 from Gold Nugget at (x,y)
    Protestor* goldNuggetProtesterFinder(GoldNugget* g);
    
    //Bonks any Protester within radius of 3 of Boulder
    void boulderBonker(Boulder *b);
    
    void annoyPerson(Actor *a, int val);
    
    void annoyTunnelMan(int val);
    
    //Increment TunnelMan's sonar kit amount by "val"
    void changeSonarAmount(int val);
    
    //Increment TunnelMan's gold amount by "val"
    void changeGoldAmount(int val);
    
    //Increment TunnelMan's water squirt amount by "val"
    void changeSquirtAmount(int val);
    
    int getTunnelManHitPoints();
    
    int getTunnelManGoldAmount();
    
    int getTunnelManSonarAmount();
    
    int getTunnelManSquirtAmount();
    
    int getTunnelManXCoord();
    
    int getTunnelManYCoord();
    
    void killTunnelMan();
    
    void decreaseProtestorCount();
   
private:
    
    TunnelMan* tm;
    Earth* earthArray[64][60];
    std::list<Actor*> actorList;
    //Returns true if there's no Barrel, Gold Nugget or Boulder in a six-square radius around (x,y)
    bool sixSquareRadiusChecker(int x, int y);
    double computeDistance(Actor* a,Actor* b);
    int barrelsLeft;
    void setDisplayText();
    int protestorTicks;
    int protestorTicksToWait;
    int targetProtestors;
    int protestorCount;
    int probabilityOfHardcore;
    int probabilityOfGoodie;
};

#endif // STUDENTWORLD_H_
