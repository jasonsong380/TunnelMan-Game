//Jason Song
#include "Actor.h"
#include "StudentWorld.h"
#include "GameController.h"
#include <queue>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(int imageID, int startX, int startY, StudentWorld* ptr, Direction dir, double size, unsigned int depth):GraphObject(imageID, startX, startY, dir,size, depth){
    alive=true;
    m_ptr=ptr;
    setVisible(true);
    visible=true;
}

Actor::~Actor(){}

void Actor::setAlive(bool lifeState){
    alive=lifeState;
}

bool Actor::getAlive(){
    return alive;
}

void Actor::setActorVisible(bool val){
    visible=val;
}

bool Actor::getActorVisible(){
    return visible;
}

StudentWorld* Actor::getWorld(){
    return m_ptr;
}

bool Actor::isInitiallyDistributedGoodie(){
    return false;
}

bool Actor::isBoulder(){
    return false;
}

bool Actor::isAnnoyable(){
    return false;
}

Earth::~Earth(){};

Earth::Earth(int startX, int startY,StudentWorld* ptr):Actor(TID_EARTH, startX, startY, ptr, right, 0.25, 3){};

void Earth::doSomething(){}

TunnelMan::TunnelMan(StudentWorld* ptr):Person(TID_PLAYER, 30, 60, ptr, right, 1.0, 0,10){
    squirts=5;
    sonarCharges=1;
    goldNuggets=0;
}

TunnelMan::~TunnelMan(){};

void TunnelMan::doSomething(){
    //Checks to see if it is currently alive.
    if(getWorld()->getTunnelManHitPoints()==0||!getAlive()){
        setAlive(false);
        getWorld()->decLives();
        return;
    }
    /* If the TunnelMan’s 4x4 image currently overlaps any Earth objects within the oil field, then it will call upon the StudentWorld object to remove/destroy the Earth objects from the 4x4 area occupied by the TunnelMan.*/
    for(int i=getX();i<=getX()+3;i++){
        for(int k=getY();k<=getY()+3;k++){
            if(i>=0 && i<=63 && k>=0 && k<=59 && getWorld()->getEarthVisibility(i, k)){
                getWorld()->earthInvisible(i, k);
            }
        }
    }
    
    int ch;
    
    //Check to see if the human player pressed a key
    if(getWorld()->getKey(ch)){
        switch (ch) {
            case KEY_PRESS_ESCAPE:
                //Allows the user to abort the current level
                setAlive(false);
                break;
            case KEY_PRESS_SPACE:
                //TunnelMan will fire a Squirt into the oil field 4 squares in front of TunnelMan (facing same direction), assuming they have sufficient water in their squirt gun to do so.
                if(squirts>0){
                    squirts--; //Reduce water count by 1
                    if(getDirection()==right && getX()+4<=63 && !getWorld()->tmCheckFourbyFour(getX()+4,getY()) && getWorld()->tunnelManBoulderChecker(getX()+4, getY())){
                        getWorld()->addToList(new Squirt(getX()+4,getY(),getWorld(),right));
                    }
                    else if(getDirection()==up && getY()+4<=59 && !getWorld()->tmCheckFourbyFour(getX(), getY()+4) && getWorld()->tunnelManBoulderChecker(getX(), getY()+4)){
                        getWorld()->addToList(new Squirt(getX(),getY()+4,getWorld(),up));
                    }
                    else if(getDirection()==left && getX()-4>=0 && !getWorld()->tmCheckFourbyFour(getX()-4, getY()) && getWorld()->tunnelManBoulderChecker(getX()-4, getY())){
                        getWorld()->addToList(new Squirt(getX()-4,getY(),getWorld(),left));
                    }
                    else if(getDirection()==down && getY()-4>=0 && !getWorld()->tmCheckFourbyFour(getX(), getY()-4) && getWorld()->tunnelManBoulderChecker(getX(), getY()-4)){
                        getWorld()->addToList(new Squirt(getX(),getY()-4,getWorld(),down));
                    }
                    
                    GameController::getInstance().playSound(SOUND_PLAYER_SQUIRT);
                }
                break;
            case KEY_PRESS_LEFT:
                /*If the player wants their TunnelMan to turn to face a different direction then it is currently facing (by pressing one of the arrow keys), then the TunnelMan’s direction should be adjusted to the indicated direction but the TunnelMan must not move in that direction. */
                if(getDirection()!=left){
                    setDirection(left);
                    break;
                }
        
                if(getX()-1>=0 && getX()-1<=60 && getY()>=0 && getY()<=60 && tunnelManHelperLeft()){
                    moveTo(getX()-1,getY());
                }
                if(getWorld()->tmCheckFourbyFour(getX(), getY())){
                GameController::getInstance().playSound(SOUND_DIG);
                }
                break;
                
            case KEY_PRESS_RIGHT:
                if(getDirection()!=right){
                    setDirection(right);
                    break;
                }
                if(getX()+1>=0 && getX()+1<=60 && getY()>=0 && getY()<=60 && tunnelManHelperRight()){
                    moveTo(getX()+1,getY());
                }
                if(getWorld()->tmCheckFourbyFour(getX(), getY())){
                GameController::getInstance().playSound(SOUND_DIG);
                }
                break;
            case KEY_PRESS_UP:
                if(getDirection()!=up){
                    setDirection(up);
                    break;
                }
                if(getX()>=0 && getX()<=60 && getY()+1>=0 && getY()+1<=60 && tunnelManHelperUp()){
                    moveTo(getX(),getY()+1);
                }
                if(getWorld()->tmCheckFourbyFour(getX(), getY())){
                GameController::getInstance().playSound(SOUND_DIG);
                }
                break;
            case KEY_PRESS_DOWN:
                if(getDirection()!=down){
                    setDirection(down);
                    break;
                }
                if(getX()>=0 && getX()<=60 && getY()-1>=0 && getY()-1<=60 && tunnelManHelperDown()){
                    moveTo(getX(),getY()-1);
                }
                if(getWorld()->tmCheckFourbyFour(getX(), getY())){
                GameController::getInstance().playSound(SOUND_DIG);
                }
                break;
            case 'Z':
            case 'z':
                if(sonarCharges>0){
                    sonarCharges--; //Decrement sonar charge count
                    getWorld()->tunnelManSonarChecker(getX(), getY()); //All hidden game objects (e.g., Gold Nuggets or Barrels of oil) that are within a radius of 12 must be made visible and revealed to the player.
                    
                    GameController::getInstance().playSound(SOUND_SONAR);
                }
                break;
            case KEY_PRESS_TAB:
                /*If the player presses the TAB key and they have one or more units of gold in their inventory, then they must add a Gold Nugget object into the oil field at their current x,y location and reduce their gold count by 1 unit. */
                if(goldNuggets>0){
                    GoldNugget* g=new GoldNugget(getX(),getY(),getWorld(),true,false,true);
                    getWorld()->addToList(g);
                    g->setGoodiesTickLife(100);
                    goldNuggets--;
                }
                
                break;
        }
        
    }
}

