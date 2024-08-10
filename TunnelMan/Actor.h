//Jason Song
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>
#include <queue>
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor:public GraphObject{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* ptr, Direction dir = right, double size = 1.0, unsigned int depth = 0);
    virtual ~Actor();
    virtual void doSomething()=0;
    void setAlive(bool lifeState);
    bool getAlive();
    StudentWorld* getWorld();
    void setActorVisible(bool val);
    bool getActorVisible();
    virtual bool isInitiallyDistributedGoodie();
    virtual bool isBoulder();
    virtual bool isAnnoyable();
private:
    bool alive; //indicates whether the Actor is still active/alive
    StudentWorld* m_ptr;
    bool visible;
};

class Earth: public Actor{
public:
    Earth(int startX, int startY, StudentWorld* ptr);
    virtual ~Earth();
private:
    virtual void doSomething();
};

class Person:public Actor{
public:
    Person(int imageID, int startX, int startY, StudentWorld* ptr, Direction dir, double size, unsigned int depth, int points);
    virtual bool isAnnoyable();
    virtual ~Person();
    int getHitPoints();
    void setHitPoints(int val);
private:
    int hitPoints;
};

class TunnelMan:public Person{
public:
    TunnelMan(StudentWorld* ptr);
    virtual ~TunnelMan();
    virtual void doSomething();
    void changeGoldNuggets(int val);
    void changeSonarCharges(int val);
    void changeSquirts(int val);
    int getSquirts();
    int getGold();
    int getSonar();
    
private:
    int squirts;
    int sonarCharges;
    int goldNuggets;
    //Returns true if there is no boulder to the left of TunnelMan
    bool tunnelManHelperLeft();
    //Returns true if there is no boulder above TunnelMan
    bool tunnelManHelperUp();
    //Returns true if there is no boulder below TunnelMan
    bool tunnelManHelperDown();
    //Returns true if there is no boulder to the right of TunnelMan
    bool tunnelManHelperRight();
};

class Protestor: public Person{
public:
    Protestor(int imageID, StudentWorld* ptr, Direction dir);
    virtual ~Protestor();
    virtual void annoyProtestor(int val, bool boulderBonked, bool squirted);
    void bribedProtestor();
protected:
    virtual void doSomething();
    void setRestingStateTicks(int val);
    int getRestingStateTicks();
    void decrementRestingStateTicks();
    int getTicksToWaitBetweenMoves();
    void setLeaveOilField(bool b);
    bool getLeaveOilField();
    int getNonShoutingTicks();
    void decrementNonShoutingTicks();
    void resetNonShoutingTicks();
    void setNumSquaresToMoveInCurrentDirection(int val);
    void decrementNumSquaresToMoveInCurrentDirection();
    int getNumSquaresToMoveInCurrentDirection();
    bool straightLineHelper(int x, int y);
    Actor::Direction indirectHelper();
    bool boulderCheckLeft();
    bool boulderCheckUp();
    bool boulderCheckRight();
    bool boulderCheckDown();
    bool intersectionHelper();
    bool canMove(Direction dir);
    bool moveAssistant(Direction dir);
    void setProtestorDirection(Direction dir);
    void setPerpendicularTurnTickCounter(int val);
    int getPerpendicularTurnTickCounter();
    void decrementPerpendicularTurnTickCounter();
    virtual bool isRegularProtestor();
    virtual bool isHardcoreProtestor();
    struct Point{
        int x;
        int y;
    };
    void pathSearcher(int x, int y, std::queue<Point>& pathQueue, bool arr[][64]);
private:
    bool leave_oil_field;
    int numSquaresToMoveInCurrentDirection;
    int ticksToWaitBetweenMoves;
    int restingStateTicks;
    int nonShoutingTicks;
    int perpendicularTurnTickCounter;
};

class RegularProtestor: public Protestor{
public:
    RegularProtestor(StudentWorld* ptr);
    virtual ~RegularProtestor();
    bool isRegularProtestor();
    virtual void doSomething();
private:
};


class HardcoreProtestor:public Protestor{
public:
    HardcoreProtestor(StudentWorld* ptr);
    virtual ~HardcoreProtestor();
    bool isHardcoreProtestor();
    virtual void doSomething();
};

class Goodies: public Actor{
public:
    Goodies(int imageID, int startX, int startY, StudentWorld* ptr, Direction dir, double size, unsigned int depth);
    void setGoodiesTickLife(int val);
    int getGoodiesTickLife();
    virtual ~Goodies();
protected:
    void setGoodiesTemp(bool val);
    bool getGoodiesTemp();
    void decreaseGoodiesTickLife();
    bool getTM_pickup();;
    void setTM_pickup(bool val);
private:
    bool tm_pickup; //can be picked up by TunnelMan
    bool visible;
    bool temp;
    int tickLife;
};

class Barrel: public Goodies{
public:
    Barrel(int startX, int startY, StudentWorld* ptr);
    virtual void doSomething();
    virtual bool isInitiallyDistributedGoodie();
    virtual ~Barrel();
private:
    
};

class SonarKit: public Goodies{
public:
    SonarKit(int startX, int startY, StudentWorld* ptr);
    virtual void doSomething();
    virtual ~SonarKit();
private:
};

class GoldNugget: public Goodies{
public:
    GoldNugget(int startX, int startY, StudentWorld* ptr, bool visibility, bool tunnelMan_pickup, bool temporary);
    virtual void doSomething();
    virtual bool isInitiallyDistributedGoodie();
    virtual ~GoldNugget();
private:
};

class Water:public Goodies{
public:
    Water(int startX, int startY, StudentWorld* ptr);
    virtual void doSomething();
    virtual ~Water();
private:
};

class Boulder:public Actor{
public:
    Boulder(int startX, int startY, StudentWorld* ptr);
    virtual void doSomething();
    virtual bool isInitiallyDistributedGoodie();
    virtual bool isBoulder();
    virtual ~Boulder();
private:
    std::string boulderState;
    int waitingTicks;
};

class Squirt: public Actor{
public:
    Squirt(int startX, int startY, StudentWorld* ptr, Direction dir);
    virtual void doSomething();
    virtual ~Squirt();
private:
    int travelDist; //initial travel distance
    
    /* Returns true if Squirt can move one square in its currently-facing direction */
    bool SquirtHelper(Direction dir);
};


#endif // ACTOR_H_



















 

