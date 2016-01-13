

#include "Cards.hpp"
#include <iostream>
#include <string>

Card::Card(int ID, short attack, short health, int abilities, int targets, int effects) 
{
	this->ID = ID;
	this->attack = attack;
	this->health = health;
	this->abilities = abilities;
	this->targets = targets;
	this->effects = effects;
}

Card::~Card()
{

}

void Card::modifySpecs(int attackChange, int healthChange) 
{
	this->attack += attackChange;
	this->health += healthChange;
}

void Card::alter_position_in_deck(int newPosition)
{
	position_in_deck = newPosition;
}



CardHolder::CardHolder(Card& card, float x, float y, float z, float scale)
	:card(card), x(x), y(y), z(z), scale(scale)
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



Deck::Deck(int size, std::list<int> cards)
	:cardsRemaining(size)
{
	for (int i = 0; i < size; i++)
	{
		//Read Cards from file, with 10 bin digits representing ID code (links with image)
		//ATM i'm just going to use 'int' for ID code.
		int ID;
		this->cardIDs.push_back(ID);
	}
}