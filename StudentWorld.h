
#ifndef STUDENTWORLD_INCLUDED
#define STUDENTWORLD_INCLUDED

#include "GameWorld.h"
#include <string>
#include <vector>
#include <cmath>
class Actor;
class GhostRacer;
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // Return a pointer to the world's GhostRacer.
    GhostRacer* getGhostRacer();

    // Add an actor to the world.
    void addActor(Actor* a);

    // Record that a soul was saved.
    void recordSoulSaved();


    // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor* a1, const Actor* a2) const;



    Actor* findCWA(double x, double y);


    bool anyOverlapsWithSpray(Spray* s);
private:
    std::vector<Actor*> gameObjects;
    GhostRacer* m_GhostRacer;
    double lastY;
    int soulsSaved;
    int bonusPoints;
};

#endif // STUDENTWORLD_INCLUDED