#include "StudentWorld.h"
#include <string>
#include <algorithm>

//
//  StudentWorld.cpp
//
//  Created by Jaehyeong Lee on 8/23/18.
//  Copyright (c) 2018 Jaehyeong Lee. All rights reserved.
//

GameWorld* createStudentWorld(std::string assetDir)
{
	return new StudentWorld(assetDir);
}


// --------------------------------------------------------- PROTESTER IMPLEMENTATIONS --------------------------------------------------------- // 

void StudentWorld::init_Protesters()
{
	int probabilityOfHardcore = std::min(90, static_cast<int>(getLevel() * 10 + 30));

	if (rand() % 101 <= probabilityOfHardcore)
		m_protesters.push_back(new Hardcore_Protester(m_player));
	else
		m_protesters.push_back(new Regular_Protester(m_player));

	timeSinceProtesterAdded = std::max(25, static_cast<int>(200 - getLevel()));
	maxNumOfProtesters = std::min(15, static_cast<int>(2 + getLevel() * 1.5));
	numOfProtesters = 1;
}


void StudentWorld::move_Protesters()
{

	int probabilityOfHardcore = std::min(90, static_cast<int>(getLevel() * 10 + 30));

	if (timeSinceProtesterAdded <= 0 && numOfProtesters < maxNumOfProtesters) {

		if (rand() % 101 <= probabilityOfHardcore)
			m_protesters.push_back(new Hardcore_Protester(m_player));
		else 
			m_protesters.push_back(new Regular_Protester(m_player));

		numOfProtesters++;
		timeSinceProtesterAdded = std::max(25, static_cast<int>(200 - getLevel()));
	}

	timeSinceProtesterAdded--;

	ProtesterContainer::iterator it = m_protesters.begin();
	
	while (it != m_protesters.end()) {

		if (!(*it)->isAlive()) {
			delete *it;
			it = m_protesters.erase(it);
			numOfProtesters--;
		}

		else {
			(*it)->doSomething(m_player);
			it++;
		}
	}


}


// --------------------------------------------------------- INITIALIZATION IMPLEMENTATIONS --------------------------------------------------------- // 

void StudentWorld::init_Oils()
{
	int L = std::min(static_cast<int>(2 + getLevel()), 21);

	for (int i = 0; i < L; i++) {

		int col = rand() % (VIEW_HEIGHT-3);
		int row = rand() % 37 + 20;

		while (!isWithinEarth(col, row) || !isValidSpotForGoodies(col, row))
		{
			col = rand() % (VIEW_HEIGHT-3);
			row = rand() % 37 + 20;
		}

		m_oils.push_back(new BarrelOil(col, row));
	}


}

void StudentWorld::init_Golds()
{
	int G = std::max(5 - static_cast<int>(getLevel() / 2), 2);

	for (int i = 0; i < G; i++) {

		int col = rand() % (VIEW_HEIGHT-3);										//must be distributed between x = 0, y = 0 and x = 60, y = 56
		int row = rand() % 37 + 20;

		while (!isWithinEarth(col, row) || !isValidSpotForGoodies(col, row))
		{
			col = rand() % (VIEW_HEIGHT-3);
			row = rand() % 37 + 20;
		}

		m_player->addGolds(col, row, false, GameObject::EARTH);
	}
}

void StudentWorld::init_Boulders()
{

	int B = std::min(static_cast<int>(getLevel() / 2 + 2), 9);

	for (int i = 0; i < B; i++) {

		int col = rand() % (VIEW_HEIGHT-3);
		int row = rand() % 37 + 20;

		while (!isWithinEarth(col, row) || !isValidSpotForGoodies(col, row))				//	Check to see a boulder doesn't overlap with one another
		{

			col = rand() % (VIEW_HEIGHT-3);										//	x=0 to x=60
			row = rand() % 37 + 20;												//	y=20 to y=56
		}

		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++)
				if (isEarthIndex(col+i, row+j) && m_earthList[col+i][row+j]) {

					delete m_earthList[col+i][row+j];
					m_earthList[col+i][row+j] = nullptr;
				}

		m_boulders.push_back(new Boulder(col, row));
	}
}

void StudentWorld::init_TunnelMan()
{
	m_player = new TunnelMan(this);
}

void StudentWorld::init_Earths()
{
	for (int col = 0; col < VIEW_WIDTH; col++) {
		std::vector<Earth*> v;

		for (int row = 0; row < VIEW_HEIGHT-4; row++) {

			if (!(col >= TUNNEL_START_X && col <= TUNNEL_END_X &&
				row >= TUNNEL_START_Y && row <= TUNNEL_END_Y))				// initializing Earth with empty tunnel in middle
				v.push_back(new Earth(col, row));					 // Earth coordinate is (column, row), NOT (row, column)
		}

		m_earthList.push_back(v);
	}


}


