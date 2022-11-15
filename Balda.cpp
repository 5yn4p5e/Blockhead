#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,//(59, 120, 255)
	LightGreen = 10,
	LightCyan = 11,//(97, 214, 214)
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15//(242, 242, 242)
};

char DBfile[] = "database.dat", UDBfile[] = "USERdatabase.dat";
FILE* DB, * UDB;

int posDisp[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }, skipUser1 = 0, skipUser2 = 0;

int uprRU(char w[])//ЇаҐ®Ўа §гҐв б«®ў® Є ўҐае­Ґ¬г аҐЈЁбваг Ё Їа®ўҐапҐв ҐЈ® Є®ааҐЄв­®бвм
{
	int l = strlen(w);
	for (int a = 0; a < l; a++)
	{
		if (w[a] >= ' ' && w[a] <= 'Ї')
			w[a] = w[a] - 32;
		else if (w[a] >= 'а' && w[a] <= 'п')
			w[a] = w[a] - 80;
		else if (w[a] >= 'Ђ' && w[a] <= 'џ')
			continue;
		else return 0;
	}
	return 1;
}

char uprRU(char w)//ЇаҐ®Ўа §®ў ­ЁҐ бЁ¬ў®«  Є ўҐае­Ґ¬г аҐЈЁбваг Ё Їа®ўҐаЄ  ­  Є®ааҐЄв­®бвм
{
	if (w >= ' ' && w <= 'Ї')
	{
		w = w - 32;
		return w;
	}
	else if (w >= 'а' && w <= 'п')
	{
		w = w - 80;
		return w;
	}
	else if (w >= 'Ђ' && w <= 'џ')
		return w;
	else return 0;
}

int AddToDict(char word[])//¤®Ў ў«пҐв б«®ў® ў б«®ў ам ­  е®¤г (ў® ўаҐ¬п ЁЈал)
{
	fopen_s(&UDB, UDBfile, "rb+");
	if (uprRU(word) && strlen(word) >= 2)
	{
		fwrite(word, sizeof(word), 1, UDB);
		fclose(UDB);
		return 1;
	}
	fclose(UDB);
	return 0;
}

struct ListWords
{
	char word[16] = "\0";
	ListWords* next;
}*dict[14], * used;

void AddToUsed(char move[])//¤®Ў ў«пҐв б«®ў® ў бЇЁб®Є ЁбЇ®«м§®ў ­­ле
{
	ListWords* added, * curr, * prev;
	int len = strlen(move);
	added = new ListWords;
	strcpy_s(added->word, 16, move);
	added->next = NULL;
	if (!used) used = added;
	else
	{
		curr = used;
		prev = NULL;
		while (curr && strcmp(curr->word, added->word) < 0)
		{
			prev = curr;
			curr = curr->next;
		}
		if (!prev) used = added;
		else prev->next = added;
		added->next = curr;
	}
}

int CheckUsedWord(char move[])//Їа®ўҐапҐв, Ўл«® «Ё ЁбЇ®«м§®ў ­® б«®ў® ўлЎа ­­®Ґ б«®ў® а ­ҐҐ; ў®§ўа в: 1 - ЁбЇ®«м§®ў «®бм, 0 - ­Ґв
{
	int len = strlen(move);
	ListWords* buff;
	buff = used;
	while (buff)
	{
		if (strcmp(move, buff->word) == 0)
			return 1;
		else if (strcmp(move, buff->word) < 0)
			return 0;
		buff = buff->next;
	}
	return 0;
}

int LoadDict()//ўлЈаг¦ Ґв ўҐбм б«®ў ам ў Ћ‡“
{
	fopen_s(&DB, DBfile, "rb");
	if (DB)
	{
		ListWords* curr, * last = NULL;
		char buff[16] = "\0";
		while (fread(buff, sizeof(buff), 1, DB))
		{
			int len = strlen(buff);
			curr = new ListWords;
			strcpy_s(curr->word, 16, buff);
			curr->next = NULL;
			if (!dict[len - 2])
				dict[len - 2] = curr;
			else
				last->next = curr;
			last = curr;
		}
		fclose(DB);
	}
	else
		return 0;
	fopen_s(&UDB, UDBfile, "rb");
	if (UDB)
	{
		ListWords* added, * curr, * prev;
		char buff[16] = "\0";
		while (fread(buff, sizeof(buff), 1, UDB))
		{
			int len = strlen(buff);
			added = new ListWords;
			strcpy_s(added->word, 16, buff);
			added->next = NULL;
			if (!dict[len - 2]) dict[len - 2] = added;
			else
			{
				curr = dict[len - 2];
				prev = NULL;
				while (curr && strcmp(curr->word, added->word) < 0)
				{
					prev = curr;
					curr = curr->next;
				}
				if (!prev) dict[len - 2] = added;
				else prev->next = added;
				added->next = curr;
			}
		}
		fclose(UDB);
		return 1;
	}
	else
		return 0;
}

int CheckCompWord(char move[])//Їа®ўҐапҐв ­ ©¤Ґ­­®Ґ Є®¬ЇмовҐа®¬ б«®ў® ­  ў®§¬®¦­®бвм Ї®е®¤Ёвм
{
	int len = strlen(move), i, flag = 0;
	ListWords* buff;
	buff = dict[len - 2];
	while (buff && !flag)
	{
		int misses = 0;
		for (i = 0; i < len; i++)
		{
			if (move[i] == ' ')
				misses++;
			else if (move[i] < buff->word[i] && !misses)
			{
				flag++;
				break;
			}
			else if (move[i] > buff->word[i] || move[i] < buff->word[i])
				break;
		}
		if (i == len && misses == 1 && !CheckUsedWord(buff->word))
		{
			strcpy_s(move, sizeof(buff->word), buff->word);
			return 1;
		}
		buff = buff->next;
	}
	return 0;
}

int CheckUserWord(char move[], int** matr, int n, int spaces)//Їа®ўҐапҐв ­ ©¤Ґ­­®Ґ Ї®«м§®ў вҐ«Ґ¬ б«®ў® ­  ў®§¬®¦­®бвм Ї®е®¤Ёвм
{
	int len = strlen(move);
	ListWords* buff;
	buff = dict[len - 2];
	if (len < 2)
		return -2;
	if (spaces != 1)
		return -3;
	int flagI = 0, flagJ = 0, z = 0;
	for (int i = n / 2; i < n && i >= 0 && !flagI; i = i + z)
	{
		for (int j = 0; j < n && !flagJ; j++)
		{
			if (matr[i][j] == strlen(move) - 1)
				goto A;
			if (matr[i][j] >= 0)
			{
				int a = 0;
				while (a != 4)
				{
					if (matr[i + posDisp[a][0]][j + posDisp[a][1]] == matr[i][j] + 1)
						break;
					a++;
				}
				if (a == 4)
					return -3;
			}
		}
		if (z < 0) z--;
		else z++;
		z = -z;
	}
A:
	while (buff)
	{
		if (strcmp(move, buff->word) == 0 && !CheckUsedWord(buff->word))
			return 1;
		else if (strcmp(move, buff->word) < 0)
			return 0;
		buff = buff->next;
	}
	return -1;
}

