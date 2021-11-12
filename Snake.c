#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#define _CRT_SECURE_NO_WARNINGS

#define UP 72  // Up Arrow 
#define DOWN 80 // Down Arrow 
#define LEFT 75  // Left Arrow 
#define RIGHT 77  // Right Arrow 
#define ESC 27  // Escape Key
#define ROWS 25
#define COLS 75
#define Border '#'
#define Snake '@'
#define Food '$'
#define Space ' '
#define MaxSnakeSize 20


//ID: 208063362
//Name: Daniel Karamzin

typedef struct Point {
	int x;
	int y;
}Point;

void MainMenu(int* levelchoice);
void PrintRow(int amount, char ch);
void gotoxy(int y, int x);
int getKey();
void StartBoard(Point* foodplace);
void GenerateFood(Point* foodplace);
void Move(Point* head, Point* tail, char* dir);
void GameState(Point* foodplace);
bool CheckMove(Point* foodplace, int* length, Point* next);
void GameOver();

void main()
{
	int levelchoice = -1;
	Point foodplace = { (0,0) };
	bool Exit = false;

	while (!Exit)
	{

		MainMenu(&levelchoice);

		switch (levelchoice)
		{
		case 0:
			Exit = true;
			continue;
		case 1:
			system("cls");
			StartBoard(&foodplace);
			GameState(&foodplace);
			continue;
			break;
		}
	}
}

//function that prints the menu and scan the player option
void MainMenu(int* levelchoice)
{
	PrintRow(55, '*');
	printf("* Main Menu:\n");
	printf("* 0 -Exit\n* 1 - Start Game\n");
	PrintRow(55, '*');
	scanf("%d", levelchoice);
}

//function that gets the amount of specific char to print in 1 row
void PrintRow(int amount, char ch)
{
	int i;

	for (i = 0; i < amount; i++)
		printf("%c", ch);
	printf("\n");
}

//the function get from the main point vairaible to save the the place that the food was printed
//this function print the game board when the player choose to start a new game
void StartBoard(Point* foodplace)
{
	int i, j;

	gotoxy(0, 0);
	PrintRow(COLS, Border);
	for (i = 1; i < ROWS; i++)
	{
		gotoxy(0, i);
		printf("%c", Border);
		gotoxy(COLS - 1, i);
		printf("%c", Border);
	}
	GenerateFood(foodplace);
	gotoxy(0, i);
	PrintRow(COLS, Border);
}

//this function get a point variable and generate new food to print on the board, and save the x,y of the food in foodplace from the main.
void GenerateFood(Point* foodplace)
{
	int foodrow, foodcol;

	srand(time(NULL));
	foodrow = rand() % (ROWS - 2) + 1;//rand number from 1 to rows-2 (without borders 0 and rows-1)
	foodcol = rand() % (COLS - 2) + 1;//rand number from 1 to cols-2 (without borders 0 and cols-1)
	gotoxy(foodcol, foodrow);
	printf("%c", Food);
	foodplace->x = foodcol;
	foodplace->y = foodrow;
}

//function that manage the game senarios.
//this function create a snake in the first excecution from the main and then responsible to move the snake 
//and check when the game finished with a win or game over.
void GameState(Point* foodplace)
{
	int i;
	Point SnakeArr[MaxSnakeSize];
	int Snakesize = 10;
	int StartX = 37, StartY = 12; //the place of the center of the board
	char dir;
	bool gameover = false, Win = false;;

	//intialize the snake arr cells to start point which id the mid of the board
	for (i = 0; i < MaxSnakeSize; i++)
	{
		SnakeArr[i].x = StartX;
		SnakeArr[i].y = StartY;
	}

	dir = getKey();  //get the first press

	while (dir != ESC && !gameover && !Win)
	{
		//check if the snake grow to maximum snake body size to not go with the pointers over the arr limits +
		//check win if snake in the maximum size which is 20 (0-19)
		if (Snakesize <= MaxSnakeSize)
		{
			Move(SnakeArr + (Snakesize - 1), SnakeArr, &dir);
			//check game situation after the move excuated. if game over return true
			gameover = CheckMove(foodplace, &Snakesize, SnakeArr + (Snakesize - 1));
		}
		else //snake in the full size and the player won.
		{
			system("cls");
			PrintRow(35, Border);
			printf("           You Win!           \n");
			PrintRow(35, Border);
			Win = true;
		}
	}
	if (dir == ESC) //if the player press ESC finish the game with a deafet and a new menu.
		GameOver();
}

//function that get the pointer of the first snake body cell (arr[0]) and the last current snake size (arr+snakesize-1) and the direction of the move
//the function make the move by print space in the tail and the snake @ in the head after direction effects on the points value of the snake body
void Move(Point* head, Point* tail, char* dir)
{
	Point* p;
	char check = '0';

	gotoxy(tail->x, tail->y);
	printf("%c", Space);
	gotoxy(26, 81);

	Sleep(100);

	for (p = tail; p < head; p++)
	{
		*p = *(p + 1);
	}

	if (_kbhit()) // if any key was hit
		check = getKey();

	//check if the key that pressed was one of the options, if not ignore it.
	if (check == RIGHT || check == LEFT || check == UP || check == DOWN || check == ESC)
		*dir = check;


	switch (*dir)
	{
	case RIGHT:
		head->x++;
		break;
	case DOWN:
		head->y++;
		break;
	case LEFT:
		head->x--;
		break;
	case UP:
		head->y--;
		break;
	default:
		break;
	}

	//if head is in the 19 cell dont go out of the limit of the arr to update the next cell.
	if (head != tail + (MaxSnakeSize - 1))
		*(head + 1) = *head;

	gotoxy(head->x, head->y);
	printf("%c", Snake);
	gotoxy(26, 81);
}

//function checking if the snake head went to boarder or runover himself, or bumpt into food and update the length of the snake
bool CheckMove(Point* foodplace, int* length, Point* head)
{
	Point* p = head - 1;

	//check if the head went into his own body, if yes then game over
	while (p != head - *length)
	{
		if (head->x == p->x && head->y == p->y)
		{
			GameOver();
			return true;
		}

		/*problem: GenerateFood printed Food on the snake body randomly and the food vanished because of the snake move.
		solution: in the first iteration foodplace==head so its Generate new food in the first time, then it checkes if the new foodplace
		is part of the snake body with a moving pointer from head to tail, if yes, its generate new food place to avoid overun from the snake body*/
		if (foodplace->y == head->y && foodplace->x == head->x)
		{
			GenerateFood(foodplace);
			*length = *length + 1;
		}

		if (foodplace->x == p->x && foodplace->y == p->y)
			GenerateFood(foodplace);

		p--;
	}

	//check if head went into one of the board game borders
	if (head->x == 0 || head->x == COLS - 1 || head->y == 0 || head->y == ROWS - 1)
	{
		GameOver();
		return true;
	}
	return false;
}


void GameOver()
{
	system("cls");
	PrintRow(35, Border);
	printf("           GAME OVER           \n");
	PrintRow(35, Border);
}



// This function moves the cursor to the line y and column x.
// (0,0) is the upper left corner of the screen.
void gotoxy(int y, int x) {
	printf("\x1b[%d;%df", x + 1, y + 1);
}

// This function gets keyCodes for special keys such as arrows , escape and others
// It returns the Key code such as 72 for arrow up, 27 for Escape... 
int getKey()
{
	char KeyStroke = _getch();
	if (KeyStroke == 0 || KeyStroke == -32)
		KeyStroke = _getch();
	return (KeyStroke);
}