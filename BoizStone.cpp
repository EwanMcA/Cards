#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <GL\glew.h>
#include <freeglut.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <deque>

#include "BoizStone.hpp"
#include "Networking.hpp"
#include "GameData.hpp"
#include "render.hpp"
#include "Cards.hpp"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

std::string msg = "";
std::list<std::string> messages;

enum eGameState {CHAT_SCREEN, START_GAME, GAME};
eGameState currentGameState;

bool ready;
bool opponentReady;
int turn = 0;

Data gameData;
Card ewan(1, 8, 8, 2, 2, 2);
CardHolder ch(ewan, 0.0f, -0.35f, 0.0f, 1);
CardHolder ch1(ewan, -0.3f, -0.6f, 1.0f, 1);

std::vector<CardHolder> hand;
std::vector<CardHolder> in_play;
std::deque<CardHolder> drawing_cards;
std::vector<CardHolder> hand_opponent;
std::vector<CardHolder> in_play_opponent;
std::deque<CardHolder> drawing_cards_opponent;
float depth = 2.25f; // camera z position.

CardHolder* hoverCard = nullptr;
bool cardGrabbed = false;
float movX = 0.0f;
float movY = 0.0f;

void changeViewPort(int width, int height)
{
	if (height == 0)
		height = 1;
	float ratio = 1.0f* width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(50.0f, ratio, 1, 10);
	glMatrixMode(GL_MODELVIEW);
}

void render()
{
	glClearColor(0.38f, 0.32f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, depth, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	switch (currentGameState) {
	case CHAT_SCREEN: 
		renderConnectionMenu(ready, msg, messages);
		break;
	case  START_GAME:
		renderGameBoard(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT);
		// render something to see play order.
		break;
	case GAME:
		renderGameBoard(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT);
		for (std::vector<CardHolder>::size_type i = 0; i < hand.size(); i++) {
			renderCard(hand.at(i), true, hoverCard == &hand.at(i));
		}
		for (std::vector<CardHolder>::size_type i = 0; i < in_play.size(); i++) {
			renderCard(in_play.at(i), true, hoverCard == &in_play.at(i));
		}
		for (std::deque<CardHolder>::size_type i = 0; i < drawing_cards.size(); i++) {
			renderCard(drawing_cards.at(i), true, false);
		}
		for (std::vector<CardHolder>::size_type i = 0; i < hand_opponent.size(); i++) {
			renderCard(hand_opponent.at(i), false, hoverCard == &hand_opponent.at(i));
		}
		for (std::vector<CardHolder>::size_type i = 0; i < in_play_opponent.size(); i++) {
			renderCard(in_play_opponent.at(i), true, hoverCard == &in_play_opponent.at(i));
		}
		for (std::deque<CardHolder>::size_type i = 0; i < drawing_cards_opponent.size(); i++) {
			renderCard(drawing_cards_opponent.at(i), false, false);
		}
		if (cardGrabbed) { renderCard(*hoverCard, true, true); }
		/*if (gameData.getPlayFirst()) {glColor3f(1.0, 0.0, 0.0);}
		else {glColor3f(0.0, 0.0, 1.0);}*/
		//glutSolidCube(0.1);
		break;
	};
	glutSwapBuffers();
}

void idleFunction() 
{
	//receivePackets loops through all packets.
	std::string incMsg;
	switch (currentGameState) {
	case CHAT_SCREEN:
		incMsg = receivePackets_ChatScreen();
		if (incMsg == "READY") {
			opponentReady = opponentReady ? false : true;
			//std::cout << std::to_string(opponentReady);
		}
		else {
			if (incMsg.size() > 0) {
				messages.push_back("Friend: " + incMsg);
				glutPostRedisplay();
			}
		}
		startGame((opponentReady && ready), messages);
		// erase old messages that no longer fit.
		if (messages.size()*0.05 >= 1.49) {
			messages.pop_front();
			glutPostRedisplay();
		}
		break;
	case START_GAME:
		receivePackets_Game(gameData);
		decideOrder(gameData);
		//initialDraw();
		currentGameState = GAME;
		glutPostRedisplay();
		break;
	case GAME:
		receivePackets_Game(gameData);
		break;
	}
	incMsg = "";
	cardApproach(); // animated card-draw;
	if (drawing_cards.size() > 0 || drawing_cards_opponent.size() > 0) { glutPostRedisplay(); }
}

void keyPress(unsigned char key, int mousex, int mousey) 
{

	switch (key) {
	case 13: 
		messages.push_back("You: "+msg);
		sendText(msg);
		msg = "";
		break;
	case 8:
		if (msg.size() > 0) { msg.pop_back(); }
		break;
	default:
		msg += key;
	}
	glutPostRedisplay();
}

void specPress(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		depth += 0.05f;
	}
	else if (key == GLUT_KEY_DOWN) {
		depth -= 0.05f;
	}
	else if (key == GLUT_KEY_RIGHT) {
		drawing_cards.push_back(getCardFromDeck());
		drawing_cards_opponent.push_back(getCardFromOpponentDeck());
		positionHand(hand);
		positionOpponentHand(hand_opponent);
	}
	else if (key == GLUT_KEY_LEFT) {
		if (hand.size() > 0) {
			hand.pop_back();
			positionHand(hand);
		} 
		if (hand_opponent.size() > 0) {
			hand_opponent.pop_back();
			positionOpponentHand(hand_opponent);
		}
	}
	glutPostRedisplay();
}

