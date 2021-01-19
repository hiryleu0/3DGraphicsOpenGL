
/* Copyright (c) Mark J. Kilgard, 1994. */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>


void display();
void reshape(int, int);
void init();
void timer(int);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowPosition(20, 10);
	glutInitWindowSize(700, 700);

	glutCreateWindow("3D Graphics");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);

	init();

	glutMainLoop();

	return 0;
}

float x = -5.0;
float y = 0.0;
int state = 1;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0 , 0.0, -8.0);
	glRotatef(y, 0.0, 0.0, 1.0);
	glRotatef(y, 0.0, 1.0, 0.0);
	glRotatef(y, 1.0, 0.0, 0.0);

	glBegin(GL_QUADS);
	{
		//front
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-1.0, 1.0, 1.0);
		glVertex3f(-1.0, -1.0, 1.0);
		glVertex3f(1.0, -1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		//back
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, 1.0, -1.0);
		glVertex3f(1.0, -1.0, -1.0);
		glVertex3f(-1.0, -1.0, -1.0);
		glVertex3f(-1.0, 1.0, -1.0);
		//right
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glVertex3f(1.0, -1.0, 1.0);
		glVertex3f(1.0, -1.0, -1.0);
		glVertex3f(1.0, 1.0, -1.0);
		//left
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-1.0, 1.0, -1.0);
		glVertex3f(-1.0, -1.0, -1.0);
		glVertex3f(-1.0, -1.0, 1.0);
		glVertex3f(-1.0, 1.0, 1.0);
		//top
		glColor3f(0.0, 1.0, 1.0);
		glVertex3f(-1.0, 1.0, -1.0);
		glVertex3f(-1.0, 1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glVertex3f(1.0, 1.0, -1.0);
		//bottom
		glColor3f(1.0, 0.0, 1.0);
		glVertex3f(-1.0, -1.0, -1.0);
		glVertex3f(-1.0, -1.0, 1.0);
		glVertex3f(1.0, -1.0, 1.0);
		glVertex3f(1.0, -1.0, -1.0);
	}
	glEnd();	

	glutSwapBuffers();
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 2.0, 50.0);
	glMatrixMode(GL_MODELVIEW);
}

void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(1, timer, 0);

	switch (state)
	{
	case 1:
		if (x < -5)
			x += 0.15;
		else
			state = -1;
		break;
	case -1:
		if (x > -15)
			x -= 0.15;
		else
			state = 1;
		break;
	}
	y += 0.3;
}