int CompMove(int hor, int ver, int n, int lenW, int spaces, char move[], int a, int** matr, char** field)//Ї®«­л© ЇҐаҐЎ®а ўбҐе б«®ў
//lenW - ­Ґ®Ўе®¤Ё¬ п ¤«Ё­  б«®ў , spaces - Є®«ЁзҐбвў® Їа®ЎҐ«®ў, move - ®Є®­з вҐ«м­л© е®¤ Є®¬ЇмовҐа , a - Ё­¤ҐЄб ЎгЄўл ў б«®ўҐ
{
	matr[hor][ver] = a;
	move[a] = field[hor][ver];
	int spacesCurr = spaces;
	if (move[a] == ' ' && spacesCurr == 1)
		return 0;
	else if (move[a] == ' ')
		spacesCurr++;
	if (lenW == strlen(move) && spaces == 1 && CheckCompWord(move))
		return 1;
	else
	{
		for (int i = 0; i < 4 && lenW != a + 1; i++)
		{
			int horCurr = hor + posDisp[i][0], verCurr = ver + posDisp[i][1];
			if (horCurr >= 0 && horCurr < n && verCurr >= 0 && verCurr < n && matr[horCurr][verCurr] < 0)
			{
				if (field[horCurr][verCurr] == ' ' && spacesCurr == 1)
					continue;
				if (CompMove(horCurr, verCurr, n, lenW, spacesCurr, move, a + 1, matr, field))
					return 1;
			}
		}
		matr[hor][ver] = -1;
		move[a] = '\0';
		return 0;
	}
}

