
#include "GameData.hpp"

Data::Data(Player& player, Player& opponent)
	:playFirst(false), orderDecided(false), my_turn(false), player(player), opponent(opponent), energy_player(), energy_opponent()
{

}

Data::~Data()
{

}

int Data::get_unplayed_card()
{
	int hand_index = played_indices.front();
	played_indices.pop_front();
	return hand_index;  
}

void Data::setup_attack(int attacker, int defender) {
	ID_attacker = attacker;
	ID_defender = defender;
}

Energy::Energy()
{

}