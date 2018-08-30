#include "Actor.h"
#include "StudentWorld.h"

//
//  Actor.cpp
//
//  Created by Jaehyeong Lee on 8/23/18.
//  Copyright (c) 2018 Jaehyeong Lee. All rights reserved.
//

// --------------------------------------------------------- PROTESTER --------------------------------------------------------- // 

Protester::Protester(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth) :
	GameObject(imageID, startX, startY, dir, size, depth)
{
	memset(maze, '.', sizeof(maze));
}

void Protester::FindExitPath(char maze[64][64], int width, int height, int startX, int startY, 
	std::string path, std::string& pathToExit, bool &found, TunnelMan *& player)
{
	if (found) return;

	for (int i = 0; i < 4; i++) {
		int next_x = startX + direction[i][0];
		int next_y = startY + direction[i][1];

		if (i == 0) {
			if (next_x >= 0 && next_x <= width && next_y >= 0 && next_y <= height && !checkForObstacles(next_x+3, next_y, right, player)) {
				if (maze[next_x][next_y] == '.')
				{
					maze[next_x][next_y] = 'X';
					FindExitPath(maze, width, height, next_x, next_y, path + dir_path[i], pathToExit, found, player);

					if (!found)
						maze[next_x][next_y] = '.';
				}
				if (next_x == 60 && next_y == 60)
				{
					path += dir_path[i];
					found = true;
					pathToExit = path;
				}
			}	
		}

		else if (i == 1) {
			if (next_x >= 0 && next_x <= width && next_y >= 0 && next_y <= height && !checkForObstacles(next_x, next_y, left, player)) {
				if (maze[next_x][next_y] == '.')
				{
					maze[next_x][next_y] = 'X';
					FindExitPath(maze, width, height, next_x, next_y, path + dir_path[i], pathToExit, found, player);

					if (!found)
						maze[next_x][next_y] = '.';
				}

				else if (next_x == 60 && next_y == 60)
				{
					path += dir_path[i];
					found = true;
					pathToExit = path;

				}
			}
		}

		if (i == 2) {
			if (next_x >= 0 && next_x <= width && next_y >= 0 && next_y <= height && !checkForObstacles(next_x, next_y, up, player)) {
				if (maze[next_x][next_y] == '.')
				{
					maze[next_x][next_y] = 'X';
					FindExitPath(maze, width, height, next_x, next_y, path + dir_path[i], pathToExit, found, player);

					if (!found)
						maze[next_x][next_y] = '.';
				}

				else if (next_x == 60 && next_y == 60)
				{
					path += dir_path[i];
					found = true;
					pathToExit = path;

				}
			}
		}

		if (i == 3) {
			if (next_x >= 0 && next_x <= width && next_y >= 0 && next_y <= height && !checkForObstacles(next_x, next_y-3, down, player)) {
				if (maze[next_x][next_y] == '.')
				{
					maze[next_x][next_y] = 'X';
					FindExitPath(maze, width, height, next_x, next_y, path + dir_path[i], pathToExit, found, player);

					if (!found)
						maze[next_x][next_y] = '.';
				}

				else if (next_x == 60 && next_y == 60)
				{
					path += dir_path[i];
					found = true;
					pathToExit = path;
				}
			}
		}

	}
}


void Protester::checkPerpenPath(int col, int row, Direction currDir, TunnelMan * player, std::vector<Direction>& dir) const
{
	StudentWorld * Game = player->getWorld();

	switch (currDir)
	{
		bool flag;

	case right: case left:
	{
		flag = true;

		for (int i = 0; i < 4; i++)
			if (!Game->isValidSpotToMove(col+i, row-1))
				flag = false;

		if (flag) dir.push_back(down);

		flag = true;

		for (int i = 0; i < 4; i++)
			if (!Game->isValidSpotToMove(col+i, row+4))
				flag = false;

		if (flag) dir.push_back(up);		
	}
	case down: case up:
	{
		flag = true;

		for (int i = 0; i < 4; i++)
			if (!Game->isValidSpotToMove(col-1, row+i))
				flag = false;

		if (flag) dir.push_back(left);

		flag = true;

		for (int i = 0; i < 4; i++)
			if (!Game->isValidSpotToMove(col+4, row+i))
				flag = false;

		if (flag) dir.push_back(right);

	}
	}

}