int UserMove(int n, int* skipUser, char move[16], char** field, int** matr, COORD** positCell, COORD positUserMove)//е®¤ Ї®«м§®ў вҐ«п
{
	HWND handle = GetConsoleWindow();
	HDC devCont = GetDC(handle);
	HANDLE handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));

	char skip[] = "Џа®ЇгбвЁвм е®¤", exit[] = "‚л©вЁ ў Ј« ў­®Ґ ¬Ґ­о", doMove[] = "‘¤Ґ« вм е®¤";
	COORD positDoMove, positSkip, positExit, positUser = positUserMove;
	positDoMove.X = positCell[0][0].X;
	positDoMove.Y = positCell[n - 1][0].Y + 3;
	positSkip.X = positDoMove.X;
	positSkip.Y = positDoMove.Y + 1;
	positExit.X = positDoMove.X;
	positExit.Y = positDoMove.Y + 2;
	SetConsoleCursorPosition(handleOutput, positDoMove);
	puts(doMove);
	SetConsoleCursorPosition(handleOutput, positSkip);
	puts(skip);
	SetConsoleCursorPosition(handleOutput, positExit);
	puts(exit);
	int hor = n / 2, ver = n / 2, value = 3, a = -1, entHor = -1, entVer = -1, spaces = 0;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matr[i][j] = -1;
	do
	{
		if (value == 2)
		{
			a = -1, entHor = -1, entVer = -1;
			for (int i = 0; i < n; i++)
				for (int j = 0; j < 16; j++)
				{
					if (j < n)
						matr[i][j] = -1;
					move[j] = '\0';
				}
			spaces = 0;
		}
		value = 3;
		if (!(hor >= 0 && hor < n && ver >= 0 && ver < n))
		{
			hor = n / 2;
			ver = n / 2;
		}
		unsigned char ch;

		SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
		SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
		printf("%c", field[hor][ver]);
		do
		{
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					if (hor >= 0 && hor < n && ver >= 0 && ver < n)
					{
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						printf("%c", field[hor][ver]);
						hor--;
						if (hor < 0)
						{
							SetConsoleCursorPosition(handleOutput, positExit);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							hor = positExit.X;
							ver = positExit.Y;
							puts(exit);
							value = -1;
						}
						else
						{
							SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							printf("%c", field[hor][ver]);
							value = 1;
						}
					}
					else if (ver == positDoMove.Y)
					{
						SetConsoleCursorPosition(handleOutput, positDoMove);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						puts(doMove);
						hor = n - 1;
						ver = 0;
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						printf("%c", field[hor][ver]);
						value = 1;
					}
					else if (ver == positSkip.Y)
					{
						SetConsoleCursorPosition(handleOutput, positSkip);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						puts(skip);
						SetConsoleCursorPosition(handleOutput, positDoMove);
						hor = positDoMove.X;
						ver = positDoMove.Y;
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						puts(doMove);
						value = 2;
					}
					else if (ver == positExit.Y)
					{
						SetConsoleCursorPosition(handleOutput, positExit);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						puts(exit);
						SetConsoleCursorPosition(handleOutput, positSkip);
						hor = positSkip.X;
						ver = positSkip.Y;
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						puts(skip);
						value = 0;
					}
					break;
				case 80:
					if (hor >= 0 && hor < n && ver >= 0 && ver < n)
					{
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						printf("%c", field[hor][ver]);
						hor++;
						if (hor == n)
						{
							SetConsoleCursorPosition(handleOutput, positDoMove);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							hor = positDoMove.X;
							ver = positDoMove.Y;
							puts(doMove);
							value = 2;
						}
						else
						{
							SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							printf("%c", field[hor][ver]);
							value = 1;
						}
					}
					else if (ver == positDoMove.Y)
					{
						SetConsoleCursorPosition(handleOutput, positDoMove);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						puts(doMove);
						SetConsoleCursorPosition(handleOutput, positSkip);
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						hor = positSkip.X;
						ver = positSkip.Y;
						puts(skip);
						value = 0;
					}
					else if (ver == positSkip.Y)
					{
						SetConsoleCursorPosition(handleOutput, positSkip);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						puts(skip);
						SetConsoleCursorPosition(handleOutput, positExit);
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						hor = positExit.X;
						ver = positExit.Y;
						puts(exit);
						value = -1;
					}
					else if (ver == positExit.Y)
					{
						SetConsoleCursorPosition(handleOutput, positExit);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						puts(exit);
						hor = 0;
						ver = 0;
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						printf("%c", field[hor][ver]);
						value = 1;
					}
					break;
				case 75:
					if (hor >= 0 && hor < n && ver >= 0 && ver < n)
					{
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						printf("%c", field[hor][ver]);
						ver--;
						if (ver < 0)
						{
							SetConsoleCursorPosition(handleOutput, positDoMove);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							hor = positDoMove.X;
							ver = positDoMove.Y;
							puts(doMove);
							value = 2;
						}
						else
						{
							SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							printf("%c", field[hor][ver]);
							value = 1;
						}
					}
					break;
				case 77:
					if (hor >= 0 && hor < n && ver >= 0 && ver < n)
					{
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						printf("%c", field[hor][ver]);
						ver++;
						if (ver == n)
						{
							SetConsoleCursorPosition(handleOutput, positDoMove);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							hor = positDoMove.X;
							ver = positDoMove.Y;
							puts(doMove);
							value = 2;
						}
						else
						{
							SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							printf("%c", field[hor][ver]);
							value = 1;
						}
					}
					break;
				}
			}
		} while (ch != 13 && ch != 8);
		if (ch == 8)
		{
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
			if (entHor > -1)
			{
				spaces = 0;
				matr[entHor][entVer] = -1;
				field[entHor][entVer] = ' ';
				SetConsoleCursorPosition(handleOutput, positCell[entHor][entVer]);
				printf("%c", field[entHor][entVer]);

			}
			if (a > -1)
			{
				move[a] = '\0';
				if (entHor == -1)
					for (int i = 0; i < n; i++)
						for (int j = 0; j < n; j++)
							if (matr[i][j] == a)
								matr[i][j] = -1;
				positUserMove.X--;
				a--;
			}
			SetConsoleCursorPosition(handleOutput, positUserMove);
			printf(" ");
			COORD currPos;
			SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
			if (hor >= 0 && hor < n && ver >= 0 && ver < n)
				SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
			else if (ver == positDoMove.Y)
				SetConsoleCursorPosition(handleOutput, positDoMove);
			else if (ver == positSkip.Y)
				SetConsoleCursorPosition(handleOutput, positSkip);
			else if (ver == positExit.Y)
				SetConsoleCursorPosition(handleOutput, positExit);
		}
		else
		{
			switch (value)
			{
			case 3:
				goto EnterPressed;
				break;
			case 0:
				(*skipUser)++;
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
				SetConsoleCursorPosition(handleOutput, positDoMove);
				puts(doMove);
				SetConsoleCursorPosition(handleOutput, positSkip);
				puts(skip);
				SetConsoleCursorPosition(handleOutput, positExit);
				puts(exit);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				return 0;
			case 1:
			EnterPressed:
				if (matr[hor][ver] < 0 && a < 16)
				{
					if (field[hor][ver] == ' ' && !spaces)
					{
						char buff = _getch();
						if (uprRU(buff))
						{
							buff = uprRU(buff);
							spaces++;
							matr[hor][ver] = ++a;
							entHor = hor;
							entVer = ver;
							move[a] = buff;
							field[hor][ver] = buff;
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
							printf("%c", field[hor][ver]);
							SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
							SetConsoleCursorPosition(handleOutput, positUserMove);
							printf("%c", field[hor][ver]);
							positUserMove.X++;
						}

					}
					else if (field[hor][ver] != ' ')
					{
						matr[hor][ver] = ++a;
						move[a] = field[hor][ver];
						SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
						SetConsoleCursorPosition(handleOutput, positCell[hor][ver]);
						printf("%c", field[hor][ver]);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
						SetConsoleCursorPosition(handleOutput, positUserMove);
						printf("%c", field[hor][ver]);
						positUserMove.X++;
					}
				}
				break;
			case 2:
				positUserMove.X = positUser.X;
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				SetConsoleCursorPosition(handleOutput, positDoMove);
				puts(doMove);
				char msg[52];
				COORD positMsg;
				positMsg.X = positExit.X;
				positMsg.Y = positExit.Y + 1;
				switch (CheckUserWord(move, matr, n, spaces))
				{
				case -3:
					strcpy_s(msg, sizeof(msg), "’ Є ў ЁЈаҐ ­Ґ«м§п е®¤Ёвм!");
					if (entHor > -1)
					{
						field[entHor][entVer] = ' ';
						SetConsoleCursorPosition(handleOutput, positCell[entHor][entVer]);
						putchar(' ');
					}
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					Sleep(2000);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					SetConsoleCursorPosition(handleOutput, positUserMove);
					puts("               ");
					break;
				case -2:
					strcpy_s(msg, sizeof(msg), "‘«ЁиЄ®¬ Є®а®вЄ®Ґ б«®ў®!");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					Sleep(2000);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					SetConsoleCursorPosition(handleOutput, positUserMove);
					puts(move);
					break;
				case -1:
					strcpy_s(msg, sizeof(msg), "‘«®ў® г¦Ґ ЁбЇ®«м§®ў «®бм а ­ҐҐ!");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					Sleep(2000);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					SetConsoleCursorPosition(handleOutput, positUserMove);
					puts(move);
					break;
				case 0:
					unsigned char buff;
					strcpy_s(msg, sizeof(msg), "„®Ў ўЁвм б«®ў® ў б«®ў ам Ё б¤Ґ« вм е®¤? Enter - ¤ ");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					buff = _getch();
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
					SetConsoleCursorPosition(handleOutput, positMsg);
					puts(msg);
					if (buff == 13)
					{
						AddToDict(move);
						AddToUsed(move);
						SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
						SetConsoleCursorPosition(handleOutput, positDoMove);
						puts(doMove);
						SetConsoleCursorPosition(handleOutput, positSkip);
						puts(skip);
						SetConsoleCursorPosition(handleOutput, positExit);
						puts(exit);
						return 1;
					}
					break;
				case 1:
					AddToUsed(move);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
					SetConsoleCursorPosition(handleOutput, positDoMove);
					puts(doMove);
					SetConsoleCursorPosition(handleOutput, positSkip);
					puts(skip);
					SetConsoleCursorPosition(handleOutput, positExit);
					puts(exit);
					return 1;
				}
				for (int i = 0; i < a; i++)
				{
					if (entHor >= 0 && entVer >= 0 && a == matr[entHor][entVer])
						field[entHor][entVer] = ' ';
					move[i] = '\0';
				}
				break;
			}
		}
	} while (value >= 0);
	return -1;
}

int PrepCompMove(int n, int moveNum, char move[], int* skipComp, int** matr, char** field)//б®§¤ ­ЁҐ ўбЇ®¬®Ј вҐ«м­®© ¬ ваЁжл, гЇа ў«Ґ­ЁҐ Ї®«­л¬ ЇҐаҐЎ®а®¬, ®в®Ўа ¦Ґ­ЁҐ
//е®¤  Є®¬ЇмовҐа  Ї®«м§®ў вҐ«о; n - а §¬Ґа­®бвм ¬ ваЁжл Ї®«п ЁЈал, moveNum - ­®¬Ґа е®¤ , field - Ї®«Ґ ЁЈал
{
	int lenW = n + moveNum, z;
	if (lenW > 15)
		lenW = 15;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			matr[i][j] = -1;
	while (lenW >= 2)
	{
		if (dict[lenW - 2])//Ґбвм «Ё б«®ў  в Є®© ¤«Ё­л ў б«®ў аҐ
		{
			z = 0;
			for (int hor = n / 2; hor < n && hor >= 0; hor = hor + z)
			{
				for (int ver = 0; ver < n; ver++)
					if (CompMove(hor, ver, n, lenW, 0, move, 0, matr, field))
					{
						AddToUsed(move);
						return 1;
					}
				if (z < 0) z--;
				else z++;
				z = -z;
			}
		}
		lenW--;
	}
	(*skipComp)++;
	return 0;
}

