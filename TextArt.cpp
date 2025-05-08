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

// Function declarations
void loadCanvas(char canvas[][MAXCOLS]);
bool loadCanvas(char canvas[][MAXCOLS], char filename[]);
void saveCanvas(char canvas[][MAXCOLS]);
bool saveCanvas(char canvas[][MAXCOLS], char filename[]);
bool loadClips(List& clips, char filename[]);
bool saveClips(List& clips, char filename[]);
void play(List& clips);
void initCanvas(char canvas[][MAXCOLS]);
void displayCanvas(char canvas[][MAXCOLS]);
void editCanvas(char canvas[][MAXCOLS]);
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS]);
void replace(char canvas[][MAXCOLS], char oldCh, char newCh);
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue);
void clearLine(int lineNum, int numOfChars);
void gotoxy(short row, short col);

int main()
{
    //Initialize the current canvas as a Node
    Node* current = newCanvas();

    // Input variables
    char input = 'a', oldChar, newChar;
    int moveRow, moveCol;
    bool animate = false;

    // Initialize the undo, redo, and clips lists
    List undoList = { NULL, 0 };
    List redoList = { NULL, 0 };
    List clipsList = { NULL, 0 };

    // Clear the screen manually using gotoxy and clearLine
    gotoxy(0, 0);
    for (int i = 0; i <= MAXROWS + 3; i++) {
        clearLine(i, CLEARCOLS);
    }

    while (input != 'q' && input != 'Q') {
        // Display the current canvas
        displayCanvas(current->item);

        // Display the top menu line with undo/redo/clip information
        clearLine(MAXROWS + 1, CLEARCOLS);
        cout << "<A>nimate: " << (animate ? 'Y' : 'N') << " / <U>ndo: " << undoList.count;
        if (redoList.count > 0) {
            cout << " / Red<O>: " << redoList.count;
        }
        cout << " / cl<I>p: " << clipsList.count;
        if (clipsList.count >= 2) {
            cout << " / <P>lay";
        }

        // Display the main menu line
        clearLine(MAXROWS + 2, CLEARCOLS);
        cout << "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";

        // Get user input
        cin >> input;
        cin.clear();
        cin.ignore((numeric_limits<streamsize>::max)(), '\n'); // clears buffer

        switch (input) {

            // toggle animation
        case 'a':
        case 'A':
            animate = !animate;
            break;

            // undo operation
        case 'u':
        case 'U':
            if (undoList.count > 0)
            {
                restore(undoList, redoList, current);
            }
            break;

            // redo operation
        case 'o':
        case 'O':
            if (redoList.count > 0)
            {
                restore(redoList, undoList, current);
            }
            break;

            // add current canvas to clips
        case 'i':
        case 'I':
            // Create a copy of the current canvas and add it to the clips list
            addNode(clipsList, newCanvas(current));
            break;

            // play animation clips
        case 'p':
        case 'P':
            if (clipsList.count >= 2) {
                play(clipsList);
            }
            break;

            // manually add characters to canvas
        case 'e':
        case 'E':
            clearLine(MAXROWS + 1, CLEARCOLS);
            cout << "Press <ESC> to stop editing ";

            // Add current state to undo list before modifying
            addUndoState(undoList, redoList, current);

            // Edit the canvas
            editCanvas(current->item);
            break;

            // moves everything in canvas by a determined amount
        case 'm':
        case 'M':
            clearLine(MAXROWS + 1, CLEARCOLS);
            cout << "Enter the column units to move: ";
            cin >> moveCol;
            cout << "Enter the row units to move: ";
            cin >> moveRow;
            clearLine(MAXROWS + 1, 50);
            clearLine(MAXROWS + 2, 50);

            // Add current state to undo list before modifying
            addUndoState(undoList, redoList, current);

            // Move the canvas contents
            moveCanvas(current->item, moveRow, moveCol);
            break;

            // replace character in canvas
        case 'r':
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

            // Add current state to undo list before modifying
            addUndoState(undoList, redoList, current);

            // Replace characters in the canvas
            replace(current->item, oldChar, newChar);

            break;

            // load canvas or animation from file
        case 'l':
        case 'L':
            clearLine(MAXROWS + 1, CLEARCOLS);
            cout << "<C>anvas or <A>nimation ? ";
            char loadType;
            cin >> loadType;
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');

            if (loadType == 'C' || loadType == 'c')
            {
                // Add current state to undo list before loading
                addUndoState(undoList, redoList, current);

                // Load a single canvas
                loadCanvas(current->item);
            }
            else if (loadType == 'A' || loadType == 'a')
            {
                // Load animation clips
                clearLine(MAXROWS + 1, CLEARCOLS);
                cout << "Enter the filename (don't enter 'txt'): ";
                char filename[FILENAMESIZE];
                cin.getline(filename, FILENAMESIZE - 15);

                // Form the base path
                char filePath[FILENAMESIZE];
                snprintf(filePath, FILENAMESIZE, "SavedFiles/%s", filename);

                // Try to load the animation clips
                if (!loadClips(clipsList, filePath))
                {
                    cout << "ERROR: File could not be read: ";
                    system("pause");
                }
                else
                {
                    // Add these lines to show success message and pause
                    // Wait for a keypress
                    cout << "Clips loaded!" << endl;
                    cout << "Press any key to continue . . .";
                    (void)_getch();
                }
            }
            break;

            // save canvas or animation to file
        case 's':
        case 'S':
            clearLine(MAXROWS + 1, CLEARCOLS);
            cout << "<C>anvas or <A>nimation ? ";
            char saveType;
            cin >> saveType;
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');

            if (saveType == 'C' || saveType == 'c')
            {
                // Save a single canvas
                saveCanvas(current->item);
            }
            else if (saveType == 'A' || saveType == 'a')
            {
                // Save animation clips
                clearLine(MAXROWS + 1, CLEARCOLS);
                cout << "Enter the filename (don't enter 'txt'): ";
                char filename[FILENAMESIZE];
                cin.getline(filename, FILENAMESIZE - 15);

                // Check for invalid characters in filename
                bool valid = true;
                for (int i = 0; filename[i] != '\0' && valid; i++) {
                    for (int j = 0; INVALIDCHARS[j] != '\0' && valid; j++) {
                        if (filename[i] == INVALIDCHARS[j]) {
                            valid = false;
                        }
                    }
                }

                if (!valid)
                {
                    cout << "ERROR: Invalid filename. ";
                    system("pause");
                }
                else {
                    // Form the base path
                    char filePath[FILENAMESIZE];
                    snprintf(filePath, FILENAMESIZE, "SavedFiles/%s", filename);

                    // Try to save the animation clips
                    if (!saveClips(clipsList, filePath))
                    {
                        cout << "ERROR: Files could not be written. ";
                        system("pause");
                    }
                    else
                    {
                        cout << "Animation files saved!\n";
                        cout << "Press any key to continue . . .";
                        (void)_getch();
                    }
                }
            }
            break;

            //draw menu
        case 'd':
        case 'D':
            menuTwo(current, undoList, redoList, clipsList, animate);
            break;

            //clear canvas
        case 'c':
        case 'C':
            // Add current state to undo list before clearing
            addUndoState(undoList, redoList, current);

            // Clear the canvas
            initCanvas(current->item);

            break;

            // handles whitespace keys in input section
        case '\n':
        case '\r':
        case '\t':
            clearLine(MAXROWS + 1, CLEARCOLS);
            clearLine(MAXROWS + 2, 100);
            break;
        }
    }

    // Clean up memory before exiting
    delete current;
    deleteList(undoList);
    deleteList(redoList);
    deleteList(clipsList);

    return 0;
}


