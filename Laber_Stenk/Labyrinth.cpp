#include <Windows.h>
#include "GL\glut.h"
#include "GL\glaux.h"
#include <stdio.h>
#include <math.h>

#pragma comment( lib, "glaux.lib" )

#define LABIRINT_WIDTH 10   // количество клеток лабиринта по ширине
#define LABIRINT_HEIGHT 8   // количество клеток лабиринта по высоте
#define CELL_AREA 50  // площадь клетки в пиксел€х

#define WALL_NO     0 // 0    нет стенок
#define WALL_BOTTOM 1 // 1 _  нижн€€ стенка
#define WALL_RIGHT  2 // 2  | права€ стенка
#define WALL_BOTH   3 // 3 _| нижн€€ и права€ стенки
#define WALL_FINN   4 // 4	  финиш

int pole[LABIRINT_HEIGHT][LABIRINT_WIDTH] = {
	4, 2, 0, 0, 2, 0, 0, 0, 1, 2,
	2, 1, 3, 1, 1, 0, 3, 0, 3, 2,
	2, 0, 0, 0, 2, 0, 1, 1, 0, 3,
	0, 3, 1, 3, 0, 0, 2, 0, 3, 2,
	3, 0, 0, 0, 2, 3, 0, 0, 1, 2,
	0, 3, 2, 3, 1, 0, 3, 0, 3, 2,
	3, 0, 0, 3, 0, 2, 0, 0, 3, 2,
	1, 1, 1, 1, 1, 1, 3, 1, 1, 3
};


int hero_X = 1;
int hero_Y = 1;
int heroX_inpole = LABIRINT_HEIGHT - 1;
int heroY_inpole = 0;


int mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
int mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
int mpsX_inpole = 1;
int mpsY_inpole = LABIRINT_HEIGHT;

int life = 3;
int swish_pole = 1;
int complexity = 100;

GLuint	texture[1];	   // дл€ текстур


GLvoid LoadGLTextures()
{
	// «агрузка картинок
	AUX_RGBImageRec *texture1;
	texture1 = auxDIBImageLoad((LPCSTR)"parket1.bmp");  // загрузить изображение в пам€ть, св€занную с texture1
	// —оздание текстуры 1
	glGenTextures(1, &texture[0]);   //  
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture1->sizeX, texture1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1->data);
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, 640.0, 0.0, 480.0, -20.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void renderBitmapString(float x, float y, float z, void *font, char *string)
{
	char *c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}


void glLine2f(float x1, float y1, float x2, float y2)
{
	glBegin(GL_LINE_STRIP);
	glVertex3f(x1, y1, 0.0);
	glVertex3f(x2, y2, 0.0);
	glEnd();
}


void hero(int x, int y)
{
	glColor3f(0.0, 1.0, 0.5);
	glTranslatef(CELL_AREA / 2, CELL_AREA / 2, 0);
	glBegin(GL_POLYGON);
	glVertex3f(hero_X - 10, hero_Y - 10, 0.0);
	glVertex3f(hero_X - 10, hero_Y + 10, 0.0);
	glVertex3f(hero_X + 10, hero_Y + 10, 0.0);
	glVertex3f(hero_X + 10, hero_Y - 10, 0.0);
	glEnd();
	glTranslatef(-CELL_AREA / 2, -CELL_AREA / 2, 0);

}

void MPS()
{
	glColor3f(0.8, 0.0, 0.0);
	glTranslatef(CELL_AREA / 2, CELL_AREA / 2, 0);
	glBegin(GL_POLYGON);
	glVertex3f(mps_X - 10, mps_Y - 10, 0.0);
	glVertex3f(mps_X - 10, mps_Y + 10, 0.0);
	glVertex3f(mps_X + 10, mps_Y + 10, 0.0);
	glVertex3f(mps_X + 10, mps_Y - 10, 0.0);
	glEnd();
	glTranslatef(-CELL_AREA / 2, -CELL_AREA / 2, 0);
}




