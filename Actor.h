#ifndef ACTOR_H_
#define ACTOR_H_

//
//  Actor.h
//
//  Created by Jaehyeong Lee on 8/23/18.
//  Copyright (c) 2018 Jaehyeong Lee. All rights reserved.
//

#include "GraphObject.h"
#include "GameConstants.h"

#include <vector>

class StudentWorld;
class TunnelMan;
class Boulder;
class Protester;

// base class for gold nuggets, sonar, etc
// abstract class 

class GameObject : public GraphObject
{
public:
	GameObject(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~GameObject() {};

	enum Current_State		{ TIME_UP, REST_STATE };
	enum Sound				{ SOUND_OFF, SOUND_ON };
	enum Actors				{ R_PROTESTER, H_PROTESTER, PLAYER, EARTH };

	// SETTER FUNCTIONS //

	void	setTemporState(int T)				{ temporState = T; }
	void	setWorld(StudentWorld * game_addr)	{ Game = game_addr; };
	void	setHitPoints(int P)					{ hitpts = P; };
	void	setDead()							{ alive = false; };
	void	Turn_Off_Sound()					{ Switch = SOUND_OFF; };
	void	Turn_On_Sound()						{ Switch = SOUND_ON; };

	// GETTER FUNCTIONS //

	bool	isAlive() const						{ return alive; };
	int		getHitPoints() const				{ return hitpts; };
	Sound	getSwitch() const					{ return Switch; };
	int	    getTemporState() const				{ return temporState; };
	StudentWorld * getWorld() const				{ return Game; };


	// INCREASE AND DECREASE STATS //

	void	 decreTemporState();
	void	 decreHitPoints(int P) { hitpts -= P; };

private:
	Sound Switch = SOUND_OFF;
	int temporState = REST_STATE;
	bool alive = true;
	int hitpts;
	StudentWorld * Game;
}; 


class Goodies : public GameObject
{
public:
	
	Goodies(int imageID, int startX, int startY, Direction dir,
		double size, unsigned int depth);	
	virtual ~Goodies() {};

	// GETTER FUNCTIONS //

	Actors getOwnership() const		{ return whoPickedUp; };

	// SETTER FUNCTIONS //

	void setOwnership(Actors actor);
	virtual void doSomething(TunnelMan * player) = 0;


private:
	Actors whoPickedUp = EARTH;
};

class Golds : public Goodies
{
																			
public:
	Golds(int startX, int startY, bool setting, Actors owner);
	virtual ~Golds() {};

	void doSomething(TunnelMan * player);

};

class Sonar : public Goodies
{
public:
	Sonar(int currLevel);
	virtual ~Sonar() {};

	void doSomething(TunnelMan * player);

};

class Squirt : public GameObject
{
public:
	Squirt(int col, int row, Direction dir);
	virtual ~Squirt() {};

	// SETTER FUNCTION //

	void doSomething(TunnelMan * player);

private:
	void move(TunnelMan * player);
	int distanceTravelled = 0;

};


class Water : public Goodies
{
public:
	Water(int startX, int startY, int currLevel);
	virtual ~Water() {};

	void doSomething(TunnelMan * player);
};

class BarrelOil : public Goodies
{
public:
	BarrelOil(int startX, int startY);
	virtual ~BarrelOil() {};

	void doSomething(TunnelMan * player);
};


class Earth : public GraphObject
{
public:
	Earth(int startX, int startY) : 
		GraphObject(TID_EARTH, startX, startY, right, 0.25, 3) { setVisible(true); };
	virtual ~Earth() {};
};


class Boulder : public GameObject
{
public:
	Boulder(int startX,	int startY);
	virtual ~Boulder() {};

	void setFallingState(bool T)		{ fallingState = T; };
	bool getFallingState() const		{ return fallingState; };

