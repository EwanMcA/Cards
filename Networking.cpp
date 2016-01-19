#include <stdio.h>
#include <tchar.h>
#define _WINSOCKAPI_
#include <Windows.h>
#include <iostream>
#include "RakPeerInterface.h"
#include <string>
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "Networking.hpp"
#include "RakNetTypes.h"  // MessageID

#define MAX_CLIENTS 1
#define SERVER_PORT 60000

RakNet::RakPeerInterface* peer = NULL;
bool isServer;
bool isConnected = false;
RakNet::Packet *packet;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_TEXT,
	ID_READY,
	ID_ORDER,
	ID_PLAY_CARD,
	ID_END_TURN,
	ID_ATTACK
};

void initServer(const int port)
{
	RakNet::SocketDescriptor sd(port, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	isServer = true;
	std::cout << "Starting the server." << std::endl;
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);
}

std::string initClient(const std::string& ipString, const int port)
{
	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	isServer = false;

	std::cout << "Starting the client." << std::endl;
	char *chstr = new char[ipString.length() + 1];

	std::strcpy(chstr, ipString.c_str());
	RakNet::ConnectionAttemptResult result = peer->Connect(chstr, port, 0, 0);
	delete[] chstr;
	switch (result)
	{
	case RakNet::CONNECTION_ATTEMPT_STARTED:
		return std::string("Attempting connection to \"") + ipString + std::string("\"...");
		break;
	case RakNet::INVALID_PARAMETER:
		return "Connection attempt failed to start:  Invalid parameter.";
		break;
	case RakNet::CANNOT_RESOLVE_DOMAIN_NAME:
		return "Connection attempt failed to start:  Cannot resolve domain name.";
		break;
	case RakNet::ALREADY_CONNECTED_TO_ENDPOINT:
		return "Connection attempt failed to start:  Already connected.";
		break;
	case RakNet::CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS:
		return "Connection attempt failed to start:  Already attemping...";
		break;
	case RakNet::SECURITY_INITIALIZATION_FAILED:
		return "Connection attempt failed to start:  Security could not be initialized.";
		break;
	}
	return "";
}

void initNetwork(bool test) 
{
//	closeNetworking();
	std::string input_str;
	peer = RakNet::RakPeerInterface::GetInstance();
	if (test) { initServer(SERVER_PORT); 
	isConnected = (peer->NumberOfConnections() > 0);
	return;
	}
	std::cout << "(C) or (S)erver?" << std::endl;
	std::getline(std::cin,input_str);
	if ((input_str[0] == 'c') || (input_str[0] == 'C'))
	{
		std::string str;
		//This is LAN ?
		std::cout << "Enter server IP or hit enter for 127.0.0.1";
		std::getline(std::cin, str);
		if (str.size()==0) {
			str = "127.0.0.1";
		}
		std::cout << initClient(str, SERVER_PORT) << std::endl;
	}
	else {
		initServer(SERVER_PORT);
	}
	isConnected = (peer->NumberOfConnections() > 0);

}

std::string receivePackets_ChatScreen()
{
	std::string result;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			std::cout << "Our connection request has been accepted." << std::endl;
			// Use a BitStream to write a custom user message
			// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			bsOut.Write("Hello Ewan!");
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (isServer) {
				printf("A client has disconnected.\n");
			}
			else {
				printf("We have been disconnected.\n");
			}
			break;
		case ID_CONNECTION_LOST:
			if (isServer) {
				printf("A client lost the connection.\n");
			}
			else {
				printf("Connection lost.\n");
			}
			break;
		case ID_GAME_MESSAGE_1:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			printf("%s\n", rs.C_String());
		}
		break;
		case ID_TEXT:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			result = rs.C_String();
		}
		break;
		case ID_READY:
		{
			result = "READY";
		}
		break;
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
	isConnected = (peer->NumberOfConnections() > 0);
	return (std::string) result;
}

void receivePackets_Game(Data& gameData, std::deque<fp>& action_pipeline)
{
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			std::cout << "Our connection request has been accepted." << std::endl;
			// Use a BitStream to write a custom user message
			// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			bsOut.Write("Hello Ewan!");
			peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (isServer) {
				printf("A client has disconnected.\n");
			}
			else {
				printf("We have been disconnected.\n");
			}
			break;
		case ID_CONNECTION_LOST:
			if (isServer) {
				printf("A client lost the connection.\n");
			}
			else {
				printf("Connection lost.\n");
			}
			break;
		case ID_GAME_MESSAGE_1:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			printf("%s\n", rs.C_String());
		}
		break;
		case ID_PLAY_CARD:
		{
			int index;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(index);
			gameData.add_played_index(index);
			action_pipeline.push_back(&opponent_play_card);
		}
		break;
		case ID_END_TURN:
		{
			action_pipeline.push_back(&end_turn);
		}
		break;
		case ID_ATTACK:
		{
			int attacker;
			int defender;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(attacker);
			bsIn.Read(defender);
			gameData.setup_attack(attacker, defender);
			action_pipeline.push_back(&attack);
			break;
		}
		case ID_ORDER:
		{
			bool playFirst;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(playFirst);
			gameData.setPlayFirst(!playFirst);
		}
		break;
		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
	isConnected = (peer->NumberOfConnections() > 0);
}

// Some helper wrappers
RakNet::SystemAddress getConnectedSystem(int index = 0);
void sendPacket(const RakNet::BitStream& bsOut, PacketReliability reliability = RELIABLE_ORDERED);

RakNet::SystemAddress getConnectedSystem(int index)
{
	if (peer == NULL)
		return RakNet::SystemAddress();
	return peer->GetSystemAddressFromIndex(index);
}

void sendPacket(const RakNet::BitStream& bsOut, PacketReliability reliability)
{
	if (!isConnected)
		return;

	peer->Send(&bsOut, HIGH_PRIORITY, reliability, 0, getConnectedSystem(), false);
}

// This isn't terribly reliable for real-time disconnects, but works fine for this kind of game.
bool net_IsConnected()
{
	return isConnected;
}

bool net_isServer()
{
	return isServer;
}

void send_ready() 
{
	if (peer == NULL || !isConnected)
		return;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_READY);
	sendPacket(bsOut);
}


void send_text(const std::string& text)
{
	if (peer == NULL || !isConnected)
		return;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_TEXT);
	bsOut.Write(RakNet::RakString(text.c_str()));

	sendPacket(bsOut);
}

void send_order(Data& gameData) 
{
	if (peer == NULL || !isConnected)
		return;
	bool playFirst = gameData.getPlayFirst();
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_ORDER);
	bsOut.Write(!playFirst);

	sendPacket(bsOut);
}

void closeNetworking()
{
	peer->Shutdown(500, 0, LOW_PRIORITY);
	RakNet::RakPeerInterface::DestroyInstance(peer);
	peer = NULL;
}

// Game Mechanics

void send_play_card(int index)
{
	if (peer == NULL || !isConnected)
		return;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_PLAY_CARD);
	bsOut.Write(index);
	sendPacket(bsOut);
}

void send_end_turn()
{
	if (peer == NULL || !isConnected)
		return;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_END_TURN);
	sendPacket(bsOut);
}

void send_attack(int attacker, int defender) 
{
	if (peer == NULL || !isConnected)
		return;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_ATTACK);
	bsOut.Write(attacker);
	bsOut.Write(defender);
	sendPacket(bsOut);
}



