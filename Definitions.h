#pragma once

const int MAXROWS = 22;
const int MAXCOLS = 80;
const int BUFFERSIZE = 20;
const int FILENAMESIZE = 255;
const int CLEARCOLS = 200;

// ASCII codes for special keys; for editing
const char ESC = 27;
const char LEFTARROW = 75;
const char UPARROW = 72;
const char RIGHTARROW = 77;
const char DOWNARROW = 80;
const char SPECIAL = (char)224;

struct DrawPoint;

/*
* A point used to represent a row/column on the screen or in the canvas array
*/
struct Point
{
	int row, col;

	Point() { row = 0; col = 0; }
	Point(int r, int c) { row = r; col = c; }
	Point(DrawPoint p);
};

/*
* A point used for the purpose of mathematical calculations (for drawing)
*/
struct DrawPoint
{
	double row, col;

	DrawPoint() { row = 0; col = 0; }
	DrawPoint(int r, int c) { row = r; col = c; }
	DrawPoint(Point p) { row = p.row;  col = p.col; }
};


/*
* Allows user to choose a location on the screen moving the cursor around with arrow keys.
* Continues until the ESC key is pressed, or an ASCII printable character is entered.
* Returns: the character entered by the user
*   Point pt will be updated to reflect the point chosen by the user
*/
char getPoint(Point& pt);

/*
* Secondary menu used for choosing the new drawing functions.
* Menu repeats until the user enters 'M' to return to the main menu.
* canvas is the main drawing canvas
* backupCanvas is the backup canvas used to support the undo operation
* animate - true: animate / false: no animation
*   animate will be updated to reflect the menu option chosen by the user
*/
void menuTwo(char canvas[][MAXCOLS], char backupCanvas[][MAXCOLS], bool& animate);

/*
* Recursive function to fill a section of the canvas. Replaces all of adjacent oldCh characters in
* the canvas section with newCh. 
* row and col is the row and column of the starting location where filling should begin
* oldCh is the character in the section to be replaced
* newCh is the character to replace with
* animate - true: animate / false: no animation
*/
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate);

/*
* Stores character ch into canvas at location p
* p is the Point(row, col) where the character is to be stored
* ch is the character to store
* if animate is true animation is also performed by drawing the character
* to the screen at the same time it is being stored in the canvas
*/
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate);

/*
* Draws a line between two points into the canvas.
* start is the point representing one end of the line
* end is the point reprsenting the other end of the line
* animate - true: animate the drawing / false: no animation
*/
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate);

/*
* Draws a box into the canvas, around a central point.
* center is the point representing the center of the box
* height is the height of the box (width is automatically proportionally chosen based on canvas size)
* animate - true: animate the drawing / false: no animation
*/
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool  animate);

/*
* Recursive function to draw a series of nested boxes into the canvas, around a central point.
* center is the point representing the center of the smallest box
* height is the height of the largest box (width is automatically proportionally chosen based on canvas size)
* animate - true: animate the drawing / false: no animation
*/
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate);

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
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate);



/*
* Finds the end point of a line, given the line's starting point, length, and angle
* angle: 0 = east, 90 = south, 180 = west, 270 = north
* Returns: the end point for the line
*/
DrawPoint findEndPoint(DrawPoint start, int len, int angle);

/*
* Converts a from degrees to radians
* Returns: angle a in radians
*/
double inline degree2radian(int a) { return (a * 0.017453292519); }

/*
* Used by the drawLine function to help fill in row gaps in a line
* col is the column; startRow is the starting row; endRow is the ending row
* ch is the character to draw; animate - true: animate the drawing / false: no animation
*/
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate);





//--------------------Old Functions---------------------------------------------------------------------

/*
* Gets a filename from the user. If file can be opened for reading,
* this function loads the file's contents into canvas.
* File is a TXT file located in the SavedFiles folder.
* If file cannot be opened, error message is displayed and
* canvas is left unchanged.
*/
void loadCanvas(char canvas[][MAXCOLS]);

/*
* Gets a filename from the user. If file can be opened for writing,
* this function writes the canvas contents into the file.
* File is a TXT file located in the SavedFiles folder.
* If file cannot be opened, error message is displayed.
*/
void saveCanvas(char canvas[][MAXCOLS]);

/*
* Initializes canvas to contain all spaces.
*/
void initCanvas(char canvas[][MAXCOLS]);

/*
* Displays canvas contents on the screen, with a border
* around the right and bottom edges.
*/
void displayCanvas(char canvas[][MAXCOLS]);

/*
* Allows user to edit the canvas by moving the cursor around and
* entering characters. Editing continues until the ESC key is pressed.
*/
void editCanvas(char canvas[][MAXCOLS]);

/*
* Copies contents of the "from" canvas into the "to" canvas.
*/
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS]);

/*
* Replaces all instances of a character in the canvas.
* oldCh is the character to be replaced.
* newCh character is the character to replace with.
*/
void replace(char canvas[][MAXCOLS], char oldCh, char newCh);

/*
* Shifts contents of the canvas by a specified number of rows and columns.
* rowValue is the number of rows by which to shift
*    positive numbers shift downward; negative numbers shift upward
* colValue is the number of rows by which to shift
*    positive numbers shift right; negative numbers shift left
*/
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue);

/*
* Clears a line on the output screen, then resets the cursor back to the
* beginning of this line.
* lineNum is the line number on the output screen to clear
* numOfChars is the number of characters to clear on this line
*/
void clearLine(int lineNum, int numOfChars);

/*
* Moves the cursor in the output window to a specified row and column.
* The next output produced by the program will begin at this position.
*/
void gotoxy(short row, short col);