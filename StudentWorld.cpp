#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetDir) : GameWorld(assetDir), m_GhostRacer(nullptr), lastY(0), soulsSaved(0), bonusPoints(5000){
}
StudentWorld::~StudentWorld() {
    cleanUp();
}
int StudentWorld::init() {
    soulsSaved = 0;
    bonusPoints = 5000;
    m_GhostRacer = new GhostRacer(this);
    for (int i = 0; i < VIEW_HEIGHT / SPRITE_HEIGHT; i++) {
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, 1.0 * i * SPRITE_HEIGHT));
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, 1.0 * i * SPRITE_HEIGHT));
    }
    for (int i = 0; i < VIEW_HEIGHT / (4 * SPRITE_HEIGHT); i++) {
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3, 4.0 * i * SPRITE_HEIGHT));
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3, 4.0 * i * SPRITE_HEIGHT));
    }
    //most recently added object is a white border line
    vector<Actor*>::iterator i = gameObjects.end() - 1;
    lastY = (*i)->getY();
    return GWSTATUS_CONTINUE_GAME;
}
int StudentWorld::move() {
    if (bonusPoints > 0)
        bonusPoints--;
    vector<Actor*>::iterator i;
    for (i = gameObjects.begin(); i != gameObjects.end(); i++) {
        if (!(*i)->isDead()) {
            (*i)->doSomething();
            if (m_GhostRacer->isDead()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (soulsSaved == getLevel() * 2 + 5) {
                increaseScore(bonusPoints);
                return GWSTATUS_FINISHED_LEVEL;
            }

        }
    }
    m_GhostRacer->doSomething();
    lastY -= 4;
    lastY -= m_GhostRacer->getVerticalSpeed();
    for (i = gameObjects.begin(); i != gameObjects.end();) {
        if ((*i)->isDead()) {
            vector<Actor*>::iterator j = i;
            delete* j;
            i = gameObjects.erase(i);
        }
        else {
            i++;
        }
    }
    //add border lines
    double new_border_y = 1.0 * VIEW_HEIGHT - 1.0 * SPRITE_HEIGHT;
    double delta_y = new_border_y - lastY;
    if (delta_y >= SPRITE_HEIGHT) {
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));
    }
    if (delta_y >= 4.0 * SPRITE_HEIGHT) {
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3, new_border_y));
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3, new_border_y));
        vector<Actor*>::iterator j = gameObjects.end() - 1;
        lastY = (*j)->getY();
    }

    //add zombie cab
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int willSomethingHappen = randInt(0, ChanceVehicle - 1);
    int cur_lane = 0;
    double startY = 0;
    double intialVSpeed = 0;
    int chosenLane = 0;
    double spotInLane = 0;
    if (willSomethingHappen == 0) {
        cur_lane = randInt(1, 3);
        if (cur_lane == 1) {
            spotInLane = 1.0 * ROAD_CENTER - 1.0 * ROAD_WIDTH / 3.0;
        }
        else if (cur_lane == 2) {
            spotInLane = ROAD_CENTER;
        }
        else {
            spotInLane = 1.0 * ROAD_CENTER + 1.0 * ROAD_WIDTH / 3.0;
        }
        for (int i = 1; i <= 3; i++) {
            Actor* a = findCWA(spotInLane, 0);
            if (a == nullptr || a->getY() > VIEW_HEIGHT / 3) {
                startY = SPRITE_HEIGHT / 2;
                intialVSpeed = getGhostRacer()->getVerticalSpeed() + randInt(2, 4);
                chosenLane = cur_lane;
                break;
            }
            a = findCWA(spotInLane, VIEW_HEIGHT);
            if (a == nullptr || a->getY() < VIEW_HEIGHT * 2.0 / 3) {
                startY = 1.0 * VIEW_HEIGHT - 1.0 * SPRITE_HEIGHT / 2;
                intialVSpeed = getGhostRacer()->getVerticalSpeed() - randInt(2, 4);
                chosenLane = cur_lane;
                break;
            }
            //moves cur_lane to next lane
            if (cur_lane < 3) {
                cur_lane++;
            }
            else {
                cur_lane = 1;
            }
        }
        if (chosenLane == 1) {
            addActor(new ZombieCab(this, 1.0 * ROAD_CENTER - 1.0 * ROAD_WIDTH / 3.0, startY));
            vector<Actor*>::iterator lastGuy = gameObjects.end() - 1;
            (*lastGuy)->setVerticalSpeed(intialVSpeed);
        }
        else if (chosenLane == 2) {
            addActor(new ZombieCab(this, ROAD_CENTER, startY));
            vector<Actor*>::iterator lastGuy = gameObjects.end() - 1;
            (*lastGuy)->setVerticalSpeed(intialVSpeed);
        }
        else if (chosenLane == 3) {
            addActor(new ZombieCab(this, 1.0 * ROAD_CENTER + 1.0 * ROAD_WIDTH / 3.0, startY));
            vector<Actor*>::iterator lastGuy = gameObjects.end() - 1;
            (*lastGuy)->setVerticalSpeed(intialVSpeed);
        }
    }
    //add oil slick
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    willSomethingHappen = randInt(0, ChanceOilSlick-1);
    if (willSomethingHappen == 0) {
        addActor(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    
    //add zombie ped
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    willSomethingHappen = randInt(0, ChanceZombiePed - 1);
    if (willSomethingHappen == 0) {
        addActor(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    //add human ped
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    willSomethingHappen = randInt(0, ChanceHumanPed - 1);
    if (willSomethingHappen == 0) {
        addActor(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    //add holy water refill goodies
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    willSomethingHappen = randInt(0, ChanceOfHolyWater - 1);
    if (willSomethingHappen == 0) {
        addActor(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }

    //add lost soul
    int ChanceOfLostSoul = 100;
    willSomethingHappen = randInt(0, ChanceOfLostSoul - 1);
    if (willSomethingHappen == 0) {
        addActor(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }
    
    //update score
    ostringstream display;
    display << "Score : " << getScore() << "  Lvl: " << getLevel() << "  Souls2Save: " << getLevel() * 2 + 5 - soulsSaved << "  Lives: " << getLives() << "  Health: " << getGhostRacer()->getHP()
        << "  Sprays: " << getGhostRacer()->getNumSprays() << "  Bonus: " << bonusPoints << endl;
    string s = display.str();
    setGameStatText(s);
    return GWSTATUS_CONTINUE_GAME;
}
void StudentWorld::cleanUp() {
    //get rid of all game objects
    vector<Actor*>::iterator i;
    for (i = gameObjects.begin(); i != gameObjects.end();) {
        vector<Actor*>::iterator j = i;
        delete* j;
        i = gameObjects.erase(i);
    }
    delete m_GhostRacer;
}
GhostRacer* StudentWorld::getGhostRacer() {
    return m_GhostRacer;
}
void StudentWorld::addActor(Actor* a) {
    gameObjects.push_back(a);
}
void StudentWorld::recordSoulSaved() {
    soulsSaved++;
    
}
bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const{
    double delta_x = abs(a1->getX() - a2->getX());
    double delta_y = abs(a1->getY() - a2->getY());
    double radius_sum = a1->getRadius() + a2->getRadius();
    return delta_x < radius_sum * .25 && delta_y < radius_sum * .6;
}

Actor* StudentWorld::findCWA(double x, double y) {
    vector<Actor*>::iterator i = gameObjects.begin();
    Actor* nearest = nullptr;
    //checks the other actors to see if they match lanes with the input and stores the Actor* who is closest to the input
    for (; i != gameObjects.end(); i++) {
        //checks to see if the current game object is collision avoidance worthy 
        if ((*i)->isCollisionAvoidanceWorthy() && !((*i)->getX() == x  && (*i)->getY()==y)) {
            if (x > LEFT_EDGE && x < 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0) {
                if ((*i)->getX() >= LEFT_EDGE && (*i)->getX() < 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0) {
                    if (nearest == nullptr) {
                        nearest = (*i);
                    }
                    else {
                        if (abs((*i)->getY() - y) < abs(nearest->getY() - y))
                            nearest = (*i);
                    }
                }
            }
            else if (x >= 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0 && x < 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3) {
                if ((*i)->getX() > 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0 && (*i)->getX() < 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3) {
                    if (nearest == nullptr) {
                        nearest = (*i);
                    }
                    else {
                        if (abs((*i)->getY() - y) < abs(nearest->getY() - y))
                            nearest = (*i);
                    }
                }
            }
            else if(x >= 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3 && x < RIGHT_EDGE){
                if ((*i)->getX() >= 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3 && (*i)->getX() < RIGHT_EDGE) {
                    if (nearest == nullptr) {
                        nearest = (*i);
                    }
                    else {
                        if (abs((*i)->getY() - y) < abs(nearest->getY() - y))
                            nearest = (*i);
                    }
                }
            }
        }
    }
   //same as above, but with ghost racer because it is not located within the gameObjects vector
        if (x >= LEFT_EDGE && x < 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0) {
            if (getGhostRacer()->getX() > LEFT_EDGE && getGhostRacer()->getX() < 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0) {
                if (nearest == nullptr) {
                    nearest = getGhostRacer();
                }
                else {
                    if (abs(getGhostRacer()->getY() - y) < abs(nearest->getY() - y))
                        nearest = getGhostRacer();
                }
            }
        }
        else if (x >= 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0 && x < 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3) {
            if (getGhostRacer()->getX() > 1.0 * LEFT_EDGE + 1.0 * ROAD_WIDTH / 3.0 && getGhostRacer()->getX() < 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3) {
                if (nearest == nullptr) {
                    nearest = getGhostRacer();
                }
                else {
                    if (abs(getGhostRacer()->getY() - y) < abs(nearest->getY() - y))
                        nearest = getGhostRacer();
                }
            }
        }
        else  if (x >= 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3 && x < ROAD_WIDTH) {
            if (getGhostRacer()->getX() >= 1.0 * RIGHT_EDGE - 1.0 * ROAD_WIDTH / 3 && getGhostRacer()->getX() < RIGHT_EDGE) {
                if (nearest == nullptr) {
                    nearest = getGhostRacer();
                }
                else {
                    if (abs(getGhostRacer()->getY() - y) < abs(nearest->getY() - y))
                        nearest = getGhostRacer();
                }
            }
        }
   
    return nearest;
}


bool StudentWorld::anyOverlapsWithSpray(Spray* s) {
    //looks for any object hit by a spray and, if there is, makes the object get sprayed
    vector<Actor*>::iterator i;
    for (i = gameObjects.begin(); i != gameObjects.end(); i++) {
        if (overlaps(s, (*i))) {
            if ((*i)->beSprayedIfAppropriate())
                return true;
        }
    }
    return false;
}