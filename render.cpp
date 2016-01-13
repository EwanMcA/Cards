#include <GL\glew.h>
#include <freeglut.h>
#include <list>
#include "render.hpp"


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

void renderGameBoard(int SCREEN_WIDTH, int SCREEN_HEIGHT) {
	float x = -1.6f;
	float y = 0.9f;
	float z = 0.0f;
	glColor3f(0.7f, 0.6f, 0.5f);
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x, -y, z);
		glVertex3f(-x, -y, z);
		glVertex3f(-x, y, z);
	glEnd();
	glColor3f(1.0f, 0.1f, 0.2f);
	glLineWidth(5.0);
	glBegin(GL_LINES);
		glVertex3f(x - 0.01f, y + 0.005f, z);
		glVertex3f(-x + 0.01f, y + 0.005f, z);
		glVertex3f(x - 0.01f, y + 0.005f, z);
		glVertex3f(x - 0.01f, -y - 0.005f, z);
		glVertex3f(-x + 0.01f, y + 0.005f, z);
		glVertex3f(-x + 0.01f, -y - 0.005f, z);
		glVertex3f(x - 0.01f, -y - 0.005f, z);
		glVertex3f(-x + 0.01f, -y - 0.005f, z);
	glEnd();
	renderDeck(60, 1.6f, -0.475f, 0.0f, 1);
	renderDeck(1, -1.8f, -0.475f, 0.0f, 1);
	renderDeck(60, -1.8f, 0.9f, 0.0f, 1);
	renderDeck(1, 1.6f, 0.9f, 0.0f, 1);

	renderEnergy(5, 0.7f, -0.925f, 0.0f);
	renderEnergy(5, -1.45f, 1.02f, 0.0f);
}

void renderEnergy(int energy, float x, float y, float z)
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
	x += ch.getScale()*0.15f*0.05f;
	y += ch.getScale()*0.3f*0.025f;
	if (face_up) { glColor3f(0.55f, 0.43f, 0.11f); }
	else { glColor3f(0.45, 0.15f, 0.0f); }
	glBegin(GL_QUADS);
		glVertex3f(x, y, z);
		glVertex3f(x +0.15f*ch.getScale()*0.9f, y, z);
		glVertex3f(x + 0.15f*ch.getScale()*0.9f, y + 0.3f*ch.getScale()*0.95f, z);
		glVertex3f(x, y + 0.3f*ch.getScale()*0.95f, z);
	glEnd();

	glColor3f(1.0f, 0.2f, 0.2f);

//	renderBitmapString(x +0.0075*ch.getScale(), y + 0.025*ch.getScale(), 0.0, GLUT_BITMAP_HELVETICA_18, std::to_string(ch.getCard().getAttack()).c_str());
//	renderBitmapString(x + 0.2*ch.getScale()-0.025*ch.getScale(), y + 0.025*ch.getScale(), 0.0, GLUT_BITMAP_HELVETICA_18, std::to_string(ch.getCard().getHealth()).c_str());

}

void renderDeck(int size, float x, float y, float z, float scale)
{
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
		glColor3f(0.55f, 0.43f, 0.11f);
		glBegin(GL_QUADS);
			glVertex3f(x, y - 0.005f*scale, 0.0f);
			glVertex3f(x + 0.2f*scale, y-0.005f*scale, 0.0f);
			glVertex3f(x + 0.2f*scale, y - 0.45f*scale, 0.0f);
			glVertex3f(x, y - 0.45f*scale, 0.0f);
		glEnd();
		x += 0.005f;
	}
}
