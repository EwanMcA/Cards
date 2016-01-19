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
#include "Util.hpp"
#include "Networking.hpp"
#include "GameData.hpp"
#include "render.hpp"
#include "Cards.hpp"

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
		renderGameBoard(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, gameData);
		// render something to see play order.
		break;
	case GAME:
		renderGameBoard(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, gameData);
		for (auto const &it : hand) {
			renderCard(it.second, true, hoverCard == &it.second);
		}
		for (auto const &it : in_play) {
			renderCard(it.second, true, hoverCard == &it.second);
		}
		for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards.begin(); it != drawing_cards.end(); ++it) {
			renderCard(it->second, true, false);
		}
		for (std::list<std::pair<int, CardHolder>>::iterator it = playing_cards.begin(); it != playing_cards.end(); ++it) {
			renderCard(it->second, true, false);
		}
		for (auto const &it : hand_opponent) {
			renderCard(it.second, false, hoverCard == &it.second);
		}
		for (auto const &it : in_play_opponent) {
			renderCard(it.second, true, hoverCard == &it.second);
		}
		for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards_opponent.begin(); it != drawing_cards_opponent.end(); ++it) {
			renderCard(it->second, false, false);
		}
		for (std::list<std::pair<int, CardHolder>>::reverse_iterator it = playing_cards_opponent.rbegin(); it != playing_cards_opponent.rend(); ++it) {
			renderCard(it->second, true, false);
		}
		if (attack_drag) {
			renderArrow(attackingCard->getX()+0.075f, attackingCard->getY()+0.15f, 0.0f, arrow_head_x, arrow_head_y, 0.0f);
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
		receivePackets_Game(gameData, action_pipeline);
		decideOrder(gameData);
		//initialDraw();
		currentGameState = GAME;
		glutPostRedisplay();
		break;
	case GAME:
		receivePackets_Game(gameData, action_pipeline);
		if (!action_pipeline.empty()) 
			action_pipeline.front()();
		drawApproach(); // animated card-draw;
		if (cardGrabbed && hoverCard->getY() > -0.3f) {
			if (hoverCard->getZ() > 0.0f) {
				hoverCard->moveBy(0.0f, 0.0f, -0.0025f);
				glutPostRedisplay();
			}
		}
		else if (cardGrabbed && hoverCard->getZ() < 1.0f) {
			hoverCard->moveTo(hoverCard->getX(), hoverCard->getY(), 1.0f);
			glutPostRedisplay();
		}
		break;
	}
	incMsg = "";
}

void keyPress(unsigned char key, int mousex, int mousey) 
{

	switch (key) {
	case 13: 
		messages.push_back("You: "+msg);
		send_text(msg);
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
		if (gameData.is_my_turn()) { action_pipeline.push_back(&end_turn); }
	}
	//else if (key == GLUT_KEY_LEFT) {
	//	if (hand.size() > 0) {
	//		hand.pop_back();
	//		position_hand(hand);
	//	} 
	//	if (hand_opponent.size() > 0) {
	//		hand_opponent.pop_back();
	//		position_hand_opponent(hand_opponent);
	//	}
	//}
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
					send_ready();
					ready = (ready) ? false : true;
				}
			}
			else if (currentGameState == GAME) {
				if (cardGrabbed) {
					if (hoverCard->getY() > -0.3f)
						play_card(hoverID);
					cardGrabbed = false;
				}
				else if (attack_drag) { //&& hoverCard != attackingCard // some allowance for click-and-move?
					// check attack ...
					if (hoverCard != nullptr) {
						// attack.
						send_attack(attackerID, hoverID);
						gameData.setup_attack(attackerID, hoverID);
						action_pipeline.push_back(&attack);
					}
					attack_drag = false;
				}
				position_hand(hand);
				movX = 0;
				movY = 0;
				arrow_head_x = 0;
				arrow_head_y = 0;
			}
		}
		else if (state == GLUT_DOWN) {
			if (currentGameState == GAME) {
				if (gameData.is_my_turn() && hoverCard != nullptr)
					if (hand.find(hoverID) != hand.end() &&
						hoverCard->getCard().getCost() <= gameData.get_energy_player().get_current()) {
						cardGrabbed = true;
					}
					else if (in_play.find(hoverID) != in_play.end() && hoverCard->getCard().can_attack()) {
						attackingCard = hoverCard;
						attackerID = hoverID;
						attack_drag = true;
						arrow_head_x = hoverCard->getX() + 0.075f;
						arrow_head_y = hoverCard->getY() + 0.15f;
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
	if (hoverCard == nullptr || !hoverCard->collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
		hoverCard = nullptr;
		position_hand(hand);
		for (auto &it : hand) {
			if (it.second.collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
				hoverCard = &it.second;
				hoverID = it.first;
			}
		}
		for (auto &it : in_play) {
			if (it.second.collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
				hoverCard = &it.second;
				hoverID = it.first;
			}
		}
		position_hand(hand);
		glutPostRedisplay();
	}
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
	if (hoverCard != nullptr && cardGrabbed) {
		float t = (hoverCard->getZ() - posZs) / (posZe - posZs);
		float x = posXs + t*(posXe - posXs);
		float y = posYs + t*(posYe - posYs);
		if (movX == 0) { movX = x; }
		if (movY == 0) { movY = y; }
		hoverCard->moveBy(x - movX, y - movY, 0.0f);
		movX = x;
		movY = y;
	}
	if (attack_drag) {
		float t = (0.0f - posZs) / (posZe - posZs); // assuming card is at z = 0.0f
		float x = posXs + t*(posXe - posXs);
		float y = posYs + t*(posYe - posYs);
		arrow_head_x = x;
		arrow_head_y = y;
		if (hoverCard == nullptr || !hoverCard->collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
			hoverCard = nullptr;
			for (auto &it : in_play_opponent) {
				if (it.second.collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
					hoverCard = &it.second;
					hoverID = it.first;
				}
			}
		}
	}
	glutPostRedisplay();
}

void request_repaint() {
	glutPostRedisplay();
}


int main(int argc, char* argv[]) {

	////////////////////////////////////////// RakNet //////////////////////////////////////////////////////

	initNetwork(false);

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

	position_hand(hand);
	position_hand_opponent(hand_opponent);
	if (net_isServer()) { gameData.toggle_turn(); }

	srand(time(NULL));

	//currentGameState = CHAT_SCREEN;
	currentGameState = GAME;
	ready = false;
	opponentReady = false;
	glutMainLoop();
	closeNetworking();
	return 0;
}
