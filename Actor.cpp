#define _USE_MATH_DEFINES
#include <cmath>
#include "Actor.h"
#include "StudentWorld.h"


//Actor
Actor::Actor(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int depth) : GraphObject(imageID, x, y, dir, size, depth), m_isAlive(true), m_SW(sw), m_vSpeed(-4) {
}
bool Actor::isDead() const{
	return !m_isAlive;
}
void Actor::setDead() {
	m_isAlive = false;
}
bool Actor::isPedestrian() const {
	return false;
}

StudentWorld* Actor::world() const {
	return m_SW;
}
double Actor::getVerticalSpeed() const{
	return m_vSpeed;
}
void Actor::setVerticalSpeed(double speed) {
	m_vSpeed = speed;
}
bool Actor::beSprayedIfAppropriate() {
	return false;
}
bool Actor::isCollisionAvoidanceWorthy() const {
	return false;
}
bool Actor::moveRelativeToGhostRacerVerticalSpeed(double dx) {
	int vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
	int horiz_speed = dx;
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);
	if (!(getX() < VIEW_WIDTH && getX() > 0 && getY() > 0 && getY() < VIEW_HEIGHT)) {
		setDead();
		return false;
	}
	return true;
}

//BorderLine
BorderLine::BorderLine(StudentWorld* sw, int imageID, double x, double y) : Actor(sw, imageID, x, y, 2.0, 0, 2){		
}
void BorderLine::doSomething() {
	if (moveRelativeToGhostRacerVerticalSpeed(0)) {
		if (world()->overlaps(this, world()->getGhostRacer())) {
			//checks left/right border, then sends the racer the other way, and has the racer take 10 damage
			if (getX() == LEFT_EDGE) {
				if (world()->getGhostRacer()->getDirection() > 90) {
					world()->getGhostRacer()->takeDamageAndPossiblyDie(10);
					
				}
				world()->getGhostRacer()->setDirection(82);
				world()->playSound(world()->getGhostRacer()->soundWhenHurt());

			}
			else if (getX() == RIGHT_EDGE) {
				if (world()->getGhostRacer()->getDirection() < 90) {
					world()->getGhostRacer()->takeDamageAndPossiblyDie(10);
				}
				world()->getGhostRacer()->setDirection(98);
				world()->playSound(world()->getGhostRacer()->soundWhenHurt());
			}
		}
	}
}

//Agent
Agent::Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp) : Actor(sw, imageID, x, y, size, dir, 0), m_hp(hp), m_movementPlanDistance(0), m_horizSpeed(0){
}
bool Agent::isCollisionAvoidanceWorthy() const{
	return true;
}
int Agent::getMovementPlanDist() const{
	return m_movementPlanDistance;
}
void Agent::setMovementPlanDist(int d) {
	m_movementPlanDistance = d;
}
int Agent::getHorizSpeed() const {
	return m_horizSpeed;
}
void Agent::setHorizSpeed(int s) {
	m_horizSpeed = s;
}
int Agent::getHP() const {
	return m_hp;
}
void Agent::increaseHP(int hp) {
	m_hp += hp;
}
bool Agent::takeDamageAndPossiblyDie(int hp) {
	m_hp -= hp;
	if (m_hp <= 0) {
		setDead();
		world()->playSound(soundWhenDie());
		//only checks isPedestrian because a human ped's takeDamageAndPossiblyDie just causes the player to lose a life/restart
		if (isPedestrian()) {
			world()->increaseScore(150);
		}
		else {
			//is a zombie cab
			world()->increaseScore(200);
		}
		return true;
	}
	world()->playSound(soundWhenHurt());
	return false;
}
int Agent::soundWhenHurt() {
	return SOUND_VEHICLE_CRASH;
}
int Agent::soundWhenDie() {
	return SOUND_VEHICLE_DIE;
}

