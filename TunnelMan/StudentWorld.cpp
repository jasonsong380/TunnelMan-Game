//Jason Song
#include "StudentWorld.h"
#include <string>
#include <cstdlib>
#include <list>
#include "GameController.h"
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir)
{
    tm=nullptr;
    if(25>=200-getLevel()){
        protestorTicks=25;
    }else{
        protestorTicks=200-getLevel();
    }
    
    protestorTicksToWait=0;
    
    if(15<=2+getLevel()*1.5){
        targetProtestors=15;
    }else{
        targetProtestors=2+getLevel()*1.5;
    }
    protestorCount=0;
    
    if(90<=getLevel()*10+30){
        probabilityOfHardcore=90;
    }else{
        probabilityOfHardcore=getLevel()*10+30;
    }
    
    probabilityOfGoodie=getLevel()*25+300;
}

int StudentWorld::init()
{
    tm=new TunnelMan(this);

    //Earth array creation
    for(int i=0;i<=63;i++){
        for(int k=0;k<=59;k++){
            earthArray[i][k]=new Earth(i,k,this);
        }
    }
    
    //Creating the middle vertical shaft
    for(int i=30;i<=33;i++){
        for(int k=4;k<=59;k++){
            earthInvisible(i, k);
        }
    }
    
    //Compute number of randomly-distributed Boulders
    int B;
    if(getLevel()/2+2<=9){
        B=getLevel()/2+2;
    }else{
        B=9;
    }
    //Compute number of randomly-distributed Gold Nuggets
    int G;
    if(5-getLevel()/2>=2){
        G=5-getLevel()/2;
    }else{
        G=2;
    }
    //Compute number of randomly-distributed Barrels of oil
    if(2+getLevel()<=21){
        barrelsLeft=2+getLevel();
    }else{
        barrelsLeft=21;
    }
    
    //Randomly-distribute Boulders in oil field
    for(;;){
        int x=rand()% 61;
        int y=20+rand()% 37;
        
        if(!(x>=27 && x<=33 && y>=1 && y<=59)){
            for(int i=x;i<=x+3;i++){
                for(int k=y;k<=y+3;k++){
                    earthInvisible(i, k); //Ensure that there is not any Earth overlapping the 4x4 square region of each Boulder
                }
            }
            actorList.push_back(new Boulder(x,y,this));
            break;
        }
    }
    
    for(int i=1;i<=B-1;i++){
        for(;;){
            int x=rand()% 61;
            int y=20+rand()% 37;
            
            if(!(x>=27 && x<=33 && y>=1 && y<=59) && sixSquareRadiusChecker(x,y)){
                for(int i=x;i<=x+3;i++){
                    for(int k=y;k<=y+3;k++){
                        earthInvisible(i, k); //Ensure that there is not any Earth overlapping the 4x4 square region of each Boulder
                    }
                }
                actorList.push_back(new Boulder(x,y,this));
                break;
            }
        }
    }

    
    //Randomly-distribute Gold Nuggets in oil field
    for(int i=1;i<=G;i++){
        for(;;){
            int x=rand()% 61;
            int y=rand()%57;
            
            if(!(x>=27 && x<=33 && y>=1 && y<=59) && sixSquareRadiusChecker(x,y)){
                actorList.push_back(new GoldNugget(x,y,this,false,true,false));
                break;
            }
        }
    }
    
    
    //Randomly-distribute Barrels in oil field
    for(int i=1;i<=barrelsLeft;i++){
        for(;;){
        int x=rand()%61;
        int y=rand()%57;
        if(!(x>=27 && x<=33 && y>=1 && y<=59) && sixSquareRadiusChecker(x,y)){
                actorList.push_back(new Barrel(x,y,this));
                break;
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME; //MUST KEEP THIS!!
}

//Returns true if there's no Barrel, Gold Nugget or Boulder in a six-square radius around (x,y)
bool StudentWorld::sixSquareRadiusChecker(int x, int y){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isInitiallyDistributedGoodie() && sqrt(pow(x-(*itr)->getX(), 2)+pow(y-(*itr)->getY(), 2))<=6){
            return false;
        }
    }
    return true;
}

void StudentWorld::decreaseProtestorCount(){
    protestorCount--;
}

double StudentWorld::computeDistance(Actor* a, Actor* b){
    double sum=pow(a->getX()-b->getX(),2)+pow(a->getY()-b->getY(), 2);
    return sqrt(sum);
}

void StudentWorld::setDisplayText(){
    int level = getLevel();
    int lives = getLives();
    int health = getTunnelManHitPoints()*10;
    int squirts = getTunnelManSquirtAmount();
    int gold = getTunnelManGoldAmount();
    int barrelsLeft = this->barrelsLeft;
    int sonar = getTunnelManSonarAmount();
    int score = getScore();

    ostringstream oss;
    oss<<"Lvl: "<<setw(2)<<level<<"  ";
    oss<<"Lives: "<<setw(1)<<lives<<"  ";
    oss<<"Hlth: "<<setw(3)<<health<<"%"<<"  ";
    oss<<"Wtr: "<<setw(2)<<squirts<<"  ";
    oss<<"Gld: "<<setw(2)<<gold<<"  ";
    oss<<"Oil Left: "<<setw(2)<<barrelsLeft<<"  ";
    oss<<"Sonar: "<<setw(2)<<sonar<<"  ";
    oss.fill('0');
    oss<<"Scr: "<<setw(6)<<score;
    string s=oss.str();
    setGameStatText(s);
}
           
int StudentWorld::move()
{// Update the Game Status Line
    setDisplayText(); // update the score/lives/level text at screen top
    // The term “Actors” refers to all Protesters, the player, Goodies, // Boulders, Barrels of oil, Holes, Squirts, the Exit, etc.
    // Give each Actor a chance to do something
    
    if(protestorTicksToWait==0 && protestorCount<targetProtestors){
        int num=1+rand()%100;
        if(num>=1 && num<=probabilityOfHardcore){
            actorList.push_back(new HardcoreProtestor(this));
        }else{
            actorList.push_back(new RegularProtestor(this));
        }
        protestorCount++;
        protestorTicksToWait=protestorTicks;
    }
    else{
        protestorTicksToWait--;
    }
    
    int num=1+rand()%probabilityOfGoodie;
    if(num==1){
        int i=1+rand()%5;
        if(i==1){
            actorList.push_back(new SonarKit(0,60,this));
        }else{
            for(;;){
                int x=rand()%61;
                int y=rand()%61;
                if(!tmCheckFourbyFour(x, y)){
                    actorList.push_back(new Water(x,y,this));
                    break;
                }
            }
        }
    }
    
    tm->doSomething();
    if(!tm->getAlive() || tm->getHitPoints()==0){
        return GWSTATUS_PLAYER_DIED;
    }
    if(barrelsLeft==0){
        GameController::getInstance().playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->getAlive()){
            (*itr)->doSomething();
            if(!tm->getAlive()){
                return GWSTATUS_PLAYER_DIED;
            }
            if(barrelsLeft==0){
                GameController::getInstance().playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    // Remove newly-dead actors after each tick
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if(!((*itr)->getAlive())){
            delete *itr;
            actorList.erase(itr);
        }
    }
    if(!tm->getAlive()){
        return GWSTATUS_PLAYER_DIED;
    }
    
    // If the player has collected all of the Barrels on the level, then
    // return the result that the player finished the level
    if(barrelsLeft==0){
        GameController::getInstance().playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    // the player hasn’t completed the current level and hasn’t died
    // let them continue playing the current level
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for(int i=0;i<=63;i++){
        for(int k=0;k<=59;k++){
            delete earthArray[i][k]; //Delete all Earth objects
        }
    }
    delete tm; //Delete TunnelMan object
    
    //Delete all remaining Actors:
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        delete *itr;
        actorList.erase(itr);
    }
    
    if(25>=200-getLevel()){
        protestorTicks=25;
    }else{
        protestorTicks=200-getLevel();
    }
    
    protestorTicksToWait=0;
    
    if(15<=2+getLevel()*1.5){
        targetProtestors=15;
    }else{
        targetProtestors=2+getLevel()*1.5;
    }
    protestorCount=0;
    
    if(90<=getLevel()*10+30){
        probabilityOfHardcore=90;
    }else{
        probabilityOfHardcore=getLevel()*10+30;
    }
    
    probabilityOfGoodie=getLevel()*25+300;
    
    while(getLives()<3){
        incLives();
    }
}

StudentWorld::~StudentWorld(){}

void StudentWorld::earthInvisible(int col, int row){
    earthArray[col][row]->setVisible(false);
    earthArray[col][row]->setActorVisible(false);
}

bool StudentWorld::getEarthVisibility(int col, int row){
    if(col<0 || col>63 || row<0 || row>59){
        return false;
    }
    return earthArray[col][row]->getActorVisible();
}

bool StudentWorld::tmCheckFourbyFour(int x, int y){
    for(int i=x;i<=x+3;i++){
        for(int k=y;k<=y+3;k++){
            if(i>=0 && i<=63 && k>=0 && k<=59 && getEarthVisibility(i, k)){
                return true;
            }
        }
    }
    return false;
}


double StudentWorld::computeDistanceToTM(Actor* actor){
    return sqrt(pow(actor->getX()-tm->getX(),2)+pow(actor->getY()-tm->getY(), 2));
}

//Decrement level's Barrel count by one when player picks up Barrel
void StudentWorld::subtractBarrel(){
    barrelsLeft--;
}

void StudentWorld::addToList(Actor* actor){
    actorList.push_back(actor);
}

//Returns true if there is no Boulder within radius of 3.0 from (x,y)
bool StudentWorld::tunnelManBoulderChecker(int x, int y){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isBoulder() && sqrt(pow(x-(*itr)->getX(), 2)+pow(y-(*itr)->getY(), 2))<=3.0){
            return false;
        }
    }
    return true;
}