void Protester::doSomething(TunnelMan * player)
{
	
	StudentWorld * Game = player->getWorld();

	
	if (update(player)) return;
	else 
	{
		decreShoutingTime();
		decreTicksSincePerpenMoves();
	}

	if (Game->isPlayerCloseBy(getX(), getY()) && !leaveOilFieldState() && getShoutingInterval() == 0)
	{
		Game->playSound(SOUND_PROTESTER_YELL);
		player->decreHitpts(20);
		setShoutingInterval(15);	
	}

	if (canSeePlayer(getX(), getY(), player))			//	If a player is in horizontal or vertical sight without objects in between
	{
		setNumSquaresToMoveInCurrentDirection(0);
		int currLevel = player->getWorld()->getLevel();
		int N = std::max(0, static_cast<int>(3 - currLevel / 4));
		setTicksToWaitBetweenMoves(N);
		return;
	}

	if (getTicksSincePerpenMoves() <= 0)
		GoToPerpenPath(player);

	if (getNumSquaresToMoveInCurrentDirection() > 0)
		GoToCurrDirection(player);
	else 
		pickNewDirection(player);


	int currLevel = player->getWorld()->getLevel();
	int N = std::max(0, static_cast<int>(3 - currLevel / 4));
	setTicksToWaitBetweenMoves(N);
}


bool Protester::update(TunnelMan * player)
{
	if (getX() == 60 && getY() == 60 && leaveOilFieldState()) {
		setDead();
		return true;
	}

	if (getTicksToWaitBetweenMoves() > 0)
	{
		decreTicksToWaitBetweenMoves();
		return true;
	}

	if (leaveOilFieldState())
	{
		GoToExit(player);
		return true;
	}

	return false;
}

void Protester::GoToPerpenPath(TunnelMan * player)
{
	std::vector<Direction> perpenDir;

	checkPerpenPath(getX(), getY(), getDirection(), player, perpenDir);

	if (perpenDir.size() == 1) {
		setDirection(perpenDir[0]);
		setTicksSincePerpenMoves(200);

		int N = rand() % 53 + 8;
		setNumSquaresToMoveInCurrentDirection(N);
	}

	else if (perpenDir.size() > 1)
	{
		int i = rand() % perpenDir.size();

		setDirection(perpenDir[i]);
		setTicksSincePerpenMoves(200);

		int N = rand() % 53 + 8;
		setNumSquaresToMoveInCurrentDirection(N);
	}
}

void Protester::GoToCurrDirection(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();
	Direction currDir = getDirection();

	switch (currDir)
	{
	case up:
		if (Game->isValidSpotToMove(getX(), getY()+4))
			moveTo(getX(), getY()+1);
		else setNumSquaresToMoveInCurrentDirection(0);
		break;
	case down:
		if (Game->isValidSpotToMove(getX(), getY()-1))
			moveTo(getX(), getY()-1);
		else setNumSquaresToMoveInCurrentDirection(0);
		break;
	case right:
		if (Game->isValidSpotToMove(getX()+4, getY()))
			moveTo(getX()+1, getY());
		else setNumSquaresToMoveInCurrentDirection(0);
		break;
	case left:
		if (Game->isValidSpotToMove(getX()-1, getY()))
			moveTo(getX()-1, getY());
		else setNumSquaresToMoveInCurrentDirection(0);
		break;
	}

	decreNumSquaresToMoveInCurrentDirection();
}

void Protester::pickNewDirection(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();
	int N;
	Direction newDir;
	bool rightDir;

	do
	{
		rightDir = true;

		newDir = static_cast<Direction>(rand() % 4 + 1);					// 1 to 4
		N = rand() % 53 + 8;												//	8 <= numSquaresToMoveInCurrentDirection <= 60 

		setNumSquaresToMoveInCurrentDirection(N);

		switch (newDir)
		{

		case right:
			if (Game->isValidSpotToMove(getX()+1, getY()))
				setDirection(right);
			else rightDir = false;
			break;

		case left:
			if (Game->isValidSpotToMove(getX()-1, getY()))
				setDirection(left);
			else rightDir = false;
			break;

		case up:
			if (Game->isValidSpotToMove(getX(), getY()+1))
				setDirection(up);
			else rightDir = false;
			break;

		case down:
			if (Game->isValidSpotToMove(getX(), getY()-1))
				setDirection(down);
			else rightDir = false;
			break;
		}
	} while (!rightDir);

}

