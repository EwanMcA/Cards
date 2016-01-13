#pragma once

#include <list>
#include "GameData.hpp"
#include "Cards.hpp"

void startGame(bool start, std::list<std::string>& messages);
void decideOrder(Data& gameData);

float * getRayFromMouse(int x, int y);

void positionHand(std::vector<CardHolder>& hand);
void positionOpponentHand(std::vector<CardHolder>& hand);

CardHolder getCardFromDeck();
CardHolder getCardFromOpponentDeck();

void play_card(CardHolder &holder);

void cardApproach();