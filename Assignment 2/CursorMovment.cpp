											/*Continue to implement undo and redo using stacks
											  undo and redo will be checked only on words not letters
											  also implement where if I press backspace and cursor is on the starting
											  node of the row and row is not empty, it takes all the text in the row
											  to the end of up row													*/

									/*Update.......
									Stack is implemented now I will make a stack and store the head of words
									for undo and redo*/

#include <iostream>
#include <Windows.h>
using namespace std;

void gotoxy(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
// Function to draw horizontal lines
void drawHorizontalLine(int x1, int x2, int y, char ch) 
{
	for (int i = x1; i <= x2; i++) 
	{
		gotoxy(i, y);
		cout << ch;
	}
}
void drawVerticalLine(int y1, int y2, int x, char ch) 
{
	for (int i = y1; i <= y2; i++) 
	{
		gotoxy(x, i);
		cout << ch;
	}
}
void drawUI() 
{
	int width = 120;  // Console width
	int height = 24;  // Console height

	// Draw the top border of the writing area

	// Draw the side borders of the writing area
	drawVerticalLine(0, height - 3, 0, '|');  // Left border
	drawVerticalLine(0, height - 3, width - 20, '|');  // Right border

	// Draw the bottom border of the writing area
	drawHorizontalLine(1, width - 21, height - 3, '_');

	// Draw the search section on the right

	// Draw the section for word suggestions
	gotoxy(1, height - 2);
	cout << "Word Suggestions";

	// Write static text in the search section
	gotoxy(width - 18, 0);
	cout << "Search";
}

struct node
{
	node* up;
	node* down;
	node* left;
	node* right;
	char data;
	node()
	{
		left = nullptr; 
		right = nullptr;
		up = nullptr; 
		down = nullptr;
		data = '\0';
	}
};

class TwoDDLinkedList
{
private:
	node* head;
	
public:
	TwoDDLinkedList()
	{
		head = nullptr;
	}
	node* getHead()
	{
		return head;
	}

	node* getRowNode(int row, int cursorX, int cursorY)
	{
		node* currentRow = head;

		// If the list is empty, create the first node as head
		if (head == nullptr)
		{
			head = new node();  // Create the head of the list
			gotoxy(1, row + 1);  // Set cursor position at the start of the first row
			return head;
		}

		// Traverse vertically to the correct row
		for (int i = 1; i < row; ++i)
		{
			if (currentRow->down == nullptr)
			{
				// Create the new row and link it
				node* newRow = new node();
				newRow->up = currentRow;
				currentRow->down = newRow;
			}
			currentRow = currentRow->down;
		}

		// Check for and create a new row below if necessary
		node* newRow = new node();
		node* rowBelow = currentRow->down;  // Capture the row below, if it exists

		// Insert the new row between currentRow and rowBelow
		newRow->up = currentRow;
		newRow->down = rowBelow;
		currentRow->down = newRow;

		if (rowBelow != nullptr)
		{
			rowBelow->up = newRow;
		}

		// Move cursor to the new row in the console (optional visual update)
		gotoxy(1, row + 1);

		return newRow;
	}





	void insertAt(int col, int row, char c)
	{
		if (row < 0 || col < 0) return;

		node* currentRow = head;

		// Create the head node if the list is empty
		if (head == nullptr)
		{
			head = new node();
			currentRow = head;
		}

		// Traverse vertically, create new rows if needed
		for (int r = 0; r < row; r++)
		{
			if (currentRow->down == nullptr)
			{
				node* newRow = new node();
				currentRow->down = newRow;
				newRow->up = currentRow;
			}
			currentRow = currentRow->down;
		}

		node* currentCol = currentRow;

		// Traverse horizontally, create new columns if needed
		for (int c = 0; c < col; c++)
		{
			if (currentCol->right == nullptr)
			{
				node* newCol = new node();
				currentCol->right = newCol;
				newCol->left = currentCol;

				// Ensure vertical linking if up or down nodes exist
				if (currentCol->up != nullptr && currentCol->up->right != nullptr)
				{
					newCol->up = currentCol->up->right;
					currentCol->up->right->down = newCol;
				}

				if (currentCol->down != nullptr && currentCol->down->right != nullptr)
				{
					newCol->down = currentCol->down->right;
					currentCol->down->right->up = newCol;
				}
			}
			currentCol = currentCol->right;
		}

		// Now we're at the correct row and column. Insert the new node with the character to insert
		node* newNode = new node();
		newNode->data = c;

		// Insert the new node horizontally
		newNode->right = currentCol->right;
		newNode->left = currentCol;

		if (currentCol->right != nullptr)
		{
			currentCol->right->left = newNode;
		}

		currentCol->right = newNode;

		// Ensure vertical links for the new node
		if (currentCol->down != nullptr)
		{
			newNode->down = currentCol->down->right; // Link to the row below
			if (newNode->down != nullptr)
			{
				newNode->down->up = newNode;
			}
		}
		else
		{
			newNode->down = nullptr; // If no row below, set down to nullptr
		}

		if (currentCol->up != nullptr && currentCol->up->right != nullptr)
		{
			newNode->up = currentCol->up->right;  // Link to the row above
			if (newNode->up != nullptr)
			{
				newNode->up->down = newNode;
			}
		}
		else
		{
			newNode->up = nullptr; // If no row above, set up to nullptr
		}

		// Now we will fix the vertical links for all nodes to the right of the new node
		node* currentRight = newNode->right;
		node* above = newNode->up;
		node* below = newNode->down;

		while (currentRight != nullptr) 
		{
			// If there's an above row, link the up pointer, otherwise set to nullptr
			if (above != nullptr && above->right != nullptr) 
			{
				currentRight->up = above->right;
				currentRight->up->down = currentRight;
			}
			else 
			{
				currentRight->up = nullptr; // No upper node, set to nullptr
			}

			// If there's a below row, link the down pointer, otherwise set to nullptr
			if (below != nullptr && below->right != nullptr) 
			{
				currentRight->down = below->right;
				currentRight->down->up = currentRight;
			}
			else 
			{
				currentRight->down = nullptr; // No lower node, set to nullptr
			}

			// Move to the next node to the right
			currentRight = currentRight->right;

			// Move above and below pointers to their respective next nodes
			if (above != nullptr)
			{
				above = above->right;
			}
			if (below != nullptr)
			{
				below = below->right;
			}
		}
	}

	void splitRowAt(node* currentNode, node*& newRowHead, int cursorX, int cursorY)
	{
		if (currentNode == nullptr || currentNode->right == nullptr) return;

		// Detach the nodes to the right of the cursor
		node* nodeToMove = currentNode->right;
		currentNode->right = nullptr;  // Detach the right part from the current node

		// Move the detached nodes to the new row
		newRowHead->right = nodeToMove;  // Now newRowHead points to the first node of the next row

		if (nodeToMove != nullptr)
		{
			nodeToMove->left = newRowHead;  // Set the left pointer to the new row head
		}

		// Update the current row's down pointer to point only to the newly created row
		if (currentNode->down != newRowHead) {
			currentNode->down = newRowHead;  // The current node's down now points to the newly created row
		}

		// Fix the vertical links of the moved nodes to match the new row positions
		node* currentRight = newRowHead->right;
		node* above = newRowHead->up;
		node* below = newRowHead->down;  // The row below the newly created row, if it exists

		while (currentRight != nullptr)
		{
			// Correctly link the "up" pointers
			if (above != nullptr && above->right != nullptr)
			{
				currentRight->up = above->right;
				currentRight->up->down = currentRight;
			}
			else
			{
				currentRight->up = nullptr;  // Ensure correct null assignment if there's no node above
			}

			// Correctly link the "down" pointers
			if (below != nullptr && below->right != nullptr)
			{
				currentRight->down = below->right;
				currentRight->down->up = currentRight;
			}
			else
			{
				currentRight->down = nullptr;  // Ensure correct null assignment if there's no node below
			}

			// Move to the next node to the right
			currentRight = currentRight->right;

			// Shift the above and below pointers to their next positions
			if (above != nullptr) {
				above = above->right;
			}
			if (below != nullptr) {
				below = below->right;
			}
		}

		// Only try to fix links if 'above' or 'below' are valid
		if (above != nullptr)
		{
			node* temp1 = above->right;
			while (temp1 != nullptr)
			{
				temp1->down = nullptr;
				temp1 = temp1->right;
			}
		}

		if (below != nullptr)
		{
			node* temp2 = below->right;
			while (temp2 != nullptr)
			{
				temp2->up = nullptr;
				temp2 = temp2->right;
			}
		}

		// Clear the remaining part of the current line in the console (to remove leftover text)
		gotoxy(cursorX, cursorY);  // Move to the current cursor position
		cout << string(79 - cursorX, ' ');  // Clear remaining characters on the console line
	}




	void display()
	{
		node* ptr = head;
		while (ptr != nullptr)
		{
			node* ptrcol = ptr;
			while (ptrcol != nullptr)
			{
				cout << ptrcol->data;
				ptrcol = ptrcol->right;
			}
			cout << endl;
			ptr = ptr->down;
		}

		/*node* ptr = head;
		while (ptr != nullptr)
		{
			node* ptrcol = ptr;
			while (ptrcol->right != nullptr)
			{
				cout << ptrcol->data <<" ";
				node* ptrrow = ptrcol;
				while (ptrrow->down != nullptr && ptrrow->down->data != '\0')
				{
					ptrrow = ptrrow->down;
					cout << ptrrow->data << " ";
				}
				cout << "|" << endl;
				ptrcol = ptrcol->right;
			}
			cout << "Reveresed" << endl;
			while (ptrcol != nullptr)
			{
				cout << ptrcol->data << " ";
				node* ptrrow = ptrcol;
				while (ptrrow->down != nullptr && ptrrow->down->data != '\0')
				{
					ptrrow = ptrrow->down;
				}
				while (ptrrow->up != nullptr)
				{
					ptrrow = ptrrow->up;
					cout << ptrrow->data << " ";
				}

				cout << "|" << endl;
				ptrcol = ptrcol->left;
			}
			cout <<"------------------"<< endl;
			ptr = ptr->down;
		}*/
	}

	int removeWord(node* wordHead) 
	{
		if (wordHead == nullptr)
		{
			return -1;
		}

		// Traverse horizontally to delete the entire word
		int count = 0;
		node* current = wordHead;
		do 
		{
			count++;
			// Temporarily store the next node to the right
			node* nextNode = current->right;

			// Handle left-side linking
			if (current->left != nullptr) {
				current->left->right = current->right;
			}

			// Handle right-side linking
			if (current->right != nullptr) {
				current->right->left = current->left;
			}

			// Handle vertical links
			if (current->up != nullptr) {
				current->up->down = current->down;
			}

			if (current->down != nullptr) {
				current->down->up = current->up;
			}

			// Delete the current node
			delete current;

			// Move to the next node in the word
			current = nextNode;
		} while (current != nullptr && current->data != ' ');
		return count;
	}

	// Delete at a specific position
	void deleteAt(int row, int col, node*& currentNode)
	{
		if (row < 0 || col < 0) return;

		node* currentRow = head;

		// If head is null, return
		if (head == nullptr)
		{
			return;
		}

		// Traverse vertically to the correct row
		for (int r = 0; r < row; r++)
		{
			if (currentRow->down == nullptr)
			{
				return;  // Row does not exist
			}
			currentRow = currentRow->down;
		}

		node* currentCol = currentRow;

		// Traverse horizontally to the correct column
		for (int c = 0; c < col; c++)
		{
			if (currentCol->right == nullptr)
			{
				return;  // Column does not exist
			}
			currentCol = currentCol->right;
		}


		// Handle horizontal links
		if (currentCol->left != nullptr)
		{
			currentCol->left->right = currentCol->right;
		}
		else
		{
			// If this is the head of the row, update the row's right pointer
			currentRow->right = currentCol->right;
		}

		if (currentCol->right != nullptr)
		{
			currentCol->right->left = currentCol->left;
		}

		// Handle vertical links (up and down)
		if (currentCol->up != nullptr)
		{
			currentCol->up->down = currentCol->down;
		}

		if (currentCol->down != nullptr)
		{
			currentCol->down->up = currentCol->up;
		}

		// Handle case where the node is the only node in the list
		if (currentCol == head && currentCol->right == nullptr && currentCol->down == nullptr)
		{
			head = nullptr;  // If it is the only node, set head to nullptr
		}
		// Update currentNode to the left node (or right if left is nullptr)
		if (currentCol->left != nullptr) {
			currentNode = currentCol->left;
		}
		else if (currentCol->right != nullptr) {
			currentNode = currentCol->right;
		}
		else {
			currentNode = nullptr;  // No neighbors, make currentNode nullptr
		}
		// Safely delete the current node
		delete currentCol;

		// Fix vertical links for nodes to the right of the deleted node
		node* currentRight = currentRow->right;  // Start from the next node in the row
		node* above = currentRow->up != nullptr ? currentRow->up->right : nullptr;
		node* below = currentRow->down != nullptr ? currentRow->down->right : nullptr;

		while (currentRight != nullptr)
		{
			// If there's an above row, link the up pointer, otherwise set to nullptr
			if (above != nullptr)
			{
				currentRight->up = above;
				above->down = currentRight;
			}
			else
			{
				currentRight->up = nullptr;
			}

			// If there's a below row, link the down pointer, otherwise set to nullptr
			if (below != nullptr)
			{
				currentRight->down = below;
				below->up = currentRight;
			}
			else
			{
				currentRight->down = nullptr;
			}

			// Move to the next node to the right
			currentRight = currentRight->right;

			// Move above and below pointers to their respective next nodes
			if (above != nullptr)
			{
				above = above->right;
			}
			if (below != nullptr)
			{
				below = below->right;
			}
		}
	}

	void deleteEmptyRow(node* currentRow, node*& currentNode, int& x, int& y)
	{
		if (currentRow == nullptr)
		{
			return;
		}

		if (currentRow->right != nullptr)
		{ 
			return; 
		}
		node* temp = currentRow;
		int tempnum = y;
		while (temp->down != nullptr)
		{
			tempnum++;
			temp = temp->down;
		}
		gotoxy(1, tempnum);
		cout << string(79, ' ');

		if (currentRow->up != nullptr) 
		{
			currentRow->up->down = currentRow->down;
		}

		if (currentRow->down != nullptr) 
		{
			currentRow->down->up = currentRow->up;
		}

		if (currentRow == head) {
			head = currentRow->down;
		}

		currentNode = currentRow->up;
		if (currentNode != nullptr) 
		{
			x = 1;
			while (currentNode->right != nullptr) 
			{
				currentNode = currentNode->right;
				x++;
			}
		}
		else 
		{
			x = 1;
		}

		y--;

		delete currentRow;
	}
	// Delete the entire line
	void deleteLine(int row) {
		// Traverse to the row and delete all characters in that line
	}

	// Delete the last character in a line
	void deleteLastInLine() {
		// Traverse to the last node in the current line and delete it
	}

	// Delete a line break (merge two lines)
	void deleteLineBreak(int row) {
		// Merge the current row with the previous row
	}

};

struct StackNode
{
	node* wordstart;
	StackNode* next;
	StackNode* prev;
};

class Stack
{
private:
	StackNode* top;
	StackNode* base;
	StackNode* baseUp;
	int capacity; 
public:
	Stack()
	{
		top = nullptr;
		base = nullptr;
		baseUp = nullptr;
		capacity = 0;
	}

	bool isEmpty()
	{
		if (top == nullptr)
		{
			return 1;
		}
		return 0;
	}

	void push(node* newwordstart)
	{
		StackNode* newNode = new StackNode();
		newNode->wordstart = newwordstart;
		newNode->next = top;
		newNode->prev = nullptr;

		if (top != nullptr) 
		{
			top->prev = newNode;
		}

		top = newNode;
		capacity++;

		if (capacity == 1) 
		{
			base = top;
			baseUp = nullptr;
		}
		else if (capacity == 2) 
		{
			baseUp = top;
		}

		if (capacity > 5) 
		{
			removeBase();
		}
	}
	node* pop()
	{
		if (isEmpty())
		{
			return nullptr;
		}

		StackNode* temp = top;
		node* wordhead = top->wordstart;
		top = top->next;

		if (top != nullptr) 
		{
			top->prev = nullptr;
		}
		else 
		{
			base = nullptr;
			baseUp = nullptr;
		}

		delete temp;
		capacity--;
		return wordhead;

	}
	void removeBase() 
	{
		if (base == nullptr)
		{
			return;
		}
		StackNode* temp = base;

		base = base->prev;

		if (base != nullptr) 
		{
			baseUp = base->prev;
			base->next = nullptr;
		}
		else 
		{
			baseUp = nullptr;
		}

		delete temp;
		capacity--;
	}
};



void reprintRow(TwoDDLinkedList& list, int rowIndex, int consoleRow);
void undoWord(TwoDDLinkedList& list, Stack& undoStack, Stack& redoStack, int& x, int& y, node*& currentNode);
void redoWord(TwoDDLinkedList& list, Stack& redoStack, Stack& undoStack, int row, int col);
void insertWord(TwoDDLinkedList& list, node* wordHead, int row, int col);


int main(int argc, char* argv[]) 
{
	TwoDDLinkedList list;
	Stack undoStack;
	Stack redoStack;
	system("cls");
	drawUI();
	gotoxy(2, 1);  // Set cursor to start writing in the writing area
	std::cout << " ";
	HANDLE  rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console

	DWORD Events = 0;     // Event count
	DWORD EventsRead = 0; // Events read from console

	bool Running = true;
	
	int x = 1, y = 1;
	gotoxy(x, y);
	node* currentNode = nullptr;
	node* currentWordHead = nullptr;

	//programs main loop
	while (Running) 
	{
			// gets the systems current "event" count
			GetNumberOfConsoleInputEvents(rhnd, &Events);

		if (Events != 0) 
		{ // if something happened we will handle the events we want

			// create event buffer the size of how many Events
			INPUT_RECORD eventBuffer[200];

			// fills the event buffer with the events and saves count in EventsRead
			ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);
			
		// loop through the event buffer using the saved count
			for (DWORD i = 0; i < EventsRead; ++i) 
			{
				// check if event[i] is a key event && if so is a press not a release
				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) 
				{			
						// check if the key press was an arrow key
					
					switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode) 
					{
						case VK_BACK:	// Handle the Backspace key
							if (x > 1) {
								// Deleting a character in the middle of the row
								list.deleteAt(y - 1, x - 1, currentNode);  // Delete character from the linked list (row = y, col = x)
								x--;  // Move cursor left
								reprintRow(list, y - 1, y);  // Reprint the row after deletion
								gotoxy(x, y);  // Move the cursor back to the correct position
							}
							else if (x == 1 && currentNode != nullptr && currentNode->right == nullptr && y > 1) 
							{
									// The row is empty, and we are at the start of the row
									node* currentRow = currentNode;  // The current row we're deleting
									if (currentRow && currentRow->right == nullptr) 
									{
										list.deleteEmptyRow(currentRow, currentNode, x, y);  // Delete the empty row and link
										reprintRow(list, y -1, y);  // Reprint the updated rows
										gotoxy(x, y);  // Move the cursor to the correct position after deletion
									}
							}
							break;

						case VK_RETURN:
						{
							node* nextRowHead = list.getRowNode(y, x, y);

							// Split the current row at the cursor position
							list.splitRowAt(currentNode, nextRowHead, x, y);

							// Move down one line and reset column position
							y++;
							x = 1;

							// Update currentNode to the new row's head
							currentNode = nextRowHead;

							reprintRow(list, y - 1, y);  // Reprint to reflect changes

							gotoxy(x, y);  // Move the cursor to the new row
							break;
						}

						

						case VK_ESCAPE:
							Running = false;  // Exit the loop
							break;

						case VK_UP:  // Move up
							if (currentNode != nullptr && currentNode->up != nullptr) {
								// Simple case: there's a node directly above, so just move down
								currentNode = currentNode->up;  // Move to the node below
								y--;
								gotoxy(x, y);  // Update the cursor position
							}
							else {
								// Complex case: No direct node above, so we need to find the corresponding node in the row below
								node* tempNode = currentNode;
								int temp1 = x;
								int temp2 = y;
								// Traverse leftwards to find a node that has a corresponding down node
								while (tempNode != nullptr && tempNode->up == nullptr) {
									tempNode = tempNode->left;
									x--;  // Move left
								}

								if (tempNode != nullptr && tempNode->up != nullptr) {
									// Now tempNode points to a node that has a corresponding down node
									currentNode = tempNode->up;  // Move to the node above
									y--;

									// Move cursor to the end of the row below
									while (currentNode->right != nullptr) {
										currentNode = currentNode->right;  // Traverse to the end of the row
										x++;
									}

									gotoxy(x, y);  // Update the cursor position
								}
								if (!tempNode)
								{
									x = temp1;
									y = temp2;
									gotoxy(x, y);  // Update the cursor position
								}
							}
							break;

						case VK_DOWN:  // Move down
							if (currentNode != nullptr && currentNode->down != nullptr) {
								// Simple case: there's a node directly below, so just move down
								currentNode = currentNode->down;  // Move to the node below
								y++;
								gotoxy(x, y);  // Update the cursor position
							}
							else {
								// Complex case: No direct node below, so we need to find the corresponding node in the row below
								node* tempNode = currentNode;
								int temp1 = x;
								int temp2 = y;
								// Traverse leftwards to find a node that has a corresponding down node
								while (tempNode != nullptr && tempNode->down == nullptr) {
									tempNode = tempNode->left;
									x--;  // Move left
								}
								
								if (tempNode != nullptr && tempNode->down != nullptr) {
									// Now tempNode points to a node that has a corresponding down node
									currentNode = tempNode->down;  // Move to the node below
									y++;

									// Move cursor to the end of the row below
									while (currentNode->right != nullptr) {
										currentNode = currentNode->right;  // Traverse to the end of the row
										x++;
									}

									gotoxy(x, y);  // Update the cursor position
								}
								if (!tempNode)
								{
									x = temp1;
									y = temp2;
									gotoxy(x, y);  // Update the cursor position
								}
							}
							break;


						case VK_RIGHT:  // Move right
							if (currentNode != nullptr && currentNode->right != nullptr)
							{
								currentNode = currentNode->right;  // Update currentNode to the right node
								x++;
								gotoxy(x, y);  // Update the cursor position
							}
							break;

						case VK_LEFT:  // Move left
							if (currentNode != nullptr && currentNode->left != nullptr)
							{
								currentNode = currentNode->left;  // Update currentNode to the left node
								x--;
								gotoxy(x, y);  // Update the cursor position
							}
							break;
						case 0xBB:  // '=' key for undo
						{
							undoWord(list, undoStack, redoStack, x, y,currentNode);
							reprintRow(list, y - 1, y);
							gotoxy(x, y);  // Move the cursor back to the correct position
							break;
						}

						case 0xBD:  // '-' key for redo
						{
							redoWord(list, redoStack, undoStack, y - 1, x - 1);
							reprintRow(list, y - 1, y);
							gotoxy(x, y);  // Move the cursor back to the correct position
							break;
						}

						default: 
						{
							char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
							if (c != '\0')
							{  // Only insert printable characters
								
								list.insertAt(x - 1, y - 1, c);  // Insert the character into the linked list
								reprintRow(list, y - 1, y);  // Reprint the row after insertion
								x++;  // Move to the next column after insertion
								gotoxy(x, y);  // Move the cursor to the next position

								// Update currentNode to reflect the newly inserted position
								currentNode = list.getHead();  // Reset to the head, then traverse
								for (int r = 0; r < y - 1; r++)
								{
									if (currentNode->down != nullptr)
										currentNode = currentNode->down;
								}
								for (int c = 0; c < x - 1; c++)
								{
									if (currentNode != nullptr && currentNode->right != nullptr)
									{
										currentNode = currentNode->right;
										
									}
								}
								if (x == 2 || currentNode != nullptr && currentNode->left->data == ' ')
								{
									if (x != 2 && currentNode != nullptr)
									{
										currentWordHead = currentNode->left;
										undoStack.push(currentWordHead);
										currentWordHead = nullptr;
									}
									else if( x == 2)
									{
										currentWordHead = currentNode;
										undoStack.push(currentWordHead);
										currentWordHead = nullptr;
									}
								}
							}
							break;
						}
					}

				}
			} // end EventsRead loop
		}

	} // end program loop
	system("cls");
	list.display();
	

	return 0;
}

