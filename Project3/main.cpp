#include <time.h>
#include "Dot.h"
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>



#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <GL/freeglut.h>

// Use standard main to have console background:
// int main()

// Use WinMain if you don't want the console in the background:
using namespace std;

float red = 1.0, green = 0.0, blue = 0.0;
int tmpx, tmpy; // store the first point when shape is line, rectangle or circle
int tmp1x, tmp1y;
int tmp2x, tmp2y;
int brushSize = 4;
int eraserSize = 1;
bool isSecond = false;
bool isRandom = false;
bool isEraser = false;
bool isRadial = false;
float window_w = 500;
float window_h = 500;

int shape = 1; // 1:point, 2:line, 3:rectangle, 4:circle, 5:brush, 6:diamond, 7:Triangle, 8:Quadrateral
int triangleMouseClick = 0; 
int quadMouseClick = 0;
std::vector<Dot> dots;		// store all the points until clear
std::list<int> undoHistory; // record for undo, maximum 20 shapes in history
std::list<int> redoHistory; // record for redo, maximum 20 shapes in history
std::vector<Dot> redoDots;  // store the dots after undo temporaly

void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < dots.size(); i++)
	{
		glColor3f(dots[i].getR(), dots[i].getG(), dots[i].getB());
		glVertex2i(dots[i].getX(), dots[i].getY());
	}
	glEnd();
	glutSwapBuffers();
}

void clear()
{
	dots.clear();
	undoHistory.clear();
	redoDots.clear();
	redoHistory.clear();
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::cout << asctime(timeinfo)

		<< "[Info] The window and the history are cleared successfully.\n";
}

void quit()
{
	std::cout << "Thank you for using this Paint tool! Goodbye!" << std::endl;
	exit(0);
}
void undo()
{
	if (undoHistory.size() > 0)
	{
		if (undoHistory.back() != dots.size() && redoHistory.back() != dots.size())
		{
			redoHistory.push_back(dots.size());
		}
		int numRemove = dots.size() - undoHistory.back();
		for (int i = 0; i < numRemove; i++)
		{
			redoDots.push_back(dots.back());
			dots.pop_back();
		}
		redoHistory.push_back(undoHistory.back());
		undoHistory.pop_back();
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] Cannot undo. This is the first record in the history.\n";
	}
}

void redo()
{
	if (redoHistory.size() > 1)
	{
		undoHistory.push_back(redoHistory.back());
		redoHistory.pop_back();
		int numRemove = redoHistory.back() - dots.size();
		for (int i = 0; i < numRemove; i++)
		{
			dots.push_back(redoDots.back());
			redoDots.pop_back();
		}
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] Cannot redo. This is the last record in the history.\n";
	}
}

void drawDot(int mousex, int mousey)
{
	Dot newDot(mousex, window_h - mousey, isEraser ? 1.0 : red, isEraser ? 1.0 : green, isEraser ? 1.0 : blue);
	dots.push_back(newDot);
}

void drawBrush(int x, int y)
{
	for (int i = 0; i < brushSize; i++)
	{
		int randX = rand() % (brushSize + 1) - brushSize / 2 + x;
		int randY = rand() % (brushSize + 1) - brushSize / 2 + y;
		drawDot(randX, randY);
	}
}

void drawLine(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	// Bresenham's line algorithm is only good when abs(dx) >= abs(dy)
	// So when abs(dx) < abs(dy), change axis x and y
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawDot(y, x);
	else
		drawDot(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawDot(y, x);
		else
			drawDot(x, y);
	}
}

/**
 * We can use drawLine function to draw the rectangle
 *
 * Top-left corner specified by the first click,
 * and the bottom-right corner specified by a second click
 */
void drawRectangle(int x1, int y1, int x2, int y2)
{
	std::cout << x1 << endl << y1 << endl << x2 << endl << y2 << endl;
	if (x1 < x2 && y1 < y2)
	{
		drawLine(x1, y1, x2, y1);
		drawLine(x2, y1, x2, y2);
		drawLine(x2, y2, x1, y2);
		drawLine(x1, y2, x1, y1);	
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] The first click should be the top-left corner, the second click should be bottom-right corner.\n";
	}
}

void drawDiamond(int x1, int y1, int x2, int y2)
{
	std::cout << x1 << endl << y1 << endl << x2 << endl << y2 << endl;
	if (x1 < x2 && y1 < y2)
	{
		y2 = y1;
		int d = (int)sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)));
		std::cout << endl << d << endl;
		int xm = (x1 + x2 + 1) / 2;
		int ym = (y1 + y2 + 1) / 2;


		drawLine(x1, y1, xm, ym + d / 2);
		drawLine(xm, ym + d / 2, x2, y2);
		drawLine(xm, ym - d / 2, x2, y2);
		drawLine(x1, y1, xm, ym - d / 2);
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] The first click should be the top-left corner, the second click should be bottom-right corner.\n";
	}
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
	drawLine(x1, y1, x2, y2);
	drawLine(x2, y2, x3, y3);
	drawLine(x3, y3, x1, y1);
}

