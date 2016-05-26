#pragma once
#include <deque>
#include "Player.hpp"

enum eGameState { CHAT_SCREEN, START_GAME, GAME };
typedef void(*fp)(long delta);


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
	int turn;
	eGameState game_state;
	bool ready_player;
	bool ready_opponent;
	bool playFirst;
	bool orderDecided;
	std::deque<fp> action_pipeline;
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
	eGameState get_game_state() const { return game_state; }
	void set_game_state(eGameState state) { game_state = state; }
	Player& get_player() { return player; }
	Player& get_opponent() { return opponent; }
	bool is_player_ready() const { return ready_player; }
	bool is_opponent_ready() const { return ready_opponent; }
	void toggle_player_ready() { ready_player = !ready_player; }
	void toggle_opponent_ready() { ready_opponent = !ready_opponent; }
	bool getPlayFirst() const { return playFirst; }
	bool getorderDecided() const { return orderDecided; }
	void setPlayFirst(bool first) { playFirst = first; }
	void setOrderDecided(bool decided) { orderDecided = decided; }
	// Action Pipeline
	void queue_action(fp action) { action_pipeline.push_back(action); }
	void insert_next_action(fp action) { action_pipeline.push_front(action); }
	fp next_action() { return action_pipeline.front(); }
	void finish_action() { action_pipeline.pop_front(); }
	bool actions_remain() { return !action_pipeline.empty(); }
	//////////////////
	void add_played_index(int index) { played_indices.push_back(index); }
	bool has_unplayed_cards() { return !played_indices.empty(); }
	int get_unplayed_card();
	void set_end_turn(bool end) { end_turn = end; }
	bool is_turn_end() const { return end_turn; }
	bool is_my_turn() const { return my_turn; }
	void toggle_turn() { my_turn = !my_turn;	}
	Energy & get_energy_player() { return energy_player; }
	Energy & get_energy_opponent() { return energy_opponent; }
	void setup_attack(int attacker, int defender);
	int get_attacker() { return ID_attacker; }
	int get_defender() { return ID_defender; }
};