void mouseClick(int button, int state, int x, int y) 
{
	float *objectCoords = getRayFromMouse(x, y);
	float posXs = *objectCoords;
	float posYs = *(++objectCoords);
	float posZs = *(++objectCoords);
	float posXe = *(++objectCoords);
	float posYe = *(++objectCoords);
	float posZe = *(++objectCoords);
	//std::cout << std::to_string(x) + " " + std::to_string(y) << std::endl;
	if (button == GLUT_LEFT_BUTTON) {
		// button release
		if (state == GLUT_UP) {
			
			if (currentGameState == CHAT_SCREEN) {
				if (x > 100 && y > 510 && x < 220 && y < 550) {
					messages.push_back("...ready...");
					glutPostRedisplay();
					sendReady();
					ready = (ready) ? false : true;
				}
			}
			else if (currentGameState == GAME) {
				if (cardGrabbed) {
					//if (mouseIsAboveField())
					play_card(*hoverCard);
					cardGrabbed = false;
				}
				positionHand(hand);
				movX = 0;
				movY = 0;
			}
		}
		else if (state == GLUT_DOWN) {
			if (currentGameState == GAME) {
				for (std::vector<CardHolder>::size_type i = 0; i < hand.size(); i++) {
					if (hoverCard == &hand.at(i)) {
						cardGrabbed = true;
						break;
					}
				}
			}
		}
	}
	glutPostRedisplay();
}

void mousePassive(int x, int y)
{
	float *objectCoords = getRayFromMouse(x, y);
	float posXs = *objectCoords;
	float posYs = *(++objectCoords);
	float posZs = *(++objectCoords);
	float posXe = *(++objectCoords);
	float posYe = *(++objectCoords);
	float posZe = *(++objectCoords);
	hoverCard = nullptr;
	for (std::vector<CardHolder>::iterator it = hand.begin(); it != hand.end(); ++it) {
		if (it->collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
			hoverCard = it._Ptr;
		}
	}
	for (std::vector<CardHolder>::iterator it = in_play.begin(); it != in_play.end(); ++it) {
		if (it->collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
			hoverCard = it._Ptr;
		}
	}
	glutPostRedisplay();
	//std::cout << std::to_string(posX) + " " + std::to_string(posY)+ " " + std::to_string(posZ) << std::endl;
}

void mouseActive(int x, int y)
{
	float *objectCoords = getRayFromMouse(x, y);
	float posXs = *objectCoords;
	float posYs = *(++objectCoords);
	float posZs = *(++objectCoords);
	float posXe = *(++objectCoords);
	float posYe = *(++objectCoords);
	float posZe = *(++objectCoords);
	if (hoverCard != nullptr) {
		if (cardGrabbed) {
			float t = (1.0f - posZs) / (posZe - posZs);
			float x = posXs + t*(posXe - posXs);
			float y = posYs + t*(posYe - posYs);
			if (movX == 0) { movX = x; }
			if (movY == 0) { movY = y; }
			hoverCard->moveBy(x - movX, y - movY, 0.0f);
			movX = x;
			movY = y;
		}
	}
	glutPostRedisplay();
}

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
		std::cout << std::to_string(gameData.getPlayFirst());
		sendOrder(gameData);
	}
}

