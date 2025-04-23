/*
* This program allows display and editing of text art (also called ASCII art).
* Project Authors: Jayden Abernethy, Jethro Stopka, Heriberto Chavez-Pullman, David Gutsa
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include <string>
#include "Definitions.h"
using namespace std;

const int MENULINE = 23;
const char INVALIDCHARS[] = "<>:\"/\\|?*";

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
//void saveCanvas(char canvas[][MAXCOLS], char fileName[]);
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


int main()
{
	char canvas[MAXROWS][MAXCOLS];
	char input = 'a', oldChar, newChar;	int moveRow, moveCol;
	bool animate = false;
	initCanvas(canvas);
	char undoCanvas[MAXROWS][MAXCOLS];
	copyCanvas(undoCanvas, canvas);
	while (input != 'q' && input != 'Q') {
		displayCanvas(canvas);
		clearLine(MAXROWS + 1, CLEARCOLS);
		cout << "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <U>ndo / <L>oad / <S>ave / <Q>uit: ";
		cin >> input;
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n'); //clears buffer
		switch (input) {
		case 'e': // manually add characters to canvas
		case 'E':
			clearLine(MAXROWS + 1, CLEARCOLS);
			cout << "Press <ESC> to stop editing ";
			copyCanvas(undoCanvas, canvas);
			editCanvas(canvas);
			break;
		case 'm': // moves everything in canvas by a determined amount
		case 'M':
			clearLine(MAXROWS + 1, CLEARCOLS);
			cout << "Enter the column units to move: ";
			cin >> moveCol;
			cout << "Enter the row units to move: ";
			cin >> moveRow;
			clearLine(MAXROWS + 1, 50);
			clearLine(MAXROWS + 2, 50);
			copyCanvas(undoCanvas, canvas);
			moveCanvas(canvas, moveRow, moveCol);
			break;
		case 'r': // replace character in canvas
		case 'R':
			clearLine(MAXROWS + 1, CLEARCOLS);
			cout << "Enter the character to be replaced: ";
			cin.get(oldChar);
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			cout << "Enter the character to replace with: ";
			cin.get(newChar);
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			clearLine(MAXROWS + 1, 50);
			clearLine(MAXROWS + 2, 50);
			copyCanvas(undoCanvas, canvas);
			replace(canvas, oldChar, newChar);
			break;
		case 'u': // undo last change to canvas, only can be preformed one time in a row
		case 'U':
			copyCanvas(canvas, undoCanvas);
			break;
		case 'l': // load canvas from file
		case 'L':
			clearLine(MAXROWS + 1, CLEARCOLS);
			copyCanvas(undoCanvas, canvas);
			loadCanvas(canvas);
			break;
		case 's': // save canvas to file
		case 'S':
			clearLine(MENULINE, MAXCOLS);
			saveCanvas(canvas);
			break;
		case 'd':
		case 'D':
			menuTwo(canvas, undoCanvas, animate);
			break;
		case 'c':
		case 'C':
			copyCanvas(undoCanvas, canvas);
			initCanvas(canvas);
		case '\n': // handles whitespace keys in input section
		case '\r':
		case '\t':
			clearLine(MAXROWS + 1, CLEARCOLS);
			clearLine(MAXROWS + 2, 100);
			break;
		}
	}
	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	for (int i = 0; i < MAXROWS; i++) { // goes over every index, checks if they have oldCh, and if so, replaces it with newCh
		for (int j = 0; j < MAXCOLS; j++) {
			if (canvas[i][j] == oldCh)
				canvas[i][j] = newCh;
		}
	}
}

void editCanvas(char canvas[][MAXCOLS])
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
			switch (input) {
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
			input = _getch();
		else if (input != '\n' && input != '\t' && input != '\r' && input != '\b') { // handles whitespace keys
			canvas[row][col] = input;
			cout << input;
			gotoxy(row, col);
		}
		input = _getch();
	}
}

void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	char newArr[MAXROWS][MAXCOLS];
	initCanvas(newArr);
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			if (i + rowValue >= 0 && i + rowValue < MAXROWS && j + colValue >= 0 && j + colValue < MAXCOLS)	// checks if new position is within bounds, 
				newArr[i + rowValue][j + colValue] = canvas[i][j];											// and if so, adds the element in the old index to the new position
		}
	}
	copyCanvas(canvas, newArr);
}


void initCanvas(char canvas[][MAXCOLS])
{
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			canvas[i][j] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	// Clear the screen
	system("cls");
	for (int i = 0; i <= MAXROWS; i++) {
		for (int j = 0; j <= MAXCOLS; j++) {
			if (i == MAXROWS)
				printf("%c", '-');
			else if (j == MAXCOLS)
				printf("|\n");
			else
				printf("%c", canvas[i][j]);
		}
	}
}

void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++)
			to[i][j] = from[i][j];
	}
}


void saveCanvas(char canvas[][MAXCOLS]) {
	char fileName[FILENAMESIZE];
	char filePath[FILENAMESIZE];
	ofstream outFile;
	bool valid = true;
	
	cout << "Enter the filename (don't enter 'txt'): ";
	cin.getline(fileName, FILENAMESIZE - 15); // room for "SavedFiles/" and ".txt"
	
	for (int i = 0; fileName[i] != '\0' && valid; i++) { // checks every character of the file name to check for disallowed characters
		for (int j = 0; INVALIDCHARS[j] != '\0' && valid; j++) {
			if (fileName[i] == INVALIDCHARS[j])
				valid = false;
		}
	}

	// Build file path
	snprintf(filePath, FILENAMESIZE, "SavedFiles/%s.txt", fileName);

	// Try to open file
	outFile.open(filePath);
	if (!outFile && !valid) // error if write error or invalid file name
	{
		cout << "ERROR: File could not be written.\n";
		system("pause");
		return;
	}

	// Write canvas to file
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++)
			outFile << canvas[i][j];
		outFile << '\n';
	}

	outFile.close();
	cout << "File saved!\n";
	cout << "Press any key to continue . . .";
	(void)_getch();
}

void loadCanvas(char canvas[][MAXCOLS]) {
	char fileName[FILENAME_MAX - 15];
	clearLine(MAXROWS + 1, CLEARCOLS);
	cout << "Enter filename to load (don't enter 'txt'): ";
	cin.getline(fileName, FILENAMESIZE - 15);
	ifstream inFile;
	char filePath[FILENAMESIZE];
	char temp[FILENAMESIZE];
	char c = 'a';
	int row = 0, col = 0;

	snprintf(filePath, FILENAMESIZE, "SavedFiles/%s.txt", fileName);
	inFile.open(filePath);

	if (!inFile) { // error state
		cout << "ERROR: File cannot be read. ";
		system("pause");
		inFile.close();
		return;
	}
	initCanvas(canvas);
	while (row < MAXROWS && c != '\0') { // loads file to array by character
		c = inFile.get();
		col = 0;
		while (col < MAXCOLS && c != '\0' && c != '\r' && c != '\n' && !inFile.eof()) {
			canvas[row][col] = c;
			c = inFile.get();
			col++;
		}
		if (c != '\n' && !inFile.eof())
			inFile.getline(temp, FILENAMESIZE);
		row++;
	}
	inFile.close();
}