void Protester::GoToExit(TunnelMan * player)
{
	if (getPathToExit() == "") {
		std::string input;
		std::string exitPath;
		bool found = false;

		FindExitPath(maze, 60, 60, getX(), getY(), input, exitPath, found, player);
		setPathToExit(exitPath);
	}
	else
	{
		char currDir = getDirection();
		char Dir = getPathToExit()[0];
		getPathToExit().erase(0, 1);

		switch (Dir)
		{
		case 'W':
			if (currDir != left)
				setDirection(left);
			moveTo(getX()-1, getY());
			break;
		case 'E':
			if (currDir != right)
				setDirection(right);
			moveTo(getX()+1, getY());
			break;
		case 'N':
			if (currDir != up)
				setDirection(up);
			moveTo(getX(), getY()+1);
			break;
		case 'S':
			if (currDir != down)
				setDirection(down);
			moveTo(getX(), getY()-1);
			break;
		}
	}

	int currLevel = player->getWorld()->getLevel();
	int N = std::max(0, static_cast<int>(3 - currLevel / 4));
	setTicksToWaitBetweenMoves(N);
}


bool Protester::checkForObstacles(int col, int row, Direction dir, TunnelMan * player) const
{
	StudentWorld * Game = player->getWorld();
	BoulderContainer &Boulder = player->getWorld()->getBoulders();

	BoulderContainer::const_iterator iter = Boulder.begin();

	switch (dir)
	{
	case right: case left:
		for (int j = 0; j < 4; j++)
			if (((*iter)->getX() == col && (*iter)->getY() == row + j) || (Game->isEarthIndex(col, row+j) && Game->getEarth()[col][row+j]))
				return true;

	case up: case down:
		for (int j = 0; j < 4; j++)
			if (((*iter)->getX() == col + j && (*iter)->getY() == row) || (Game->isEarthIndex(col+j, row) && Game->getEarth()[col+j][row]))
				return true;
	}

	return false;
}

bool Protester::canSeePlayer(int col, int row, TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();
	int playerX = player->getX();
	int playerY = player->getY();
	int i = 0;

	if (col == playerX && row != playerY)								//	if a player is in vertical sight
	{
		int distance = row - playerY;
		if (distance < 0)												//	if a player is above protester
		{
			while (distance + i != 0)
			{
				if (checkForObstacles(col, row+i, up, player))
					return false;
				i++;
			}

			setDirection(up);
			moveTo(getX(), getY()+1);

			return true;
		}
		else																//	if a player is below protester
		{
			while (distance - i != 0)
			{
				if (checkForObstacles(col, row-i, down, player))
					return false;
				i++;
			}

			setDirection(down);
			moveTo(getX(), getY()-1);

			return true;
		}
	}

	else if (row == playerY && col != playerX)							// if a player is in horizontal sight
	{
		int distance = col - playerX;

		if (distance < 0)												//	if a player is more right 
		{
			while (distance + i != 0) 
			{
				if (checkForObstacles(col+i, row, right, player))
					return false;
				i++;
			}

			setDirection(right);
			moveTo(getX()+1, getY());

			return true;

		}
		else																//	if a player is more left
		{
			while (distance - i != 0)
			{
				if (checkForObstacles(col-i, row, left, player))
					return false;
				i++;
			}

			setDirection(left);
			moveTo(getX()-1, getY());

			return true;

		}
	}

	return false;
}

Regular_Protester::Regular_Protester(TunnelMan * player) :
	Protester(TID_PROTESTER, 60, 60, left, 1.0, 0)
{
	int currLevel = player->getWorld()->getLevel();
	int T = std::max(0, static_cast<int>(3 - currLevel / 4));
	int N = rand() % 53 + 8;

	setTicksToWaitBetweenMoves(T);
	setVisible(true);
	setHitPoints(5);
	setTicksToWaitBetweenTurns(200);
	setNumSquaresToMoveInCurrentDirection(N);
	setTicksSincePerpenMoves(0);
}

Hardcore_Protester::Hardcore_Protester(TunnelMan * player) :
	Protester(TID_HARD_CORE_PROTESTER, 60, 60, left, 1.0, 0)
{
	int currLevel = player->getWorld()->getLevel();
	int T = std::max(0, static_cast<int>(3 - currLevel / 4));
	int N = rand() % 53 + 8;

	totalDistanceFromPlayer = 16 + currLevel * 2;
	setTicksToWaitBetweenMoves(T);
	setVisible(true);
	setHitPoints(20);
	setTicksToWaitBetweenTurns(200);
	setNumSquaresToMoveInCurrentDirection(N);
	setTicksSincePerpenMoves(0);
}