int TunnelMan::getSquirts()
{
    return squirts;
}
int TunnelMan::getGold(){
    return goldNuggets;
}

int TunnelMan::getSonar(){
    return sonarCharges;
}

//Returns true if there is no boulder to the left of TunnelMan
bool TunnelMan::tunnelManHelperLeft(){
    for(int k=getY()-3;k<=getY()+3;k++){
        if(getWorld()->isBoulder(getX()-4,k)){
            return false;
        }
    }
    return true;
}

//Returns true if there is no boulder above TunnelMan
bool TunnelMan::tunnelManHelperUp(){
    for(int i=getX()-3;i<=getX()+3;i++){
        if(getWorld()->isBoulder(i, getY()+4)){
            return false;
        }
    }
    return true;
}


//Returns true if there is no boulder below TunnelMan
bool TunnelMan::tunnelManHelperDown(){
    for(int i=getX()-3;i<=getX()+3;i++){
        if(getWorld()->isBoulder(i, getY()-4)){
            return false;
        }
    }
    return true;
}

//Returns true if there is no boulder to the right of TunnelMan
bool TunnelMan::tunnelManHelperRight(){
    for(int k=getY()-3;k<=getY()+3;k++){
        if(getWorld()->isBoulder(getX()+4,k)){
            return false;
        }
    }
    return true;
}

void TunnelMan::changeSonarCharges(int val){
    sonarCharges+=val;
}


void TunnelMan::changeGoldNuggets(int val){
    goldNuggets+=val;
}

void TunnelMan::changeSquirts(int val){
    squirts+=val;
}

bool Goodies::getTM_pickup(){
    return tm_pickup;
}

void Goodies::setTM_pickup(bool val){
    tm_pickup=val;
}

Goodies::Goodies(int imageID, int startX, int startY, StudentWorld* ptr, Direction dir, double size, unsigned int depth):Actor(imageID, startX, startY, ptr, dir, size, depth){
    tm_pickup=true;
    visible=true;
    temp=false;
    tickLife=0;
}

Goodies::~Goodies(){}

void Goodies::setGoodiesTemp(bool val){
    temp=val;
}

bool Goodies::getGoodiesTemp(){
    return temp;
}

void Goodies::decreaseGoodiesTickLife(){
    tickLife--;
}

void Goodies::setGoodiesTickLife(int val){
    tickLife=val;
}

int Goodies::getGoodiesTickLife(){
    return tickLife;
}

Barrel::Barrel(int startX, int startY, StudentWorld* ptr):Goodies(TID_BARREL,startX,startY,ptr,right,1.0,2){
    setVisible(false);
    setActorVisible(false);
}

Barrel::~Barrel(){}

void Barrel::doSomething(){
    //Check to see if it is currently alive
    if(!getAlive()){
        return;
    }
    /* If the Barrel is not currently visible AND the TunnelMan is within a radius of 4.0 of it, then the Barrel must make itself visible with the setVisible() method.*/
    if(!getActorVisible() && getWorld()->computeDistanceToTM(this)<=4.0){
        setVisible(true);
        setActorVisible(true);
        return;
    }
    if(getWorld()->computeDistanceToTM(this)<=3.0){
        setAlive(false);
        GameController::getInstance().playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        //Inform the StudentWorld object that it was picked up. Once all Barrels on the level have been picked up, the player finishes the level and may advance to the next level.
        getWorld()->subtractBarrel();
    }
}

bool Barrel::isInitiallyDistributedGoodie(){
    return true;
}

