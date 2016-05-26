
#include <string>
#include <iostream>
#include "Util.hpp"
#include "Networking.hpp"
#include "BoizStone.hpp"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
float depth = 2.25f;


std::string msg = "";
std::list<std::string> messages;

Player player;
Player opponent;
Data gameData(player, opponent);

std::list<std::pair<int, CardHolder>> drawing_cards;
std::list<std::pair<int, CardHolder>> playing_cards;
std::list<std::pair<int, CardHolder>> drawing_cards_opponent;
std::list<std::pair<int, CardHolder>> playing_cards_opponent;

CardHolder* hoverCard = nullptr;
CardHolder* attackingCard = nullptr;
int hoverID = 0;
int attackerID = 0;
bool cardGrabbed = false;
bool attack_drag = false;
float movX = 0.0f;
float movY = 0.0f;
float arrow_head_x = 0.0f;
float arrow_head_y = 0.0f;

CardCache cache;

void countDown(bool start, std::list<std::string>& messages, Data gameData)
{
	static int i = 5;
	if (!start) { i = 5; }
	else {
		messages.push_back(std::to_string(i));
		i--;
		if (i < -750) { gameData.set_game_state(START_GAME); }
	}
}

void decideOrder(Data& gameData)
{
	if (net_isServer()) {
		gameData.setPlayFirst((std::rand() % 2) != 0);
		//std::cout << std::to_string(gameData.getPlayFirst());
		send_order(gameData);
	}
}

void position_field(std::map<int, CardHolder>& field, bool my_side)
{
	int index = 1;
	float xInc = 0.175f;
	float y = 0.1f;
	if (my_side)
		y = -0.3f;
	typedef std::map<int, CardHolder>::iterator it_type;
	for (it_type it = field.begin(); it != field.end(); ++it, index++) {
		it->second.moveTo(-0.075f + xInc*(index / 2)*pow(-1, index), y, 0.0f);
	}
}

void position_hand(std::map<int, CardHolder>& hand) {
	int size = hand.size();
	float xInc = 0.175f;
	if (size > 5) { xInc = 0.75f / size; }
	//float width = size*0.15f;
	int index = 0;
	typedef std::map<int, CardHolder>::iterator it_type;
	for (it_type it = hand.begin(); it != hand.end(); ++it, index++) {
		it->second.moveTo(-0.4f + xInc*index, -0.75f, 1.0f);
		if (&(it->second) == hoverCard)
			hoverCard->moveTo(hoverCard->getX(), hoverCard->getY() + 0.25f, 1.24f);
	}
}

void position_hand_opponent(std::map<int, CardHolder>& hand) {
	int size = hand.size();
	float xInc = 0.175f;
	if (size > 5) { xInc = 0.75f / size; }
	//float width = size*0.15f;
	int index = 0;
	typedef std::map<int, CardHolder>::iterator it_type;
	for (it_type it = hand.begin(); it != hand.end(); ++it, index++) {
		it->second.moveTo(-0.4f + xInc*index, 0.75f, 0.5f);
	}
}

void drawApproach(long delta)
{
	for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards.begin(); it != drawing_cards.end(); ++it) {
		it->second.moveBy(-0.001f, -0.00005f*(delta), 0.0f);
		if (it->second.getX() < 0) {
			player.hand.insert(*it);
			drawing_cards.pop_front();
			position_hand(player.hand);
			request_repaint();
			break;
		}
		request_repaint();
	}
	for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards_opponent.begin(); it != drawing_cards_opponent.end(); ++it) {
		it->second.moveBy(0.001f, -0.00005f*(delta), 0.0f);
		if (it->second.getX() > 0) {
			opponent.hand.insert(*it);
			drawing_cards_opponent.pop_front();
			position_hand_opponent(opponent.hand);
			request_repaint();
			break;
		}
		request_repaint();
	}
}

void playApproach(long delta)
{
	//Disregard:
	// At the moment, the first block is obsolete, as the card-over-field animation brings the depth up beforehand.
	// It's a matter of taste, but I currently like the over-field animation, feels more like player's control.
	if (!playing_cards.empty()) {
		CardHolder * ch = &playing_cards.front().second;
		ch->moveBy(0.0f, 0.0f, -0.00025f*(delta));
		if (ch->getZ() <= 1.0f) {
			ch->moveTo(ch->getX(), ch->getY(), 0.0f);
			player.in_play.insert(playing_cards.front());
			playing_cards.pop_front();
			position_field(player.in_play, true);
			gameData.finish_action();
		}
		request_repaint();
	} else if (!playing_cards_opponent.empty()) {
		CardHolder * ch = &playing_cards_opponent.front().second;
		if (ch->getY() > 0.1f)
			ch->moveBy(0.0f, -0.0005f*(delta), 0.0005f*(delta));
		else
			ch->moveBy(0.0f, 0.0f, 0.0005f*(delta));
		if (ch->getZ() > 1.1f) {
			ch->moveBy(0.0f, 0.0f, -0.1f*(delta));
			opponent.in_play.insert(playing_cards_opponent.front());
			playing_cards_opponent.pop_front();
			position_field(opponent.in_play, false);
			gameData.finish_action();
		}
		request_repaint();
	}
}

