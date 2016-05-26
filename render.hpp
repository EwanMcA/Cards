#pragma once

#include <string>
#include "Cards.hpp"
#include "GameData.hpp"
#include "Player.hpp"


void renderBitmapString(float x, float y, float z, void *font, const char *string);

GLuint load_texture(const char * filename);

void renderConnectionMenu(bool ready, std::string currentMessage, std::list<std::string>& messages);

void renderTextEntryBox(float x, float y, float z, float width, float height, std::string currentMessage);

void renderMessageDisplayBox(float x, float y, float z, float width, float height, std::list<std::string>& messages);

void renderReadyButton(bool ready, float x, float y, float z, float width, float height);

void renderGameBoard(int SCREEN_WIDTH, int SCREEN_HEIGHT, Data& gameData);

void renderPlayer(Player &player, float x, float y, float z);

void render_end_turn_button(float x, float y, float z, float scale);

void renderEnergy(short energy, float x, float y, float z);

void renderCard(CardHolder ch, bool face_up, bool highlight);

void renderDeck(int size, float x, float y, float z, float scale);

void renderArrow(float xs, float ys, float zs, float xe, float ye, float ze);

//void renderDiscardPile(int size, float x, float y, float z, float scale);