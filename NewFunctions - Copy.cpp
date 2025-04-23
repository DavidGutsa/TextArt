#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <string>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}
// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(char canvas[][MAXCOLS], char backupCanvas[][MAXCOLS], bool& animate)
{
	char input = 'a';
	int height, branchAngle;
	char pointChar;
	Point userPoint, userPoint2;
	char animateChar;
	if (animate)
		animateChar = 'Y';
	else
		animateChar = 'N';
	int boxSize;
	while (input != 'm' && input != 'M') {
		displayCanvas(canvas);
		clearLine(MAXROWS + 1, CLEARCOLS);
		cout << "<A>nimate: " << animateChar << " / <F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";
		cin >> input;
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n'); //clears buffer
		switch (input) {
			case 'a':
			case 'A':
				animate = !animate;
				if (animate)
					animateChar = 'Y';
				else
					animateChar = 'N';
				break;
			case 'b':
			case 'B':
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Enter size: ";
				cin >> boxSize;
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel ";
				pointChar = getPoint(userPoint);
				if (pointChar == ESC)
					break;
				copyCanvas(backupCanvas, canvas);
				if (pointChar == 'c' || pointChar == 'C') {
					userPoint.row = MAXROWS / 2;
					userPoint.col = MAXCOLS / 2;
				}
				drawBox(canvas, userPoint, boxSize, animate);
				break;
			case 'n':
			case 'N':
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Enter size of largest box: ";
				cin >> boxSize;
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel ";
				pointChar = getPoint(userPoint);
				if (pointChar == ESC)
					break;
				copyCanvas(backupCanvas, canvas);
				if (pointChar == 'c' || pointChar == 'C') {
					userPoint.row = MAXROWS / 2;
					userPoint.col = MAXCOLS / 2;
				}
				drawBoxesRecursive(canvas, userPoint, boxSize, animate);
				break;
			case 'l':
			case 'L':
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Type any letter to choose start point / <ESC> to cancel";
				pointChar = getPoint(userPoint);
				if (pointChar == ESC) 
					break;
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Type any letter to choose end point / <ESC> to cancel";
				pointChar = getPoint(userPoint2);
				if (pointChar == ESC)
					break;
				copyCanvas(backupCanvas, canvas);
				drawLine(canvas, userPoint, userPoint2, animate);
				displayCanvas(canvas);
				break;
			case 't':
			case 'T':
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Enter approximate tree height: ";
				cin >> height;
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Enter branch angle: ";
				cin >> branchAngle;
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel";
				pointChar = getPoint(userPoint);
				if (pointChar == ESC)
					break;
				if (pointChar == 'c' || pointChar == 'C') {
					userPoint.col = MAXCOLS / 2;
					userPoint.row = MAXROWS - 1;
				}
				copyCanvas(backupCanvas, canvas);
				treeRecursive(canvas, userPoint, height, 270, branchAngle, animate);
				break;
			case 'f':
			case 'F':
				clearLine(MAXROWS + 1, CLEARCOLS);
				cout << "Enter character to fill with from current location / <ESC> to cancel ";
				pointChar = getPoint(userPoint);
				if (pointChar == ESC)
					break;
				copyCanvas(backupCanvas, canvas);
				fillRecursive(canvas, userPoint.row, userPoint.col, canvas[userPoint.row][userPoint.col], pointChar, animate);
				break;
		}

	}
}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	char input;
	int row = 0, col = 0;

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	input = _getch();
	while (input != ESC) {
		if (input == SPECIAL) {
			input = _getch();
			switch (input) { // moves cursor around by arrow keys
			case LEFTARROW:
				if (col > 0 && col <= MAXCOLS) {
					col--;
					gotoxy(row, col);
				}
				break;
			case RIGHTARROW:
				if (col >= 0 && col < MAXCOLS - 1) {
					col++;
					gotoxy(row, col);
				}
				break;
			case UPARROW:
				if (row > 0 && row <= MAXROWS) {
					row--;
					gotoxy(row, col);
				}
				break;
			case DOWNARROW:
				if (row >= 0 && row < MAXROWS - 1) {
					row++;
					gotoxy(row, col);
				}
				break;
			}
		}
		else if (input == '\0') // handles function keys
			input = _getch(); // gets input again
		else if (input != '\n' && input != '\t' && input != '\r' && input != '\b') { // handles whitespace keys
			cout << input;
			gotoxy(row, col);
			pt = { row, col }; // updates pointer to location user entered location at
			return input; // returns the character user entered
		}
		input = _getch();
	}
	return ESC;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	Point point(row, col);
	// base case ends if character is not what needs to be filled or out of bounds
	if (canvas[row][col] != oldCh || row < 0 || row >= MAXROWS || col < 0 || col >= MAXCOLS)
		return;
	// recursive fills current position with newCh then runs again on cardinal directions from current index
	drawHelper(canvas, point, newCh, animate);
	fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
	return;
}

/*
* Recursive function to draw a fractal tree into the canvas.
* start is the starting point for the tree (the base of the trunk)
* height is the approximate height of the entire tree
* startAngle represents the direction in which to draw the trunk where
*     0 = east, 90 = south, 180 = west, 270 = north
* branchAngle represents the angle of the branch to the tree trunk
*     45 means each branch will be at a 45 degree angle from the tree trunk
* animate - true: animate the drawing / false: no animation
*/
// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	DrawPoint end;
	// base case ends if start of next branch is out of bounds or next branch would be size zero
	if (height <= 2 || start.row < 0 || start.row >= MAXROWS || start.col < 0 || start.col >= MAXCOLS)
		return;
	end = findEndPoint(start, (height/3), startAngle); 
	drawLine(canvas, start, end, animate); // draws 1 for depth 0 of tree, 2 for depth 1, 4 for depth 2, 8 for depth 3, and so on
	treeRecursive(canvas, end, height-2, startAngle + branchAngle, branchAngle, animate); // right branch
	treeRecursive(canvas, end, height-2, startAngle - branchAngle, branchAngle, animate); // left branch
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	// base case
	if (height <= 1) // ends when the box would be too small to draw
		return;
	// recursive 
	drawBox(canvas, center, height, animate);
	drawBoxesRecursive(canvas, center, height-2, animate); // draws next box inside previous box
}