// --------------------------------------------------------- ACTION IMPLEMENTATIONS --------------------------------------------------------- // 



void StudentWorld::move_Oils()
{
	OilContainer::iterator it = m_oils.begin();

	while (it != m_oils.end()) {

		if (!((*it)->isAlive())) {
			delete *it;
			*it = nullptr;
			it = m_oils.erase(it);
		}

		else {
			(*it)->doSomething(m_player);
			it++;
		}
	}
}

void StudentWorld::move_Sonar()
{
	int currLevel = getLevel();

	int G = currLevel * 25 + 300;
	int chanceOfAdded = rand() % G + 1;

	if (chanceOfAdded == 1)								//	Add Sonar kit by (1 / currLevel * 25 + 300) chance
		m_sonars.push_back(new Sonar(currLevel));

	SonarContainer::iterator it = m_sonars.begin();

	while (it != m_sonars.end()) {

		if (!((*it)->isAlive())) {
			delete *it;
			*it = nullptr;
			it = m_sonars.erase(it);
		}

		else {
			(*it)->doSomething(m_player);
			it++;
		}
	}



}

void StudentWorld::move_Boulders()
{

	BoulderContainer::iterator it = m_boulders.begin();

	while (it != m_boulders.end())
	{
		if (!(*it)->isAlive()) {									// ** AFTER POINTERS IN VECTORS ARE DELETED, VECTOR DOESN'T SHIFT ** 
			delete *it;												// SO MAKE SURE IF POINTERS IN SOME INDEXES IN VECTOR ARE NOT EMPTY
			it = m_boulders.erase(it);		
		}

		else 
		{
			if ((*it)->getFallingState() == true) 
				(*it)->decreTemporState();								//	After 30 ticks, it will fall	
																		//	decrement waiting state by 1 each move

			(*it)->doSomething(m_player);
			it++;
		}
	}
}


void StudentWorld::move_Water()
{
	int currLevel = getLevel();

	int G = currLevel * 25 + 300;
	int chanceOfAdded = rand() % G + 1;

	if (chanceOfAdded == 1)								//	Add water kit by (1 / currLevel * 25 + 300) chance
	{
		int col = rand() % (VIEW_WIDTH-3);
		int row = rand() % (VIEW_HEIGHT-3);

		// (56 - 60) (7 - 11)

		while (isWithinRange(col, row) || isWithinRange(col, row, m_player) ||
			isWithinRange_Boulder(col, row))									//	Make sure there is no overlapping with boulder, earth, and a player
		{
			col = rand() % (VIEW_WIDTH-3);
			row = rand() % (VIEW_HEIGHT-3);
		}

		m_waters.push_back(new Water(col, row, currLevel));

	}

	WaterContainer::iterator it = m_waters.begin();

	while (it != m_waters.end()) {

		if (!((*it)->isAlive())) {
			delete *it;
			*it = nullptr;
			it = m_waters.erase(it);
		}

		else {
			(*it)->doSomething(m_player);
			it++;
		}
	}
}

// --------------------------------------------------------- MAIN FUNCTIONS --------------------------------------------------------- // 