void GameWithComp(int n, HWND handle, HDC devCont, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo)
//ЁЈа  Ї®«м§®ў вҐ«п б Є®¬ЇмовҐа®¬
{
	RECT rect;
	GetClientRect(handle, &rect);
	PCONSOLE_FONT_INFOEX conFontInfoEX = new CONSOLE_FONT_INFOEX;
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	int xmax, ymax;
	xmax = scrBuffInfo->dwSize.X;
	ymax = scrBuffInfo->dwSize.Y;

	srand(time(NULL));
	char key[16] = "\0";
	ListWords* buff;
	buff = dict[n - 2];
	int random;
	switch (n)
	{
	case 5: random = 600; break;
	case 7: random = 500; break;
	case 9: random = 400; break;
	case 11: random = 12; break;
	}
	while (buff->next)
	{
		int r = rand() % random;
		if (r)
			buff = buff->next;
		else
		{
			strcpy_s(key, sizeof(key), buff->word);
			buff = NULL;
			break;
		}
	}
	if (buff && !buff->next)
		strcpy_s(key, sizeof(key), dict[n - 2]->word);
	AddToUsed(key);
	COORD** positCell = new COORD * [n];
	char** field = new char* [n];
	int** matr = new int* [n];
	for (int i = 0; i < n; i++)
	{
		positCell[i] = new COORD[n];
		field[i] = new char[n];
		matr[i] = new int[n];
	}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			positCell[i][j].X = (xmax - n * 4) / 2 + 4 * j;
			positCell[i][j].Y = 4 + 2 * i;
			field[i][j] = ' ';
			matr[i][j] = -1;
			COORD positSym;
			if (!j || j == n - 1)
			{
				positSym.Y = positCell[i][j].Y;
				if (!j)
				{
					positSym.X = positCell[i][j].X - 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(186);
					if (!i)
					{
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(201);
					}
					else if (i == n - 1)
					{
						positSym.Y++;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(200);
					}
					else
					{
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(199);
						positSym.Y += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(199);
						positSym.X += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.Y -= 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
					}
					positSym.X += 4;
				}
				else
				{
					positSym.X = positCell[i][j].X + 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(186);
					if (!i)
					{
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(187);
					}
					else if (i == n - 1)
					{
						positSym.Y++;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(188);
					}
					else
					{

						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(182);
						positSym.Y += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(182);
						positSym.X -= 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.Y -= 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
					}
					positSym.X -= 4;
				}
			}
			if (!i || i == n - 1)
			{
				positSym.X = positCell[i][j].X;
				if (!i)
				{
					positSym.Y = positCell[i][j].Y - 1;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X++;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X -= 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					if (j > 0 && j < n - 1)
					{
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(209);
						positSym.X += 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(209);
						positSym.Y++;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
						positSym.X -= 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
					}
					positSym.Y += 2;
				}
				else
				{
					positSym.Y = positCell[i][j].Y + 1;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X++;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X -= 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					if (j > 0 && j < n - 1)
					{
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(207);
						positSym.X += 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(207);
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
						positSym.X -= 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
					}
					positSym.Y -= 2;
				}
			}
			if (i > 0 && i < n - 1 && j > 0 && j < n - 1)
			{
				positSym.X = positCell[i][j].X - 2;
				positSym.Y = positCell[i][j].Y - 1;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.Y++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(179);
				positSym.Y++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.Y--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(179);
				positSym.Y--;
			}
		}
	for (int i = 0; i < n; i++)
		field[n / 2][i] = key[i];
	int a = n, skipComp = 0, skipUser = 0, compScores = 0, userScores = 0, compRand, userRand;
	if (compRand = rand() % 2)
		userRand = 0;
	else
		userRand = 1;
	char comp[] = "Љ®¬ЇмовҐа:", user[] = "‚л:";
	COORD positComp, positUser;
	positComp.X = (xmax - strlen(comp)) * 2 / 15;
	positComp.Y = 1;
	positUser.X = (xmax - strlen(user)) * 13 / 15;
	positUser.Y = 1;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			SetConsoleCursorPosition(handleOutput, positCell[i][j]);
			printf("%c", field[i][j]);
		}
	SetConsoleCursorPosition(handleOutput, positComp);
	puts(comp);
	SetConsoleCursorPosition(handleOutput, positUser);
	puts(user);
	COORD positCompMove, positUserMove;
	positCompMove.X = positComp.X, positCompMove.Y = 2, positUserMove.X = positUser.X, positUserMove.Y = 2;
	do
	{
		char move[16] = "\0";
		if (compRand < userRand)
		{
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
			SetConsoleCursorPosition(handleOutput, positComp);
			puts(comp);
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
			int moveNum = a - n;
			if (PrepCompMove(n, moveNum, move, &skipComp, matr, field))
			{
				compScores += strlen(move);
				for (int i = 0; i < n; i++)
					for (int j = 0; j < n; j++)
					{
						if (matr[i][j] != -1 && field[i][j] != move[matr[i][j]])
						{
							SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
							field[i][j] = move[matr[i][j]];
							SetConsoleCursorPosition(handleOutput, positCell[i][j]);
							printf("%c", field[i][j]);
						}
						if (matr[i][j] != -1)
						{
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							SetConsoleCursorPosition(handleOutput, positCell[i][j]);
							printf("%c", field[i][j]);
						}
					}
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				SetConsoleCursorPosition(handleOutput, positCompMove);
				puts(move);
				Sleep(3000);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				for (int i = 0; i < n; i++)
					for (int j = 0; j < n; j++)
						if (matr[i][j] != -1)
						{
							SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
							SetConsoleCursorPosition(handleOutput, positCell[i][j]);
							printf("%c", field[i][j]);
						}
			}
			else
			{
				strcpy_s(move, sizeof(move), "Џа®ЇгбЄ");
				SetConsoleCursorPosition(handleOutput, positCompMove);
				puts(move);
				if (skipComp == 3 && skipUser < 3)
				{
					Sleep(2000);
					char win[] = "3 Їа®ЇгбЄ  Є®¬ЇмовҐа ! ‚л ўлЁЈа «Ё!";
					COORD positWin;
					positWin.X = (xmax - strlen(win)) / 2;
					positWin.Y = 5;
					SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
					system("cls");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positWin);
					puts(win);
					Sleep(2000);
					return;
				}
			}
			COORD positCompScores;
			positCompScores.X = positComp.X + strlen(comp) + 1;
			positCompScores.Y = positComp.Y;
			positCompMove.Y++;
			compRand += 2;
			SetConsoleCursorPosition(handleOutput, positCompScores);
			printf("%d", compScores);
			SetConsoleCursorPosition(handleOutput, positComp);
			puts(comp);
		}
		else
		{
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
			SetConsoleCursorPosition(handleOutput, positUser);
			puts(user);
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
			switch (UserMove(n, &skipUser, move, field, matr, positCell, positUserMove))
			{
			case -1:
				return;
			case 0:
				strcpy_s(move, sizeof(move), "Џа®ЇгбЄ");
				SetConsoleCursorPosition(handleOutput, positUserMove);
				puts(move);
				if (skipUser == 3 && skipComp < 3)
				{
					Sleep(2000);
					char defeat[] = "3 Їа®ЇгбЄ  е®¤ ! ‚л Їа®ЁЈа «Ё!";
					COORD positDefeat;
					positDefeat.X = (xmax - strlen(defeat)) / 2;
					positDefeat.Y = 5;
					SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
					system("cls");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positDefeat);
					puts(defeat);
					Sleep(2000);
					return;
				}
				break;
			case 1:
				userScores += strlen(move);
				for (int i = 0; i < n; i++)
					for (int j = 0; j < n; j++)
						if (matr[i][j] != -1)
						{
							SetConsoleTextAttribute(handleOutput, (WORD)((LightGray << 4) | Red));
							SetConsoleCursorPosition(handleOutput, positCell[i][j]);
							printf("%c", field[i][j]);
						}
				Sleep(3000);
				for (int i = 0; i < n; i++)
					for (int j = 0; j < n; j++)
						if (matr[i][j] != -1)
						{
							SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
							SetConsoleCursorPosition(handleOutput, positCell[i][j]);
							printf("%c", field[i][j]);
						}
				SetConsoleCursorPosition(handleOutput, positUserMove);
				puts(move);
				break;
			}
			COORD positUserScores;
			positUserScores.X = positUser.X + strlen(user) + 1;
			positUserScores.Y = positUser.Y;
			positUserMove.Y++;
			userRand += 2;
			SetConsoleCursorPosition(handleOutput, positUserScores);
			printf("%d", userScores);
			SetConsoleCursorPosition(handleOutput, positUser);
			puts(user);
		}
		a++;
	} while (a != n * n);
	COORD positMsg;
	char msg[15] = "\0";
	if (compScores > userScores)
		strcpy_s(msg, sizeof(msg), "‚л Їа®ЁЈа «Ё!");
	else if (compScores < userScores)
		strcpy_s(msg, sizeof(msg), "‚л ўлЁЈа «Ё!");
	else if (compScores == userScores)
		strcpy_s(msg, sizeof(msg), "ЌЁзмп");
	else
	{
		char msg[] = "€Ја  ®Є®­зҐ­ , ­® зв®-в® Ї®и«® ­Ґ в Є. ЋЎа вЁвҐбм Є а §а Ў®взЁЄг! 31neol@gmail.com";
		positMsg.X = (xmax - strlen(msg)) / 2;
		positMsg.Y = 5;
		SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
		system("cls");
		SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
		SetConsoleCursorPosition(handleOutput, positMsg);
		printf("%s\n", msg);
		system("Pause");
		return;
	}
	positMsg.X = (xmax - strlen(msg)) / 2;
	positMsg.Y = 5;
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	SetConsoleCursorPosition(handleOutput, positMsg);
	puts(msg);
	Sleep(2000);
	return;
}

