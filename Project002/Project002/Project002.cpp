#include "pch.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include "math.h"

int width = 400;
int height = 400;
enum status{ MAIN_MENU, START_GAME, RESTART_GAME };
status gStatus;
struct rect {
	int x;
	int y;
	int width;
	int height;
	bool exist;
};
rect mRect[20];
rect board;
rect bRect;
float xPos, yPos, radius;
float xChange, yChange;
float speed;
int mousePrevX, mousePrevY;
int score;
int highScore = 0;
char s[50];

void initMainMenu(void) {
	bRect.width = 200;
	bRect.height = 100;
	bRect.x = width / 2 - bRect.width / 2;
	bRect.y = height / 2 - bRect.height / 2;
}

void initGame(void) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 4; j++) {
			mRect[i + 5 * j].exist = true;
			mRect[i + 5 * j].x = 20 + i * 76;
			mRect[i + 5 * j].y = 360 - j * 40;
			mRect[i + 5 * j].width = 56;
			mRect[i + 5 * j].height = 20;
		}
	}

	board.width = 60;
	board.height = 20;
	board.x = width / 2 - board.width / 2;
	board.y = 20;

	xPos = width / 2;
	yPos = height / 2;
	radius = 10;

	speed = 0.1f;
	xChange = 0;
	yChange = -1 * speed;
	mousePrevX = 0;
	mousePrevY = 0;

	score = 0;
}

void initRestart(void) {
	bRect.width = 200;
	bRect.height = 100;
	bRect.x = width / 2 - bRect.width / 2;
	bRect.y = height / 2 - bRect.height / 2;
}

void drawPoligon(int x, int y, int xsize, int ysize) {
	float fx = (float(x) / width) * 2 - 1;
	float fy = (float(y) / height) * 2 - 1;
	float fw = (float(xsize) / width) * 2;
	float fh = (float(ysize) / height) * 2;
	glBegin(GL_POLYGON);
	glVertex3f(fx, fy, 0.0);
	glVertex3f(fx + fw, fy, 0.0);
	glVertex3f(fx + fw, fy + fh, 0.0);
	glVertex3f(fx, fy + fh, 0.0);
	glEnd();
}

void drawCircle(float x, float y) {
	float fx = (x / width) * 2 - 1;
	float fy = (y / height) * 2 - 1;
	float fr = radius / (width / 2) ;

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(fx, fy); // вершина в центре круга
	for (int i = 0; i <= 50; i++) {
		float a = (float)i / 50.0f * 3.1415f * 2.0f;
		glVertex2f(fx + cos(a) * fr, fy + sin(a) * fr);
	}
	glEnd();
}

void Update(void) {
	xPos += xChange;
	yPos += yChange;

	//пересечение
	if (board.y + board.height > yPos - radius
		&& board.x < xPos
		&& board.x + board.width > xPos) {
		float acos, asin;
		acos = (float(xPos - board.x) / float(board.width)) * 2 - 1;
		asin = sqrt(1 - acos* acos);
		xChange = speed * acos;
		yChange = speed * asin;
	}


	for (int i = 0; i < 20; i++)
		if (mRect[i].exist) {
			if (mRect[i].x < xPos
				&& mRect[i].x + mRect[i].width > xPos) {
				if ((mRect[i].y < yPos + radius && mRect[i].y + mRect[i].height > yPos + radius)
					|| (mRect[i].y > yPos - radius && mRect[i].y + mRect[i].height < yPos - radius)) {
					yChange = -1 * yChange;
					mRect[i].exist = false;
					score += 10;
				}
			}
			if (mRect[i].y < yPos
				&& mRect[i].y + mRect[i].height > yPos) {
				if ((mRect[i].x < xPos + radius && mRect[i].x + mRect[i].width > xPos + radius)
					|| (mRect[i].y > xPos - radius && mRect[i].y + mRect[i].width < xPos - radius)) {
					xChange = -1 * xChange;
					mRect[i].exist = false;
					score += 10;
				}
			}
		}

	//границы
	if (xPos - radius < 0 || xPos + radius > width)
		xChange = -1 * xChange;
	if (yPos + radius > height)
		yChange = -1 * yChange;
	if (yPos - radius < 0) {
		gStatus = RESTART_GAME;
		initRestart();
	}

	int k = 0;
	for (int i = 0; i < 20; i++)
		if (mRect[i].exist)
			k++;
	if (k == 0) {
		gStatus = RESTART_GAME;
		initRestart();
	}

	if (score > highScore)
		highScore = score;
}

