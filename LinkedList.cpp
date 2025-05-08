#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include "Definitions.h"
using namespace std;

Node* newCanvas()
{
	// Create a new node
	Node* newNode = new Node;

	// Initialize the next pointer to null
	newNode->next = NULL;

	// Initialize the canvas with spaces
	initCanvas(newNode->item);

	// Return the new node
	return newNode;
}

Node* newCanvas(Node* oldNode)
{
	// Create a new node
	Node* newNode = new Node;

	// Initialize the next pointer to null
	newNode->next = NULL;

	// Copy the canvas from the old node to the new node
	copyCanvas(newNode->item, oldNode->item);

	// Return the new node
	return newNode;
}

void play(List& clips)
{
	// Check if there are enough clips to play an animation
	if (clips.count < 2)
	{
		return;
	}

	// Display message at the bottom of the screen
	clearLine(MAXROWS + 1, CLEARCOLS);
	cout << "Hold <ESC> to stop";

	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		// Play the animation once through
		playRecursive(clips.head, clips.count);
	}

	clearLine(MAXROWS + 1, CLEARCOLS);
}

void playRecursive(Node* head, int count)
{
	// Base case: no more clips to play
	if (head == NULL)
	{
		return;
	}

	// Recursive case: play the rest of the clips first (to reverse the order)
	playRecursive(head->next, count - 1);

	// shortens wait while hitting escape dramatically
	if ((GetKeyState(VK_ESCAPE) & 0x8000))
		return;

	// Display the current clip
	displayCanvas(head->item);

	// Clears the area where the clip number is displayed to reset display clip count 
	gotoxy(MAXROWS + 1, MAXCOLS - 50);
	for (int i = 0; i < 30; i++)
	{
		cout << " ";
	}

	// Display the clip number
	gotoxy(MAXROWS + 1, MAXCOLS - 50);
	cout << "Clip: " << count;

	// Pause for 100 milliseconds to slow down animation

	Sleep(100);
}

void addUndoState(List& undoList, List& redoList, Node* current)
{
	// Create a new node with a copy of the current canvas
	Node* undoNode = newCanvas(current);

	// Add the new node to the undo list
	addNode(undoList, undoNode);

	// Delete the redo list since we can no longer redo operations
	deleteList(redoList);
}

void restore(List& undoList, List& redoList, Node*& current)
{
	// Add the current canvas to the redo list
	addNode(redoList, current);

	// Get the canvas from the undo list and make it the current canvas
	current = removeNode(undoList);
}

void addNode(List& list, Node* nodeToAdd)
{
	// Set the new node's next pointer to point to the current head
	nodeToAdd->next = list.head;

	// Update the head pointer to point to the new node
	list.head = nodeToAdd;

	// Increment the count of nodes in the list
	list.count++;
}

Node* removeNode(List& list)
{
	// Check if the list is empty
	if (list.head == NULL) {
		return NULL;
	}

	// Get the node to be removed
	Node* nodeToRemove = list.head;

	// Update the head pointer to point to the next node
	list.head = list.head->next;

	// Disconnect the removed node from the list
	nodeToRemove->next = NULL;

	// Decrement the count of nodes in the list
	list.count--;

	// Return the removed node
	return nodeToRemove;
}

void deleteList(List& list)
{
	// Create a temporary pointer to track current node
	Node* current = list.head;
	Node* next = NULL;

	// Traverse the list and delete each node
	while (current != NULL) {
		// Save the next node
		next = current->next;

		// Delete the current node
		delete current;

		// Move to the next node
		current = next;
	}

	// Reset the list
	list.head = NULL;
	list.count = 0;
}

bool loadClips(List& clips, char filename[])
{
	char fullPath[FILENAMESIZE];
	int clipNumber = 1;
	bool success = false;
	bool continueLoading = true;

	// Clear the existing clips
	deleteList(clips);

	// Try to load clips until we find one that doesn't exist
	while (continueLoading) {
		// Form the filename for the current clip
		snprintf(fullPath, FILENAMESIZE, "%s-%d.txt", filename, clipNumber);

		// Create a new node for this clip
		Node* newNode = newCanvas();

		// Try to load the file into the new node
		// Failed to load this clip, must have reached the end
		if (!loadCanvas(newNode->item, fullPath))
		{

			delete newNode;
			continueLoading = false;
		}
		// Successfully loaded - add to the front of the list
		else
		{

			addNode(clips, newNode);
			success = true;
			clipNumber++;
		}
	}

	return success;
}

bool saveClips(List& clips, char filename[])
{
	// No clips to save
	if (clips.count == 0)
	{
		return false;
	}

	char fullPath[FILENAMESIZE];
	bool allSaved = true;

	// Create an array to temporarily store pointers to all nodes in reverse order
	Node** clipArray = new Node * [clips.count];

	// Fill the array with pointers to each node in the list
	Node* current = clips.head;
	for (int i = 0; i < clips.count; i++)
	{
		// Store the node at the opposite end of the array (to reverse the order)
		clipArray[clips.count - 1 - i] = current;
		current = current->next;
	}

	// Save each clip with the proper numbering
	for (int i = 0; i < clips.count; i++)
	{
		// Form the filename for the current clip
		snprintf(fullPath, FILENAMESIZE, "%s-%d.txt", filename, i + 1);

		// Save the clip to a file
		if (!saveCanvas(clipArray[i]->item, fullPath))
		{
			allSaved = false;
		}
	}

	// Free the temporary array
	delete[] clipArray;

	return allSaved;
}