void TwoUsersGame(int n, HWND handle, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo)
//ЁЈа  ¤ўге «о¤Ґ©
{
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	int xmax, ymax;
	xmax = scrBuffInfo->dwSize.X;
	ymax = scrBuffInfo->dwSize.Y;
	char user1[16] = "\0", user2[16] = "\0";
	COORD userName;
	userName.X = xmax / 2 - 10;
	userName.Y = 5;
	SetConsoleCursorPosition(handleOutput, userName);
	printf("ЏҐаўл© ЁЈа®Є, ўўҐ¤ЁвҐ ‚ иҐ Ё¬п: ");
	gets_s(user1, 15);
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | White));
	SetConsoleCursorPosition(handleOutput, userName);
	printf("ЏҐаўл© ЁЈа®Є, ўўҐ¤ЁвҐ ‚ иҐ Ё¬п: %s", user1);
	SetConsoleCursorPosition(handleOutput, userName);
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	printf("‚в®а®© ЁЈа®Є, ўўҐ¤ЁвҐ ‚ иҐ Ё¬п: ");
	gets_s(user2, 15);
	system("cls");
	srand(time(NULL));
	char key[16] = "\0";
	ListWords* buff;
	buff = dict[n - 2];
	int random;
	switch (n)
	{
	case 5: random = 600; break;
	case 7: random = 500; break;
	case 9: random = 400; break;
	case 11: random = 12; break;
	}
	while (buff->next)
	{
		int r = rand() % random;
		if (r)
			buff = buff->next;
		else
		{
			strcpy_s(key, sizeof(key), buff->word);
			buff = NULL;
			break;
		}
	}
	if (buff && !buff->next)
		strcpy_s(key, sizeof(key), dict[n - 2]->word);
	AddToUsed(key);
	COORD** positCell = new COORD * [n];
	char** field = new char* [n];
	int** matr = new int* [n];
	for (int i = 0; i < n; i++)
	{
		positCell[i] = new COORD[n];
		field[i] = new char[n];
		matr[i] = new int[n];
	}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			positCell[i][j].X = (xmax - n * 4) / 2 + 4 * j;
			positCell[i][j].Y = 4 + 2 * i;
			field[i][j] = ' ';
			matr[i][j] = -1;
			COORD positSym;
			if (!j || j == n - 1)
			{
				positSym.Y = positCell[i][j].Y;
				if (!j)
				{
					positSym.X = positCell[i][j].X - 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(186);
					if (!i)
					{
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(201);
					}
					else if (i == n - 1)
					{
						positSym.Y++;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(200);
					}
					else
					{
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(199);
						positSym.Y += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(199);
						positSym.X += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.Y -= 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
					}
					positSym.X += 4;
				}
				else
				{
					positSym.X = positCell[i][j].X + 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(186);
					if (!i)
					{
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(187);
					}
					else if (i == n - 1)
					{
						positSym.Y++;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(188);
					}
					else
					{

						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(182);
						positSym.Y += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(182);
						positSym.X -= 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X += 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.Y -= 2;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(196);
					}
					positSym.X -= 4;
				}
			}
			if (!i || i == n - 1)
			{
				positSym.X = positCell[i][j].X;
				if (!i)
				{
					positSym.Y = positCell[i][j].Y - 1;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X++;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X -= 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					if (j > 0 && j < n - 1)
					{
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(209);
						positSym.X += 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(209);
						positSym.Y++;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
						positSym.X -= 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
					}
					positSym.Y += 2;
				}
				else
				{
					positSym.Y = positCell[i][j].Y + 1;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X++;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					positSym.X -= 2;
					SetConsoleCursorPosition(handleOutput, positSym);
					putchar(205);
					if (j > 0 && j < n - 1)
					{
						positSym.X--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(207);
						positSym.X += 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(207);
						positSym.Y--;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
						positSym.X -= 4;
						SetConsoleCursorPosition(handleOutput, positSym);
						putchar(179);
					}
					positSym.Y -= 2;
				}
			}
			if (i > 0 && i < n - 1 && j > 0 && j < n - 1)
			{
				positSym.X = positCell[i][j].X - 2;
				positSym.Y = positCell[i][j].Y - 1;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.Y++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(179);
				positSym.Y++;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(196);
				positSym.X--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(197);
				positSym.Y--;
				SetConsoleCursorPosition(handleOutput, positSym);
				putchar(179);
				positSym.Y--;
			}
		}
	for (int i = 0; i < n; i++)
		field[n / 2][i] = key[i];
	int a = n, skipUser1 = 0, skipUser2 = 0, user1Scores = 0, user2Scores = 0, user1Rand, user2Rand;
	if (user1Rand = rand() % 2)
		user2Rand = 0;
	else
		user2Rand = 1;
	COORD positUser1, positUser2;
	positUser1.X = (xmax - strlen(user1)) * 2 / 15;
	positUser1.Y = 1;
	positUser2.X = (xmax - strlen(user2)) * 13 / 15;
	positUser2.Y = 1;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			SetConsoleCursorPosition(handleOutput, positCell[i][j]);
			printf("%c", field[i][j]);
		}
	SetConsoleCursorPosition(handleOutput, positUser1);
	puts(user1);
	SetConsoleCursorPosition(handleOutput, positUser2);
	puts(user2);
	COORD positUser1Move, positUser2Move;
	positUser1Move.X = positUser1.X, positUser1Move.Y = 2, positUser2Move.X = positUser2.X, positUser2Move.Y = 2;
	do
	{
		char move[16] = "\0";
		if (user1Rand < user2Rand)
		{
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
			SetConsoleCursorPosition(handleOutput, positUser1);
			puts(user1);
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
			switch (UserMove(n, &skipUser1, move, field, matr, positCell, positUser1Move))
			{
			case -1:
				return;
			case 0:
				strcpy_s(move, sizeof(move), "Џа®ЇгбЄ");
				SetConsoleCursorPosition(handleOutput, positUser1Move);
				puts(move);
				if (skipUser1 == 3 && skipUser2 < 3)
				{
					Sleep(2000);
					COORD positDefeat;
					positDefeat.X = (xmax - 35) / 2;
					positDefeat.Y = 5;
					SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
					system("cls");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positDefeat);
					printf("3 Їа®ЇгбЄ  е®¤ ! %s Їа®ЁЈа «Ё!", user1);
					Sleep(2000);
					return;
				}
				break;
			case 1:
				user1Scores += strlen(move);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				SetConsoleCursorPosition(handleOutput, positUser1Move);
				puts(move);
				break;
			}
			COORD positUser1Scores;
			positUser1Scores.X = positUser1.X + strlen(user1);
			positUser1Scores.Y = positUser1.Y;
			positUser1Move.Y++;
			user1Rand += 2;
			SetConsoleCursorPosition(handleOutput, positUser1Scores);
			printf(": %d", user1Scores);
			SetConsoleCursorPosition(handleOutput, positUser1);
			puts(user1);
		}
		else
		{
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
			SetConsoleCursorPosition(handleOutput, positUser2);
			puts(user2);
			SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
			switch (UserMove(n, &skipUser2, move, field, matr, positCell, positUser2Move))
			{
			case -1:
				return;
			case 0:
				strcpy_s(move, sizeof(move), "Џа®ЇгбЄ");
				SetConsoleCursorPosition(handleOutput, positUser2Move);
				puts(move);
				if (skipUser2 == 3 && skipUser1 < 3)
				{
					Sleep(2000);
					COORD positDefeat;
					positDefeat.X = (xmax - 35) / 2;
					positDefeat.Y = 5;
					SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
					system("cls");
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					SetConsoleCursorPosition(handleOutput, positDefeat);
					printf("3 Їа®ЇгбЄ  е®¤ ! %s Їа®ЁЈа «Ё!", user1);
					Sleep(2000);
					return;
				}
				break;
			case 1:
				user2Scores += strlen(move);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				SetConsoleCursorPosition(handleOutput, positUser2Move);
				puts(move);
				break;
			}
			COORD positUser2Scores;
			positUser2Scores.X = positUser2.X + strlen(user2);
			positUser2Scores.Y = positUser2.Y;
			positUser2Move.Y++;
			user2Rand += 2;
			SetConsoleCursorPosition(handleOutput, positUser2Scores);
			printf(": %d", user2Scores);
			SetConsoleCursorPosition(handleOutput, positUser2);
			puts(user2);
		}
		a++;
	} while (a != n * n);
	COORD positMsg;
	char msg[16] = "\0";
	if (user1Scores > user2Scores)
		strcpy_s(msg, sizeof(msg), user1);
	else if (user1Scores < user2Scores)
		strcpy_s(msg, sizeof(msg), user2);
	else if (user1Scores == user2Scores)
		strcpy_s(msg, sizeof(msg), "¤аг¦Ў ");
	else
	{
		char msg[] = "€Ја  ®Є®­зҐ­ , ­® зв®-в® Ї®и«® ­Ґ в Є. ЋЎа вЁвҐбм Є а §а Ў®взЁЄг! 31neol@gmail.com";
		positMsg.X = (xmax - strlen(msg)) / 2;
		positMsg.Y = 5;
		SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
		system("cls");
		SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
		SetConsoleCursorPosition(handleOutput, positMsg);
		printf("%s\n", msg);
		system("Pause");
		return;
	}
	positMsg.X = (xmax - 35) / 2;
	positMsg.Y = 5;
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	SetConsoleCursorPosition(handleOutput, positMsg);
	printf("€Ја  ®Є®­зҐ­ ! Џ®ЎҐ¤ЁвҐ«м - %s", msg);
	Sleep(2000);
	return;
}