void printWall(int type, int x, int y) {
	switch (type)
	{
	case WALL_BOTTOM:
		glLine2f(x * CELL_AREA, y * CELL_AREA, (x + 1) * CELL_AREA, y * CELL_AREA);
		break;
	case WALL_RIGHT:
		glLine2f((x + 1) * CELL_AREA, y * CELL_AREA, (x + 1) * CELL_AREA, (y + 1) * CELL_AREA);
		break;
	case WALL_BOTH:
		glLine2f(x * CELL_AREA, y * CELL_AREA, (x + 1) * CELL_AREA, y * CELL_AREA);
		glLine2f((x + 1) * CELL_AREA, y * CELL_AREA, (x + 1) * CELL_AREA, (y + 1) * CELL_AREA);
		break;
	case WALL_FINN:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glBegin(GL_QUADS); // начало рисовани€ пр€моугольника, z == const
		glTexCoord2f(0.0f, 0.0f);  glVertex3f((x + 1) * CELL_AREA, (y + 1) * CELL_AREA, 0);
		glTexCoord2f(1.0f, 0.0f);  glVertex3f((x)* CELL_AREA, (y + 1) * CELL_AREA, 0);
		glTexCoord2f(1.0f, 1.0f);  glVertex3f(x * CELL_AREA, y * CELL_AREA, 0);
		glTexCoord2f(0.0f, 1.0f);  glVertex3f((x + 1) * CELL_AREA, y * CELL_AREA, 0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		break;
	}
}

int regime = 0;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.5, 1.0, 0);
	if (regime == 0)
	{
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(80, 400, 0, GLUT_BITMAP_9_BY_15, "game:   LABYRINTH");
		glColor3f(0.0, 0.0, 0.0);
		renderBitmapString(70, 350, 0, GLUT_BITMAP_9_BY_15, "s - start");
		renderBitmapString(70, 300, 0, GLUT_BITMAP_9_BY_15, "esc - ext");
		renderBitmapString(70, 250, 0, GLUT_BITMAP_9_BY_15, "life = 3");
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(10, 100, 0, GLUT_BITMAP_9_BY_15, "teacher: Anna Nikolaevna");
		renderBitmapString(10, 50, 0, GLUT_BITMAP_9_BY_15, "create: Dima Chistaykov");

	}
	if (regime == 2)
	{
		glColor3f(1.0, 1.0, 0.0);
		renderBitmapString(70, 400, 0, GLUT_BITMAP_9_BY_15, "you WIN");
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(70, 350, 0, GLUT_BITMAP_9_BY_15, "r - repeat");
		renderBitmapString(70, 300, 0, GLUT_BITMAP_9_BY_15, "esc - ext");
	}
	if (regime == 5)
	{
		glColor3f(1.0, 1.0, 0.0);
		renderBitmapString(70, 400, 0, GLUT_BITMAP_9_BY_15, "complexity;");
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(70, 350, 0, GLUT_BITMAP_9_BY_15, "1 - easy");
		renderBitmapString(70, 300, 0, GLUT_BITMAP_9_BY_15, "2 - medium");
		renderBitmapString(70, 250, 0, GLUT_BITMAP_9_BY_15, "3 - hard");
		renderBitmapString(70, 200, 0, GLUT_BITMAP_9_BY_15, "esc - ext");
	}
	if (regime == 3)
	{
		glColor3f(0.3, 0.0, 0.0);
		renderBitmapString(70, 400, 0, GLUT_BITMAP_9_BY_15, "GAME OVER");
		glColor3f(1.0, 1.0, 1.0);
		renderBitmapString(70, 350, 0, GLUT_BITMAP_9_BY_15, "r - repeat");
		renderBitmapString(70, 300, 0, GLUT_BITMAP_9_BY_15, "esc - ext");
		swish_pole = 1;
	}
	if (regime == 1)
	{

		glTranslatef(CELL_AREA, CELL_AREA, 0);
		glColor3f(0.0, 0.2, 1.0);
		glRectf(0, 0, LABIRINT_WIDTH*CELL_AREA, LABIRINT_HEIGHT*CELL_AREA);

		glLineWidth(10);   //размер линий
		MPS();
		// стены
		glColor3f(1.0, 0.5, 0.0);
		for (int i = 0; i < LABIRINT_WIDTH; i++)
			for (int j = 0; j < LABIRINT_HEIGHT; j++)
			{
				printWall(pole[LABIRINT_HEIGHT - j - 1][i], i, j);
			}

		hero(1, 1);

		glColor3f(1.0, 0.5, 0.0);
		glLine2f(0, 0, 0, LABIRINT_HEIGHT*CELL_AREA);
		glLine2f(0, LABIRINT_HEIGHT*CELL_AREA, LABIRINT_WIDTH*CELL_AREA, LABIRINT_HEIGHT*CELL_AREA);

		glTranslatef(-CELL_AREA, -CELL_AREA, 0);
	}

	if (regime == 4)
	{
		glTranslatef(CELL_AREA, CELL_AREA, 0);
		glColor3f(0.0, 0.2, 1.0);
		glRectf(0, 0, LABIRINT_WIDTH*CELL_AREA, LABIRINT_HEIGHT*CELL_AREA);

		glLineWidth(10);   //размер линий
		MPS();
		// стены
		glColor3f(0.5, 0.0, 0.0);
		for (int i = 0; i < LABIRINT_WIDTH; i++)
			for (int j = 0; j < LABIRINT_HEIGHT; j++)
			{
				printWall(pole[LABIRINT_HEIGHT - j - 1][i], i, j);
			}

		hero(1, 1);

		glColor3f(0.5, 0.0, 0.0);
		glLine2f(0, 0, 0, LABIRINT_HEIGHT*CELL_AREA);
		glLine2f(0, LABIRINT_HEIGHT*CELL_AREA, LABIRINT_WIDTH*CELL_AREA, LABIRINT_HEIGHT*CELL_AREA);

		glTranslatef(-CELL_AREA, -CELL_AREA, 0);

	}

	glFlush();
	glFinish();
}