float * getRayFromMouse(int x, int y)
{

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY;
	GLdouble posXs, posYs, posZs;
	GLdouble posXe, posYe, posZe;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	//glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject(winX, winY, 0.0f, modelview, projection, viewport, &posXs, &posYs, &posZs);
	gluUnProject(winX, winY, 1.0f, modelview, projection, viewport, &posXe, &posYe, &posZe);
	float coords[6] = { posXs, posYs, posZs, posXe, posYe, posZe };
	return coords;
}

void positionHand(std::vector<CardHolder>& hand) {
	int size = hand.size();
	float xInc = 0.175f;
	if (size > 5) { xInc = 0.75f/size; }
	//float width = size*0.15f;
	for (std::vector<CardHolder>::size_type i = 0; i < hand.size(); i++) {
		hand.at(i).moveTo(-0.4f + xInc*i, -0.6f, 1.0f);
	}
}

void positionOpponentHand(std::vector<CardHolder>& hand) {
	int size = hand.size();
	float xInc = 0.175f;
	if (size > 5) { xInc = 0.75f / size; }
	//float width = size*0.15f;
	for (std::vector<CardHolder>::size_type i = 0; i < hand.size(); i++) {
		hand.at(i).moveTo(-0.4f + xInc*i, 0.6f, 0.5f);
	}
}

void cardApproach() {
	for (std::deque<CardHolder>::size_type i = 0; i < drawing_cards.size(); i++) {
		drawing_cards.at(i).moveBy(-0.001f, -0.00005f, 0.0f);
		if (drawing_cards.at(i).getX() < 0) {
			hand.push_back(drawing_cards.front());
			drawing_cards.pop_front();
			positionHand(hand);
		}
		glutPostRedisplay();
	}
	for (std::deque<CardHolder>::size_type i = 0; i < drawing_cards_opponent.size(); i++) {
		drawing_cards_opponent.at(i).moveBy(0.001f, -0.00005f, 0.0f);
		if (drawing_cards_opponent.at(i).getX() > 0) {
			hand_opponent.push_back(drawing_cards_opponent.front());
			drawing_cards_opponent.pop_front();
			positionOpponentHand(hand_opponent);
		}
		glutPostRedisplay();
	}
}

CardHolder getCardFromDeck() {
	CardHolder holder(ewan, 0.85f, -0.5f, 1.0f, 1);
	return holder;
}

CardHolder getCardFromOpponentDeck() {
	CardHolder holder(ewan, -1.2f, 0.55f, 0.5f, 1);
	return holder;
}

void play_card(CardHolder &holder)
{
	//activate abilities.
	//hand.erase()
}

int main(int argc, char* argv[]) {

	////////////////////////////////////////// RakNet //////////////////////////////////////////////////////

	initNetwork(true);

	////////////////////////////////////////// OPEN GL //////////////////////////////////////////////////////
	// Initialize GLUT
	glutInit(&argc, argv);
	// Set up some memory buffers for our display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// This one allows me to close the network before the program exits after "X" is pressed.
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("BoizStone");
	//glutFullScreen();
	// Bind functions
	glutReshapeFunc(changeViewPort);
	glutDisplayFunc(render);
	glutIdleFunc(idleFunction);

	glutKeyboardFunc(keyPress);
	glutSpecialFunc(specPress);
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mousePassive);
	glutMotionFunc(mouseActive);

	// Very important!  This initializes the entry points in the OpenGL driver so we can 
	// call all the functions in the API.
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}

	in_play.push_back(ch);
	hand.push_back(ch1);
	hand_opponent.push_back(ch1);
	hand_opponent.push_back(ch1);
	positionHand(hand);
	positionOpponentHand(hand_opponent);

	srand(time(NULL));

	//currentGameState = CHAT_SCREEN;
	currentGameState = GAME;
	ready = false;
	opponentReady = false;
	glutMainLoop();
	closeNetworking();
	return 0;
}
