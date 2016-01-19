#pragma once
#include <deque>
#include "Player.hpp"

class Energy {
	short max = 0;
	short current = 0;
public:
	Energy();
	short get_max() const { return max; }
	short get_current() const { return current; }
	void alter_max(short amount) { max += amount; }
	void alter_current(short amount) { current += amount; }
	void set_current(short at) { current = at; }
};

class Data {
	bool playFirst;
	bool orderDecided;
	std::deque<int> played_indices;
	bool my_turn;
	bool end_turn;
	Energy energy_player;
	Energy energy_opponent;
	Player& player;
	Player& opponent;
	int ID_attacker;
	int ID_defender;
public:
	Data(Player& player, Player& opponent);
	~Data();
	Player& get_player() const { return player; }
	Player& get_opponent() const { return opponent; }
	bool getPlayFirst() const { return playFirst; }
	bool getorderDecided() const { return orderDecided; }
	void setPlayFirst(bool first) { playFirst = first; }
	void setOrderDecided(bool decided) { orderDecided = decided; }
	void add_played_index(int index) { played_indices.push_back(index); }
	bool has_unplayed_cards() { return !played_indices.empty(); }
	int get_unplayed_card();
	void set_end_turn(bool end) { end_turn = end; }
	bool is_turn_end() const { return end_turn; }
	bool is_my_turn() const { return my_turn; }
	void toggle_turn() { 
		//std::cout << std::to_string(my_turn);
		my_turn = !my_turn;
		//std::cout << std::to_string(my_turn);
	}
	Energy & get_energy_player() { return energy_player; }
	Energy & get_energy_opponent() { return energy_opponent; }
	void setup_attack(int attacker, int defender);
	int get_attacker() { return ID_attacker; }
	int get_defender() { return ID_defender; }
};

