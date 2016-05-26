#include <GL\glew.h>
#include <freeglut.h>
#include <list>
#include "render.hpp"
#include <iostream>
#include "SOIL.h"


void renderBitmapString(
	float x,
	float y,
	float z,
	void *font,
	const char *string) {
	const char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

GLuint load_texture(const char * filename)
{/* load an image file directly as a new OpenGL texture */
	GLuint tex_2d = SOIL_load_OGL_texture
		(
			filename,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	/* check for an error during the load process */
	if (0 == tex_2d)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}
	return tex_2d;
}

// CHAT SCREEN

void renderConnectionMenu(bool ready, std::string currentMessage, std::list<std::string>& messages) {
	renderTextEntryBox(0.05f, -0.8f, 0.0f, 0.8f, 0.075f, currentMessage);
	renderMessageDisplayBox(0.05f, -0.6f, 0.0f, 0.8f, 1.5f, messages);
	renderReadyButton(ready, -0.75f, -0.8f, 0.0f, 0.3f, 0.1f);
}

void renderReadyButton(bool ready, float x, float y, float z, float width, float height) 
{
	std::string label;
	if (ready) { 
		glColor3f(0.2f, 0.9f, 0.3f);
		label = "Ready!";
	} else { 
		glColor3f(0.9f, 0.2f, 0.3f);
		label = "Ready?";
	}
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x, y + height, z);
		glVertex3f(x + width, y + height, z);
		glVertex3f(x + width, y, z);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	renderBitmapString(x+width/4+0.0125f, y+height/4+0.01f, 0.0f, GLUT_BITMAP_9_BY_15, label.c_str());
}

void renderTextEntryBox(float x, float y, float z, float width, float height, std::string currentMessage) 
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x, y+height, z);
		glVertex3f(x+width, y+height, z);
		glVertex3f(x+width, y, z);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	renderBitmapString(0.06f, -0.775f, 0.0f, GLUT_BITMAP_9_BY_15, currentMessage.c_str());
}

void renderMessageDisplayBox(float x, float y, float z, float width, float height, std::list<std::string>& messages) {
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x, y + height, z);
		glVertex3f(x + width, y + height, z);
		glVertex3f(x + width, y, z);
	glEnd();
	int i = 0;
	for (std::list<std::string>::iterator e = messages.begin(); e != messages.end(); e++)
	{
		if (e->find("F") == 0) { glColor3f(0.2f, 0.2f, 1.0f); }
		else { glColor3f(0.0f, 0.0f, 0.0f); }
		renderBitmapString(0.06f, 0.85f - i*0.05f, 0.0f, GLUT_BITMAP_9_BY_15, e->c_str());
		i++;
	}
}


// GAME

void renderGameBoard(int SCREEN_WIDTH, int SCREEN_HEIGHT, Data& gameData) {
	float x = -1.6f;
	float y = 0.9f;
	float z = 0.0f;
	// Do this in the loading screen eventually...
	static GLuint fg_texture = load_texture("sand_texture.jpg");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fg_texture);
	glColor3f(0.7f, 0.6f, 0.5f);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(x, y, z);
		glTexCoord2f(0, 1); glVertex3f(x, -y, z);
		glTexCoord2f(1, 1); glVertex3f(-x, -y, z);
		glTexCoord2f(1, 0); glVertex3f(-x, y, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.1f, 0.2f);
	renderDeck(60, 1.6f, -0.475f, 0.0f, 1);
	renderDeck(1, -1.8f, -0.475f, 0.0f, 1);
	renderDeck(60, -1.8f, 0.9f, 0.0f, 1);
	renderDeck(1, 1.6f, 0.9f, 0.0f, 1);
	renderEnergy(gameData.get_energy_player().get_current(), 0.7f, -0.925f, 0.0f);
	renderEnergy(gameData.get_energy_opponent().get_current(), -1.45f, 1.02f, 0.0f);
	renderPlayer(gameData.get_opponent(), -0.2f, 0.45f, 0.0f);
	renderPlayer(gameData.get_player(), -0.2f, -0.9f, 0.0f);
	render_end_turn_button(1.7f, 0.0f, 0.0f, 1.0f);
}