int StudentWorld::init()
{
	srand(time(NULL));

	init_TunnelMan();
	init_Earths();
	init_Oils();		
	init_Golds();		
	init_Boulders();	
	init_Protesters();

	return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld::move()
{
	setGameText();

	if (m_oils.size() == 0)
		return GWSTATUS_FINISHED_LEVEL;

	if (m_player->isAlive()) {
		m_player->doSomething();
		m_player->move_Squirts();
		m_player->move_Golds();
	}
	else 
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	move_Water();
	move_Boulders();
	move_Sonar();		
	move_Oils();	
	move_Protesters();

	return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{

	for (auto it = m_earthList.begin(); it != m_earthList.end(); ) {
		for (auto earth_it = (*it).begin(); earth_it != (*it).end(); ) {
			delete *earth_it;
			*earth_it = nullptr;

			earth_it = (*it).erase(earth_it);
		}

		it = m_earthList.erase(it);

	}

	for (auto it = m_waters.begin(); it != m_waters.end(); )
	{
		delete *it;
		*it = nullptr;

		it = m_waters.erase(it);
	}

	for (auto it = m_boulders.begin(); it != m_boulders.end(); )
	{
		delete *it;
		*it = nullptr;

		it = m_boulders.erase(it);
	}

	for (auto it = m_oils.begin(); it != m_oils.end(); )
	{
		delete *it;
		*it = nullptr;

		it = m_oils.erase(it);
	}

	for (auto it = m_sonars.begin(); it != m_sonars.end(); )
	{
		delete *it;
		*it = nullptr;

		it = m_sonars.erase(it);
	}

	for (auto it = m_protesters.begin(); it != m_protesters.end(); )
	{
		delete *it;
		*it = nullptr;

		it = m_protesters.erase(it);
	}

	GoldContainer &Gold = m_player->getGolds();

	for (auto it = Gold.begin(); it != Gold.end(); )
	{
		delete *it;
		*it = nullptr;

		it = Gold.erase(it);
	}

	SquirtContainer &Squirt = m_player->getSquirts();

	for (auto it = Squirt.begin(); it != Squirt.end(); )
	{
		delete *it;
		*it = nullptr;

		it = Squirt.erase(it);
	}


	delete m_player;
}


// --------------------------------------------------------- GAME DISPLAY IMPLEMENTATIONS --------------------------------------------------------- // 

void StudentWorld::setGameText()
{
	int healthpts = m_player->geHealthPts();
	int numOfWater = m_player->getNumOfWaters();
	int numOfGold = m_player->getnumOfGolds();
	int numOfOil = m_player->getNumOfOils();
	int numOfSonar = m_player->getNumOfSonars();
	int currScore = m_player->getCurrScore();


	std::string text = " Lvl: " + std::to_string(getLevel()) + " Lives: " + std::to_string(getLives())
		+ " Hlth: " + std::to_string(healthpts) + "% Wtr: " + std::to_string(numOfWater)
		+ " Gld: " + std::to_string(numOfGold) + " Oil Left: " + std::to_string(numOfOil)
		+ " Sonar: " + std::to_string(numOfSonar) + " Scr: " + std::to_string(currScore);

	setGameStatText(text);
}

// --------------------------------------------------------- GAME LOGIC IMPLEMENTATIONS --------------------------------------------------------- // 


bool StudentWorld::isWithinRange_Boulder(int col, int row) const
{
	auto boulder_it = m_boulders.begin();

	while (boulder_it  != m_boulders.end())
	{
		if (calculateRadius(col, row, (*boulder_it)->getX(), (*boulder_it)->getY()) <= 3)
			return true;

		boulder_it++;
	}

	return false;
}

bool StudentWorld::isValidSpotToMove(int col, int row) const
{

	if (col >= 0 && col <= VIEW_WIDTH - 4 && row >= 0 && row <= VIEW_HEIGHT - 4 &&
		!isWithinRange_Boulder(col, row) &&
		(!isEarthIndex(col, row) || (isEarthIndex(col, row) && !m_earthList.at(col).at(row))))
		return true;

	return false;
}

bool StudentWorld::isPlayerCloseBy(int x_cord, int y_cord) const
{
	if (calculateRadius(x_cord, y_cord, m_player->getX(), m_player->getY()) <= 4)
		return true;

	return false;
}

Protester * StudentWorld::hitProtester(int x_cord, int y_cord) const			//	return the protester who has been hit
{
	auto it = std::find_if(m_protesters.begin(), m_protesters.end(), [=](const Protester * ptr) -> bool {
		return calculateRadius(x_cord, y_cord, ptr->getX(), ptr->getY()) <= 3; });


	return it != m_protesters.end() ? (*it) : nullptr;

	
}

bool StudentWorld::isValidSpotForGoodies(int col, int row) const
{

	auto oil_it = m_oils.begin();

	while (oil_it != m_oils.end())
	{
		if (calculateRadius(col, row, (*oil_it)->getX(), (*oil_it)->getY()) <= 6)
			return false;
		else oil_it++;
	}

	auto boulder_it = m_boulders.begin();

	while (boulder_it != m_boulders.end())
	{
		if (calculateRadius(col, row, (*boulder_it)->getX(), (*boulder_it)->getY()) <= 6)
			return false;
		else boulder_it++;
	}

	auto gold_it = m_player->getGolds().begin();
	auto gold_end = m_player->getGolds().end();

	while (gold_it != gold_end)
	{
		if (calculateRadius(col, row, (*gold_it)->getX(), (*gold_it)->getY()) <= 6)
			return false;
		else gold_it++;
	}

	auto water_it = m_waters.begin();

	while (water_it != m_waters.end())
	{
		if (calculateRadius(col, row, (*water_it)->getX(), (*water_it)->getY()) <= 6)
			return false;
		else water_it++;
	}

	auto sonar_it = m_sonars.begin();

	while (sonar_it != m_sonars.end())
	{
		if (calculateRadius(col, row, (*sonar_it)->getX(), (*sonar_it)->getY()) <= 6)
			return false;
		else sonar_it++;
	}

	return true;
}


bool StudentWorld::isWithinEarth(int col, int row) const
{
	try {
		if (isEarthIndex(col, row) && m_earthList.at(col).at(row) &&
			isEarthIndex(col+4, row) && m_earthList.at(col+4).at(row) &&
			isEarthIndex(col, row+4) && m_earthList.at(col).at(row+4) &&
			isEarthIndex(col+4, row+4) && m_earthList.at(col+4).at(row+4))
			return true;
	}

	catch (const std::out_of_range& e) {}

	return false;
}


double StudentWorld::calculateRadius(int x_1, int y_1, int x_2, int y_2) const
{
	int y_dis = y_1 - y_2;
	int x_dis = x_1 - x_2;

	return sqrt(pow(y_dis, 2) + pow(x_dis, 2));
}

bool StudentWorld::isWithinRange(int x_cord, int y_cord, GameObject * actor) const
{
	if (calculateRadius(x_cord, y_cord, actor->getX(), actor->getY()) <= 3)
		return true;

	return false;
}


bool StudentWorld::isWithinRange(int col, int row) const
{

	for (int i = 0; i < 4; i++)						
		for (int j = 0; j < 4; j++)
			if (isEarthIndex(col+i, row+j) && m_earthList[col+i][row+j])
				return true;

	return false;
}


bool StudentWorld::isEarthBelow(int col, int row) const
{
	for (int i = 0; i < 4; i++)
		if (isEarthIndex(col+i, row-1) && m_earthList[col+i][row-1])
			return true;

	return false;
}




bool StudentWorld::isEarthIndex(int col, int row) const
{
	if (row < 0 || row > EARTH_HEIGHT || (col >= TUNNEL_START_X && col <= TUNNEL_END_X &&
		row >= TUNNEL_START_Y && row <= TUNNEL_END_Y))
		return false;

	return true;
}



void StudentWorld::activateSonars()
{
	int x_cord = m_player->getX();
	int y_cord = m_player->getY();

	auto oil_it = m_oils.begin();

	auto gold_it = m_player->getGolds().begin();
	auto gold_end = m_player->getGolds().end();

	while (oil_it != m_oils.end())
	{
		if (calculateRadius(x_cord, y_cord, (*oil_it)->getX(), (*oil_it)->getY()) <= 12)
			(*oil_it)->setVisible(true);
		
		oil_it++;
	}

	while (gold_it != gold_end)
	{
		if (calculateRadius(x_cord, y_cord, (*gold_it)->getX(), (*gold_it)->getY()) <= 12)
			(*gold_it)->setVisible(true);

		gold_it++;
	}
}

void StudentWorld::dig_Earth()
{
	int col = m_player->getX();
	int row = m_player->getY();
	Direction playerDir = m_player->getDirection();
	
	switch (playerDir)
	{
	case Direction::right:
		for (int i = 0; i < PLAYER_SIZE; i++)								//	PLAYER_SIZE = 4 (occupies 2x2 squares)
			if (isEarthIndex(col+3, row+i)) {									//	Need to check if we can access given subscript
				if (m_earthList[col+3][row+i]) {
					delete m_earthList[col+3][row+i];						//	Delete 2x2 Earth objects
					m_earthList[col+3][row+i] = nullptr;

					playSound(SOUND_DIG);
				}
			}
		break;

	case Direction::left:
		for (int i = 0; i < PLAYER_SIZE; i++)
			if (isEarthIndex(col, row+i)) {
				if (m_earthList[col][row+i]) {
					delete m_earthList[col][row+i];
					m_earthList[col][row+i] = nullptr;

					playSound(SOUND_DIG);
				}
			}
		break;

	case Direction::up:
		for (int i = 0; i < PLAYER_SIZE; i++)
			if (isEarthIndex(col+i, row+3)) {
				if (m_earthList[col+i][row+3]) {
					delete m_earthList[col+i][row+3];
					m_earthList[col+i][row+3] = nullptr;

					playSound(SOUND_DIG);
				}
			}
		break;

	case Direction::down:
		for (int i = 0; i < PLAYER_SIZE; i++)
			if (isEarthIndex(col+i, row)) {
				if (m_earthList[col+i][row]) {
					delete m_earthList[col+i][row];
					m_earthList[col+i][row] = nullptr;

					playSound(SOUND_DIG);
				}
			}
		break;
	}
}