void renderBitmapString(
	float x,
	float y,
	float z,
	void *font,
	char *string) {
	char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void drawString(int x, int y, void *font, char *string) {
	float fx = (float(x) / width) * 2 - 1;
	float fy = (float(y) / height) * 2 - 1;
	renderBitmapString(fx, fy, 0.0f, font, string);
}

void drawMainMenu(void) {
	glColor3f(1.0, 1.0, 1.0);
	drawPoligon(bRect.x, bRect.y, bRect.width, bRect.height);

	glColor3f(0.0, 0.0, 0.0);
	sprintf_s(s, "Start Game");
	drawString(bRect.x + 50, bRect.y + 40, GLUT_BITMAP_TIMES_ROMAN_24, s);
}

void drawGame(void) {
	glColor3f(0.0, 0.0, 1.0);
	for (int i = 0; i < 20; i++)
		if (mRect[i].exist)
			drawPoligon(mRect[i].x, mRect[i].y, mRect[i].width, mRect[i].height);

	glColor3f(1.0, 1.0, 1.0);
	drawPoligon(board.x, board.y, board.width, board.height);

	drawCircle(xPos, yPos);

	Update();

	sprintf_s(s, "Score: %d HighScore: %d", score, highScore);
	drawString(0, 384, GLUT_BITMAP_HELVETICA_12, s);
}

void drawRestart(void) {
	glColor3f(1.0, 1.0, 1.0);
	drawPoligon(bRect.x, bRect.y, bRect.width, bRect.height);

	glColor3f(0.0, 0.0, 0.0);
	sprintf_s(s, "Restart");
	drawString(bRect.x + 50, bRect.y + 40, GLUT_BITMAP_TIMES_ROMAN_24, s);
}

void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (gStatus == MAIN_MENU)
		drawMainMenu();
	else if (gStatus == START_GAME)
		drawGame();
	else if (gStatus == RESTART_GAME)
		drawRestart();
	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int x, int y) {
	if (gStatus == MAIN_MENU)
		return;
	switch (key) {
	case GLUT_KEY_LEFT:
		board.x -= 5;
		if (board.x < 0)
			board.x = 0;
		break;
	case GLUT_KEY_RIGHT:
		board.x += 5;
		if (board.x + board.width > width)
			board.x = width - board.width;
		break;
	}
}

void mouseMove(int x, int y) {
	int mouseX = x;
	int mouseY = -1 * y + height;

	if (gStatus == START_GAME)
		if (mouseX > board.x && mouseX < board.x + board.width && mouseY > board.y && mouseY < board.y + board.height) {
			board.x += mouseX - mousePrevX;
			if (board.x < 0)
				board.x = 0;
			if (board.x + board.width > width)
				board.x = width - board.width;
			mousePrevX = mouseX;
			mousePrevY = mouseY;
		}
}

void mouseButton(int button, int state, int x, int y) {

	if (gStatus == MAIN_MENU || gStatus == RESTART_GAME) {
		int mouseX = x;
		int mouseY = -1 * y + height;
		if (mouseX > bRect.x && mouseX < bRect.x + bRect.width && mouseY > bRect.y && mouseY < bRect.y + bRect.height) {
			gStatus = START_GAME;
			initGame();
		}
	}
	else if (gStatus == START_GAME) {
		mousePrevX = x;
		mousePrevY = -1 * y + height;
	}
}

void init(void) {
	gStatus = MAIN_MENU;
	initMainMenu();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Arcanoid");
	
	init();

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	glutMainLoop();

	return 1;
}