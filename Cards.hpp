#pragma once

#include <list>
#include <map>


enum status {
	STATUS_TAUNT		= 1,
	STATUS_SPECBUFF		= 2,
	STATUS_SPECDEBUFF	= 4,
	STATUS_SILENCE		= 8,
	STATUS_STEALTH		= 16,
	//...
	};

enum abilities {// have a button in the maker for each ability, then a 
	//series of 'targets' and triggered effects as blocks.
	WHEN_PLAYED  = 1, // This covers most spells.
	PASSIVE		 = 2, // i.e. while on the field all others get +1/+1
	BATTLECRY	 = 4,
	CHARGE		 = 8,
	COMBO		 = 16,
	DEATHRATTLE	 = 32,
	ENRAGE		 = 64,
	PER_TURN	 = 128,
	//...
};

enum targets {
	PLAYER				 = 1,
	ENEMY_PLAYER		 = 2,
	ENEMY_ENTITY		 = 4,
	PLAYER_CREATURE		 = 8,
	ENEMY_CREATURE		 = 16,
	ALL_CREATURES		 = 32,
	ALL_PLAYER_CREATURES = 64,
	ALL_ENEMY_CREATURES	 = 128,
	ALL_ENEMIES			 = 256,
	ALL_ENTITIES		 = 512
};

enum effects {
	// one bit field for all effects, extra bit fields for magnitude (+3/+3 = 3 bit fields)
	// OR, one bit field, 1st 4 bits are magnitude in binary...
	MAGNITUDE1 = 1,
	MAGNITUDE2 = 2,
	MAGNITUDE3 = 4,
	MAGNITUDE4 = 8,
	DAMAGE = 16,
	HEAL = 32,
	DEBUFFATTACK = 64,
	BUFFATTACK = 128,
	BUFFHEALTH = 256,
	TAUNT = 512,
	SILENCE = 1024,
	STEALTH = 2048,
	DISCARD_CARDS = 4096,

};

class Card { // eventually might want to make 2 classes - card (predefined/unchanging) and entity (playable/mutable)
	std::string type;
	short attack;
	short health;
	short cost;
	int status; // bit-field.
	int abilities; // bit-field.
	int targets; // bit-field.
	int effects; // bit-field.
	int ID;

	bool attack_flag;
	short current_attack;
	short current_health;
	int current_status;
public:
	Card(int ID, std::string type, short attack, short health, short cost, int abilities, int targets, int effects);
	Card(std::string line);
	~Card();
	void set_play_ID(int ID) { this->ID = ID; }
	short get_current_attack() const { return current_attack; }
	short get_current_health() const { return current_health; }
	short get_attack() const { return attack; }
	short get_health() const { return health; }
	short getCost() const { return cost; }
	int getAbilities() const { return abilities; }
	int getTargets() const { return targets; }
	int getEffects() const { return effects; }
	int getID() const { return ID; }
	void modifySpecs(short attackChange, short healthChange);
	void modifyStatus(int statusChanges);
	void alter_position_in_deck(int newPosition);

	bool can_attack() const { return attack_flag; }
	void set_can_attack(bool canAttack) { attack_flag = canAttack; }
};

class CardHolder{
	float x, y, z;
	float scale;
	Card& card;
	bool face_up;
	//Border
	//blah blah
public:
	CardHolder(Card& card, float x, float y, float z, float scale, bool face_up);
	Card& getCard() const { return card; }
	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }
	float getScale() const { return scale; }
	void moveBy(float x, float y, float z);
	void moveTo(float x, float y, float z);
	void resize(float scale) { this->scale = scale; }
	bool collidesWithRay(float xs, float ys, float zs, float xe, float ye, float ze); // seems like this will be tricky with perspectives
	bool collidesWithPoint(float x, float y, float z);
	bool is_face_up() { return face_up; }
	bool set_face_up(bool up) { face_up = up; }
	void set_z(float z) { this->z = z; }

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
	Deck();
	Deck(int size, std::list<int> cardIDs);
	int getTopID() const { return cardIDs.front(); }
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
	int numStored = 0;
	int IDcount = 0;
	std::map<int, Card> cached_cards;
	//std::map<int, image> imageMap;
public:
	CardCache();
	void loadCard(int ID);
	Card& get_card(int global_ID);
};
