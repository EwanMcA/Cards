#pragma once

#include <list>
#include "GameData.hpp"
#include "Cards.hpp"
#include "Player.hpp"


extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern std::string msg;
extern std::list<std::string> messages;

extern float depth;

extern std::list<std::pair<int, CardHolder>> drawing_cards;
extern std::list<std::pair<int, CardHolder>> playing_cards;
extern std::list<std::pair<int, CardHolder>> drawing_cards_opponent;
extern std::list<std::pair<int, CardHolder>> playing_cards_opponent;

extern CardHolder* hoverCard;
extern CardHolder* attackingCard;
extern int hoverID;
extern int attackerID;
extern bool cardGrabbed;
extern bool attack_drag;
extern float movX;
extern float movY;
extern float arrow_head_x;
extern float arrow_head_y;

extern CardCache cache;
extern Player player;
extern Player opponent;
extern Data gameData;


void countDown(bool start, std::list<std::string>& messages, Data gameData);
void decideOrder(Data& gameData);


void position_field(std::map<int, CardHolder>& in_play, bool my_side);
void position_hand(std::map<int, CardHolder>& hand);
void position_hand_opponent(std::map<int, CardHolder>& hand);

CardHolder getCardFromDeck();
CardHolder getCardFromOpponentDeck();

void play_card(int index);
void opponent_play_card(long delta);

void start_game();

void drawApproach(long delta);
void playApproach(long delta);
void attack(long delta);
void end_turn(long delta);

void end_game(long delta);

//

extern std::string &ltrim(std::string &s);

// trim from end
extern std::string &rtrim(std::string &s);

// trim from both ends
extern std::string &trim(std::string &s);