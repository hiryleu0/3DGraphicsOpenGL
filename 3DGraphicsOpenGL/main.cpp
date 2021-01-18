
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

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);

	glutInitWindowPosition(20, 10);
	glutInitWindowSize(1000, 700);

	glutCreateWindow("3D Graphics");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	init();

	glutMainLoop();

	return 0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	


	glFlush();
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.1);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}