SonarKit::SonarKit(int startX, int startY, StudentWorld* ptr):Goodies(TID_SONAR,startX,startY,ptr,right,1.0,2){
    setGoodiesTemp(true);
    if(100>=300-10*(getWorld()->getLevel())){
        setGoodiesTickLife(100);
    }else{
        setGoodiesTickLife(300-10*(getWorld()->getLevel()));
    }
}

SonarKit::~SonarKit(){}

void SonarKit::doSomething(){
    if(!getAlive()){
        return;
    }
    if(getGoodiesTickLife()>0){
        decreaseGoodiesTickLife();
    }
    if(getWorld()->computeDistanceToTM(this)<=3.0){
        setAlive(false);
        GameController::getInstance().playSound(SOUND_GOT_GOODIE);
        //The Sonar Kit must tell the TunnelMan object that it just received a new Sonar Kit so it can update its inventory.
        getWorld()->changeSonarAmount(2);
        getWorld()->increaseScore(75);
    }
    if(getGoodiesTickLife()==0){
        setAlive(false);
    }
}

GoldNugget::GoldNugget(int startX, int startY, StudentWorld* ptr, bool visibility, bool tunnelMan_pickup, bool temporary):Goodies(TID_GOLD,startX,startY,ptr,right,1.0,2){
    //Nuggets buried within the Earth of the oil field always start out invisible, whereas Nuggets dropped by the TunnelMan start out visible.
    setVisible(visibility);
    setActorVisible(visibility);
    setGoodiesTemp(temporary);
    setTM_pickup(tunnelMan_pickup);
    if(100>=300-10*(getWorld()->getLevel())){
        setGoodiesTickLife(100);
    }else{
        setGoodiesTickLife(300-10*(getWorld()->getLevel()));
    }
    
}

GoldNugget::~GoldNugget(){}

void GoldNugget::doSomething(){
    if(!getAlive()){
        return;
    }
    if(getGoodiesTemp() && getGoodiesTickLife()>0){
        decreaseGoodiesTickLife();
    }
    
    /*If the Gold Nugget is not currently visible and the TunnelMan is within a radius of 4.0 of it, then the Gold Nugget must make itself visible. */
    if(!getActorVisible() && getWorld()->computeDistanceToTM(this)<=4.0){
        setVisible(true);
        setActorVisible(true);
        return;
    }
    if(getTM_pickup() && getWorld()->computeDistanceToTM(this)<=3.0){
        setAlive(false);
        GameController::getInstance().playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        //The Gold Nugget must tell the TunnelMan object that it just received a new Nugget so it can update its inventory.
        getWorld()->changeGoldAmount(1);
    }
    else if(!getTM_pickup()){
        Protestor* p=getWorld()->goldNuggetProtesterFinder(this);
        if(p!=nullptr){
        setAlive(false);
        GameController::getInstance().playSound(SOUND_PROTESTER_FOUND_GOLD);
//The Gold Nugget must tell the Protester object that it just received a new Nugget so it can react appropriately (e.g., be bribed).
        p->bribedProtestor();
        }
    }
    
    //If the Gold Nugget has a temporary state, then it will check to see if its tick lifetime has elapsed, and if so it must set its state to dead
    if(getGoodiesTemp() && getGoodiesTickLife()==0){
        setAlive(false);
    }
}

bool GoldNugget::isInitiallyDistributedGoodie(){
    return true;
}

Water::Water(int startX, int startY, StudentWorld* ptr):Goodies(TID_WATER_POOL, startX, startY, ptr, right, 1.0, 2){
    setGoodiesTemp(true);
    if(100>=300-10*(getWorld()->getLevel())){
        setGoodiesTickLife(100);
    }else{
        setGoodiesTickLife(300-10*(getWorld()->getLevel()));
    }
}

Water::~Water(){}

void Water::doSomething(){
    if(!getAlive()){
        return;
    }
    decreaseGoodiesTickLife();
    if(getWorld()->computeDistanceToTM(this)<=3.0){
        setAlive(false);
        GameController::getInstance().playSound(SOUND_GOT_GOODIE);
        //The Water pool must tell the TunnelMan object that it just received 5 new squirts of water so it can update its inventory.
        getWorld()->changeSquirtAmount(5);
        getWorld()->increaseScore(100);
    }
    if(getGoodiesTickLife()==0){
        setAlive(false);
    }
}

Boulder::Boulder(int startX, int startY, StudentWorld* ptr):Actor(TID_BOULDER, startX, startY, ptr, down, 1.0, 1){
    boulderState="stable";
    waitingTicks=0;
}

Boulder::~Boulder(){}

bool Boulder::isBoulder(){
    return true;
}