// --------------------------------------------------------- BARREL OIL --------------------------------------------------------- // 

BarrelOil::BarrelOil(int startX, int startY) : Goodies(TID_BARREL, startX, startY, right, 1.0, 2)
{
	setVisible(false);
}

void BarrelOil::doSomething(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();

	if (Game->isPlayerCloseBy(getX(), getY()))
		setVisible(true);

	if (Game->isWithinRange(getX(), getY(), player))
	{
		Game->playSound(SOUND_FOUND_OIL);
		player->addOil();
		player->addScore(1000);
		Game->setGameText();

		setDead();
	}
}

// --------------------------------------------------------- GOLDS --------------------------------------------------------- // 


Golds::Golds(int startX, int startY, bool setting, Actors owner) : 
	Goodies(TID_GOLD, startX, startY, right, 1.0, 2)
{
	setOwnership(owner);
	setVisible(setting);
	setTemporState(100);

}

void Golds::doSomething(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();

	if (getTemporState() == TIME_UP) {
		setDead();
		return;
	}
	
	else if (getOwnership() == PLAYER && !Game->isWithinRange(getX(), getY()))
	{																		//	If gold nugget is in earthless spot, set the timer
		decreTemporState();
	}

	if (Game->isPlayerCloseBy(getX(), getY()))
	{
		setVisible(true);
	}

	if (Game->isWithinRange(getX(), getY(), player) && getOwnership() == EARTH)
	{
		Game->playSound(SOUND_GOT_GOODIE);
		player->addGold();
		player->addScore(10);
		Game->setGameText();

		setDead();
	}

	Protester * bribedProtester = Game->hitProtester(getX(), getY());

	if (bribedProtester && !bribedProtester->leaveOilFieldState() && getOwnership() == PLAYER)
	{
		Game->playSound(SOUND_PROTESTER_FOUND_GOLD);
		if (dynamic_cast<Hardcore_Protester*>(bribedProtester)) {

			player->addScore(50);
			int currLevel = player->getWorld()->getLevel();
			int ticks_to_stare = std::max(50, 100 - currLevel * 10);
			bribedProtester->setTicksToWaitBetweenMoves(ticks_to_stare);
			Game->setGameText();
			setDead();
			return;
;		}
		else
			player->addScore(25);

		bribedProtester->setToLeaveOilFieldState();
		Game->setGameText();	

		setDead();
	}

}



// --------------------------------------------------------- SONAR --------------------------------------------------------- // 

Sonar::Sonar(int currLevel) : Goodies(TID_SONAR, 0, 60, right, 1.0, 2)
{
	setVisible(true);
	int T = std::max(100, 300 - 10 * currLevel);
	setTemporState(T);
}

void Sonar::doSomething(TunnelMan * player)
{
	if (getTemporState() == TIME_UP) {
		setDead();
		return;
	}
	else decreTemporState();

	StudentWorld * Game = player->getWorld();

	if (Game->isWithinRange(getX(), getY(), player))
	{
		Game->playSound(SOUND_GOT_GOODIE);
		player->addSonar();
		player->addScore(75);
		Game->setGameText();

		setDead();
	}
}


// --------------------------------------------------------- BOULDER --------------------------------------------------------- // 

Boulder::Boulder(int startX, int startY) :
	GameObject(TID_BOULDER, startX, startY, right, 1.0, 1)
{
	setVisible(true); 	
	setTemporState(30);
}

void Boulder::doSomething(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();

	if (!Game->isEarthBelow(getX(), getY())) 						//	if there is no earth below, fall
		fallingState = true;
	

	if (fallingState && getTemporState() == TIME_UP) 
	{

		if (getSwitch() == SOUND_OFF)
		{
			Game->playSound(SOUND_FALLING_ROCK);		
			Turn_On_Sound();								
		}

		Protester * damagedProtester = Game->hitProtester(getX(), getY());

		if (damagedProtester && !damagedProtester->leaveOilFieldState())
		{
			Game->playSound(SOUND_PROTESTER_GIVE_UP);
			player->addScore(500);
			damagedProtester->setToLeaveOilFieldState();
		}

		if (Game->isWithinRange(getX(), getY(), player))				// Collision with a player
		{
			player->setDead();
			setDead();
		}
		else if (Game->isWithinRange_sameObjs(getX(), getY(), Game->getBoulders().begin(), Game->getBoulders().end()))		// Collision with another boulder	
		{
			setDead();
		}
		else if (Game->isWithinRange(getX(), getY()))			//	Collision with earth
		{
			setDead();
		}

		else 
		{
			moveTo(getX(), getY()-1);
		}

	}
}


