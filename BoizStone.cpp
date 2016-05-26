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

long get_delta_time() {
	long time_elapsed = glutGet(GLUT_ELAPSED_TIME);
	static long old_time_elapsed = time_elapsed;
	long delta = time_elapsed - old_time_elapsed;
	old_time_elapsed = time_elapsed;
	return delta;
}

//float * getRayFromMouse(int x, int y)
//{
//
//	GLint viewport[4];
//	GLdouble modelview[16];
//	GLdouble projection[16];
//	GLfloat winX, winY;
//	GLdouble posXs, posYs, posZs;
//	GLdouble posXe, posYe, posZe;
//
//	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
//	glGetDoublev(GL_PROJECTION_MATRIX, projection);
//	glGetIntegerv(GL_VIEWPORT, viewport);
//
//	winX = (float)x;
//	winY = (float)viewport[3] - (float)y;
//	//glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
//	gluUnProject(winX, winY, 0.0f, modelview, projection, viewport, &posXs, &posYs, &posZs);
//	gluUnProject(winX, winY, 1.0f, modelview, projection, viewport, &posXe, &posYe, &posZe);
//	float coords[6] = { posXs, posYs, posZs, posXe, posYe, posZe };
//	return coords;
//}

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
	// Do this in the loading screen eventually...
	static GLuint bg_texture = load_texture("wood_grain.jpg");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bg_texture);
	///
	//if (gameData.is_my_turn()) { glColor3f(1.0f, 0.9f, 0.9f); }
	//else { glColor3f(0.8f, 0.8f, 1.0f); }
	glColor3f(1.0f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-2.2f, -1.4f, 0.0f);
	glTexCoord2f(0, 1); glVertex3f(-2.2f, 1.4f, 0.0f);
	glTexCoord2f(1, 1); glVertex3f(2.2f, 1.4f, 0.0f);
	glTexCoord2f(1, 0); glVertex3f(2.2f, -1.4f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	switch (gameData.get_game_state()) {
	case CHAT_SCREEN: 
		renderConnectionMenu(gameData.is_player_ready(), msg, messages);
		break;
	case  START_GAME:
		renderGameBoard(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, gameData);
		// render something to see play order.
		break;
	case GAME:
		renderGameBoard(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, gameData);
		for (auto const &it : player.hand) {
			renderCard(it.second, true, hoverCard == &it.second);
		}
		for (std::list<std::pair<int, CardHolder>>::iterator it = drawing_cards.begin(); it != drawing_cards.end(); ++it) {
			renderCard(it->second, true, false);
		}
		for (std::list<std::pair<int, CardHolder>>::iterator it = playing_cards.begin(); it != playing_cards.end(); ++it) {
			renderCard(it->second, true, false);
		}
		for (auto const &it : opponent.hand) {
			renderCard(it.second, false, hoverCard == &it.second);
		}
		for (auto const &it : opponent.in_play) {
			renderCard(it.second, true, (hoverCard == &it.second) || it.second.isAttackable());
		}
		for (auto const &it : player.in_play) {
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
	long delta = get_delta_time();
	//receivePackets loops through all packets.
	std::string incMsg;
	switch (gameData.get_game_state()) {
	case CHAT_SCREEN:
		incMsg = receivePackets_ChatScreen();
		if (incMsg == "READY") {
			gameData.toggle_opponent_ready();
		}
		else {
			if (incMsg.size() > 0) {
				messages.push_back("Friend: " + incMsg);
				glutPostRedisplay();
			}
		}
		countDown((gameData.is_player_ready() && gameData.is_opponent_ready()), messages, gameData);
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
		gameData.set_game_state(GAME);
		glutPostRedisplay();
		break;
	case GAME:
		receivePackets_Game(gameData);
		if (gameData.actions_remain())
			gameData.next_action()(delta);
		drawApproach(delta); // animated card-draw;
		if (cardGrabbed && hoverCard->getY() > -0.5f && hoverCard->getY() < 0.0f) {
			hoverCard->moveTo(hoverCard->getX(), hoverCard->getY(), 1.24f - 1.24f*((0.5f + hoverCard->getY())/0.5f));
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
		if (gameData.is_my_turn()) { gameData.queue_action(&end_turn); }
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

	if (button == GLUT_LEFT_BUTTON) {
		// button release
		if (state == GLUT_UP) {
			if (gameData.get_game_state() == CHAT_SCREEN) {
				if (x > 100 && y > 510 && x < 220 && y < 550) {
					messages.push_back("...ready...");
					glutPostRedisplay();
					send_ready();
					gameData.toggle_player_ready();
				}
			}
			else if (gameData.get_game_state() == GAME) {
				if (cardGrabbed) {
					if (hoverCard->getY() > -0.35f)
						play_card(hoverID);
					cardGrabbed = false;
				}
				else if (attack_drag) { //&& hoverCard != attackingCard // some allowance for click-and-move?
					// check attack ...
					if (hoverCard != nullptr && hoverCard->isAttackable()) {
						// attack.
						send_attack(attackerID, hoverID);
						gameData.setup_attack(attackerID, hoverID);
						gameData.queue_action(&attack);
					}
					attack_drag = false;
					for (auto &it : opponent.in_play) {
						it.second.set_attackable(false);
					}
				}
				position_hand(player.hand);
				movX = 0;
				movY = 0;
				arrow_head_x = 0;
				arrow_head_y = 0;
			}
		}
		else if (state == GLUT_DOWN) {
			if (gameData.get_game_state() == GAME) {
				if (gameData.is_my_turn() && hoverCard == nullptr) {
					float t = (0.0f - posZs) / (posZe - posZs);
					float x = posXs + t*(posXe - posXs);
					float y = posYs + t*(posYe - posYs);
					if (x > 1.7f && y > 0.0f && x < 2.0f && y < 0.15f) {
						gameData.queue_action(&end_turn);
					}
				}
				if (gameData.is_my_turn() && hoverCard != nullptr) {
					if (player.hand.find(hoverID) != player.hand.end() &&
						hoverCard->getCard().getCost() <= gameData.get_energy_player().get_current()) {
						cardGrabbed = true;
					}
					else if (player.in_play.find(hoverID) != player.in_play.end() && hoverCard->getCard().can_attack()) {
						attackingCard = hoverCard;
						attackerID = hoverID;
						attack_drag = true;
						arrow_head_x = hoverCard->getX() + 0.075f;
						arrow_head_y = hoverCard->getY() + 0.15f;
						bool taunt_present = false;
						for (auto &it : opponent.in_play) {   // Highlighting creatures that can be attacked.
														// Problem -> visual feedback on which one you're choosing.
							if (it.second.getCard().getStatus() & STATUS_TAUNT) {
								it.second.set_attackable(true);
								taunt_present = true;
							}
						}
						for (auto &it : opponent.in_play) {
							if (!(it.second.getCard().getStatus() & STATUS_TAUNT)) {
								if (!taunt_present) {
									it.second.set_attackable(true);
								}
								else {
									it.second.set_attackable(false);
								}
							}
						}
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
	if (hoverCard == nullptr || !hoverCard->collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
		hoverCard = nullptr;
		position_hand(player.hand);
		for (auto &it : player.hand) {
			if (it.second.collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
				hoverCard = &it.second;
				hoverID = it.first;
			}
		}
		for (auto &it : player.in_play) {
			if (it.second.collidesWithRay(posXs, posYs, posZs, posXe, posYe, posZe)) {
				hoverCard = &it.second;
				hoverID = it.first;
			}
		}
		position_hand(player.hand);
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
			for (auto &it : opponent.in_play) {
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

	if (net_isServer()) { gameData.toggle_turn(); }

	srand(time(NULL));

	start_game();
	glutMainLoop();
	closeNetworking();
	return 0;
}