void Boulder::doSomething(){
    //Check to see if it’s still alive
    if(!getAlive()){
        return;
    }
    if(waitingTicks>0){
        waitingTicks--;
        return;
    }
    //If the Boulder is currently in the stable state, then it must check to see if there is any Earth in the 4 squares immediately below it.
    if(boulderState=="stable"){
        for(int j=getX();j<=getX()+3;j++){
            if(j<=63 && getY()-1>=0 && getWorld()->getEarthVisibility(j, getY()-1)){
                //If there is any Earth below the Boulder, it does nothing.
                return;
            }
        }
        //If none of the 4 squares beneath the Boulder have any Earth, then the Boulder must transition into a waiting state for the next 30 ticks.
        boulderState="waiting";
        waitingTicks=30;
        return;
    }
    //If the Boulder is in a waiting state and 30 ticks have elapsed, then it must transition into a falling state and play the sound SOUND_FALLING_ROCK.
    if(boulderState=="waiting"){
        boulderState="falling";
        GameController::getInstance().playSound(SOUND_FALLING_ROCK);
        return;
    }
    if(boulderState=="falling"){
        /*It must continue to move downward one square during each tick until it either (a) hits the bottom of the oil field (i.e., it tries to move to y=-1), (b) runs into the top of another Boulder, or (c) it runs into Earth (i.e., by moving down a square, the Boulder would overlap over one or more Earth objects). When any of the above conditions are met the Boulder must set its state to dead so it can be removed from the game at the end of the current tick.*/
        bool b=false;
        for(int i=getX()-3;i<=getX()+3;i++){
            if(getWorld()->isBoulder(i, getY()-4)){
                b=true;
            }
        }
        if(getY()-1<0 || b || getWorld()->tmCheckFourbyFour(getX(), getY()-1)){
            setAlive(false);
        }else{
            //Otherwise, move down by one.
            moveTo(getX(), getY()-1);
        }
        /* If the Boulder comes within a radius of 3 of any Protester(s) or the TunnelMan while falling, it must cause 100 points of annoyance to those actors (effectively completely annoying them in one fell swoop). If the Boulder annoys the TunnelMan, the player will instantly lose a life. */
        if(getWorld()->computeDistanceToTM(this)<=3){
            getWorld()->annoyTunnelMan(100);
            getWorld()->killTunnelMan();
            getWorld()->decLives();
        }
        
        //Bonks any Protester within radius of 3 of Boulder
        getWorld()->boulderBonker(this);
    }
}

bool Boulder::isInitiallyDistributedGoodie(){
    return true;
}

Squirt::Squirt(int startX, int startY, StudentWorld* ptr, Direction dir): Actor(TID_WATER_SPURT, startX, startY, ptr, dir,1.0,1){
    travelDist=4;
}

Squirt::~Squirt(){}