//GhostRacer
GhostRacer::GhostRacer(StudentWorld* sw, double x, double y) : Agent(sw, IID_GHOST_RACER, x, y, 4, 90, 100), holyWaterAmmo(10) {
	setVerticalSpeed(0);
}
void GhostRacer::doSomething() {
	if (isDead())
		return;
	int key;

	//takes key input and adjusts accordingly
	if (world()->getKey(key)) {
		if (key == KEY_PRESS_SPACE && holyWaterAmmo >= 1) {
			world()->playSound(SOUND_PLAYER_SPRAY);
			world()->addActor(new Spray(world(), getX(), getY(), getDirection()));
			holyWaterAmmo--;
		}
		else if (key == KEY_PRESS_LEFT && getDirection() < 114) {
			setDirection(getDirection() + 8);
		}
		else if (key == KEY_PRESS_RIGHT && getDirection() > 66) {
			setDirection(getDirection() - 8);
		}
		else if (key == KEY_PRESS_UP) {
			if (getVerticalSpeed() < 5)
				setVerticalSpeed(getVerticalSpeed() + 1);
		}
		else if (key == KEY_PRESS_DOWN) {
			if (getVerticalSpeed() > -1)
				setVerticalSpeed(getVerticalSpeed()-1);
		}
	}

	double max_shift_per_tick = 4.0;
	int direction = getDirection();
	double delta_x = cos(direction * M_PI / 180) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);
}
int GhostRacer::soundWhenDie() const {
	return SOUND_PLAYER_DIE;
}
int GhostRacer::getNumSprays() const {
	return holyWaterAmmo;
}
void GhostRacer::increaseSprays(int amt) {
	holyWaterAmmo += amt;
}
void GhostRacer::spin() {
	int dirChange = randInt(5, 20);
	//random choice of clockwise or counterclockwise
	int CWorCCW = randInt(1, 2);
	if (CWorCCW == 1) {
		//if GhostRacer goes beyond 60 or 120, set to 60 or 120
		if (getDirection() - dirChange < 60) {
			setDirection(60);
		}
		else {
			setDirection(getDirection() - dirChange);
		}
	}
	else {
		if (getDirection() + dirChange > 120) {
			setDirection(120);
		}
		else {
			setDirection(getDirection() + dirChange);
		}
	}
}

//Pedestrian
Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size) : Agent(sw, imageID, x, y, size, 0, 2) {
}
int Pedestrian::soundWhenHurt() const{
	return SOUND_PED_HURT;
}
int Pedestrian::soundWhenDie() const {
	return SOUND_PED_DIE;
}
bool Pedestrian::isPedestrian() const {
	return true;
}

void Pedestrian::moveAndPossiblyPickPlan() {
	setMovementPlanDist(getMovementPlanDist() - 1);
	if (getMovementPlanDist() > 0) {
		return;
	}
	else {
		//randomly chooses if there will be a positive or negative horizontal speed, since 0 is not allowed
		int posOrNeg = randInt(1, 2);
		if (posOrNeg == 1) {
			setHorizSpeed(randInt(-3, -1));
		}
		else {
			setHorizSpeed(randInt(1, 3));
		}
		setMovementPlanDist(randInt(4, 32));
		if (getHorizSpeed() < 0) {
			setDirection(180);
		}
		else if (getHorizSpeed() > 0) {
			setDirection(0);
		}
	}
}
void Pedestrian::doSomething() {
	if (isDead())
		return;
	if (world()->overlaps(this, world()->getGhostRacer())) {
		//since hitting a human ped causes the level to restart, it doesn't matter that the ghost racer takes 5 damage
		world()->getGhostRacer()->takeDamageAndPossiblyDie(5);
		takeDamageAndPossiblyDie(2);
		return;
	}
	doActivity();
	if(moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed()))
		moveAndPossiblyPickPlan();
}
void Pedestrian::doActivity() {
	return;
}

//Human Ped
HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y) : Pedestrian(sw, IID_HUMAN_PED, x, y, 2.0){
}
bool HumanPedestrian::beSprayedIfAppropriate() {
	setHorizSpeed(getHorizSpeed() * -1);
	if (getDirection() == 0) {
		setDirection(180);
	}
	else if(getDirection() == 180){
		setDirection(0);
	}
	world()->playSound(soundWhenHurt());
	return true;
}
bool HumanPedestrian::takeDamageAndPossiblyDie(int hp) {
	world()->playSound(soundWhenDie());
	world()->getGhostRacer()->setDead();
	return true;
}

//Zombie Ped
ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y) : Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3.0), ticksUntilNextGrunt(0) {
}
void ZombiePedestrian::doActivity() {
	if (isDead())
		return;
	if (abs(getX() - world()->getGhostRacer()->getX()) <= 30 && getY() > world()->getGhostRacer()->getY()) {
		setDirection(270);
		if (getX() < world()->getGhostRacer()->getX()) {
			setHorizSpeed(1);
		}
		else if (getX() > world()->getGhostRacer()->getX()) {
			setHorizSpeed(-1);
		}
		else {
			setHorizSpeed(0);
		}
		ticksUntilNextGrunt--;
		if (ticksUntilNextGrunt <= 0) {
			world()->playSound(SOUND_ZOMBIE_ATTACK);
			ticksUntilNextGrunt = 20;
		}
	}
}
bool ZombiePedestrian::beSprayedIfAppropriate() {
	if (takeDamageAndPossiblyDie(1)) {
		int chanceSpawnHealingGoodie = randInt(1, 5);
		if (chanceSpawnHealingGoodie == 1) {
			world()->addActor(new HealingGoodie(world(), getX(), getY()));
		}
	}
	return true;
}