	void doSomething(TunnelMan * player);
private:
	bool fallingState = false;

};


class Protester : public GameObject
{
public:
	Protester(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~Protester() {};
	virtual void doSomething(TunnelMan * player);

	// SETTERS //

	void	setPathToExit(std::string path)					{ pathToExit = path; };
	void	setTicksToWaitBetweenMoves(int T)				{ TicksToWaitBetweenMoves = T; };
	void	setTicksToWaitBetweenTurns(int T)				{ TicksToWaitBetweenTurns = T; };
	void	setNumSquaresToMoveInCurrentDirection(int T)	{ numSquaresToMoveInCurrentDirection = T; };
	void	setShoutingInterval(int T)						{ shouting_interval = T; };
	void	setToLeaveOilFieldState()						{ leaveOilField = true; };
	void	setTicksSincePerpenMoves(int T)				    { TicksSincePerpenMoves = T; };


	// GETTERS //

	std::string& getPathToExit()							{ return pathToExit; };
	int		 getTicksToWaitBetweenMoves() const				{ return TicksToWaitBetweenMoves; };
	int		 getTicksToWaitBetweenTurns() const				{ return TicksToWaitBetweenTurns; };
	int		 getShoutingInterval() const					{ return shouting_interval; };
	int		 getNumSquaresToMoveInCurrentDirection() const	{ return numSquaresToMoveInCurrentDirection; };
	int		 getTicksSincePerpenMoves() const			    { return TicksSincePerpenMoves; };
	bool	 leaveOilFieldState() const						{ return leaveOilField; };

	// INCREASE && DECREASE //
	void	 decreTicksToWaitBetweenMoves()					{ TicksToWaitBetweenMoves--; };
	void   	 decreNumSquaresToMoveInCurrentDirection()	    { if (numSquaresToMoveInCurrentDirection > 0) numSquaresToMoveInCurrentDirection--; }
	void	 decreTicksSincePerpenMoves()				    { if (TicksSincePerpenMoves > 0) TicksSincePerpenMoves--; };
	void	 decreShoutingTime()							{ if (shouting_interval > 0) shouting_interval--; };

private:

	// A.I. //

	void	GoToExit(TunnelMan * player);
	void	GoToPerpenPath(TunnelMan * player);
	void	GoToCurrDirection(TunnelMan * player);
	void	pickNewDirection(TunnelMan * player);
	bool	update(TunnelMan * player);

	// TOOLS TO FIND EXIT //

	int direction[4][2] = { {1,0},{ -1,0 }, {0,1}, {0,-1} };
	std::string dir_path[4] = { "E", "W", "N", "S" };
	std::string pathToExit;
	char maze[64][64];

	// HELPER FUNCTIONS //

	bool	checkForObstacles(int col, int row, Direction dir, TunnelMan * player) const;
	bool	canSeePlayer(int col, int row, TunnelMan * player);

	void	checkPerpenPath(int m_col, int m_row, Direction currDir,
			TunnelMan * player, std::vector<Direction>& dir) const;

	void	FindExitPath(char maze[64][64], int width, int height, int startX, int startY,
			std::string path, std::string& pathToExit, bool &found, TunnelMan *& player);

	int		shouting_interval = 0;
	bool	leaveOilField = false;
	int		TicksToWaitBetweenTurns = 0;
	int		TicksToWaitBetweenMoves = 0;
	int		TicksSincePerpenMoves;
	int		numSquaresToMoveInCurrentDirection = 0;
};

class Regular_Protester : public Protester
{
public:
	Regular_Protester(TunnelMan * player);
	virtual ~Regular_Protester() {};
};

class Hardcore_Protester : public Protester
{
public:
	Hardcore_Protester(TunnelMan * player);
	virtual ~Hardcore_Protester() {};

private:
	int totalDistanceFromPlayer;
};

class TunnelMan : public GameObject
{
public:
	TunnelMan(StudentWorld * game_adr);
	virtual ~TunnelMan() {};

	// INITIALIZE //

	void	init_Golds();
	void	init_Squirts();

	// ACTION //

	void	doSomething();

	void	move_Golds();
	void	move_Squirts();
	void	move_Sonars();

	// GETTER FUNCTIONS //

	int		geHealthPts() const				 { return healthpts; };
	int		getNumOfSonars() const			 { return numOfSonar; };
	int		getNumOfWaters() const			 { return numOfWater; };
	int		getnumOfGolds() const			 { return numOfGolds; };
	int		getNumOfOils() const			 { return numOfOil; };
	int		getCurrScore() const			 { return currScore; };

	std::vector<Golds*>& getGolds()			 { return m_golds; };
	std::vector<Squirt*>& getSquirts()		 { return m_squirts; };

	// INCREASE INVENTORIES //

	void	addOil()						 { numOfOil++; };
	void	addSonar()						 { numOfSonar += 2; };
	void	addWater()						 { numOfWater += 5; };
	void	addScore(int score)				 { currScore += score; };
	void	addGold()						 { numOfGolds++; };

	void	addGolds(int col, int row, bool setting, Actors owner);

	// DECREASE INVENTORIES //

	void	decreSonar()					 { numOfSonar--; };
	void	decreWater()					 { numOfWater--; };
	void	decreGold()						 { numOfGolds--; };
	void	decreHitpts(int damage)			 { healthpts -= damage; };

private:	
	std::vector<Squirt*> m_squirts;
	std::vector<Golds*>	 m_golds;

	// INVENTORY //

	int		numOfOil   = 0;
	int		numOfSonar = 1;
	int		numOfWater = 5;
	int		numOfGolds = 0;
	int		currScore  = 0;
	int		healthpts  = 100;
};

#endif // ACTOR_H_
