
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

std::string msg = "";
std::list<std::string> messages;
int turn = 0;
bool ready = false;
bool opponentReady = false;

Player player;
Player opponent;
Data gameData(player, opponent);
std::deque<fp> action_pipeline;

std::map<int, CardHolder> hand;
std::map<int, CardHolder> in_play;
std::list<std::pair<int, CardHolder>> drawing_cards;
std::list<std::pair<int, CardHolder>> playing_cards;
std::map<int, CardHolder> hand_opponent;
std::map<int, CardHolder> in_play_opponent;
std::list<std::pair<int, CardHolder>> drawing_cards_opponent;
std::list<std::pair<int, CardHolder>> playing_cards_opponent;

eGameState currentGameState;

float depth = 2.25f;

//Card ewan(1, 7, 5, 1, 2, 2, 2);
//CardHolder ch(ewan, 0.0f, -0.35f, 0.0f, 1, );
//CardHolder ch1(ewan, -0.3f, -0.6f, 1.0f, 1);
 // camera z position.

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
Deck deck_player;


void startGame(bool start, std::list<std::string>& messages)
{
	static int i = 5;
	if (!start) { i = 5; }
	else {
		messages.push_back(std::to_string(i));
		i--;
		if (i < -750) { currentGameState = START_GAME; }
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

void drawApproach()
{
	for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards.begin(); it != drawing_cards.end(); ++it) {
		it->second.moveBy(-0.001f, -0.00005f, 0.0f);
		if (it->second.getX() < 0) {
			hand.insert(*it);
			drawing_cards.pop_front();
			position_hand(hand);
			request_repaint();
			break;
		}
		request_repaint();
	}
	for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards_opponent.begin(); it != drawing_cards_opponent.end(); ++it) {
		it->second.moveBy(0.001f, -0.00005f, 0.0f);
		if (it->second.getX() > 0) {
			hand_opponent.insert(*it);
			drawing_cards_opponent.pop_front();
			position_hand_opponent(hand_opponent);
			request_repaint();
			break;
		}
		request_repaint();
	}
}

void playApproach()
{
	//Disregard:
	// At the moment, the first block is obsolete, as the card-over-field animation brings the depth up beforehand.
	// It's a matter of taste, but I currently like the over-field animation, feels more like player's control.
	if (!playing_cards.empty()) {
		CardHolder * ch = &playing_cards.front().second;
		ch->moveBy(0.0f, 0.0f, -0.00025f);
		if (ch->getZ() <= 1.0f) {
			ch->moveTo(ch->getX(), ch->getY(), 0.0f);
			in_play.insert(playing_cards.front());
			playing_cards.pop_front();
			position_field(in_play, true);
			action_pipeline.pop_front();
		}
		request_repaint();
	} else if (!playing_cards_opponent.empty()) {
		CardHolder * ch = &playing_cards_opponent.front().second;
		if (ch->getY() > 0.1f)
			ch->moveBy(0.0f, -0.0005f, 0.0005f);
		else
			ch->moveBy(0.0f, 0.0f, 0.0005f);
		if (ch->getZ() > 1.1f) {
			ch->moveBy(0.0f, 0.0f, -0.1f);
			in_play_opponent.insert(playing_cards_opponent.front());
			playing_cards_opponent.pop_front();
			position_field(in_play_opponent, false);
			action_pipeline.pop_front();
		}
		request_repaint();
	}
}

CardHolder getCardFromDeck() {
	//cache.loadCard(deck_player.getTopID())
	cache.loadCard(0);
	Card & nextCard = cache.get_card(0);
	CardHolder holder(nextCard, 0.85f, -0.5f, 1.0f, 1, true);
	return holder;
}

CardHolder getCardFromOpponentDeck() {
	//cache.loadCard(deck_opponent.getTopID())
	cache.loadCard(0);
	Card & nextCard = cache.get_card(0);
	CardHolder holder(nextCard, -1.2f, 0.55f, 0.5f, 1, false);
	return holder;
}

void play_card(int ID)
{
	//activate abilities.
	auto it = hand.find(ID);
	playing_cards.push_back(*it);
	gameData.get_energy_player().alter_current(-(it->second.getCard().getCost()));
	hand.erase(it);
	position_hand(hand);
	action_pipeline.push_back(&playApproach);
	send_play_card(ID);
}

void opponent_play_card()
{
	//std::cout << std::to_string(ID) << std::endl;
	//add to in_play if necessary.
	//activate abilities - wait for targets etc...?
	if (gameData.has_unplayed_cards()) {
		auto it = hand_opponent.find(gameData.get_unplayed_card());
		playing_cards_opponent.push_back(*it);
		gameData.get_energy_opponent().alter_current(-(it->second.getCard().getCost()));
		hand_opponent.erase(it);
		position_hand_opponent(hand_opponent);
		action_pipeline.push_back(&playApproach);
		action_pipeline.pop_front();
	}

}

void end_turn() {
	if (gameData.is_my_turn()) {
		CardHolder holder = getCardFromOpponentDeck();
		drawing_cards_opponent.push_back(std::pair<int, CardHolder>(holder.getCard().getID(), holder));
		gameData.get_energy_opponent().alter_max(1);
		gameData.get_energy_opponent().set_current(gameData.get_energy_opponent().get_max());

		send_end_turn();
	}
	else {
		CardHolder holder = getCardFromDeck();
		drawing_cards.push_back(std::pair<int, CardHolder>(holder.getCard().getID(), holder));
		gameData.get_energy_player().alter_max(1);
		gameData.get_energy_player().set_current(gameData.get_energy_player().get_max());
	}
	action_pipeline.pop_front();
	gameData.toggle_turn();
}

//void player_attack()
//{
//	// how to check for taunt etc... ?
//	if ((hoverCard->getCard().get_current_health() - attackingCard->getCard().get_current_attack()) <= 0) {
//		// kill.
//	} else {
//		hoverCard->getCard().modifySpecs(0, -attackingCard->getCard().get_current_attack());
//	}
//	if ((attackingCard->getCard().get_current_health() - hoverCard->getCard().get_current_attack()) <= 0) {
//		// kill.
//	}
//	else {
//		attackingCard->getCard().modifySpecs(0, -hoverCard->getCard().get_current_attack());
//	}
//}

void attack() {
	CardHolder* attacker;
	CardHolder* defender;
	std::cout << std::to_string(gameData.get_attacker()) << std::endl;
	if (gameData.is_my_turn()) {
		attacker = &(in_play.at(gameData.get_attacker()));
		defender = &(in_play_opponent.at(gameData.get_defender()));
	}
	else {
		attacker = &(in_play_opponent.at(gameData.get_attacker()));
		defender = &(in_play.at(gameData.get_defender()));
	}

	defender->getCard().modifySpecs(0, -attacker->getCard().get_current_attack());
	attacker->getCard().modifySpecs(0, -defender->getCard().get_current_attack());

	if (gameData.is_my_turn()) {
		if (defender->getCard().get_current_health() <= 0) {
			in_play_opponent.erase(gameData.get_defender());
		}
		if (attacker->getCard().get_current_health() <= 0) {
			in_play.erase(gameData.get_attacker());
		}
	}
	else {
		if (defender->getCard().get_current_health() <= 0) {
			in_play.erase(gameData.get_defender());
		}
		if (attacker->getCard().get_current_health() <= 0) {
			in_play_opponent.erase(gameData.get_attacker());
		}
	}
	
	action_pipeline.pop_front();
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