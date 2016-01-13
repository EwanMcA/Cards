#pragma once

#include <string>
#include <list>
#include "GameData.hpp"

void initNetwork(bool test);
void initServer(const int port);
std::string initClient(const std::string& ipString, const int port);

void closeNetworking();

//bool isConnected();
bool net_isServer();


// Send packets
//void net_GoToNetMenu();
//void net_StartGame();
void sendReady();
void sendText(const std::string& text);
void sendOrder(Data& gameData);
//void net_GameAttack(int playerNum, int button);
//void net_GameBlock(int playerNum, int button, int reactionMilliseconds);
//void net_GameReset();
//void net_GameQuit();
//void net_GameOver(int winningPlayer);
//
//void net_SendPing();
//int net_GetPing();

// Handle packets

std::string receivePackets_ChatScreen();
void receivePackets_Game(Data& gameData);

//void checkPackets_Gameloop(GameData& data);
//void checkPackets_GameOver(GameData& data, bool& done);
//void checkPackets_ConnectMenu(bool& startGame, std::list<std::string>& messages);
//std::string checkPackets_NetMenu(bool& startGame);