void undoWord(TwoDDLinkedList& list, Stack& undoStack, Stack& redoStack, int& x, int& y, node*& currentNode)
{
	if (undoStack.isEmpty())
	{
		return;
	}

	node* wordHead = undoStack.pop();

	redoStack.push(wordHead);

	int newX = 0;
	int newY = 1;
	newX = list.removeWord(wordHead);
	node* traverse = list.getHead();

	x = x - newX;
	
	
	if (x == 1 && y != 1)
	{
		for (int c = 1; c < y; c++)
		{
			traverse = traverse->down;
		}
		traverse = traverse->up;
		while (traverse->right != nullptr)
		{
			x++;
			traverse = traverse->right;
		}
		y--;
	}
	else
	{
		for (int c = 1; c < y; c++)
		{
			traverse = traverse->down;
		}
		for (int c = 1; c < x; c++)
		{
			traverse = traverse->right;
		}

	}
		currentNode = traverse;
}

void insertWord(TwoDDLinkedList& list, node* wordHead, int row, int col) 
{
	node* current = wordHead;
	while (current != nullptr && current->data != ' ') {
		// Insert each character back at its original position
		list.insertAt(col, row, current->data);
		col++;  // Move to the next column
		current = current->right;
	}
}

void redoWord(TwoDDLinkedList& list, Stack& redoStack, Stack& undoStack, int row, int col) 
{
	if (redoStack.isEmpty()) 
	{
		//cout << "Nothing to redo." << endl;
		return;
	}

	// Pop the head of the last undone word from the redo stack
	node* wordHead = redoStack.pop();

	// Reinsert the word into the list (add back the word at its original position)
	insertWord(list, wordHead, row, col);

	// Push the word back to the undo stack
	undoStack.push(wordHead);
}

void reprintRow(TwoDDLinkedList& list, int rowIndex, int consoleRow)
{
	// Start from the head
	node* row = list.getHead();
	int currentConsoleRow = 1;  // Start at the top of the console

	// Traverse through the entire list row by row
	while (row != nullptr) {
		gotoxy(1, currentConsoleRow);  // Move cursor to the start of the current row
		cout << string(79, ' ');  // Clear the row (assuming 79 columns wide)

		gotoxy(1, currentConsoleRow);  // Move cursor back to the start
		node* col = row;

		// Print each character in the row
		while (col != nullptr) {
			cout << col->data;
			col = col->right;
		}

		// Move to the next row
		row = row->down;
		currentConsoleRow++;
	}
}