//Makes visible all hidden game objects that are within radius of 12 from (x,y)
void StudentWorld::tunnelManSonarChecker(int x, int y){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isInitiallyDistributedGoodie() && sqrt(pow(x-(*itr)->getX(), 2)+pow(y-(*itr)->getY(), 2))<=12){
            (*itr)->setVisible(true);
            (*itr)->setActorVisible(true);
        }
    }
}

bool StudentWorld::isBoulder(int x, int y){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isBoulder() && x==(*itr)->getX() && y==(*itr)->getY()){
            return true;
        }
    }
    return false;
}

bool StudentWorld::isAnnoyable(int x, int y){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isAnnoyable() && x==(*itr)->getX() && y==(*itr)->getY()){
            return true;
        }
    }
    return false;
}

/*If a Squirt is within a radius of 3.0 of one or more Protesters (up to and including a distance of 3.0 squares away), it will cause 2 points of annoyance to these Protester(s), and then immediately set its state to dead, so it can be removed from the oil field at the end of the tick.*/
void StudentWorld::squirtAssistant(Squirt* s){
    bool annoyedProtestor=false;
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isAnnoyable() && computeDistance(s, *itr)<=3.0){
            dynamic_cast<Protestor*>(*itr)->annoyProtestor(2, false, true);
            annoyedProtestor=true;
        }
    }
    if(annoyedProtestor){
        s->setAlive(false);
    }
}


