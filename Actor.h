
#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int depth);

    // Action to perform for each tick.
    virtual void doSomething() = 0;

    // Is this actor dead?
    bool isDead() const;

    // Mark this actor as dead.
    void setDead();

    

    // Get this actor's vertical speed.
    double getVerticalSpeed() const;

    // Set this actor's vertical speed.
    void setVerticalSpeed(double speed);

    // If this actor is affected by holy water projectiles, then inflict that
    // affect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate();

    // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const;

    
protected:
    // Get this actor's world
    StudentWorld* world() const;
    //is the actor a pedestrian?
    virtual bool isPedestrian() const;
    // Adjust the x coordinate by dx to move to a position with a y coordinate
    // determined by this actor's vertical speed relative to GhostRacser's
    // vertical speed.  Return true if the new position is within the view;
    // otherwise, return false, with the actor dead.
    virtual bool moveRelativeToGhostRacerVerticalSpeed(double dx);
private:
    bool m_isAlive;
    StudentWorld* m_SW;
    double m_vSpeed;
};

class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, int imageID, double x, double y);
    virtual void doSomething();
private:
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp);
    //is an agent collision avoidance worthy
    virtual bool isCollisionAvoidanceWorthy() const;

    // Get hit points.
    int getHP() const;

    // Increase hit points by hp.
    void increaseHP(int hp);

    // Do what the spec says happens when hp units of damage is inflicted.
    // Return true if this agent dies as a result, otherwise false.
    virtual bool takeDamageAndPossiblyDie(int hp);

    // What sound should play when this agent is damaged but does not die?
    virtual int soundWhenHurt();

    
protected:
    //gets movement plan distance
    int getMovementPlanDist() const;

    //sets movement plan distance
    void setMovementPlanDist(int d);

    // Get the pedestrian's horizontal speed
    int getHorizSpeed() const;

    // Set the pedestrian's horizontal speed
    void setHorizSpeed(int s);

    // What sound should play when this agent is damaged and dies?
    virtual int soundWhenDie();
    
private:
    int m_hp;
    int m_movementPlanDistance;
    int m_horizSpeed;
};

class GhostRacer : public Agent
{
public:
    GhostRacer(StudentWorld* sw, double x = 128, double y = 32);
    virtual void doSomething();
    
    //returns death sound for ghost racer
    virtual int soundWhenDie() const;

    // How many holy water projectiles does the object have?
    int getNumSprays() const;

    // Increase the number of holy water projectiles the object has.
    void increaseSprays(int amt);

    // Spin as a result of hitting an oil slick.
    void spin();
protected:

private:
    int holyWaterAmmo;
};

class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
    
    //returns pedestrian's hurt sound
    virtual int soundWhenHurt() const;

    //returns pedestrian's death sound
    virtual int soundWhenDie() const;

    //returns that this object is a pedestrian
    virtual bool isPedestrian() const;


    virtual void doSomething();

    

protected:
    // Move the pedestrian.  If the pedestrian doesn't go off screen and
    // should pick a new movement plan, pick a new plan.
    void moveAndPossiblyPickPlan();
    //do pedestrian's specific activity
    virtual void doActivity();
};

class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    virtual bool beSprayedIfAppropriate();
    virtual bool takeDamageAndPossiblyDie(int hp);
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    virtual void doActivity();
    virtual bool beSprayedIfAppropriate();
private:
    int ticksUntilNextGrunt;
};


class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
private:
    bool hasDamagedGR;
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual bool isProjectile() const;
    virtual void doSomething();
private:
    int amtMoved;
};


class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    virtual bool beSprayedIfAppropriate();

    virtual void doSomething();

    // Do the object's special activity (increase health, spin Ghostracer, etc.)
    virtual void doActivity(GhostRacer* gr) = 0;

    // Return the object's increase to the score when activated.
    virtual int getScoreIncrease() const = 0;

    // Return the sound to be played when the object is activated.
    virtual int getSound() const;

    

protected:
    // Return whether the object dies after activation.
    virtual bool selfDestructs() const = 0;

    // Return whether the object is affected by a holy water projectile.
    virtual bool isSprayable() const = 0;
};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    //virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual int getSound() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    //virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    //virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void doActivity(GhostRacer* gr);
    virtual int getScoreIncrease() const;
    virtual int getSound() const;
    virtual bool selfDestructs() const;
    virtual bool isSprayable() const;
};

#endif // ACTOR_INCLUDED