void drawQuad(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
	drawLine(x1, y1, x2, y2);
	drawLine(x2, y2, x3, y3);
	drawLine(x3, y3, x4, y4);
	drawLine(x4, y4, x1, y1);
}

/**
 * Midpoint circle algorithm
 */
void drawCircle(int x1, int y1, int x2, int y2)
{
	int r = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	double d;
	int x, y;

	x = 0;
	y = r;
	d = 1.25 - r;

	while (x <= y)
	{
		drawDot(x1 + x, y1 + y);
		drawDot(x1 - x, y1 + y);
		drawDot(x1 + x, y1 - y);
		drawDot(x1 - x, y1 - y);
		drawDot(x1 + y, y1 + x);
		drawDot(x1 - y, y1 + x);
		drawDot(x1 + y, y1 - x);
		drawDot(x1 - y, y1 - x);
		x++;
		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			y--;
			d += 2 * (x - y) + 5;
		}
	}
}

void drawRadialBrush(int x, int y)
{
	int xc = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int yc = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	int dx, dy;

	dx = xc - x;
	dy = yc - y;

	drawDot(xc + dx, yc + dy);
	drawDot(xc - dx, yc + dy);
	drawDot(xc + dx, yc - dy);
	drawDot(xc - dx, yc - dy);
	drawDot(xc + dy, yc + dx);
	drawDot(xc - dy, yc + dx);
	drawDot(xc + dy, yc - dx);
	drawDot(xc - dy, yc - dx);
}

void erase(int x, int y)
{
	for (int i = -eraserSize; i <= eraserSize; i++)
	{
		for (int j = -eraserSize; j <= eraserSize; j++)
		{
			drawDot(x + i, y + j);
		}
	}
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	isSecond = false;
	switch (key)
	{
	case 'q':
	case 27: // 27 is the esc key
		quit();
		break;
	case 'c':
		clear();
		break;
	case '+':
		if (shape == 5 && !isEraser)
		{
			if (brushSize < 16)
				brushSize += 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Airbrush's size cannot be larger. It is already the largest.\n";
			}
		}
		else if (isEraser)
		{
			if (eraserSize < 10)
				eraserSize += 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Eraser's size cannot be larger. It is already the largest.\n";
			}
		}
		break;
	case '-':
		if (shape == 5 && !isEraser)
		{
			if (brushSize > 4)
				brushSize -= 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Airbrush's size cannot be smaller. It is already the smallest.\n";
			}
		}
		else if (isEraser)
		{
			if (eraserSize > 2)
				eraserSize -= 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Eraser's size cannot be smaller. It is already the smallest.\n";
			}
		}
		break;
	case 'u':
		undo();
		break;
	case 'r':
		redo();
		break;
	}
}

void mouse(int bin, int state, int x, int y)
{
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (isRandom)
		{
			srand(time(NULL));
			red = float(rand()) / float(RAND_MAX);
			green = float(rand()) / float(RAND_MAX);
			blue = float(rand()) / float(RAND_MAX);
		}
		if (isEraser)
		{
			undoHistory.push_back(dots.size());
			erase(x, y);
		}
		else
		{
			if (shape == 1)
			{
				undoHistory.push_back(dots.size());
				if (isRadial)
					drawRadialBrush(x, y);
				else
					drawDot(x, y);
			}
			else if (shape == 5)
			{
				undoHistory.push_back(dots.size());
				drawBrush(x, y);
			}
			else if (shape == 7) {
				if (triangleMouseClick == 0) {
					tmpx = x;
					tmpy = y;
					triangleMouseClick = 1;
				}
				else if (triangleMouseClick == 1) {
					tmp1x = x;
					tmp1y = y;
					triangleMouseClick = 2;
				}
				else {
					triangleMouseClick = 0;
					if (undoHistory.back() != dots.size())
						undoHistory.push_back(dots.size());
					drawTriangle(tmpx, tmpy, tmp1x, tmp1y, x, y);
				}
			}
			else if (shape == 8) {
				if (quadMouseClick == 0) {
					tmpx = x;
					tmpy = y;
					quadMouseClick = 1;
				}else if (quadMouseClick == 1) {
					tmp1x = x;
					tmp1y = y;
					quadMouseClick = 2;
				}else if (quadMouseClick == 2) {
					tmp2x = x;
					tmp2y = y;
					quadMouseClick = 3;
				}
				else {
					quadMouseClick = 0;
					if (undoHistory.back() != dots.size())
						undoHistory.push_back(dots.size());
					drawQuad(tmpx, tmpy, tmp1x, tmp1y, tmp2x, tmp2y, x, y);
				}
			}
			else{
				if (!isSecond)
				{
					tmpx = x;
					tmpy = y;
					isSecond = true;
				}
				else
				{
					if (undoHistory.back() != dots.size())
						undoHistory.push_back(dots.size());
					if (shape == 2)
						drawLine(tmpx, tmpy, x, y);
					else if (shape == 3)
						drawRectangle(tmpx, tmpy, x, y);
					else if (shape == 4)
						drawCircle(tmpx, tmpy, x, y);
					else if (shape == 6)
						drawDiamond(tmpx, tmpy, x, y);
					isSecond = false;
				}
			}
		}
		if (undoHistory.size() > 20)
		{
			undoHistory.pop_front();
		}
	}
}