void keyboard(unsigned char Key, int X, int Y)
{
	if (Key == 's' && regime == 0)
	{
		regime = 5;
		swish_pole = 1;
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
		swish_pole = 1;
		life = 3;
	}
	if (Key == '1' && regime == 5)
	{
		complexity = 300;
		regime = 1;
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
	}
	if (Key == '2' && regime == 5)
	{
		complexity = 200;
		regime = 1;
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
	}
	if (Key == '3' && regime == 5)
	{
		complexity = 100;
		regime = 1;
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
	}
	if (Key == '4')
	{
		complexity = 100;
		regime = 1;
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
	}
	if (Key == 'r' && regime == 2 || regime == 3)
	{
		regime = 0;
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
		swish_pole = 1;
		life = 3;
	}

	if (Key == 27) exit(30);
}


void keyboard_s(int c, int x, int y)
{
	if (c == GLUT_KEY_LEFT) {
		int ly = heroY_inpole - 1;
		int lx = heroX_inpole;
		if ((pole[lx][ly] == 0 || pole[lx][ly] == 1 || pole[lx][ly] == 4) && ly > -1)
		{
			hero_X -= CELL_AREA;
			heroY_inpole -= 1;
		}
	}

	if (c == GLUT_KEY_RIGHT) {
		int ly = heroY_inpole;
		int lx = heroX_inpole;
		if (pole[lx][ly] == 0 || pole[lx][ly] == 1 || pole[lx][ly] == 4)
		{
			hero_X += CELL_AREA;
			heroY_inpole += 1;
		}
	}

	if (c == GLUT_KEY_DOWN) {
		int ly = heroY_inpole;
		int lx = heroX_inpole;
		if (pole[lx][ly] == 0 || pole[lx][ly] == 2 || pole[lx][ly] == 4)
		{
			hero_Y -= CELL_AREA;
			heroX_inpole += 1;
		}
	}

	if (c == GLUT_KEY_UP) {
		int ly = heroY_inpole;
		int lx = heroX_inpole - 1;
		if ((pole[lx][ly] == 0 || pole[lx][ly] == 2) && lx > -1 || pole[lx][ly] == 4)
		{
			hero_Y += CELL_AREA;
			heroX_inpole -= 1;
		}
	}
	if (pole[heroX_inpole][heroY_inpole] == 4)
	{
		regime = 4;
		complexity = complexity - 30;
		if (swish_pole == 0)
		{
			regime = 2;
			hero_X = 1;
			hero_Y = 1;
			heroX_inpole = LABIRINT_HEIGHT - 1;
			heroY_inpole = 0;
			mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
			mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
			mpsX_inpole = 1;
			mpsY_inpole = LABIRINT_HEIGHT;
		}
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
		mps_X = (LABIRINT_WIDTH - 2)*CELL_AREA + 1;
		mps_Y = (LABIRINT_HEIGHT - 2)*CELL_AREA + 1;
		mpsX_inpole = 1;
		mpsY_inpole = LABIRINT_HEIGHT;
		swish_pole = swish_pole - 1;
	}


	if ((hero_X == mps_X) && (hero_Y == mps_Y))
	{
		life = life - 1;
		if (life == 0)
		{
			regime = 3;
		}
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
	}
}



