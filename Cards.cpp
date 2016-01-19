

#include "Cards.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include "Util.hpp"


Card::Card(int ID, std::string type, short attack, short health, short cost, int abilities, int targets, int effects) 
	:ID(ID), type(type), attack(attack), health(health), cost(cost), 
	abilities(abilities), targets(targets), effects(effects),
	current_attack(attack), current_health(health), current_status(status)
{
	if (type == "Creature") {
		attack_flag = true;
	}
	else {
		attack_flag = false;
	}
}

Card::~Card()
{

}

void Card::modifySpecs(short attackChange, short healthChange) 
{
	this->current_attack += attackChange;
	this->current_health += healthChange;
}



CardHolder::CardHolder(Card& card, float x, float y, float z, float scale, bool face_up)
	:card(card), x(x), y(y), z(z), scale(scale), face_up(face_up)
{

}

bool CardHolder::collidesWithRay(float xs, float ys, float zs, float xe, float ye, float ze)
{
	float t = (this->z - zs) / (ze - zs);
	float x = xs + t*(xe - xs);
	float y = ys + t*(ye - ys);
	//std::cout << std::to_string(this->x)+" "+std::to_string(this->y) << std::endl;
	return (x > this->x && y > this->y && x < this->x + 0.15 && y < this->y + 0.3);
}

bool CardHolder::collidesWithPoint(float x, float y, float z)
{
	return (x > this->x && y > this->y && x < this->x + 0.15 && y < this->y + 0.3);
}

void CardHolder::moveBy(float x, float y, float z)
{
	this->x += x;
	this->y += y;
	this->z += z;
}

void CardHolder::moveTo(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


Deck::Deck()
{

}

Deck::Deck(int size, std::list<int> cards)
	:cardsRemaining(size)
{
	for (int i = 0; i < size; i++)
	{
		//Read Cards from file, with 10 bin digits representing ID code (links with image)
		//ATM i'm just going to use 'int' for ID code.
		this->cardIDs.push_back(i);
	}
}

CardCache::CardCache() 
	: numStored(0)
{

}

void CardCache::loadCard(int global_ID)
{
	std::string line;
	std::ifstream card_file("Cards.txt");
	if (card_file.is_open())
	{
		while (line != std::to_string(global_ID) && getline(card_file, line)){}
		getline(card_file, line);
		std::string type = line;
		getline(card_file, line);
		std::string name = line;
		getline(card_file, line);
		getline(card_file, line);
		short cost = (short) std::stoi(trim(line));
		getline(card_file, line);
		getline(card_file, line);
		short attack = (short) std::stoi(trim(line));
		getline(card_file, line);
		getline(card_file, line);
		short health = (short) std::stoi(trim(line));
		getline(card_file, line);
		getline(card_file, line);
		int ability = std::stoi(line);
		int target;
		int effect;
		if (ability != 0) {
			getline(card_file, line);
			getline(card_file, line);
			target = std::stoi(line);
			getline(card_file, line);
			getline(card_file, line);
			effect = std::stoi(line);
		}
		else {
			target = 0;
			effect = 0;
		}
		card_file.close();
		Card card(global_ID, type, attack, health, cost, ability, target, effect); // This ID will change when drawn.
		cached_cards.insert(std::pair<int, Card>(global_ID, card));
	}

	else std::cout << "Unable to open file";
}

Card& CardCache::get_card(int global_ID) 
{ 
	cached_cards.at(global_ID).set_play_ID(IDcount);
	IDcount++;
	return cached_cards.at(global_ID);
}