void ChooseDifficulty(HWND handle, HDC devCont, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo, int mode)
//ЇаҐ¤®бв ў«пҐв Ї®«м§®ў вҐ«о Їа ў® ўлЎ®а  б«®¦­®бвЁ ЁЈал, ЇҐаҐ¤ св ўлЎ®а ў дг­ЄжЁо ЁЈал
{
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));

	char title[] = "‚лЎҐаЁвҐ б«®¦­®бвм:";
	int xmax, ymax;
	xmax = scrBuffInfo->dwSize.X;
	ymax = scrBuffInfo->dwSize.Y;
	char choice[4][8] = { "5x5", "7x7", "9x9", "11x11" };

	COORD positTitle, positChoice[5];
	positTitle.X = (xmax - strlen(title)) / 2;
	positTitle.Y = 5;
	for (int i = 0; i < 4; i++)
	{
		positChoice[i].X = (xmax - strlen(choice[i])) / 2;
		positChoice[i].Y = 7 + i;
	}
	SetConsoleCursorPosition(handleOutput, positTitle);
	puts(title);
	for (int i = 0; i < 4; i++)
	{
		SetConsoleCursorPosition(handleOutput, positChoice[i]);
		puts(choice[i]);
	}
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
	int item = 0;
	SetConsoleCursorPosition(handleOutput, positChoice[item]);
	puts(choice[item]);
	unsigned char ch;
	do
	{
		ch = _getch();
		if (ch == 224)
		{
			ch = _getch();
			switch (ch)
			{
			case 72:
				SetConsoleCursorPosition(handleOutput, positChoice[item]);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				puts(choice[item]);
				item--;
				if (item < 0) item = 3;
				SetConsoleCursorPosition(handleOutput, positChoice[item]);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
				puts(choice[item]);
				break;
			case 80:
				SetConsoleCursorPosition(handleOutput, positChoice[item]);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
				puts(choice[item]);
				item++;
				if (item > 3) item = 0;
				SetConsoleCursorPosition(handleOutput, positChoice[item]);
				SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
				puts(choice[item]);
				break;
			}
		}
	} while (ch != 13);
	int n;
	switch (item)
	{
	case 0: n = 5; break;
	case 1: n = 7; break;
	case 2: n = 9; break;
	case 3: n = 11; break;
	}
	if (mode)
		GameWithComp(n, handle, devCont, handleOutput, scrBuffInfo, winInfo);
	else
		TwoUsersGame(n, handle, handleOutput, scrBuffInfo, winInfo);
}