// --------------------------------------------------------- GAME OBJECT --------------------------------------------------------- // 


GameObject::GameObject(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	: GraphObject(imageID, startX, startY, dir, size, depth)
{}



void GameObject::decreTemporState()
{
	if (temporState == TIME_UP)
		return;

	temporState--;
}


// --------------------------------------------------------- TUNNEL MAN --------------------------------------------------------- // 


TunnelMan::TunnelMan(StudentWorld * game_adr) : GameObject(TID_PLAYER, 30, 60, right, 1.0, 0)
{
	setVisible(true); 
	setWorld(game_adr);
}

void TunnelMan::move_Squirts()
{

	if (m_squirts.size() == 0)
		return;

	auto it = m_squirts.begin();

	while (it != m_squirts.end())
	{
		(*it)->doSomething(this);
		
		if (!(*it)->isAlive()) {
			delete *it;
			*it = nullptr;
			it = m_squirts.erase(it);
		}

		else it++;
	}

}

void TunnelMan::init_Squirts()
{
	Direction dir = getDirection();

	if (numOfWater > 0)
	{

		switch (dir)
		{
		case right:
			if (!getWorld()->isWithinRange(getX()+3, getY()) && !getWorld()->isWithinRange_Boulder(getX()+3, getY()))
				m_squirts.push_back(new Squirt(getX()+3, getY(), dir));
			break;

		case left:
			if (!getWorld()->isWithinRange(getX()-3, getY()) && !getWorld()->isWithinRange_Boulder(getX()-3, getY()))
				m_squirts.push_back(new Squirt(getX()-3, getY(), dir));
			break;

		case up:
			if (!getWorld()->isWithinRange(getX(), getY()+3) && !getWorld()->isWithinRange_Boulder(getX(), getY()+3))
				m_squirts.push_back(new Squirt(getX(), getY()+3, dir));
			break;

		case down:
			if (!getWorld()->isWithinRange(getX(), getY()-3) && !getWorld()->isWithinRange_Boulder(getX(), getY()-3))
				m_squirts.push_back(new Squirt(getX(), getY()-3, dir));
			break;

		}

		getWorld()->playSound(SOUND_PLAYER_SQUIRT);
		numOfWater--;

	}
}


// --------------------------------------------------------- GOODIES --------------------------------------------------------- // 

Goodies::Goodies(int imageID, int startX, int startY, Direction dir,
	double size, unsigned int depth) : GameObject(imageID, startX, startY, dir, size, depth) {}

void Goodies::setOwnership(Actors actor)
{
	whoPickedUp = actor;
}



// --------------------------------------------------------- WATER --------------------------------------------------------- // 

Water::Water(int col, int row, int currLevel) : Goodies(TID_WATER_POOL, col, row, right, 1.0, 2)
{
	setVisible(true);
	int T = std::max(100, 300 - 10 * currLevel);
	setTemporState(T);
}

void Water::doSomething(TunnelMan * player)
{
	if (getTemporState() == TIME_UP) {
		setDead();
		return;
	}
	else decreTemporState();

	StudentWorld * Game = player->getWorld();

	if (Game->isWithinRange(getX(), getY(), player))
	{
		Game->playSound(SOUND_GOT_GOODIE);
		player->addWater();
		player->addScore(100);
		Game->setGameText();

		setDead();
	}
}


// --------------------------------------------------------- SQUIRT --------------------------------------------------------- // 

Squirt::Squirt(int col, int row, Direction dir) : 
	GameObject(TID_WATER_SPURT, col, row, dir, 1.0, 1)
{
	setVisible(true);
}

void Squirt::move(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();
	int col = getX();
	int row = getY();

	Direction dir = getDirection();

	switch (dir)
	{
	case right:
		if (Game->isValidSpotToMove(col+1, row)) {
			moveTo(col+1, row);
			distanceTravelled++;
		}
		else setDead();
		break;

	case left:
		if (Game->isValidSpotToMove(col-1, row)) {
			moveTo(col-1, row);
			distanceTravelled++;
		}
		else setDead();
		break;

	case up:
		if (Game->isValidSpotToMove(col, row+1)) {
			moveTo(col, row+1);
			distanceTravelled++;
		}
		else setDead();
		break;

	case down:
		if (Game->isValidSpotToMove(col, row-1)) {
			moveTo(col, row-1);
			distanceTravelled++;
		}
		else setDead();
		break;
	}
}

void Squirt::doSomething(TunnelMan * player)
{
	StudentWorld * Game = player->getWorld();

	if (distanceTravelled == 4)
	{
		setDead();
		return;
	}

	Protester * damagedProtester = Game->hitProtester(getX(), getY());

	if (damagedProtester && !damagedProtester->leaveOilFieldState())		//	If the protester isn't in leave state
	{
		damagedProtester->decreHitPoints(2);

		if (damagedProtester->getHitPoints() <= 0)
		{
			Game->playSound(SOUND_PROTESTER_GIVE_UP);

			if (dynamic_cast<Hardcore_Protester*>(damagedProtester))
				player->addScore(250);
			else
				player->addScore(100);

			damagedProtester->setToLeaveOilFieldState();
			damagedProtester->setTicksToWaitBetweenMoves(0);
			setDead();
			return;
		}

		Game->playSound(SOUND_PROTESTER_ANNOYED);

		int N = std::max(50, static_cast<int>(100 - Game->getLevel() * 10));
		damagedProtester->setTicksToWaitBetweenMoves(N);
		setDead();
		return;
	}

	move(player);
}



// --------------------------------------------------------- TUNNEL MAN --------------------------------------------------------- // 

void TunnelMan::doSomething()
{
	if (healthpts == 0) {
		setDead();
		return;
	}

	int input;

	if (getWorld()->getKey(input))
	{

		Direction curDir = getDirection();

		switch (input)
		{
		case KEY_PRESS_RIGHT:
			if (curDir == right && getX() < VIEW_WIDTH-4) {
				if (!getWorld()->isWithinRange_Boulder(getX()+1, getY()))
					moveTo(getX()+1, getY());
			}
			else moveTo(getX(), getY());

				setDirection(right);
				break;

		case KEY_PRESS_LEFT:
			if (curDir == left && getX() > 0) {
				if (!getWorld()->isWithinRange_Boulder(getX()-1, getY()))
					moveTo(getX()-1, getY());
			}
			else moveTo(getX(), getY());

				setDirection(left);
				break;

		case KEY_PRESS_UP:
			if (curDir == up && getY() < VIEW_HEIGHT-4) {
				if (!getWorld()->isWithinRange_Boulder(getX(), getY()+1))
					moveTo(getX(), getY()+1);
			}
			else moveTo(getX(), getY());

				setDirection(up);
				break;

		case KEY_PRESS_DOWN:
			if (curDir == down && getY() > 0) {
				if (!getWorld()->isWithinRange_Boulder(getX(), getY()-1))
					moveTo(getX(), getY()-1);
			}
			else moveTo(getX(), getY());

				setDirection(down);
				break;

		case KEY_PRESS_SPACE:
			init_Squirts();
			break;

		
		case 'z' : case 'Z':
			move_Sonars();
			break;


		case KEY_PRESS_TAB:
			init_Golds();
		}
	}

	getWorld()->dig_Earth();
}

void TunnelMan::init_Golds()
{
	Direction dir = getDirection();

	if (numOfGolds> 0)
	{
		numOfGolds--;
		m_golds.push_back(new Golds(getX(), getY(), true, PLAYER));	
	}
}

void TunnelMan::addGolds(int col, int row, bool setting, Actors owner)
{
	m_golds.push_back(new Golds(col, row, setting, owner));
}

void TunnelMan::move_Golds()
{

	GoldContainer::iterator it = m_golds.begin();

	while (it != m_golds.end()) {

		if (!((*it)->isAlive())) {
			delete *it;
			*it = nullptr;
			it = m_golds.erase(it);
		}

		else {
			(*it)->doSomething(this);
			it++;
		}
	}
}


void TunnelMan::move_Sonars()
{
	if (numOfSonar > 0)
	{
		getWorld()->activateSonars();
		numOfSonar--;
		getWorld()->playSound(SOUND_SONAR);
	}

}
