#pragma once

#include <list>
#include <map>


enum status {
	TAUNT		= 1,
	SPECBUFF	= 2,
	SPECDEBUFF	= 4,
	SILENCED	= 8
	//...
	};

enum abilities {// have a button in the maker for each ability, then a 
	//series of 'targets' and triggered effects as blocks.
	PASSIVE		 = 1,
	BATTLECRY	 = 2,
	CHARGE		 = 4,
	COMBO		 = 8,
	DEATHRATTLE	 = 16,
	ENRAGE		 = 32,
	PER_TURN	 = 64,
	//...
};

enum targets {
	PLAYER,
	ENEMY_PLAYER,
	PLAYER_CREATURE,
	ENEMY_CREATURE,
	ALL_CREATURES,
	ALL_PLAYER_CREATURES,
	ALL_ENEMY_CREATURES
};

enum effects {
	// one bit field for all effects, extra bit fields for magnitude (+3/+3 = 3 bit fields)
	// OR, one bit field, 1st 4 bits are magnitude in binary...
	MAGNITUDE1,
	MAGNITUDE2,
	MAGNITUDE3,
	MAGNITUDE4,
	DAMAGE,
	DEBUFFATTACK,
	BUFFATTACK,
	BUFFHEALTH,
	DISCARD_CARDS,

};

class Card {
	short attack;
	short health;
	int status; // bit-field.
	int abilities; // bit-field.
	int targets; // bit-field.
	int effects; // bit-field.
	int position_in_deck;
	int ID;
public:
	Card(int ID, short attack, short health, int abilities, int targets, int effects);
	Card(std::string line);
	~Card();
	short getAttack() const { return attack; }
	short getHealth() const { return health; }
	/*int getStatus() const { return attack; }
	bool hasStatus() { return attack; }*/
	int getAbilities() const { return abilities; }
	int getTargets() const { return targets; }
	int getEffects() const { return effects; }
	int getPosition_in_deck() const { return position_in_deck; }
	int getID() const { return ID; }
	void modifySpecs(int attackChange, int healthChange);
	void modifyStatus(int statusChanges);
	void alter_position_in_deck(int newPosition);
};

class CardHolder{
	float x, y, z;
	float scale;
	Card& card;
	//bool highlight;
	//Border
	//blah blah
public:
	//pass by ref?????????????????????????????????????
	CardHolder(Card& card, float x, float y, float z, float scale);
	Card& getCard() const { return card; }
	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }
	float getScale() const { return scale; }
	void moveBy(float x, float y, float z);
	void moveTo(float x, float y, float z);
	void resize(float scale);
	bool collidesWithRay(float xs, float ys, float zs, float xe, float ye, float ze); // seems like this will be tricky with perspectives
	//bool isHighlighted() const { return highlight; }
	//void setHighlight(bool on) { highlight = on; }
};

//class Hand { ///// If all it's going to do is position them, may as well do it with a function.
//	int size;
//	int max;
//	std::list<CardHolder&> holders;
//public:
//	Hand(std::list<CardHolder&>& holders);
//	void addHolder(CardHolder& holder) { holders.push_back(holder); }
//	void removeHolder(CardHolder& holder) { holders.remove(holder); }
//	std::list<CardHolder&>& getHolders() { return holders; }
//	void positionCards();
//};

class Deck {
	int cardsRemaining;
	std::list<int> cardIDs;
public:
	Deck(int size, std::list<int> cardIDs);
	int getTopID();
	void shuffle();
	void discard(int num);
	//void readDeckFromFile(std::string filename);
};

class DiscardPile {
	int size;
	std::list<int> cardIDs;
public:
	DiscardPile();
	void addCard(int CardID);
};

class CardCache {
	int numStored;
	std::map<int, Card> cardMap;
	//std::map<int, image> imageMap;
public:
	void loadCard(int);
};