void renderPlayer(Player &player, float x, float y, float z)
{
	glColor3f(0.8f, 0.8f, 0.8f);
	static GLuint avatar = load_texture("avatar.jpg");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, avatar);
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(x, y, z);
		glTexCoord2d(1, 0); glVertex3f(x + 0.4, y, z);
		glTexCoord2d(1, 1); glVertex3f(x + 0.4, y + 0.5, z);
		glTexCoord2d(0, 1); glVertex3f(x, y + 0.5, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void render_end_turn_button(float x, float y, float z, float scale) {
	glColor3f(0.3f, 0.3f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x + 0.3, y, z);
		glVertex3f(x + 0.3, y + 0.15, z);
		glVertex3f(x, y + 0.15, z);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	renderBitmapString(x + 0.025, y + 0.05, z, GLUT_BITMAP_HELVETICA_18, "End Turn");
}


void renderEnergy(short energy, float x, float y, float z)
{
	glColor3f(0.7f, 0.6f, 0.5f);
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x + 0.7f, y, z);
		glVertex3f(x + 0.7f, y - 0.1f, z);
		glVertex3f(x, y - 0.1f, z);
	glEnd();
	glColor3f(0.2f, 0.6f, 1.0);
	y += 0.005f;
	x -= 0.1f;
	for (energy; energy > 0; energy--) {
		x += 0.15;
		glBegin(GL_TRIANGLES);
			glVertex3f(x, y, z);
			glVertex3f(x - 0.05f, y - 0.05f, z);
			glVertex3f(x + 0.05f, y - 0.05f, z);
			glVertex3f(x + 0.05f, y - 0.05f, z);
			glVertex3f(x - 0.05f, y - 0.05f, z);
			glVertex3f(x, y - 0.1f, z);
		glEnd();
	}
}

void renderCard(CardHolder ch, bool face_up, bool highlight) {
	//use filename to get image.
	float x = ch.getX();
	float y = ch.getY();
	float z = ch.getZ();
	if (highlight) {
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	else {
		glColor3f(0.0f, 0.0f, 0.0f);
	}
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x + 0.15f*ch.getScale(), y, z);
		glVertex3f(x + 0.15f*ch.getScale(), y + 0.3f*ch.getScale(), z);
		glVertex3f(x, y + 0.3f*ch.getScale(), z);
	glEnd();
	static GLuint card_back = load_texture("paper.jpg");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, card_back);
	x += ch.getScale()*0.15f*0.05f;
	y += ch.getScale()*0.3f*0.025f;
	if (face_up) { glColor3f(0.55f, 0.43f, 0.11f); }
	else { glColor3f(0.45, 0.15f, 0.0f); }
	glBegin(GL_QUADS);
		glTexCoord2d(0, 0); glVertex3f(x, y, z);
		glTexCoord2d(0, 1); glVertex3f(x +0.15f*ch.getScale()*0.9f, y, z);
		glTexCoord2d(1, 1); glVertex3f(x + 0.15f*ch.getScale()*0.9f, y + 0.3f*ch.getScale()*0.95f, z);
		glTexCoord2d(1, 0); glVertex3f(x, y + 0.3f*ch.getScale()*0.95f, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glColor3f(1.0f, 0.2f, 0.2f);
	void * font = GLUT_BITMAP_HELVETICA_18;
	if (ch.getZ() < 1.0f) { font = GLUT_BITMAP_HELVETICA_12; }
	if (face_up) {
		renderBitmapString(x+0.01*ch.getScale(), 
			y + 0.025*ch.getScale(), ch.getZ(), font, std::to_string(ch.getCard().get_current_attack()).c_str());
		renderBitmapString(x + 0.15f*ch.getScale() - 0.05*ch.getScale(), 
			y + 0.025*ch.getScale(), ch.getZ(), font, std::to_string(ch.getCard().get_current_health()).c_str());
	}
}

void renderDeck(int size, float x, float y, float z, float scale)
{
	static GLuint card_back = load_texture("paper.jpg");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, card_back);
	if (size > 5) { size = 5; }
	for (size; size > 0; size--) {
		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
			glVertex3f(x, y, 0.0f);
			glVertex3f(x + 0.2f*scale, y, 0.0f);
			glVertex3f(x + 0.2f*scale, y - 0.45f*scale, 0.0f);
			glVertex3f(x, y - 0.45f*scale, 0.0f);
		glEnd();
		x += 0.005f;
		glColor3f(0.45, 0.15f, 0.0f);
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex3f(x, y - 0.005f*scale, 0.0f);
			glTexCoord2d(0, 1); glVertex3f(x + 0.2f*scale, y-0.005f*scale, 0.0f);
			glTexCoord2d(1, 1); glVertex3f(x + 0.2f*scale, y - 0.45f*scale, 0.0f);
			glTexCoord2d(1, 0); glVertex3f(x, y - 0.45f*scale, 0.0f);
		glEnd();
		x += 0.005f;
	}
	glDisable(GL_TEXTURE_2D);
}

void renderArrow(float xs, float ys, float zs, float xe, float ye, float ze)
{
	
	float half_width = 0.1f;
	glColor3f(1.0f, 0.0f, 0.0f);
	/*glBegin(GL_QUADS);
		glVertex3f(xs - half_width, ys, zs);
		glVertex3f(xe - half_width, ye, zs);
		glVertex3f(xs + half_width, ye, zs);
		glVertex3f(xe + half_width, ys, zs);
	glEnd();*/
	glBegin(GL_LINES);
		glVertex3f(xs, ys, zs);
		glVertex3f(xe, ye, ze);
	glEnd();
}