/* Returns true if Squirt can move one square in its currently-facing direction */
bool Squirt::SquirtHelper(Direction dir){
    if(dir==right){
        //Check if there's any Earth to the right of Squirt object
        for(int k=getY(); k<=getY()+3;k++){
            if(getX()+4<=63 && k<=59 && getWorld()->getEarthVisibility(getX()+4, k)){
                return false;
            }
        }
        //Check if there's a Boulder anywhere to the right of Squirt object
        for(int i=getX()-1;i<=getX()+4;i++){
            for(int k=getY()-3; k<=getY()+3;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    else if(dir==up){
        //Check if there's any Earth on top of Squirt object
        for(int i=getX();i<=getX()+3;i++){
            if(i<=63 && getY()+4<=59 && getWorld()->getEarthVisibility(i, getY()+4)){
                return false;
            }
        }
        //Check if there's a Boulder anywhere on top of Squirt object
        for(int i=getX()-3;i<=getX()+3;i++){
            for(int k=getY()-1; k<=getY()+4;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    else if(dir==left){
        //Check if there's any Earth to the left of Squirt object
        for(int k=getY(); k<=getY()+3;k++){
            if(getX()-1>=0 && k<=59 && getWorld()->getEarthVisibility(getX()-1, k)){
                return false;
            }
        }
        //Check if there's a Boulder anywhere to the left of Squirt object
        for(int i=getX()-4;i<=getX()+1;i++){
            for(int k=getY()-3; k<=getY()+3;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    else if(dir==down){
        //Check if there's any Earth below Squirt object
        for(int i=getX();i<=getX()+3;i++){
            if(i<=63 && getY()-1>=0 && getWorld()->getEarthVisibility(i, getY()-1)){
                return false;
            }
        }
        //Check if there's a Boulder anywhere below Squirt object
        for(int i=getX()-3;i<=getX()+3;i++){
            for(int k=getY()-4; k<=getY()+1;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    
    return true;
}

void Squirt::doSomething(){
    /*If a Squirt is within a radius of 3.0 of one or more Protesters (up to and including a distance of 3.0 squares away), it will cause 2 points of annoyance to these Protester(s), and then immediately set its state to dead, so it can be removed from the oil field at the end of the tick.*/
    getWorld()->squirtAssistant(this);
    
    //If a Squirt has traveled through its full travel distance, then it immediately sets its state to dead, so it can be removed from the oil field at the end of the current tick.
    if(travelDist==0){
        setAlive(false);
    }
//  Otherwise, the Squirt must check to see if it can move one square in its currently- facing direction. If this target location is occupied by either Earth or a Boulder, then the Squirt immediately sets its state to dead so it can be removed from the oil field at the end of the current tick.
    else if(!SquirtHelper(getDirection())){
        setAlive(false);
    }
    //Otherwise, the Squirt moves one square forward in its currently-facing direction, and then returns.
    else{
        switch (getDirection()) {
            case up:
                moveTo(getX(), getY()+1);
                break;
            case down:
                moveTo(getX(),getY()-1);
                break;
            case left:
                moveTo(getX()-1, getY());
                break;
            case right:
                moveTo(getX()+1, getY());
                break;
            default:
                break;
        }
        travelDist--;
    }
    
}


int Person::getHitPoints(){
    return hitPoints;
}

bool Person::isAnnoyable(){
    return true;
}

Person::~Person(){}

Person::Person(int imageID, int startX, int startY, StudentWorld* ptr, Direction dir,double size, unsigned int depth, int points):Actor(imageID, startX, startY, ptr, dir){
    hitPoints=points;
};

void Person::setHitPoints(int val){
    hitPoints=val;
}


Protestor::Protestor(int imageID, StudentWorld* ptr, Direction dir):Person(imageID, 60, 60, ptr, dir, 1.0, 0, 0){
    leave_oil_field=false;
    numSquaresToMoveInCurrentDirection=8+rand()%53;
    ticksToWaitBetweenMoves=5;
//    if(3-(getWorld()->getLevel())/4>=0){
//        ticksToWaitBetweenMoves=3-getWorld()->getLevel()/4;
//    }else{
//        ticksToWaitBetweenMoves=0;
//    }
    nonShoutingTicks=0;
    restingStateTicks=0;
}

void Protestor::setRestingStateTicks(int val){
    restingStateTicks=val;
}

int Protestor::getTicksToWaitBetweenMoves(){
    return ticksToWaitBetweenMoves;
}

void Protestor::setLeaveOilField(bool b){
    leave_oil_field=b;
}

bool Protestor::getLeaveOilField(){
    return leave_oil_field;
}

int Protestor::getRestingStateTicks(){
    return restingStateTicks;
}

void Protestor::decrementRestingStateTicks(){
    restingStateTicks--;
}

int Protestor::getNonShoutingTicks(){
    return nonShoutingTicks;
}

void Protestor::decrementNonShoutingTicks(){
    nonShoutingTicks--;
}

void Protestor::resetNonShoutingTicks(){
    nonShoutingTicks=15;
}

void Protestor::setNumSquaresToMoveInCurrentDirection(int val){
    numSquaresToMoveInCurrentDirection=val;
}

void Protestor::decrementNumSquaresToMoveInCurrentDirection(){
    numSquaresToMoveInCurrentDirection--;
}

int Protestor::getNumSquaresToMoveInCurrentDirection(){
    return numSquaresToMoveInCurrentDirection;
}

Protestor::~Protestor(){}

bool Protestor::straightLineHelper(int x, int y){
    if(getX()>x && getY()==y){
        for(int i=x; i<getX();i++){
            for(int k=getY(); k<=getY()+3;k++){
                if(getWorld()->getEarthVisibility(i, k)){
                    return false;
                }
            }
        }
        for(int i=x;i<=getX()-4;i++){
            for(int k=y-3;k<=y+3;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    
    if(getX()<x && getY()==y){
        for(int i=getX(); i<x;i++){
            for(int k=y;k<=y+3;k++){
                if(getWorld()->getEarthVisibility(i, k)){
                    return false;
                }
            }
        }
        for(int i=getX();i<=x-4;i++){
            for(int k=y-3;k<=y+3;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    
    if(getX()==x && getY()>y){
        for(int i=getX(); i<getX()+4;i++){
            for(int k=y;k<=getY()-1;k++){
                if(getWorld()->getEarthVisibility(i, k)){
                    return false;
                }
            }
        }
        
        for(int i=x-3;i<=x+3;i++){
            for(int k=y;k<=getY()-4;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    
    if(getX()==x && getY()<y){
        for(int i=getX(); i<getX()+4;i++){
            for(int k=getY();k<=y-1;k++){
                if(getWorld()->getEarthVisibility(i, k)){
                    return false;
                }
            }
        }
        
        for(int i=x-3;i<=x+3;i++){
            for(int k=getY();k<=y-4;k++){
                if(getWorld()->isBoulder(i, k)){
                    return false;
                }
            }
        }
    }
    
    return true;
}

RegularProtestor::RegularProtestor(StudentWorld* ptr):Protestor(TID_PROTESTER, ptr, left){
    setHitPoints(5);
}

RegularProtestor::~RegularProtestor(){}

void Protestor::bribedProtestor(){
    if(isRegularProtestor()){
        getWorld()->increaseScore(25);
        setLeaveOilField(true);
    }
    
    if(isHardcoreProtestor()){
        getWorld()->increaseScore(50);
        int num=50;
        if(100-getWorld()->getLevel()*10>50){
            num=100-getWorld()->getLevel()*10;
        }
        setRestingStateTicks(num);
    }
}


void Protestor::pathSearcher(int x, int y, queue<Point>& pathQueue, bool arr[][64]){
    if(pathQueue.empty()){
        return;
    }
    int curx=(pathQueue.front()).x;
    int cury=(pathQueue.front()).y;
    pathQueue.pop();
    if((curx+1==x && cury==y) || (curx-1==x && cury==y) || (curx==x && cury+1==y) || (curx==x && cury-1==y)){
        moveTo(curx, cury);
        return;
    }
    if(!getWorld()->tmCheckFourbyFour(curx-1, cury) && getWorld()->tunnelManBoulderChecker(curx-1, cury) && curx-1>=0 && !arr[curx-1][cury]){
        arr[curx-1][cury]=true;
        Point p={curx-1,cury};
        pathQueue.push(p);
    }else if(!getWorld()->tmCheckFourbyFour(curx+1, cury) && getWorld()->tunnelManBoulderChecker(curx+1, cury) && curx+1<=63 && !arr[curx+1][cury]){
        arr[curx+1][cury]=true;
        Point p={curx+1,cury};
        pathQueue.push(p);
    }else if(!getWorld()->tmCheckFourbyFour(curx, cury+1) && getWorld()->tunnelManBoulderChecker(curx, cury+1) && cury+1<=63 && !arr[curx][cury+1]){
        arr[curx][cury+1]=true;
        Point p={curx,cury+1};
        pathQueue.push(p);
    }else if(!getWorld()->tmCheckFourbyFour(curx, cury-1) && getWorld()->tunnelManBoulderChecker(curx, cury-1) && cury-1>=0 && !arr[curx][cury-1]){
        arr[curx][cury-1]=true;
        Point p={curx,cury-1};
        pathQueue.push(p);
    }
    pathSearcher(x,y,pathQueue,arr);
}


void RegularProtestor::doSomething(){
    Protestor::doSomething();
}

void HardcoreProtestor::doSomething(){
    Protestor::doSomething();
}

void Protestor::doSomething(){
    //Check to see if it is currently alive
    if(!getAlive()){
        return;
    }
    //If the Regular Protester is in a “rest state” during the current tick, it must do nothing other than update its resting tick count, and immediately return.
    if(!getLeaveOilField() && getRestingStateTicks()>0){
        decrementRestingStateTicks();
        return;
    }
    
    if(getLeaveOilField()){
        decrementNonShoutingTicks();
        decrementPerpendicularTurnTickCounter();
        //If already at exit point, set status to dead.
        if(getX()==60 && getY()==60){
            setAlive(false);
            return;
        }
        //Escape Algorithm
        std::queue<Point> pathQueue;
        bool discoveredArray[64][64];
        for(int i=0;i<64;i++){
            for(int k=0;k<64;k++){
                discoveredArray[i][k]=false;
            }
        }
        Point start={60,60};
        pathQueue.push(start);
        discoveredArray[60][60]=true;
        pathSearcher(getX(), getY(), pathQueue, discoveredArray);
        return;
    }
    bool facingSameDirectionAsTM=false;
    int tm_x=getWorld()->getTunnelManXCoord();
    int tm_y=getWorld()->getTunnelManYCoord();
    //All 8 possible scenarios in which the protestor would be facing in the TunnelMan's direction:
    bool northeast= getX()>tm_x && getY()>tm_y && (getDirection()==left || getDirection()==down);
    bool southeast= getX()>tm_x && getY()<tm_y && (getDirection()==left || getDirection()==up);
    bool northwest= getX()<tm_x && getY()>tm_y && (getDirection()==right || getDirection()==down);
    bool southwest= getX()<tm_x && getY()<tm_y && (getDirection()==right || getDirection()==up);
    bool directlyNorth= getX()==tm_x && getY()>tm_y && getDirection()==down;
    bool directlySouth= getX()==tm_x && getY()<tm_y && getDirection()==up;
    bool directlyEast= getY()==tm_y && getX()>tm_x && getDirection()==left;
    bool directlyWest= getY()==tm_y && getX()<tm_x && getDirection()==right;
    
    if(northeast||southeast||northwest||southwest||directlyNorth||directlySouth||directlyEast||directlyWest){
        facingSameDirectionAsTM=true;
    }

    //Check to see if protestor is within a distance of 4 units from the TunnelMan and if protestor is currently facing in the TunnelMan’s direction
    if(getWorld()->computeDistanceToTM(this)<=4 && facingSameDirectionAsTM && getNonShoutingTicks()==0){
        GameController::getInstance().playSound(SOUND_PROTESTER_YELL);
        getWorld()->annoyTunnelMan(2);
        resetNonShoutingTicks(); //Update some state variable in a manner that prevents this protester from shouting again for at least 15 non-resting ticks.
        decrementPerpendicularTurnTickCounter();
        setRestingStateTicks(getTicksToWaitBetweenMoves());
        return;
    }
    
    //If protestor is in straight horizontal/vertical line of sight to TunnelMan, is more than 4 units away from TunnelMan and could actually move the entire way to the TunnelMan with no Earth or Boulders blocking its path, protestor will change its direction to face TunnelMan and take one step towards him.
    if((getX()==tm_x || getY()==tm_y) && getWorld()->computeDistanceToTM(this)>4.0 && straightLineHelper(tm_x, tm_y)){
        if(getX()>tm_x && getY()==tm_y && getWorld()->tunnelManBoulderChecker(getX()-1, getY()) && getX()-1>=0){
            setProtestorDirection(left);
            moveTo(getX()-1, getY());
        }
        else if(getX()<tm_x && getY()==tm_y && getWorld()->tunnelManBoulderChecker(getX()+1, getY()) && getX()+1<=60){
            moveTo(getX()+1, getY());
            setProtestorDirection(right);
        }
        else if(getX()==tm_x && getY()>tm_y && getWorld()->tunnelManBoulderChecker(getX(), getY()-1) && getY()-1>=0){
            setProtestorDirection(down);
            moveTo(getX(), getY()-1);
        }
        else if(getX()==tm_x && getY()<tm_y && getWorld()->tunnelManBoulderChecker(getX(), getY()+1) && getY()+1<=60){
            setProtestorDirection(up);
            moveTo(getX(), getY()+1);
        }
        setNumSquaresToMoveInCurrentDirection(0);
        if(getNonShoutingTicks()>0){
            decrementNonShoutingTicks();
        }
        setRestingStateTicks(getTicksToWaitBetweenMoves());
        return;
    }
    
    if(!(getX()==tm_x || getY()==tm_y)){
        decrementNumSquaresToMoveInCurrentDirection();
        if(getNonShoutingTicks()>0){
            decrementNonShoutingTicks();
        }
        if(getNumSquaresToMoveInCurrentDirection()<=0){
            setDirection(indirectHelper());
            setNumSquaresToMoveInCurrentDirection(8+rand()%53);
        }
        else if(intersectionHelper() && getPerpendicularTurnTickCounter()<=0){
            if((getDirection()==left || getDirection()==right) && canMove(up) && !canMove(down)){
                setProtestorDirection(up);
            }
            else if((getDirection()==left || getDirection()==right) && !canMove(up) && canMove(down)){
                setProtestorDirection(down);
            }
            else if((getDirection()==left || getDirection()==right) && canMove(up) && canMove(down)){
                int num=rand()%2;
                switch (num) {
                    case 0:
                        setProtestorDirection(up);
                        break;
                    default:
                        setProtestorDirection(down);
                        break;
                }
            }
            else if((getDirection()==up || getDirection()==down) && canMove(left) && !canMove(right)){
                setProtestorDirection(left);
            }
            else if((getDirection()==up || getDirection()==down) && !canMove(left) && canMove(right)){
                setProtestorDirection(right);
            }
            else if((getDirection()==up || getDirection()==down) && canMove(left) && canMove(right)){
                int num=rand()%2;
                switch (num) {
                    case 0:
                        setProtestorDirection(left);
                        break;
                    default:
                        setProtestorDirection(right);
                        break;
                }
            }
            setNumSquaresToMoveInCurrentDirection(8+rand()%53);
            setPerpendicularTurnTickCounter(200);
        }
        if(!moveAssistant(getDirection())){
            setNumSquaresToMoveInCurrentDirection(0);
        }
        setRestingStateTicks(getTicksToWaitBetweenMoves());
    }
}




void Protestor::setProtestorDirection(Direction dir){
    if(dir!=getDirection()){
        setPerpendicularTurnTickCounter(200);
    }else{
        decrementPerpendicularTurnTickCounter();
    }
    setDirection(dir);
}

void Protestor::annoyProtestor(int val, bool boulderBonked, bool squirted){
    if(getLeaveOilField()){
        return;
    }
    setHitPoints(getHitPoints()-val);
    if(getHitPoints()>0){
        GameController::getInstance().playSound(SOUND_PROTESTER_ANNOYED);
        int max;
        if(50>=100-getWorld()->getLevel()*10){
            max=50;
        }else{
            max=100-getWorld()->getLevel()*10;
        }
        setRestingStateTicks(max);
    }
    else if(getHitPoints()<=0){
        setLeaveOilField(true);
        getWorld()->decreaseProtestorCount();
        GameController::getInstance().playSound(SOUND_PROTESTER_GIVE_UP);
        setRestingStateTicks(0);
        if(boulderBonked==true && squirted==false){
            getWorld()->increaseScore(500);
        }
        else if(isRegularProtestor() && boulderBonked==false && squirted==true){
            getWorld()->increaseScore(100);
        }
        else if(isHardcoreProtestor() && boulderBonked==false && squirted==true){
            getWorld()->increaseScore(250);
        }
    }
}

void Protestor::setPerpendicularTurnTickCounter(int val){
    perpendicularTurnTickCounter=val;
}

int Protestor::getPerpendicularTurnTickCounter(){
    return perpendicularTurnTickCounter;
}

void Protestor::decrementPerpendicularTurnTickCounter(){
    perpendicularTurnTickCounter--;
}

bool Protestor::moveAssistant(Direction dir){
    bool b=true;
    if(dir==left && boulderCheckLeft() && getWorld()->tunnelManBoulderChecker(getX()-1, getY()) && getX()-1>=0 && !getWorld()->getEarthVisibility(getX()-1, getY()) && !getWorld()->getEarthVisibility(getX()-1, getY()+1) && !getWorld()->getEarthVisibility(getX()-1, getY()+2) && !getWorld()->getEarthVisibility(getX()-1, getY()+3)){
        moveTo(getX()-1, getY());
        return b;
    }
    if(dir==right && getWorld()->tunnelManBoulderChecker(getX()+1, getY()) && boulderCheckRight() && getX()+1<=60 && !getWorld()->getEarthVisibility(getX()+4, getY()) && !getWorld()->getEarthVisibility(getX()+4, getY()+1) && !getWorld()->getEarthVisibility(getX()+4, getY()+2) && !getWorld()->getEarthVisibility(getX()+4, getY()+3)){
        moveTo(getX()+1, getY());
        return b;
    }
    if(dir==up && getWorld()->tunnelManBoulderChecker(getX(), getY()+1) && boulderCheckUp() && getY()+1<=60 && !getWorld()->getEarthVisibility(getX(), getY()+4) && !getWorld()->getEarthVisibility(getX()+1, getY()+4) && !getWorld()->getEarthVisibility(getX()+2, getY()+4) && !getWorld()->getEarthVisibility(getX()+3, getY()+4)){
        moveTo(getX(), getY()+1);
        return b;
    }
    if(dir==down && getWorld()->tunnelManBoulderChecker(getX(), getY()-1) && boulderCheckDown() && getY()-1>=0 && !getWorld()->getEarthVisibility(getX(), getY()-1) && !getWorld()->getEarthVisibility(getX()+1, getY()-1) && !getWorld()->getEarthVisibility(getX()+2, getY()-1) && !getWorld()->getEarthVisibility(getX()+3, getY()-1)){
        moveTo(getX(), getY()-1);
        return b;
    }
    b=false;
    return b;
}

bool Protestor::intersectionHelper(){
    if((getDirection()==left || getDirection()==right) && (canMove(up)|| canMove(down))){
        return true;
    }
    if((getDirection()==up || getDirection()==down) && (canMove(left)|| canMove(right))){
        return true;
    }
    return false;
}

bool Protestor::canMove(Actor::Direction dir){
    if(dir==up && boulderCheckUp() && getY()+1<=60 && !getWorld()->getEarthVisibility(getX(), getY()+4) && !getWorld()->getEarthVisibility(getX()+1, getY()+4) && !getWorld()->getEarthVisibility(getX()+2, getY()+4) && !getWorld()->getEarthVisibility(getX()+3, getY()+4)){
        return true;
    }
    if(dir==left && boulderCheckLeft() && getX()-1>=0 && !getWorld()->getEarthVisibility(getX()-1, getY()) && !getWorld()->getEarthVisibility(getX()-1, getY()+1) && !getWorld()->getEarthVisibility(getX()-1, getY()+2) && !getWorld()->getEarthVisibility(getX()-1, getY()+3)){
        return true;
    }
    if(dir==down && boulderCheckDown() && getY()-1>=0 && !getWorld()->getEarthVisibility(getX(), getY()-1) && !getWorld()->getEarthVisibility(getX()+1, getY()-1) && !getWorld()->getEarthVisibility(getX()+2, getY()-1) && !getWorld()->getEarthVisibility(getX()+3, getY()-1)){
        return true;
    }
    if(dir==right && boulderCheckRight() && getX()+4<=60 && !getWorld()->getEarthVisibility(getX()+4, getY()) && !getWorld()->getEarthVisibility(getX()+4, getY()+1) && !getWorld()->getEarthVisibility(getX()+4, getY()+2) && !getWorld()->getEarthVisibility(getX()+4, getY()+3)){
        return true;
    }
    return false;
}

Actor::Direction Protestor::indirectHelper(){
    Direction dir;
    for(;;){
        int num=rand()%4;
        if(num==0 && boulderCheckLeft() && getX()-1>=0 && !getWorld()->getEarthVisibility(getX()-1, getY()) && !getWorld()->getEarthVisibility(getX()-1, getY()+1) && !getWorld()->getEarthVisibility(getX()-1, getY()+2) && !getWorld()->getEarthVisibility(getX()-1, getY()+3)){
            dir=left;
            break;
        }
        if(num==1 && boulderCheckUp() && getY()+1<=60 && !getWorld()->getEarthVisibility(getX(), getY()+4) && !getWorld()->getEarthVisibility(getX()+1, getY()+4) && !getWorld()->getEarthVisibility(getX()+2, getY()+4) && !getWorld()->getEarthVisibility(getX()+3, getY()+4)){
            dir=up;
            break;
        }
        if(num==2 && boulderCheckDown() && getY()-1>=0 && !getWorld()->getEarthVisibility(getX(), getY()-1) && !getWorld()->getEarthVisibility(getX()+1, getY()-1) && !getWorld()->getEarthVisibility(getX()+2, getY()-1) && !getWorld()->getEarthVisibility(getX()+3, getY()-1)){
            dir=down;
            break;
        }
        if(num==3 && boulderCheckRight() && getX()+1<=60 && !getWorld()->getEarthVisibility(getX()+4, getY()) && !getWorld()->getEarthVisibility(getX()+4, getY()+1) && !getWorld()->getEarthVisibility(getX()+4, getY()+2) && !getWorld()->getEarthVisibility(getX()+4, getY()+3)){
            dir=right;
            break;
        }
    }
    return dir;
}

//Returns true if there is no boulder to the left of protestor
bool Protestor::boulderCheckLeft(){
    for(int k=getY()-3;k<=getY()+3;k++){
        if(getWorld()->isBoulder(getX()-4,k)){
            return false;
        }
    }
    return true;
}

//Returns true if there is no boulder above protestor
bool Protestor::boulderCheckUp(){
    for(int i=getX()-3;i<=getX()+3;i++){
        if(getWorld()->isBoulder(i, getY()+4)){
            return false;
        }
    }
    return true;
}


//Returns true if there is no boulder below protestor
bool Protestor::boulderCheckDown(){
    for(int i=getX()-3;i<=getX()+3;i++){
        if(getWorld()->isBoulder(i, getY()-4)){
            return false;
        }
    }
    return true;
}

//Returns true if there is no boulder to the right of protestor
bool Protestor::boulderCheckRight(){
    for(int k=getY()-3;k<=getY()+3;k++){
        if(getWorld()->isBoulder(getX()+4,k)){
            return false;
        }
    }
    return true;
}

bool Protestor::isRegularProtestor(){
    return false;
}

bool RegularProtestor::isRegularProtestor(){
    return true;
}

bool Protestor::isHardcoreProtestor(){
    return false;
}

bool HardcoreProtestor::isHardcoreProtestor(){
    return true;
}

HardcoreProtestor::~HardcoreProtestor(){}

HardcoreProtestor::HardcoreProtestor(StudentWorld* ptr):Protestor(TID_HARD_CORE_PROTESTER, ptr, left){
    setHitPoints(20);
}
