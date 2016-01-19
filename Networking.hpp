#pragma once

#include <string>
#include <list>
#include "GameData.hpp"
#include "Util.hpp"

void initNetwork(bool test);
void initServer(const int port);
std::string initClient(const std::string& ipString, const int port);

void closeNetworking();

//bool isConnected();
bool net_isServer();


// Send packets
//void net_GoToNetMenu();
//void net_StartGame();
void send_ready();
void send_text(const std::string& text);
void send_order(Data& gameData);
void send_play_card(int index);
void send_attack(int attacker, int defender);
void send_end_turn();
//void net_GameReset();
//void net_GameQuit();
//void net_GameOver(int winningPlayer);
//
//void net_SendPing();
//int net_GetPing();

// Handle packets

std::string receivePackets_ChatScreen();
void receivePackets_Game(Data& gameData, std::deque<fp>& action_pipeline);

//void checkPackets_Gameloop(GameData& data);
//void checkPackets_GameOver(GameData& data, bool& done);
//void checkPackets_ConnectMenu(bool& startGame, std::list<std::string>& messages);
//std::string checkPackets_NetMenu(bool& startGame);

