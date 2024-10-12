/*there is an issue in the redo function where when I redo words in the next row
it does keep the value of y by which it keeps on adding extra nodes to the left
where the actual words are being stored after those extra nodes please look into this*/

#include <iostream>
#include <fstream>
#include <filesystem>
#include <Windows.h>

class TwoDDLinkedList;
struct node;
void gotoxy(int x, int y)
{
	COORD c = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void drawHorizontalLine(int x1, int x2, int y, char ch)
{
	for (int i = x1; i <= x2; i++)
	{
		gotoxy(i, y);
		std::cout << ch;
	}
}
void drawVerticalLine(int y1, int y2, int x, char ch)
{
	for (int i = y1; i <= y2; i++)
	{
		gotoxy(x, i);
		std::cout << ch;
	}
}
void drawUI()
{
	int width = 120;
	int height = 26;

	drawVerticalLine(0, height - 3, 0, '|');
	drawVerticalLine(0, height - 3, width - 20, '|');

	drawHorizontalLine(1, width - 21, height - 3, '_');

	gotoxy(1, height - 2);
	std::cout << "Word Suggestions";

	gotoxy(width - 18, 0);
	std::cout << "Search";
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

	int getWordLength(node* wordHead)
	{
		int length = 0;
		node* current = wordHead;
		while (current != nullptr && current->data != ' ')
		{
			length++;
			current = current->right;
		}
		return length;
	}

	node* getRowNode(int row, int cursorX, int cursorY)
	{
		node* currentRow = head;

		if (head == nullptr)
		{
			head = new node();
			gotoxy(1, row + 1);
			return head;
		}

		for (int i = 1; i < row; ++i)
		{
			if (currentRow->down == nullptr)
			{
				node* newRow = new node();
				newRow->up = currentRow;
				currentRow->down = newRow;
			}
			currentRow = currentRow->down;
		}

		node* newRow = new node();
		node* rowBelow = currentRow->down;

		newRow->up = currentRow;
		newRow->down = rowBelow;
		currentRow->down = newRow;

		if (rowBelow != nullptr)
		{
			rowBelow->up = newRow;
		}

		gotoxy(1, row + 1);

		return newRow;
	}

	void insertAt(int col, int row, char c)
	{
		if (row < 0 || col < 0) return;

		node* currentRow = head;

		if (head == nullptr)
		{
			head = new node();
			currentRow = head;
		}

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

		for (int c = 0; c < col; c++)
		{
			if (currentCol->right == nullptr)
			{
				node* newCol = new node();
				currentCol->right = newCol;
				newCol->left = currentCol;

				// Make vertical links if up or down nodes exist
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

		// Insert the new node with the character to insert
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
			newNode->down = currentCol->down->right;
			if (newNode->down != nullptr)
			{
				newNode->down->up = newNode;
			}
		}
		else
		{
			newNode->down = nullptr;
		}

		if (currentCol->up != nullptr && currentCol->up->right != nullptr)
		{
			newNode->up = currentCol->up->right;
			if (newNode->up != nullptr)
			{
				newNode->up->down = newNode;
			}
		}
		else
		{
			newNode->up = nullptr;
		}

		// Now we will fix the vertical links for all nodes to the right of the new node
		node* currentRight = newNode->right;
		node* above = newNode->up;
		node* below = newNode->down;

		while (currentRight != nullptr)
		{
			if (above != nullptr && above->right != nullptr)
			{
				currentRight->up = above->right;
				currentRight->up->down = currentRight;
			}
			else
			{
				currentRight->up = nullptr;
			}

			if (below != nullptr && below->right != nullptr)
			{
				currentRight->down = below->right;
				currentRight->down->up = currentRight;
			}
			else
			{
				currentRight->down = nullptr;
			}

			currentRight = currentRight->right;

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
		if (currentNode == nullptr || currentNode->right == nullptr)
		{
			return;
		}

		// Detach the nodes to the right of the cursor
		node* nodeToMove = currentNode->right;
		currentNode->right = nullptr;

		// Move the detached nodes to the new row
		newRowHead->right = nodeToMove;

		if (nodeToMove != nullptr)
		{
			nodeToMove->left = newRowHead;
		}

		// Update the current row's down pointer to point only to the newly created row
		if (currentNode->down != newRowHead)
		{
			currentNode->down = newRowHead;
		}

		// Fix the vertical links of the moved nodes to match the new row positions
		node* currentRight = newRowHead->right;
		node* above = newRowHead->up;
		node* below = newRowHead->down;

		while (currentRight != nullptr)
		{
			if (above != nullptr && above->right != nullptr)
			{
				currentRight->up = above->right;
				currentRight->up->down = currentRight;
			}
			else
			{
				currentRight->up = nullptr;
			}

			if (below != nullptr && below->right != nullptr)
			{
				currentRight->down = below->right;
				currentRight->down->up = currentRight;
			}
			else
			{
				currentRight->down = nullptr;
			}

			currentRight = currentRight->right;

			if (above != nullptr) {
				above = above->right;
			}
			if (below != nullptr) {
				below = below->right;
			}
		}

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

		// Traverse upwards and update all rows above the current split
		node* rowAbove = currentNode->up;
		while (rowAbove != nullptr)
		{
			node* colAbove = rowAbove;
			node* belowRow = rowAbove->down;

			while (colAbove != nullptr)
			{
				if (belowRow != nullptr && belowRow->right != nullptr)
				{
					colAbove->down = belowRow->right;
					if (colAbove->down != nullptr)
					{
						colAbove->down->up = colAbove;
					}
				}
				else
				{
					colAbove->down = nullptr;
				}

				colAbove = colAbove->right;
				if (belowRow != nullptr)
				{
					belowRow = belowRow->right;
				}
			}

			rowAbove = rowAbove->up;
		}
		gotoxy(cursorX, cursorY);
	}




	void display()
	{
		node* ptr = head;
		while (ptr != nullptr)
		{
			node* ptrcol = ptr;
			while (ptrcol != nullptr)
			{
				std::cout << ptrcol->data;
				ptrcol = ptrcol->right;
			}
			std::cout << std::endl;
			ptr = ptr->down;
		}
	}

	int removeWord(node* wordHead)
	{
		if (wordHead == nullptr)
		{
			return -1;
		}
		int count = 0;
		node* current = wordHead;
		do
		{
			count++;
			node* nextNode = current->right;

			if (current->left != nullptr) 
			{
				current->left->right = current->right;
			}

			if (current->right != nullptr) 
			{
				current->right->left = current->left;
			}

			if (current->up != nullptr) 
			{
				current->up->down = current->down;
			}

			if (current->down != nullptr) 
			{
				current->down->up = current->up;
			}

			delete current;

			current = nextNode;
		} while (current != nullptr && current->data != ' ');
		return count;
	}

	void deleteAt(int row, int col, node*& currentNode)
	{
		if (row < 0 || col < 0) 
		{
			return; 
		}

		node* currentRow = head;

		if (head == nullptr)
		{
			return;
		}

		for (int r = 0; r < row; r++)
		{
			if (currentRow->down == nullptr)
			{
				return;
			}
			currentRow = currentRow->down;
		}

		node* currentCol = currentRow;

		for (int c = 0; c < col; c++)
		{
			if (currentCol->right == nullptr)
			{
				return;
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
			currentRow->right = currentCol->right;
		}

		if (currentCol->right != nullptr)
		{
			currentCol->right->left = currentCol->left;
		}

		// Handle vertical links
		if (currentCol->up != nullptr)
		{
			currentCol->up->down = currentCol->down;
		}

		if (currentCol->down != nullptr)
		{
			currentCol->down->up = currentCol->up;
		}

		// Case where the node is the only node in the list
		if (currentCol == head && currentCol->right == nullptr && currentCol->down == nullptr)
		{
			head = nullptr;
		}
		if (currentCol->left != nullptr) 
		{
			currentNode = currentCol->left;
		}
		else if (currentCol->right != nullptr) 
		{
			currentNode = currentCol->right;
		}
		else 
		{
			currentNode = nullptr;
		}

		delete currentCol;


		// Fix vertical links for nodes to the right of the deleted node
		node* currentRight = currentRow->right;
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

			currentRight = currentRight->right;

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
		std::cout << "                                                                                                   ";

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
};


struct RedoData
{
	int row;
	int col;
	node* wordHead;
};
struct StackNode
{
	node* wordstart;
	RedoData redoData;
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
	void pushRedoData(const RedoData& newData)
	{
		StackNode* newNode = new StackNode();
		newNode->redoData = newData;
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
	RedoData popRedoData()
	{
		if (isEmpty())
		{
			return RedoData();
		}

		StackNode* temp = top;
		RedoData redoData = top->redoData;
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
		return redoData;
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


void openOrCreateFile(std::fstream& file, TwoDDLinkedList& list, node*& currentNode, int& x, int& y);
void displayMenu();
void saveFile(TwoDDLinkedList& list, std::fstream& file);
void loadFile(std::fstream& file, std::filesystem::path& filePath, TwoDDLinkedList& list, node*& currentNode, int& x, int& y);

void openOrCreateFile(std::fstream& file, TwoDDLinkedList& list, node*& currentNode, int& x, int& y)
{
	std::filesystem::path filePath;

	std::cout << "Enter the file name (press ENTER to finish): ";

	char c;
	while (std::cin.get(c) && c != '\n') 
	{
		filePath += c;
	}

	if (!std::filesystem::exists(filePath)) 
	{
		file.open(filePath, std::ios::out);
		if (!file) 
		{
			std::cout << "Error creating file: " << filePath << std::endl;
			return;
		}
		std::cout << "New file created: " << filePath << std::endl;
		file.close();
	}
	else 
	{
		std::cout << "File exists. Loading content...\n";
		loadFile(file, filePath, list, currentNode, x, y);
	}
}

void displayMenu()
{
	std::cout << "						====== Menu ======\n\n\n" << std::endl;
	std::cout << "						1. Create a new file\n\n\n" << std::endl;
	std::cout << "						2. Load an existing file\n\n\n" << std::endl;
	std::cout << "						3. Exit\n\n\n" << std::endl;
	std::cout << "						Choose an option: ";
}

void saveFile(TwoDDLinkedList& list, std::fstream& file)
{
	std::filesystem::path filePath;
	char c;

	std::cout << "Enter the file name to save (press ENTER to finish): ";

	while (std::cin.get(c) && c != '\n')
	{
		filePath += c;
	}

	file.open(filePath, std::ios::out | std::ios::trunc);
	if (!file)
	{
		std::cout << "Error opening file for saving: " << filePath << std::endl;
		return;
	}

	node* row = list.getHead();
	while (row != nullptr)
	{
		node* col = row;
		while (col != nullptr)
		{
			file.put(col->data);
			col = col->right;
		}
		file.put('\n');
		row = row->down;
	}

	file.close();
	std::cout << "File saved successfully: " << filePath << std::endl;
}

void loadFile(std::fstream& file, std::filesystem::path& newfilePath, TwoDDLinkedList& list, node*& currentNode, int& x, int& y)
{
	std::filesystem::path filePath;

	if (std::filesystem::exists(newfilePath))
	{
		filePath = newfilePath;
	}
	else
	{
		std::cout << "Enter the file name to load (press ENTER to finish): ";

		char c;
		while (std::cin.get(c) && c != '\n') 
		{
			filePath += c;
		}
	}
	file.open(filePath, std::ios::in | std::ios::out);

	if (!file)
	{
		std::cout << "Error opening file: " << filePath << std::endl;
		return;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();

	if (fileSize == 0)
	{
		std::cout << "The file is empty or newly created: " << filePath << std::endl;
		file.close();
		return;
	}

	file.seekg(0, std::ios::beg);

	node* tempNode = nullptr;
	int row = 0, col = 0;

	char ch;
	while (file.get(ch)) 
	{
		if (ch == '\n') 
		{
			row++;
			col = 0;
		}
		else 
		{
			list.insertAt(col - 1, row, ch);
			col++;
		}
	}

	file.close();

	currentNode = list.getHead();
	if (currentNode != nullptr)
	{
		while (currentNode->down != nullptr)
		{
			currentNode = currentNode->down;
		}
		while (currentNode->right != nullptr)
		{
			currentNode = currentNode->right;
			x++;
		}
		y = row;
	}

	gotoxy(x, y);
}





void reprintRow(TwoDDLinkedList& list, int rowIndex, int consoleRow);
void undoWord(TwoDDLinkedList& list, Stack& undoStack, Stack& redoStack, int& x, int& y, node*& currentNode);
void redoWord(TwoDDLinkedList& list, Stack& redoStack, Stack& undoStack, int& x, int& y, node*& currentNode);
node* deepCopyWord(node* wordHead);



int main(int argc, char* argv[])
{
	std::fstream file;
	std::filesystem::path filePath;
	TwoDDLinkedList list;
	Stack undoStack;
	Stack redoStack;
	bool Running = true;
	bool running = true;
	system("cls");

	gotoxy(2, 1);
	std::cout << " ";
	HANDLE  rhnd = GetStdHandle(STD_INPUT_HANDLE);

	DWORD Events = 0;
	DWORD EventsRead = 0;


	int x = 1, y = 1;
	gotoxy(x, y);
	node* currentNode = nullptr;
	node* currentWordHead = nullptr;


	while (running)
	{
		system("cls");
		displayMenu();
		int choice;
		std::cin >> choice;
		std::cin.ignore();

		switch (choice) 
		{
			case 1:
				std::cout << "\n\n";
				openOrCreateFile(file, list, currentNode, x, y);
				system("cls");
				reprintRow(list, y - 1, y);
				running = false;
				break;
			case 2:
				std::cout << "\n\n";
				loadFile(file, filePath, list, currentNode, x, y);
				system("cls");
				reprintRow(list, y - 1, y);
				running = false;
				break;
			case 3:
				running = false;
				system("cls");
				return 0;
				break;
			default:
				system("cls");
				std::cout << "Invalid option! Please try again.\n";
				break;
		}
	}
	drawUI();
	gotoxy(x, y);

	while (Running)
	{
		GetNumberOfConsoleInputEvents(rhnd, &Events);

		if (Events != 0)
		{
			INPUT_RECORD eventBuffer[200];

			ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);

			for (DWORD i = 0; i < EventsRead; ++i)
			{
				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown)
				{

					switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode)
					{
						case VK_BACK:
							if (x > 1) 
							{
								list.deleteAt(y - 1, x - 1, currentNode);
								x--;
								reprintRow(list, y - 1, y);
								gotoxy(x, y);
							}
							else if (x == 1 && currentNode != nullptr && currentNode->right == nullptr && y > 1)
							{
								node* currentRow = currentNode;
								if (currentRow && currentRow->right == nullptr)
								{
									list.deleteEmptyRow(currentRow, currentNode, x, y);
									reprintRow(list, y - 1, y);
									gotoxy(x, y);
								}
							}
							break;

						case VK_RETURN:
						{
							if (y >= 22)
							{
								std::cout << "\a";
								break;
							}
							node* nextRowHead = list.getRowNode(y, x, y);
							list.splitRowAt(currentNode, nextRowHead, x, y);
							y++;
							x = 1;
							currentNode = nextRowHead;
							reprintRow(list, y - 1, y);
							gotoxy(x, y);
							break;
						}
						case VK_ESCAPE:
							Running = false;
							break;

						case VK_UP:
							if (currentNode != nullptr && currentNode->up != nullptr) 
							{
								currentNode = currentNode->up;
								y--;
								gotoxy(x, y);
							}
							else 
							{
								node* tempNode = currentNode;
								int temp1 = x;
								int temp2 = y;
								while (tempNode != nullptr && tempNode->up == nullptr)
								{
									tempNode = tempNode->left;
									x--;
								}

								if (tempNode != nullptr && tempNode->up != nullptr) 
								{
									currentNode = tempNode->up;
									y--;

									while (currentNode->right != nullptr) 
									{
										currentNode = currentNode->right;
										x++;
									}

									gotoxy(x, y);
								}
								if (!tempNode)
								{
									x = temp1;
									y = temp2;
									gotoxy(x, y);
								}
							}
							break;

						case VK_DOWN:
							if (currentNode != nullptr && currentNode->down != nullptr) 
							{
								currentNode = currentNode->down;
								y++;
								gotoxy(x, y);
							}
							else
							{
								node* tempNode = currentNode;
								int temp1 = x;
								int temp2 = y;
								while (tempNode != nullptr && tempNode->down == nullptr) 
								{
									tempNode = tempNode->left;
									x--;
								}

								if (tempNode != nullptr && tempNode->down != nullptr) 
								{
									currentNode = tempNode->down;
									y++;

									while (currentNode->right != nullptr) 
									{
										currentNode = currentNode->right;
										x++;
									}

									gotoxy(x, y);
								}
								if (!tempNode)
								{
									x = temp1;
									y = temp2;
									gotoxy(x, y);
								}
							}
							break;


						case VK_RIGHT:
							if (currentNode != nullptr && currentNode->right != nullptr)
							{
								currentNode = currentNode->right;
								x++;
								gotoxy(x, y);
							}
							break;

						case VK_LEFT:
							if (currentNode != nullptr && currentNode->left != nullptr)
							{
								currentNode = currentNode->left;
								x--;
								gotoxy(x, y);
							}
							break;
						case 0x5A:  // 'z' key for undo
						{
							if (eventBuffer[i].Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
							{
								undoWord(list, undoStack, redoStack, x, y, currentNode);
								reprintRow(list, y - 1, y);
								gotoxy(x, y);
								break;
							}
							else
							{
								char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
								list.insertAt(x - 1, y - 1, c);
								reprintRow(list, y - 1, y);
								x++;
								gotoxy(x, y);

								currentNode = list.getHead();
								for (int r = 0; r < y - 1; r++)
								{
									if (currentNode->down != nullptr)
									{
										currentNode = currentNode->down;
									}
								}
								for (int c = 0; c < x - 1; c++)
								{
									if (currentNode->right != nullptr) 
									{
										currentNode = currentNode->right;
									}
								}
							}
							break;
						}

						case 0xBD:  // '-' key for redo
						{
							redoWord(list, redoStack, undoStack, y, x, currentNode);
							reprintRow(list, y - 1, y);
							gotoxy(x, y);
							break;
						}
						case 0x53:  // 'S' key for save
						{
							if (eventBuffer[i].Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
							{
								saveFile(list, file);
								break;
							}
							else
							{
								char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
								list.insertAt(x - 1, y - 1, c);
								reprintRow(list, y - 1, y);
								x++;
								gotoxy(x, y);

								currentNode = list.getHead();
								for (int r = 0; r < y - 1; r++)
								{
									if (currentNode->down != nullptr)
									{
										currentNode = currentNode->down;
									}
								}
								for (int c = 0; c < x - 1; c++)
								{
									if (currentNode->right != nullptr)
									{
										currentNode = currentNode->right;
									}
								}
							}
							break;
						}
						default:
						{
							char c = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;
							if (c != '\0' && (int)c >=65 && (int)c <= 90 || (int)c >= 97 && (int)c <= 122 || (int)c == 32)
							{
								const int writingWidth = 120 - 21;
								const int writingHeight = 26 - 4;
								if (x >= 1 && x <= writingWidth && y >= 1 && y <= writingHeight)
								{
									list.insertAt(x - 1, y - 1, c);
									reprintRow(list, y - 1, y);
									x++;
									gotoxy(x, y);

									currentNode = list.getHead();
									for (int r = 0; r < y - 1; r++)
									{
										if (currentNode->down != nullptr)
										{
											currentNode = currentNode->down;
										}
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
										else if (x == 2)
										{
											currentWordHead = currentNode;
											undoStack.push(currentWordHead);
											currentWordHead = nullptr;
										}
									}
								}
								else
								{
									std::cout << "\a";
								}
							}
							break;
						}
					}

				}
			}
		}

	}
	system("cls");
	list.display();
}

node* deepCopyWord(node* wordHead)
{
	if (wordHead == nullptr) return nullptr;

	node* newHead = new node();
	newHead->data = wordHead->data;

	node* currentOriginal = wordHead->right;
	node* currentCopy = newHead;

	if (currentOriginal == nullptr)
	{
		return nullptr;
	}

	while (currentOriginal != nullptr && currentOriginal->data != ' ')
	{
		node* newNode = new node();
		newNode->data = currentOriginal->data;

		currentCopy->right = newNode;
		newNode->left = currentCopy;

		currentCopy = newNode;
		currentOriginal = currentOriginal->right;
	}

	return newHead;
}



void undoWord(TwoDDLinkedList& list, Stack& undoStack, Stack& redoStack, int& x, int& y, node*& currentNode)
{
	if (undoStack.isEmpty())
	{
		return;
	}

	node* wordHead = undoStack.pop();

	if (wordHead == nullptr)
	{
		std::cout << "Undo operation aborted: word head is nullptr." << std::endl;
		return;
	}

	node* copiedWord = deepCopyWord(wordHead);
	if (copiedWord == nullptr)
	{
		return;
	}


	RedoData redoData;

	redoData.wordHead = copiedWord;
	redoData.row = y - 1;
	redoData.col = x - 1;

	redoStack.pushRedoData(redoData);

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

void redoWord(TwoDDLinkedList& list, Stack& redoStack, Stack& undoStack, int& x, int& y, node*& currentNode)
{
	if (redoStack.isEmpty())
	{
		return;
	}

	RedoData redoData = redoStack.popRedoData();

	node* wordHead = redoData.wordHead;

	currentNode = list.getHead();

	for (int i = 0; i < redoData.row; i++)
	{
		if (currentNode->down != nullptr)
		{
			currentNode = currentNode->down;
		}
	}

	node* insertPosition = currentNode;
	for (int i = 0; i < redoData.col; i++)
	{
		if (insertPosition->right != nullptr)
		{
			insertPosition = insertPosition->right;
		}
	}

	node* temp = wordHead;
	int tempCol = redoData.col;
	if (x != redoData.row + 1)
	{
		y = 1;
	}

	while (temp != nullptr)
	{
		list.insertAt(y - 1, redoData.row, temp->data);
		temp = temp->right;
		y++;
	}

	x = redoData.row + 1;

	currentNode = list.getHead();

	for (int i = 0; i < x - 1; i++)
	{
		if (currentNode->down != nullptr)
		{
			currentNode = currentNode->down;
		}
	}

	for (int i = 0; i < y - 1; i++)
	{
		if (currentNode->right != nullptr)
		{
			currentNode = currentNode->right;
		}
	}

	gotoxy(x, y);
}

void reprintRow(TwoDDLinkedList& list, int rowIndex, int consoleRow)
{
	node* row = list.getHead();
	int currentConsoleRow = 1;

	while (row != nullptr) 
	{
		gotoxy(1, currentConsoleRow);
		/*for (int i = 0; i < 99; i++)
		{
			std::cout << " ";
		}*/
		std::cout << "                                                                                                   ";


		gotoxy(1, currentConsoleRow);
		node* col = row;

		while (col != nullptr)
		{
			std::cout << col->data;
			col = col->right;
		}

		row = row->down;
		currentConsoleRow++;
	}
}