CardHolder getCardFromDeck() {
	cache.loadCard(player.deck.getTopID());
	//cache.loadCard(0);
	Card nextCard = cache.get_card(player.deck.getTopID());
	CardHolder holder(nextCard, 0.85f, -0.5f, 1.0f, 1, true);
	player.deck.pop_front_card();
	return holder;
}

CardHolder getCardFromOpponentDeck() {
	cache.loadCard(opponent.deck.getTopID());
	//cache.loadCard(0);
	Card nextCard = cache.get_card(opponent.deck.getTopID());
	CardHolder holder(nextCard, -1.2f, 0.55f, 0.5f, 1, false);
	opponent.deck.pop_front_card();
	return holder;
}

void play_card(int ID)
{
	//activate abilities.
	auto it = player.hand.find(ID);
	playing_cards.push_back(*it);
	gameData.get_energy_player().alter_current(-(it->second.getCard().getCost()));
	player.hand.erase(it);
	position_hand(player.hand);
	gameData.insert_next_action(&playApproach);
	send_play_card(ID);
}

void opponent_play_card(long delta)
{
	//add to in_play if necessary.
	//activate abilities - wait for targets etc...?
	if (drawing_cards_opponent.empty()) {
		if (gameData.has_unplayed_cards()) {
			auto it = opponent.hand.find(gameData.get_unplayed_card());
			playing_cards_opponent.push_back(*it);
			gameData.get_energy_opponent().alter_current(-(it->second.getCard().getCost()));
			opponent.hand.erase(it);
			position_hand_opponent(opponent.hand);
			gameData.finish_action();
			gameData.insert_next_action(&playApproach);
		}
	}
}

void start_game() {
	if (net_isServer()) {
		for (int i = 0; i < 4; i++) {
			CardHolder start_card_player = getCardFromDeck();
			drawing_cards.push_back(std::pair<int, CardHolder>(start_card_player.getCard().getID(), start_card_player));
			CardHolder start_card_opponent = getCardFromOpponentDeck();
			drawing_cards_opponent.push_back(std::pair<int, CardHolder>(start_card_opponent.getCard().getID(), start_card_opponent));
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			CardHolder start_card_opponent = getCardFromOpponentDeck();
			drawing_cards_opponent.push_back(std::pair<int, CardHolder>(start_card_opponent.getCard().getID(), start_card_opponent));
			CardHolder start_card_player = getCardFromDeck();
			drawing_cards.push_back(std::pair<int, CardHolder>(start_card_player.getCard().getID(), start_card_player));
		}
	}
}

void end_turn(long delta) {
	if (gameData.is_my_turn()) {
		CardHolder holder = getCardFromOpponentDeck();
		drawing_cards_opponent.push_back(std::pair<int, CardHolder>(holder.getCard().getID(), holder));
		gameData.get_energy_opponent().alter_max(1);
		gameData.get_energy_opponent().set_current(gameData.get_energy_opponent().get_max());
		send_end_turn();
		for (auto &it : player.in_play) {
			it.second.getCard().set_can_attack(true);
		}
	}
	else {
		CardHolder holder = getCardFromDeck();
		drawing_cards.push_back(std::pair<int, CardHolder>(holder.getCard().getID(), holder));
		gameData.get_energy_player().alter_max(1);
		gameData.get_energy_player().set_current(gameData.get_energy_player().get_max());

	}
	gameData.finish_action();
	gameData.toggle_turn();
}

void attack(long delta) {
	CardHolder* attacker;
	CardHolder* defender;
	if (gameData.is_my_turn()) {
		attacker = &(player.in_play.at(gameData.get_attacker()));
		defender = &(opponent.in_play.at(gameData.get_defender()));
	}
	else {
		attacker = &(opponent.in_play.at(gameData.get_attacker()));
		defender = &(player.in_play.at(gameData.get_defender()));
	}
	if (attacker->moveToward(defender->getX(), defender->getY(), 0.0f, delta)) { // This is elegant, but really animation should be de-coupled from logic.
		defender->getCard().modifySpecs(0, -attacker->getCard().get_current_attack());
		attacker->getCard().modifySpecs(0, -defender->getCard().get_current_attack());

		if (gameData.is_my_turn()) {
			if (defender->getCard().get_current_health() <= 0) {
				opponent.in_play.erase(gameData.get_defender());
			}
			if (attacker->getCard().get_current_health() <= 0) {
				player.in_play.erase(gameData.get_attacker());
			}
		}
		else {
			if (defender->getCard().get_current_health() <= 0) {
				player.in_play.erase(gameData.get_defender());
			}
			if (attacker->getCard().get_current_health() <= 0) {
				opponent.in_play.erase(gameData.get_attacker());
			}
		}
		position_field(player.in_play, true);
		position_field(opponent.in_play, false);
		attacker->getCard().set_can_attack(false);
		gameData.finish_action();
	}
	request_repaint();
}

void end_game(long delta) {
	closeNetworking();
	std::exit(0);
}

//

// trim from start
std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}