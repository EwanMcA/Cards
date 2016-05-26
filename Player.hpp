#pragma once

#include <map>
#include "Cards.hpp"

class Player {
	short health;
public:
	Player();
	short getHealth() const { return health; }
	void alter_health(short change) { health += change; }
	std::map<int, CardHolder> hand;
	std::map<int, CardHolder> in_play;
	Deck deck;
	//field
	//deck
	//discard
};