void motion(int x, int y)
{
	if (isEraser)
		erase(x, y);
	else
	{
		if (shape == 1)
		{
			if (isRadial)
				drawRadialBrush(x, y);
			else
				drawDot(x, y);
		}
		if (shape == 5)
			drawBrush(x, y);
	}
}

void reshape(int w, int h)
{
	window_w = w;
	window_h = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void processMainMenu(int value)
{
	switch (value)
	{
	case 0:
		quit();
		break;
	case 1:
		clear();
		break;
	case 2:
		undo();
		break;
	case 3:
		redo();
		break;
	}
}

void processBrushSizeMenu(int value)
{
	shape = 5;
	isEraser = false;
	brushSize = value;
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

void processColourMenu(int value)
{
	isSecond = false;
	isEraser = false;
	isRandom = false;

	switch (value)
	{
	case 1: // red
		red = 1.0;
		green = 0.0;
		blue = 0.0;
		break;
	case 2: // green
		red = 0.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 3: // blue
		red = 0.0;
		green = 0.0;
		blue = 1.0;
		break;
	case 4: // purple
		red = 0.5;
		green = 0.0;
		blue = 0.5;
		break;
	case 5: // yellow
		red = 1.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 6: // random
		isRandom = true;
		break;
	}
}

void processShapeMenu(int value)
{
	shape = value;
	isEraser = false;
	isSecond = false;
	isRadial = false;
	triangleMouseClick = 0;
	quadMouseClick = 0;

	switch (shape)
	{
	case 1:
		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
		break;
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
		if (shape == 6)
			std::cout << "Diamond Shape" << endl << endl;
		else if (shape == 7)
			std::cout << "Triangle Shape" << endl << endl;
		else if (shape == 8)
			std::cout << "Quad Shape" << endl << endl;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}

void processEraserSizeMenu(int value)
{
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	eraserSize = value;
	isEraser = true;
}

void processRadicalBrushMenu(int value)
{
	isRadial = value == 1 ? true : false;
}

void createOurMenu()
{
	int colourMenu = glutCreateMenu(processColourMenu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Purple", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Random", 6);

	int sizeMenu = glutCreateMenu(processBrushSizeMenu);
	glutAddMenuEntry("4px", 4);
	glutAddMenuEntry("8px", 8);
	glutAddMenuEntry("12px", 12);
	glutAddMenuEntry("16px", 16);

	int shapeMenu = glutCreateMenu(processShapeMenu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Rectangle", 3);
	glutAddMenuEntry("Circle", 4);
	glutAddMenuEntry("Diamond", 6);
	glutAddMenuEntry("Trianlge", 7);
	glutAddMenuEntry("Quadrilateral", 8);
	glutAddSubMenu("Airbrush", sizeMenu);

	int eraserSizeMenu = glutCreateMenu(processEraserSizeMenu);
	glutAddMenuEntry("Small", 2);
	glutAddMenuEntry("Medium", 6);
	glutAddMenuEntry("Large", 10);

	//int radicalBrushMenu = glutCreateMenu(processRadicalBrushMenu);
	//glutAddMenuEntry("True", 1);
	//glutAddMenuEntry("False", 2);

	int main_id = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Colour", colourMenu);
	glutAddSubMenu("Shapes", shapeMenu);
	//glutAddSubMenu("Radical Paint Brush", radicalBrushMenu);
	glutAddSubMenu("Eraser", eraserSizeMenu);
	glutAddMenuEntry("Undo", 2);
	glutAddMenuEntry("Redo", 3);
	glutAddMenuEntry("Clear", 1);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(void)
{
	/* background color */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(red, green, blue);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, window_w, 0.0, window_h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void FPS(int val)
{
	glutPostRedisplay();
	glutTimerFunc(0, FPS, 0);
}

void callbackInit()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(17, FPS, 0);
}

void printGuide()
{
	std::cout << "#########################################################################\n"
		<< "#                    Welcome to use this Paint tool!                    #\n"
		<< "#########################################################################\n"
		<< "-> close the window.\n"
		<< "Keyboard 'q'\t"
		<< "-> close the window.\n"
		<< "Keyboard 'esc'\t"
		<< "-> close the window.\n"
		<< "Keyboard 'c'\t"
		<< "-> clear all the points and clear the history.\n"
		<< "Keyboard '+'\t"
		<< "-> larger size of eraser or brush.\n"
		<< "Keyboard '-'\t"
		<< "-> smaller size of eraser or brush.\n"
		<< "Keyboard 'u'\t"
		<< "-> undo, the history can keep maximum 20 records.\n"
		<< "Keyboard 'r'\t"
		<< "-> redo, the history can keep maximum 20 records.\n"
		<< "################################# Paint #################################" << std::endl;
}

int main(int argc, char** argv)
{
	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(300, 400);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Paint");
	callbackInit();
	init();
	printGuide();
	createOurMenu();
	glutMainLoop();
	return 0;
}