/*
  Moves the cursor in the output window to a specified row and column.
  The next output produced by the program will begin at this position.
*/
void gotoxy(short row, short col)
{
    COORD pos = { col, row };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


/*
  Clears a line on the output screen, then resets the cursor back to the
  beginning of this line.
  lineNum is the line number on the output screen to clear
  numOfChars is the number of characters to clear on this line
*/
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


/*
  Replaces all instances of a character in the canvas.
  oldCh is the character to be replaced.
  newCh character is the character to replace with.
*/
void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
    // Goes over every index, checks if they have oldCh, and if so, replaces it with newCh
    for (int i = 0; i < MAXROWS; i++) {
        for (int j = 0; j < MAXCOLS; j++) {
            if (canvas[i][j] == oldCh)
                canvas[i][j] = newCh;
        }
    }
}


/*
  Allows user to edit the canvas by moving the cursor around and
  entering characters. Editing continues until the ESC key is pressed.
*/
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
        // handles function keys
        else if (input == '\0')
            input = _getch();
        // handles whitespace keys
        else if (input != '\n' && input != '\t' && input != '\r' && input != '\b')
        {
            canvas[row][col] = input;
            cout << input;
            gotoxy(row, col);
        }
        input = _getch();
    }
}


/*
  Shifts contents of the canvas by a specified number of rows and columns.
  rowValue is the number of rows by which to shift
  positive numbers shift downward; negative numbers shift upward
  colValue is the number of rows by which to shift
  positive numbers shift right; negative numbers shift left
*/
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
    char newArr[MAXROWS][MAXCOLS];
    initCanvas(newArr);
    for (int i = 0; i < MAXROWS; i++)
    {
        for (int j = 0; j < MAXCOLS; j++)
        {
            // Checks if new position is within bounds, and if so, adds the element in the old index to the new position
            if (i + rowValue >= 0 && i + rowValue < MAXROWS && j + colValue >= 0 && j + colValue < MAXCOLS)
                newArr[i + rowValue][j + colValue] = canvas[i][j];
        }
    }
    copyCanvas(canvas, newArr);
}


/*
  Initializes canvas to contain all spaces.
*/
void initCanvas(char canvas[][MAXCOLS])
{
    for (int i = 0; i < MAXROWS; i++)
    {
        for (int j = 0; j < MAXCOLS; j++)
        {
            canvas[i][j] = ' ';
        }
    }
}