void FillDict(HWND handle, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo)//§ Ї®«­Ґ­ЁҐ Ї®«м§®ў вҐ«мбЄ®Ј® б«®ў ап
{
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));

	fopen_s(&UDB, UDBfile, "rb+");
	if (UDB)
	{
		char word[16] = "\0", buff = '\0';
		system("cls");
		do
		{
			printf("‚ўҐ¤ЁвҐ б«®ў®, Є®в®а®Ґ е®вЁвҐ ¤®Ў ўЁвм ў б«®ў ам (­Ґ Ў®«ҐҐ 15 ЎгЄў): ");
			gets_s(word, 16);
			if (uprRU(word) && strlen(word) >= 2)
			{
				fwrite(word, sizeof(word), 1, UDB);
				fflush(UDB);
			}
			else printf("‚л ўўҐ«Ё ­ҐЄ®ааҐЄв­®Ґ б«®ў®. Џ®ўв®аЁвҐ Ї®ЇлвЄг.\n");
			printf("Џа®¤®«¦Ёвм ўў®¤? \"¤\" - ¤ , \"­\" - ­Ґв\n");
			if (_kbhit()) _getch();
			buff = getchar();
			getchar();
		} while (buff == '„' || buff == '¤');
		fclose(UDB);
	}
	else
	{
		char fail[] = "‡ Єа®©вҐ ЇаЁ«®¦Ґ­ЁҐ Ё ЇҐаҐ¬ҐбвЁвҐ д ©« USERdatabase.dat ў Є®а­Ґўго Ї ЇЄг!";
		int xmax, ymax;

		COORD positFail;
		xmax = scrBuffInfo->dwSize.X;
		ymax = scrBuffInfo->dwSize.Y;
		positFail.X = (xmax - strlen(fail)) / 2;
		positFail.Y = 5;
		SetConsoleCursorPosition(handleOutput, positFail);
		puts(fail);

		return;
	}
}

void ClearDict(HWND handle, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo)//®зЁбвЄ  Ї®«м§®ў вҐ«мбЄ®Ј® б«®ў ап
{
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));

	char warn[] = "‚л гўҐаҐ­л? „Ґ©бвўЁҐ Ќ…‚Ћ‡ЊЋ†ЌЋ Ћ’Њ…Ќ€’њ! Enter - „Ђ";
	int xmax, ymax;

	COORD positWarn;
	xmax = scrBuffInfo->dwSize.X;
	ymax = scrBuffInfo->dwSize.Y;
	positWarn.X = (xmax - strlen(warn)) / 2;
	positWarn.Y = 5;
	SetConsoleCursorPosition(handleOutput, positWarn);
	puts(warn);

	unsigned char ch;
	ch = _getch();
	if (ch == 13)
	{
		fopen_s(&UDB, UDBfile, "wb+");
		fclose(UDB);
		char msg[] = "‘«®ў ам гбЇҐи­® ®зЁйҐ­!";
		COORD positMsg;
		positMsg.X = (xmax - strlen(msg)) / 2;
		positMsg.Y = 7;
		SetConsoleCursorPosition(handleOutput, positMsg);
		puts(msg);
		Sleep(500);
	}
	return;
}

void Manual(HWND handle, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo)
{
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	printf("\n\n€Ја  \"Ѓ «¤ \" Їа®ў®¤Ёвбп Ї® ®ЎйҐЇаЁ­пвл¬ Їа ўЁ« ¬. „«п ЇҐаҐ¬ҐйҐ­Ёп Ї® ¬Ґ­о ЁбЇ®«м§говбп бваҐ«ЄЁ,   Є« ўЁи \nEnter Ї®¤вўҐа¦¤ Ґв ®ЇҐа жЁо.\n");
	printf("—в®Ўл б®ўҐаиЁвм е®¤, ­г¦­® б®Ўа вм б«®ў® Ї® Є ¦¤®© ЎгЄўҐ, ЇҐаҐ¬Ґй пбм Ё ­ ¦Ё¬ п Є« ўЁиг Enter. ЏаЁ\n­Ґ®Ўе®¤Ё¬®бвЁ ўЇЁб вм");
	printf(" ЎгЄўг: Ї®б«Ґ ­ ¦ вЁп Є« ўЁиЁ\nEnter бЁ¬ў®« ўў®¤Ёвбп б Ї®¬®ймо Є« ўЁ вгал.");
	printf(" ’ Є¦Ґ Ґбвм ў®§¬®¦­®бвм ¤®Ў ў«пвм б«®ў  ў б«®ў ам\n(USERdatabase.dat) Ё ®зЁй вм ҐЈ®, ­Ґ § ва ЈЁў п ®б­®ў­®© б«®ў ам\nЇа®Ја ¬¬л.\n");
	unsigned char ch;
	printf("\n\nЌ ¦¬ЁвҐ Enter ¤«п ўле®¤  ў ¬Ґ­о\n");
	do
	{
		ch = _getch();
	} while (ch != 13);
}

void About(HWND handle, HANDLE handleOutput, PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo, PWINDOWINFO winInfo)
{
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
	system("cls");
	SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
	char str1[] = "€Ја  \"ЃЂ‹„Ђ\" а §а Ў®в ­  Є Є Єгаб®ў п а Ў®в ", str2[] = "бвг¤Ґ­в®¬ €ѓќ“ ЌҐўҐ©ЄЁ­л¬ Ћ.ѓ.", str3[] = "All rights reserved. 2021";
	COORD positStr1, positStr2, positStr3;
	int xmax, ymax;
	xmax = scrBuffInfo->dwSize.X;
	ymax = scrBuffInfo->dwSize.Y;
	positStr1.X = (xmax - strlen(str1)) / 2;
	positStr1.Y = 3;
	positStr2.X = (xmax - strlen(str2)) / 2;
	positStr2.Y = 4;
	positStr3.X = (xmax - strlen(str3)) / 2;
	positStr3.Y = 7;
	SetConsoleCursorPosition(handleOutput, positStr1);
	puts(str1);
	SetConsoleCursorPosition(handleOutput, positStr2);
	puts(str2);
	SetConsoleCursorPosition(handleOutput, positStr3);
	puts(str3);
	unsigned char ch;
	char msg[] = "Ќ ¦¬ЁвҐ Enter ¤«п ўле®¤  ў ¬Ґ­о";
	COORD positMsg;
	positMsg.X = (xmax - strlen(msg)) / 2;
	positMsg.Y = 12;
	SetConsoleCursorPosition(handleOutput, positMsg);
	puts(msg);
	do
	{
		ch = _getch();
	} while (ch != 13);
}