void timer(int mass)
{
	//	while (true)
	for (int i = 0; i < 2; i++)
	{
		int r = rand() % 4;
		if (i == 0)
		{
			if (abs(mps_X - hero_X) > abs(mps_Y - hero_Y))
			{
				if (mps_X < hero_X){
					r = 0;
				}
				else {
					r = 2;
				}
			}
			else{
				if (mps_Y < hero_Y){
					r = 1;
				}
				else {
					r = 3;
				}
			}
		}
		if (r == 0)				//right
		{
			int ly = mpsY_inpole;
			int lx = mpsX_inpole;
			if (pole[lx][ly] == 0 || pole[lx][ly] == 1 || pole[lx][ly] == 4)
			{
				if (mps_X < LABIRINT_WIDTH*CELL_AREA)
				{
					mps_X += CELL_AREA;
					mpsY_inpole += 1;
					break;
				}
			}
		}

		if (r == 1)				//up
		{
			int ly = mpsY_inpole;
			int lx = mpsX_inpole - 1;
			if ((pole[lx][ly] == 0 || pole[lx][ly] == 2 || pole[lx][ly] == 4) && lx > -1)
			{
				if (mps_Y < LABIRINT_HEIGHT*CELL_AREA)
				{
					mps_Y += CELL_AREA;
					mpsX_inpole -= 1;
					break;
				}
			}
		}
		if (r == 2)		//left
		{
			int ly = mpsY_inpole - 1;
			int lx = mpsX_inpole;
			if ((pole[lx][ly] == 0 || pole[lx][ly] == 1 || pole[lx][ly] == 4) && ly > -1)
			{
				if (mps_X > 0)
				{
					mps_X -= CELL_AREA;
					mpsY_inpole -= 1;
					break;
				}
			}
		}


		if (r == 3)		//doun
		{
			int ly = mpsY_inpole;
			int lx = mpsX_inpole;
			if (pole[lx][ly] == 0 || pole[lx][ly] == 2 || pole[lx][ly] == 4)
			{
				if (mps_Y > 0)
				{
					mps_Y -= CELL_AREA;
					mpsX_inpole += 1;
					break;
				}
			}
		}
	}
	if ((hero_X == mps_X) && (hero_Y == mps_Y))
	{
		life = life - 1;
		if (life == 0)
		{
			regime = 3;
		}
		hero_X = 1;
		hero_Y = 1;
		heroX_inpole = LABIRINT_HEIGHT - 1;
		heroY_inpole = 0;
	}
	// планирование следующего вызова этой функции (функции timer)
	glutTimerFunc(complexity, timer, 0);
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(400, 200);

	glDepthFunc(GL_LESS);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("MAZE (тест)");

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutSpecialFunc(keyboard_s);
	glutTimerFunc(1000, timer, 0); // таймер

	LoadGLTextures();

	glutMainLoop();

	return 0;
}