/*
  Displays canvas contents on the screen, with a border
  around the right and bottom edges.
  Uses screen buffering technique to avoid flickering and cursor movement
*/
void displayCanvas(char canvas[][MAXCOLS]) {
    static char buffer[MAXROWS + 1][MAXCOLS + 2];

    // copies items in array along with the right border with newlines
    for (int row = 0; row < MAXROWS; row++)
    {
        memcpy(buffer[row], canvas[row], MAXCOLS);
        memcpy(&buffer[row][MAXCOLS], "|\n", 2);
    }

    // creates the bottom border
    for (int col = 0; col < MAXCOLS + 1; col++)
    {
        buffer[MAXROWS][col] = '-';
    }
    buffer[MAXROWS][MAXCOLS + 1] = '\n';

    // resets cursor back to top to get ready for write
    gotoxy(0, 0);
    // writes buffer to screen
    cout.write(&buffer[0][0],(MAXROWS + 1) * (MAXCOLS + 2));
}

/*
  Copies contents of the "from" canvas into the "to" canvas.
*/
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
    for (int i = 0; i < MAXROWS; i++) {
        for (int j = 0; j < MAXCOLS; j++)
            to[i][j] = from[i][j];
    }
}


/*
  Gets a filename from the user. If file can be opened for reading,
  this function loads the file's contents into canvas.
  File is a TXT file located in the SavedFiles folder.
  If file cannot be opened, error message is displayed and
  canvas is left unchanged.
*/
void loadCanvas(char canvas[][MAXCOLS])
{
    char fileName[FILENAMESIZE - 15];
    clearLine(MAXROWS + 1, CLEARCOLS);
    cout << "Enter filename to load (don't enter 'txt'): ";
    cin.getline(fileName, FILENAMESIZE - 15);

    char filePath[FILENAMESIZE];
    // Build full file path
    snprintf(filePath, FILENAMESIZE, "SavedFiles/%s.txt", fileName);

    //Attempt to load the file in error
    if (!loadCanvas(canvas, filePath))
    {
        cout << "ERROR: File cannot be read. ";
        system("pause");
    }
}


/*
  Opens the specified TXT filename for reading.
  If the file can be opened for reading, this function loads the
  file's contents into current canvas, and then returns TRUE.
  If the file cannot be opened for reading, returns FALSE.
  If the file cannot be opened, canvas is unchanged.
*/
bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
    ifstream inFile;
    char temp[FILENAMESIZE];
    char c = 'a';
    int row = 0, col = 0;

    inFile.open(filename);

    // Error state
    if (!inFile)
    {
        return false;
    }

    initCanvas(canvas);

    // Load file to array by character
    while (row < MAXROWS && !inFile.eof())
    {
        c = inFile.get();
        col = 0;
        while (col < MAXCOLS && c != '\0' && c != '\r' && c != '\n' && !inFile.eof())
        {
            canvas[row][col] = c;
            c = inFile.get();
            col++;
        }
        if (c != '\n' && !inFile.eof())
            inFile.getline(temp, FILENAMESIZE);
        row++;
    }

    inFile.close();
    return true;
}


/*
* Gets a filename from the user. If file can be opened for writing,
* this function writes the canvas contents into the file.
* File is a TXT file located in the SavedFiles folder.
* If file cannot be opened, error message is displayed.
*/
void saveCanvas(char canvas[][MAXCOLS])
{
    char fileName[FILENAMESIZE - 15];
    char filePath[FILENAMESIZE];
    bool valid = true;

    clearLine(MAXROWS + 1, CLEARCOLS);
    cout << "Enter the filename (don't enter 'txt'): ";
    cin.getline(fileName, FILENAMESIZE - 15);

    // Check for invalid characters in filename
    for (int i = 0; fileName[i] != '\0' && valid; i++)
    {
        for (int j = 0; INVALIDCHARS[j] != '\0' && valid; j++)
        {
            if (fileName[i] == INVALIDCHARS[j])
            {
                valid = false;
            }
        }
    }

    if (!valid)
    {
        cout << "ERROR: Invalid filename.\n";
        system("pause");
    }
    else
    {
        // Build file path
        snprintf(filePath, FILENAMESIZE, "SavedFiles/%s.txt", fileName);

        //Attempt to save the file
        if (!saveCanvas(canvas, filePath)) {
            cout << "ERROR: File could not be written.\n";
            system("pause");
        }
        else {
            cout << "File saved!\n";
            cout << "Press any key to continue . . .";
            (void)_getch();
        }
    }
}


/*
  Opens the specified filename for writing; assumed to be a TXT file.
  If the file can be opened for writing, this function writes the
  canvas contents into the file, and then returns TRUE.
  If the file cannot be opened for writing, returns FALSE.
*/
bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
    ofstream outFile;

    // Try to open file
    outFile.open(filename);
    if (!outFile)
    {
        return false; // File could not be opened for writing
    }

    // Write canvas to file
    for (int i = 0; i < MAXROWS; i++)
    {
        for (int j = 0; j < MAXCOLS; j++)
            outFile << canvas[i][j];
        outFile << '\n';
    }

    outFile.close();
    return true;
}