int main()
{
	HWND handle = GetConsoleWindow();
	HDC devCont = GetDC(handle);
	HANDLE handleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	PCONSOLE_CURSOR_INFO structCursorInfo = new CONSOLE_CURSOR_INFO;
	GetConsoleCursorInfo(handleOutput, structCursorInfo);
	structCursorInfo->bVisible = FALSE;
	SetConsoleCursorInfo(handleOutput, structCursorInfo);


	RECT r;
	GetWindowRect(handle, &r); //stores the console's current dimensions
	MoveWindow(handle, r.left, r.top, 1340, 755, TRUE);
	CONSOLE_FONT_INFO conFontInfo;
	GetCurrentConsoleFont(handleOutput, false, &conFontInfo);
	COORD fontSize = GetConsoleFontSize(handleOutput, conFontInfo.nFont);
	PCONSOLE_FONT_INFOEX conFontInfoEX = new CONSOLE_FONT_INFOEX;

	conFontInfoEX->dwFontSize.X = 14;
	conFontInfoEX->dwFontSize.Y = 30;
	conFontInfoEX->nFont = 11;
	conFontInfoEX->cbSize = sizeof(CONSOLE_FONT_INFOEX);
	conFontInfoEX->FontWeight = 400;
	lstrcpyW(conFontInfoEX->FaceName, L"Lucida Console");
	conFontInfoEX->FontFamily = FF_DONTCARE;

	bool b = SetCurrentConsoleFontEx(handle, false, conFontInfoEX);
	fontSize = GetConsoleFontSize(handleOutput, conFontInfo.nFont);
	int const count = 7;
	char title[] = "ЃЂ‹„Ђ";
	int item = 0;
	COORD positNames[count];
	char mn[count][25] = { "€Ја вм б Є®¬ЇмовҐа®¬", "€Ја вм ў¤ў®с¬", "‡ Ї®«­Ёвм б«®ў ам", "ЋзЁбвЁвм б«®ў ам", "‘Їа ўЄ ", "Ћ Їа®Ја ¬¬Ґ", "‚ле®¤" };
	if (!LoadDict())
	{
		int xmax, ymax;
		PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo = new CONSOLE_SCREEN_BUFFER_INFO;
		PWINDOWINFO winInfo = new WINDOWINFO;
		GetConsoleScreenBufferInfo(handleOutput, scrBuffInfo);
		GetWindowInfo(handle, winInfo);
		xmax = scrBuffInfo->dwSize.X;
		ymax = scrBuffInfo->dwSize.Y;
		char msg[] = "‡ Єа®©вҐ ЇаЁ«®¦Ґ­ЁҐ Ё ЇҐаҐ¬ҐбвЁвҐ д ©«л database.dat Ё USERdatabase.dat ў Є®а­Ґўго Ї ЇЄг!";
		COORD positMsg;
		positMsg.X = (xmax - strlen(msg)) / 2;
		positMsg.Y = 5;
		SetConsoleCursorPosition(handleOutput, positMsg);
		puts(msg);
		Sleep(2500);
		return 0;
	}
	do
	{
		int xmax, ymax;
		PCONSOLE_SCREEN_BUFFER_INFO scrBuffInfo = new CONSOLE_SCREEN_BUFFER_INFO;
		PWINDOWINFO winInfo = new WINDOWINFO;
		GetConsoleScreenBufferInfo(handleOutput, scrBuffInfo);
		GetWindowInfo(handle, winInfo);
		xmax = scrBuffInfo->dwSize.X;
		ymax = scrBuffInfo->dwSize.Y;
		for (int i = 0; i < count; i++)
		{
			positNames[i].X = (xmax - strlen(mn[i])) / 2;
			positNames[i].Y = 8 + i;
		}
		SetConsoleTextAttribute(handleOutput, (WORD)(White << 4));
		system("cls");
		SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
		COORD positTitle;
		positTitle.X = (xmax - strlen(title)) / 2;
		positTitle.Y = 5;
		SetConsoleCursorPosition(handleOutput, positTitle);
		puts(title);
		for (int i = 0; i < count; i++)
		{
			SetConsoleCursorPosition(handleOutput, positNames[i]);
			puts(mn[i]);
		}
		SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
		SetConsoleCursorPosition(handleOutput, positNames[item]);
		puts(mn[item]);
		unsigned char ch;
		PCONSOLE_CURSOR_INFO structCursorInfo = new CONSOLE_CURSOR_INFO;
		GetConsoleCursorInfo(handleOutput, structCursorInfo);
		structCursorInfo->bVisible = FALSE;
		SetConsoleCursorInfo(handleOutput, structCursorInfo);
		do
		{
			while (used)
			{
				ListWords* curr = used;
				used = used->next;
				delete curr;
			}
			ch = _getch();
			if (ch == 224)
			{
				ch = _getch();
				switch (ch)
				{
				case 72:
					SetConsoleCursorPosition(handleOutput, positNames[item]);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					puts(mn[item]);
					item--;
					if (item < 0) item = 6;
					SetConsoleCursorPosition(handleOutput, positNames[item]);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
					puts(mn[item]);
					break;
				case 80:
					SetConsoleCursorPosition(handleOutput, positNames[item]);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightBlue));
					puts(mn[item]);
					item++;
					if (item > 6) item = 0;
					SetConsoleCursorPosition(handleOutput, positNames[item]);
					SetConsoleTextAttribute(handleOutput, (WORD)((White << 4) | LightCyan));
					puts(mn[item]);
					break;
				}
			}
		} while (ch != 13);
		switch (item)
		{
		case 0: ChooseDifficulty(handle, devCont, handleOutput, scrBuffInfo, winInfo, 1); break;
		case 1: ChooseDifficulty(handle, devCont, handleOutput, scrBuffInfo, winInfo, 0); break;
		case 2: FillDict(handle, handleOutput, scrBuffInfo, winInfo); break;
		case 3: ClearDict(handle, handleOutput, scrBuffInfo, winInfo); break;
		case 4: Manual(handle, handleOutput, scrBuffInfo, winInfo); break;
		case 5: About(handle, handleOutput, scrBuffInfo, winInfo); break;
		}
	} while (item != 6);
	return 0;
}