//Bonks any Protester within radius of 3 of Boulder
void StudentWorld::boulderBonker(Boulder *b){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isAnnoyable() && computeDistance(b, *itr)<=3.0){
            (*itr)->setAlive(false);
            increaseScore(500);
            dynamic_cast<Protestor*>(*itr)->annoyProtestor(100, true, false);
        }
    }
}

//Returns pointer to a Protestor (if any) within radius of 3 from Gold Nugget at (x,y)
Protestor* StudentWorld::goldNuggetProtesterFinder(GoldNugget* g){
    for(list<Actor*>::iterator itr=actorList.begin();itr!=actorList.end();itr++){
        if((*itr)->isAnnoyable() && computeDistance(g, *itr)<=3.0){
            return dynamic_cast<Protestor*>(*itr);
        }
    }
    return nullptr;
}

void StudentWorld::annoyTunnelMan(int val){
    tm->setHitPoints(getTunnelManHitPoints()-val);
    if(getTunnelManHitPoints()<=0){
        tm->setAlive(false);
        decLives();
        GameController::getInstance().playSound(SOUND_PLAYER_GIVE_UP);
    }
}

//Increment TunnelMan's gold amount by "val"
void StudentWorld::changeGoldAmount(int val){
    tm->changeGoldNuggets(val);
}

//Increment TunnelMan's sonar kit amount by "val"
void StudentWorld::changeSonarAmount(int val){
    tm->changeSonarCharges(val);
}

//Increment TunnelMan's water squirt amount by "val"
void StudentWorld::changeSquirtAmount(int val){
    tm->changeSquirts(val);
}


int StudentWorld::getTunnelManHitPoints(){
    return tm->getHitPoints();
}

int StudentWorld::getTunnelManGoldAmount(){
    return tm->getGold();
}

int StudentWorld::getTunnelManSonarAmount(){
    return tm->getSonar();
}

int StudentWorld::getTunnelManSquirtAmount(){
    return tm->getSquirts();
}

int StudentWorld::getTunnelManXCoord(){
    return tm->getX();
}

int StudentWorld::getTunnelManYCoord(){
    return tm->getY();
}

void StudentWorld::killTunnelMan(){
    tm->setAlive(false);
}