//Zombie Cab
ZombieCab::ZombieCab(StudentWorld* sw, double x, double y) : Agent(sw, IID_ZOMBIE_CAB, x, y, 4.0,90,3), hasDamagedGR(false){
}
void ZombieCab::doSomething() {
	if (isDead())
		return;
	if (world()->overlaps(this, world()->getGhostRacer())) {
		if (!hasDamagedGR) {
			world()->playSound(soundWhenHurt());
			world()->getGhostRacer()->takeDamageAndPossiblyDie(20);
			if (getX() <= world()->getGhostRacer()->getX()) {
				setHorizSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else {
				setHorizSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			hasDamagedGR = true;
		}
	}
	if (moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed())) {
		Actor* CWA = world()->findCWA(getX(), getY());
		if (getVerticalSpeed() > world()->getGhostRacer()->getVerticalSpeed() && CWA != nullptr) {
			if (CWA->getY() - getY() < 96 && CWA->getY() - getY() > 0) {
				setVerticalSpeed(getVerticalSpeed() - .5);
				return;
			}
		}
		else if (getVerticalSpeed() <= world()->getGhostRacer()->getVerticalSpeed() && CWA != nullptr && CWA != world()->getGhostRacer()) {
			if (getY() - CWA->getY() < 96 && getY() - CWA->getY() > 0) {
				setVerticalSpeed(getVerticalSpeed() + .5);
				return;
			}
		}

		setMovementPlanDist(getMovementPlanDist() - 1);
		if (getMovementPlanDist() > 0) {
			return;
		}
		else {
			setMovementPlanDist(randInt(4, 32));
			setVerticalSpeed(getVerticalSpeed() + randInt(-2, 2));
		}
	}
}
bool ZombieCab::beSprayedIfAppropriate() {
	if (takeDamageAndPossiblyDie(1)) {
		int chanceSpawnOilSlick = randInt(1, 5);
		if (chanceSpawnOilSlick == 1) {
			world()->addActor(new OilSlick(world(), getX(), getY()));
		}
	}
	return true;
}

//Spray
Spray::Spray(StudentWorld* sw, double x, double y, int dir) :Actor(sw, IID_HOLY_WATER_PROJECTILE,x,y, 1.0,dir, 1), amtMoved(0){
}
bool Spray::isProjectile() const{
	return true;
}
void Spray::doSomething() {
	if (isDead())
		return;
	if (world()->anyOverlapsWithSpray(this)) {
		setDead();
		return;
	}
	moveForward(SPRITE_HEIGHT);
	amtMoved += SPRITE_HEIGHT;
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT || amtMoved >= 160) {
		setDead();
	}
}

//GhostRacerActivatedObject
GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir) : Actor(sw, imageID, x, y, size, dir, 2){
}
bool GhostRacerActivatedObject::beSprayedIfAppropriate() {
	if (isSprayable()) {
		setDead();
		return true;
	}
	return false;
}
void GhostRacerActivatedObject::doSomething() {
	if (moveRelativeToGhostRacerVerticalSpeed(0)) {
		if (world()->overlaps(this, world()->getGhostRacer())) {
			world()->playSound(getSound());
			doActivity(world()->getGhostRacer());
			world()->increaseScore(getScoreIncrease());
			if (selfDestructs())
				setDead();
		}
		
	}
}
int GhostRacerActivatedObject::getSound() const{
	return SOUND_GOT_GOODIE;
}

//OilSlick
OilSlick::OilSlick(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(2,5), 0){
}
void OilSlick::doActivity(GhostRacer* gr) {
	gr->spin();
}
int OilSlick::getScoreIncrease() const {
	return 0;
}
int OilSlick::getSound() const {
	return SOUND_OIL_SLICK;
}
bool OilSlick::selfDestructs() const{
	return false;
}
bool OilSlick::isSprayable() const {
	return false;
}

//HealingGoodie
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1, 0){
}
void HealingGoodie::doActivity(GhostRacer* gr) {
	if (gr->getHP() <= 90) {
		gr->increaseHP(10);
	}
	else {
		gr->increaseHP(100 - gr->getHP());
	}
}
int HealingGoodie::getScoreIncrease() const{
	return 250;
}
bool HealingGoodie::selfDestructs() const{
	return true;
}
bool HealingGoodie::isSprayable() const {
	return true;
}

//HolyWaterGoodie
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2, 90){
}
void HolyWaterGoodie::doActivity(GhostRacer* gr) {
	gr->increaseSprays(10);
}
int HolyWaterGoodie::getScoreIncrease() const {
	return 50;
}
bool HolyWaterGoodie::selfDestructs() const{
	return true;
}
bool HolyWaterGoodie::isSprayable() const {
	return true;
}

//SoulGoodie
SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y) : GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4, 0) {
}
void SoulGoodie::doSomething() {
	GhostRacerActivatedObject::doSomething();
	setDirection(getDirection() - 10);
}
void SoulGoodie::doActivity(GhostRacer* gr) {
	world()->recordSoulSaved();
}
int SoulGoodie::getScoreIncrease() const {
	return 100;
}
int SoulGoodie::getSound() const {
	return SOUND_GOT_SOUL;
}
bool SoulGoodie::selfDestructs() const {
	return true;
}
bool SoulGoodie::isSprayable() const {
	return false;
}


