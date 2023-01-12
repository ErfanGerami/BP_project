#define  _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <math.h>

#include "colorize.h"
#include "helper_windows.h"
#include "music.h"
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <windowsx.h>
#include <mmsystem.h>



#pragma region consts
const int WHITE = 15;
const int MAX = 15;
const int YELLOW = 6;
const int RED = 20;
const int GREEN = 2;
const int BLUE = 3;
const int ORDSCORE = 1;
const int LONGSCORE = 2;
const int HARDSCORE = 2;
const int GAMEWIDTH=26;
const int MInSHOWNhEIGHT=1;
#pragma endregion

#pragma region structs, unions,Enums

typedef enum State { Main, Register, SignIn,InGame } State;
typedef enum WordKind { OrdinaryWord, LongWord, HardWord, UnClearWord } WordKind;

typedef struct Entry {
	const char* text;
	char input[20];
	int Pos[2];
	int Cnt;
	int IsSelected;
	int max;


}Entry;

typedef struct Option {
	const char* text;
	int Pos[2];
	int IsSelected;




}Option;

typedef union Thing
{
	Option option;
	Entry entry;



}Thing;

typedef struct OptionOrEntry {
	int OptionOrEntry;
	Thing thing;



}OptionOrEntry;
typedef struct Game {
	int BeenPlayed;
	int score;
	int hardness;
	int LastWave;
}Game;
typedef struct Plyer {
	char name[20];
	char LastName[20];
	char NickName[20];
	char password[20];
	Game PrevGames[3];
	int age;
}Player;

typedef struct Word {
	char word[25];
	WordKind kind;
	Word* next;
	int height;
	int IsItUnClear;
	int UnCLearLenght;
}Word;


#pragma endregion

#pragma region public variables
State CurrentState;
OptionOrEntry* AllInMenu;
int CurserPos[2];
int Busy;
int Selected;
int Color;
int OptionCnt;
HANDLE BlinkThread;
HANDLE MusicPlayer;
Player player;

Word* Heads;
int NumberOfHeads;


int WhereInOrd;
int WhereInHard;
int WherInLong;
int wave;
Word** UnClears;
int UnCLearCnt;
HANDLE UnClearChangingSizeThread;
Word* SelectedWord;
#pragma endregion

#pragma region funciton prototypes
void StartWave();
void GameMenu();
void PrintGameMenu();
void my_callback_on_key_arrival(char c);
void PrintMainMenu();
void PrintRegisterMenu();
void PrintOptionOrEntry(OptionOrEntry opORen, int color);
void Log(const char* text, int color);
void PrintSquare(int width, int  height, int StartX, int StartY, int Color);
void DeleteSquare(int width, int  height, int StartX, int StartY);
void ChangeOption(int to);
void ClearTerminal();
DWORD WINAPI Colorize();
DWORD WINAPI Music();
void Fill(char c);
void RegisterEval();
void FillFiles();
//Menu Functions-------------
void MainMenu();
void RegisterMenu();
void StatusMenu();
void HashPass(char pass[20]);
void welcome();
void SignInMenu();
void SignInEval();
WordKind GenerateUnclear(char word[25]);
void CreatNewLinkedList(int ord, int Long, int hard, int Unclear,int HeadHeight);
void PrintFace(int happy);
void PrintLinkedList(Word* head, int HeadHeight,int color,int n);
void shuffle(Word** head);
void GetDown();
void RestGameMenu();
#pragma endregion



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int main(void) {
	//initilizinfg public varaibkes-------------
	Color = WHITE;
	UnCLearCnt = 0;
	wave = 0;
	NumberOfHeads = 0;
	WhereInOrd=0;
	WhereInHard=0;
	WherInLong=0;
	//------------------------------------------

	srand(time(NULL));

	FillFiles();
	
	//Remember To Uncomment;

	//MusicPlayer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Music, NULL, 0, NULL);
	//welcome();

	Busy = 0;
	CurserPos[0] = 0;
	CurserPos[1] = 0;

	system("cls");
	//initializing////////
	

	//MainMenu();
	GameMenu();


	HANDLE BlinkThread= CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Colorize, NULL, 0, NULL);
	HANDLE thread_id = start_listening(my_callback_on_key_arrival);

	WaitForSingleObject(thread_id, INFINITE);

	return 0;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------



#pragma region thread functions
DWORD WINAPI UnClearSizeChange() {
	while (1) {
		Sleep(800);
		while (Busy);
		Busy = 1;
		for (int i = 0; i < UnCLearCnt; i++) {
			if (UnClears[i]->height >= MInSHOWNhEIGHT) {
				setcolor(rand() % 10 + 1);
				gotoxy((GAMEWIDTH - UnClears[i]->UnCLearLenght) / 2 + 2, UnClears[i]->height);
				printf("             ");
				if (UnClears[i]->UnCLearLenght > 15) {
					printf("      ");
				}
				UnClears[i]->UnCLearLenght = (UnClears[i]->UnCLearLenght + 2) % 20;
				gotoxy((GAMEWIDTH - UnClears[i]->UnCLearLenght) / 2 + 2 , UnClears[i]->height);
				for (int j = 0; j < UnClears[i]->UnCLearLenght; j++) {
					printf("*");
				}

				
				

				
			}

		}
		setcolor(Color);
		Busy = 0;
	}
}
DWORD WINAPI Music() {
	while (1) {
		Play();
	}
}
DWORD WINAPI Colorize() {
	int prevSelected = Selected;
	Option CurrentOption;
	while (1) {

		Sleep(400);
		if (CurrentState!=InGame) {
			while (Busy);
			Busy = 1;



			if (AllInMenu[Selected].OptionOrEntry == 0) {
				CurrentOption = AllInMenu[Selected].thing.option;
				gotoxy(CurrentOption.Pos[0], CurrentOption.Pos[1]);
				setcolor(GREEN);
				printf(CurrentOption.text);

				setcolor(Color);
			}
			Busy = 0;
			Sleep(400);
			Busy = 1;

			if (AllInMenu[Selected].OptionOrEntry == 0) {
				CurrentOption = AllInMenu[Selected].thing.option;
				gotoxy(CurrentOption.Pos[0], CurrentOption.Pos[1]);
				setcolor(BLUE);
				printf(CurrentOption.text);

				setcolor(Color);
			}
			Busy = 0;
		}
	}
}
void my_callback_on_key_arrival(char c) {
	while (Busy);
	Busy = 1;
	if (c == 'a') {
		PrintFace(time(NULL) % 2);

	}
	
	if (CurrentState != InGame) {
		if (c == 17) {
			TerminateThread(MusicPlayer, 0);
		}
		else if (c == 72) {
			ChangeOption(-1);
		}
		else if (c == 80) {
			ChangeOption(1);
		}

		else if (c == 13) {
			if (AllInMenu[Selected].OptionOrEntry == 1) {
				ChangeOption(1);
			}
			else {
				if (CurrentState == Main) {
					if (AllInMenu[Selected].thing.option.text[0] == 'R') {
						RegisterMenu();
					}
					else if (AllInMenu[Selected].thing.option.text[0] == 'E') {
						system("cls");
						exit(0);
					}
					else if (AllInMenu[Selected].thing.option.text[0] == 'S') {
						SignInMenu();
					}
				}
				else if (CurrentState == Register) {
					if (AllInMenu[Selected].thing.option.text[0] == 'B') {
						MainMenu();
					}
					else if (AllInMenu[Selected].thing.option.text[0] == 'S') {
						RegisterEval();
					}
				}
				else if (CurrentState == SignIn) {
					if (AllInMenu[Selected].thing.option.text[0] == 'B') {
						MainMenu();
					}
					else if (AllInMenu[Selected].thing.option.text[0] == 'S') {
						SignInEval();
					}
				}
			}
		}
		else if (AllInMenu[Selected].OptionOrEntry == 1 && c != -32) {
			if (c == 32) {
				Log("No Spaces", RED);
				return;
			}
			Fill(c);

		}
	}
	Busy = 0;
}
void ClearTerminal() {
	gotoxy(0, 0);
	//system("cls");
	for (int i = 1; i < 50; i++) {
		for (int i = 1; i < 40; i++) {
			setcolor(rand() % 20);
			printf("---");

		}
		printf("\n");
	}
	gotoxy(CurserPos[0], CurserPos[1]);

}
#pragma endregion

#pragma region register function

void RegisterEval() {
	char name[42];
	char LastName[20];
	char NickName[20];
	char password[20];
	char FileName[45];

	FileName[0] = '\0';

	strcpy(name, AllInMenu[0].thing.entry.input);
	strcpy(LastName, AllInMenu[1].thing.entry.input);
	strcpy(NickName, AllInMenu[2].thing.entry.input);
	int age = atoi(AllInMenu[3].thing.entry.input);
	strcpy(password, AllInMenu[4].thing.entry.input);
	strcat(FileName, "Users/");
	strcat(FileName, name);
	strcat(FileName, " ");
	strcat(FileName, LastName);
	strcat(FileName, ".bin");
	if (name[0] == '\0' || password[0] == '\0' || LastName[0] == '\0' || NickName[0] == '\0') {
		Log("Only Age Field Can Be Empty\a", RED);
		return;
	}



	FILE* file = fopen(FileName, "rb");
	if (file != NULL) {
		Log("You've Registered before\a", RED);
		return;
	}


	strcpy(player.name, name);
	strcpy(player.LastName, LastName);
	strcpy(player.NickName, NickName);
	strcpy(player.password, password);
	player.PrevGames[0].BeenPlayed = 0;
	player.PrevGames[1].BeenPlayed = 0;
	player.PrevGames[2].BeenPlayed = 0;
	player.age = age;

	HashPass(player.password);
	file = fopen(FileName, "wb");
	printf(player.password);
	fwrite(&player, sizeof(Player), 1, file);
	fclose(file);
	StatusMenu();



}
void SignInEval() {
	char name[42];
	char LastName[20];
	char password[20];
	char FileName[45];

	FileName[0] = '\0';

	strcpy(name, AllInMenu[0].thing.entry.input);
	strcpy(LastName, AllInMenu[1].thing.entry.input);
	strcpy(password, AllInMenu[2].thing.entry.input);
	HashPass(password);
	strcat(FileName, "Users/");
	strcat(FileName, name);
	strcat(FileName, " ");
	strcat(FileName, LastName);
	strcat(FileName, ".bin");
	FILE* file = fopen(FileName, "rb");
	if (file == NULL) {
		Log("No Such User(Or Access Denied)\a", RED);
		return;
	}

	fread(&player, sizeof(Player), 1, file);

	if (strcmp(player.password, password) != 0) {
		Log("Password Is Not Correct\a", RED);
	}
	StatusMenu();
}

#pragma endregion

#pragma region printing functions

void DeleteSquare(int width, int  height, int StartX, int StartY) {

	gotoxy(StartX, StartY);
	printf(" ");
	for (int i = 0; i < width - 2; i++) {
		printf(" ");
	}
	printf(" ");
	for (int i = 1; i <= height - 2; i++) {
		gotoxy(StartX, StartY + i);
		printf(" ");
		gotoxy(StartX + width - 1, StartY + i);
		printf(" ");
	}
	gotoxy(StartX, StartY + height - 1);
	printf(" ");
	for (int i = 0; i < width - 2; i++) {
		printf(" ");
	}
	printf(" ");
}
void PrintSquare(int width, int  height, int StartX, int StartY, int Color) {

	gotoxy(StartX, StartY);
	setcolor(Color);
	printf("|");
	for (int i = 0; i < width - 2; i++) {
		printf("-");
	}
	printf("|");
	for (int i = 1; i <= height - 2; i++) {
		gotoxy(StartX, StartY + i);
		printf("|");
		gotoxy(StartX + width - 1, StartY + i);
		printf("|");
	}
	gotoxy(StartX, StartY + height - 1);
	printf("|");
	for (int i = 0; i < width - 2; i++) {
		printf("-");
	}
	printf("|");

	setcolor(Color);


}
void Log(const char* text, int color) {
	gotoxy(7, 2);
	setcolor(color);
	printf(text);
	gotoxy(CurserPos[0], CurserPos[1]);
	setcolor(Color);
}
void PrintOptionOrEntry(OptionOrEntry opORen,int color) {
	setcolor(color);
	if (opORen.OptionOrEntry == 0) {
		gotoxy(opORen.thing.option.Pos[0], opORen.thing.option.Pos[1]);
		printf(opORen.thing.option.text);
	}
	else {
		gotoxy(opORen.thing.entry.Pos[0], opORen.thing.entry.Pos[1]);
		printf(opORen.thing.entry.text);
		setcolor(BLUE);
		printf(">>> ");

	}
	gotoxy(CurserPos[0], CurserPos[1]);
	setcolor(Color);
}
void welcome() {
	int cnt = 4;
	setcolor(BLUE);
	while (cnt--) {


		printf("  ###    ##  ###      ####    ##         #####     ####    ###   ###  ####       ####      \n");
		printf(" #####   ##   ###    ######   ##        ######    ######  ##### ##### ####      #####      \n");
		printf(" #  ###  ###  ###   ### ###   ##       ##   ##   ##   ### #  #### ##### ##     ##  ###    \n");
		printf(" #   ##  ###  ##    ##  ###   ##       ##        ##   ###    #### ####  ##    ###  ##    \n");
		printf("     ##  ###  ##    ##  ##    ##      ###       ###    ##    ###  ####  ##    ### ###    \n");
		printf("     ##########    ######     ##      ###       ###    ##    ###  ###   ##    ######      \n");
		printf("     ####  ####     ###       ##      ###       ###    ##    ##   ###   ##    ###         \n");
		printf("      ###  ###      ###    #  ##  #   ###    ##  ##   ###    ##   ###   ##  # ###    ##    \n");
		printf("      ###  ###      ########  ## ##    #######   ###  ##     ##   ###   #####  ### ###     \n");
		printf("      ##   ##        #####    ####      #####     #####      ##   ###   ####    #####  \n");
		//------------------------------
		Sleep(300); gotoxy(0, 0); system("cls"); setcolor(GREEN);



		printf("                      ##\n");
		printf(" ###   #   ##   ####  ##      ####    ####  ### ### ####    ####\n");
		printf(" # ##  ##  ##  ## ##  ##     ##  #   ##  #### # # ### ##   ## ##\n");
		printf("    #  ##  #   ## ##  ##     ##      #   ###  ##  ### ##   ## ##\n");
		printf("    ########   ####   ##     ##     ##   ##   ##  ##  ##   ####\n");
		printf("    ### ###    ##     ##  #  ##     ##   ##   ##  ##  ##   ##\n");
		printf("    ### ###    ##   # ## ##  ##   #  ##  #    ##  ##  ## # ##   #\n");
		printf("     #   #      ####   ###    ####   #####    ##  ##  ###   #### \n");
		//------------------------------
					//------------------------------
		Sleep(300); gotoxy(0, 0); system("cls"); setcolor(BLUE);
	}
	setcolor(GREEN);

	printf("_________________________\n");
	printf("|"); setcolor(BLUE); printf("######################"); setcolor(GREEN); printf("/\n");
	printf("|"); setcolor(BLUE); printf("####################"); setcolor(GREEN); printf("//\n");
	printf("|"); setcolor(BLUE); printf("###################"); setcolor(GREEN); printf("//\n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("| --------------\n");

	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|\n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|\n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|\n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|"); setcolor(GREEN); printf("---------------     _\n");

	printf("|"); setcolor(BLUE); printf("####"); setcolor(YELLOW); printf("###############"); setcolor(GREEN); printf("\/    /"); setcolor(BLUE); printf("#"); setcolor(GREEN); printf("|\n");

	printf("|"); setcolor(BLUE); printf("####"); setcolor(YELLOW); printf("#############"); setcolor(GREEN); printf("|     /"); setcolor(BLUE); printf("##"); setcolor(GREEN); printf("|\n");


	printf("|"); setcolor(BLUE); printf("####"); setcolor(YELLOW); printf("###############"); setcolor(GREEN); printf("\\  /"); setcolor(BLUE); printf("###"); setcolor(GREEN); printf("|\n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|"); setcolor(GREEN); printf("--------------- "); setcolor(GREEN); printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("| \n");

	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|"); setcolor(BLUE); printf("                "); setcolor(GREEN); printf("|");  setcolor(BLUE); printf("####"); setcolor(GREEN); printf("| \n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|"); setcolor(BLUE); printf("                "); setcolor(GREEN); printf("|");  setcolor(BLUE); printf("####"); setcolor(GREEN); printf("| \n");
	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|"); setcolor(BLUE); printf("                "); setcolor(GREEN); printf("|");  setcolor(BLUE); printf("####"); setcolor(GREEN); printf("| \n");

	printf("|"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|"); setcolor(GREEN); printf("_______________ |"); setcolor(BLUE); printf("####"); setcolor(GREEN); printf("|\n");

	printf("|"); setcolor(BLUE); printf("##########################"); setcolor(GREEN); printf("| \n");
	printf("|"); setcolor(BLUE); printf("##########################"); setcolor(GREEN); printf("| \n");
	printf("|"); setcolor(BLUE); printf("##########################"); setcolor(GREEN); printf("| \n");
	printf("----------------------------\n");



	printf("\n welcome to my project! check out the code in my github :");
	setcolor(GREEN);
	printf("https://github.com/ErfanGerami");
	setcolor(WHITE);
	printf(" \n\n \n___________________________________PRESS ENTER KEY TO CONTINUE____________________________________");
	while (getchar() != 10);

}
void PrintFace(int happy) {
	int start = 9;
	if (happy == 0) {
		setcolor(RED);
		gotoxy(31, start);
		printf("----------$$$$$$---------------$$$$$$-------\n"); gotoxy(31, start);
		printf("---------$$$$$$$$-------------$$$$$$$$------\n"); gotoxy(31, start+1);
		printf("--------$$$$$$$$$$-----------$$$$$$$$$$-----\n"); gotoxy(31, start + 2);
		printf("---------$$$$$$$$-------------$$$$$$$$------\n"); gotoxy(31, start + 3);
		printf("----------$$$$$$---------------$$$$$$-------\n"); gotoxy(31, start + 4);
		printf("--------------------#######-----------------\n"); gotoxy(31, start + 5);
		printf("----------------################------------\n"); gotoxy(31, start + 6);
		printf("----------- ######################----------\n"); gotoxy(31, start + 7);
		printf("---------- ###########--############--------\n"); gotoxy(31, start + 8);
		printf("----------########---------##########-------\n"); gotoxy(31, start + 9);
		printf("---------#####------------------######------\n"); gotoxy(31, start + 10);
		printf("--------###-------------------------####----\n"); gotoxy(31, start + 11);
		printf("--------#------------------------------#----\n"); gotoxy(31, start + 12);
	}
	else {
		setcolor(GREEN);
		gotoxy(31, start);
		printf("----------$$$$$$---------------$$$$$$-------\n"); gotoxy(31, start);
		printf("---------$$$$$$$$-------------$$$$$$$$------\n"); gotoxy(31, start + 1);
		printf("--------$$$$$$$$$$-----------$$$$$$$$$$-----\n"); gotoxy(31, start + 2);
		printf("---------$$$$$$$$-------------$$$$$$$$------\n"); gotoxy(31, start + 3);
		printf("----------$$$$$$---------------$$$$$$-------\n"); gotoxy(31, start + 4);
		printf("--------#------------------------------#----\n"); gotoxy(31, start + 5);
		printf("--------###-------------------------####----\n"); gotoxy(31, start + 6);
		printf("---------#####------------------######------\n"); gotoxy(31, start + 7);
		printf("----------########---------##########-------\n"); gotoxy(31, start + 8);
		printf("---------- ###########--############--------\n"); gotoxy(31, start + 9);
		printf("----------- ######################----------\n"); gotoxy(31, start + 10);
		printf("----------------################------------\n"); gotoxy(31, start + 11);
		printf("---------------------######-----------------\n"); gotoxy(31, start + 12);


	}
	setcolor(Color);
	gotoxy(CurserPos[0], CurserPos[1]);
}

void PrintLinkedList(Word* head,int HeadHeight,int color,int n) {
	Word* temp = head;
	setcolor(color);
	
	while (n--) {
		//if (temp->height < MInSHOWNhEIGHT)
			//break;


		if (temp->IsItUnClear == 0) {
			gotoxy((GAMEWIDTH - strlen(temp->word)) / 2 + 2, HeadHeight);
			printf(temp->word);
		}
		else {
			gotoxy((GAMEWIDTH - temp->UnCLearLenght) / 2 +2, HeadHeight);
			for (int i = 0; i < temp->UnCLearLenght; i++) {
				printf("*");
			}
			int t=3;
		}
		temp = temp->next;
		HeadHeight--;
	}
	setcolor(Color);
	
}
void PrintMainMenu() {

	setcolor(WHITE);
	printf("____________________________________________  \n");
	printf("||   logs:                                ||  \n");
	printf("||                                        ||  \n");
	printf("||- - - - - - - - - - - - - - - - - - - - ||  \n");
	printf("|| - - - - - - - - - - - - - - - - - - - -||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");

	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||Press CTRL+q to stop music              ||  \n");
	printf("--------------------------------------------  \n");
	gotoxy(CurserPos[0], CurserPos[1]);


}
void PrintRegisterMenu() {

	setcolor(WHITE);
	printf("____________________________________________  \n");
	printf("||   logs:                                ||  \n");
	printf("||                                        ||  \n");
	printf("||- - - - - - - - - - - - - - - - - - - - ||  \n");
	printf("|| - - - - - - - - - - - - - - - - - - - -||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("||                                        ||  \n");
	printf("--------------------------------------------  \n");
	gotoxy(CurserPos[0], CurserPos[1]);


}

void PrintGameMenu() {
	//system("cls");
	setcolor(WHITE);
	gotoxy(0, 0);
	
	printf("____________________________________________________________________________\n");
	printf("||                          || User:                                      || \n");
	printf("||                          ||----------------------------------------------\n");
	printf("||                          || Wave:                                      || \n");
	printf("||                          ||----------------------------------------------\n");
	printf("||                          || Score:                                     || \n");
	printf("||                          ||----------------------------------------------\n");
	printf("||                          || logs:                                      ||\n");
	printf("||                          ||----------------------------------------------  \n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n");
	printf("||                          ||                                            ||\n"); setcolor(RED);
	printf("----------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------\n");

	PrintFace(1);



	setcolor(Color);
	gotoxy(CurserPos[0], CurserPos[1]);
}
void RestGameMenu() {
	int n = 21;
	gotoxy(0, 1);
	setcolor(WHITE);
	while (n--) {
		printf("||                          ||\n");
	}
	setcolor(Color);

}


#pragma endregion

#pragma region menu functions
void MainMenu() {
	CurrentState = Main;
	OptionCnt = 3;
	Selected = 0;
	AllInMenu = (OptionOrEntry*)malloc(3 * sizeof(OptionOrEntry));
	AllInMenu[0].OptionOrEntry = 0;
	AllInMenu[0].thing.option.IsSelected = 1;
	AllInMenu[0].thing.option.text = "REGISTER";
	AllInMenu[0].thing.option.Pos[0] = 5; AllInMenu[0];	AllInMenu[0].thing.option.Pos[1] = 6;

	AllInMenu[1].OptionOrEntry = 0;
	AllInMenu[1].thing.option.IsSelected = 0;
	AllInMenu[1].thing.option.text = "SIGN IN";
	AllInMenu[1].thing.option.Pos[0] = 20; AllInMenu[1];	AllInMenu[1].thing.option.Pos[1] = 9;

	AllInMenu[2].OptionOrEntry = 0;
	AllInMenu[2].thing.option.IsSelected = 0;
	AllInMenu[2].thing.option.text = "EXIT";
	AllInMenu[2].thing.option.Pos[0] = 35; AllInMenu[1];	AllInMenu[2].thing.option.Pos[1] = 12;
	Busy = 1;
	gotoxy(0, 0);
	ClearTerminal();
	PrintMainMenu();


	for (int i = 0; i < 3; i++) {
		PrintOptionOrEntry(AllInMenu[i], 3);
	}
	Option CurrentOption = AllInMenu[Selected].thing.option;
	PrintSquare(strlen(CurrentOption.text) + 2, 3, CurrentOption.Pos[0] - 1, CurrentOption.Pos[1] - 1, GREEN);
	Busy = 0;

}
void RegisterMenu() {
	CurrentState = Register;
	OptionCnt = 7;
	Selected = 0;
	AllInMenu = (OptionOrEntry*)realloc(AllInMenu, OptionCnt * sizeof(OptionOrEntry));
	AllInMenu[0].OptionOrEntry = 1;
	AllInMenu[0].thing.entry.IsSelected = 1;
	AllInMenu[0].thing.entry.text = "name";
	AllInMenu[0].thing.entry.Pos[0] = 5; AllInMenu[0];	AllInMenu[0].thing.entry.Pos[1] = 6;
	AllInMenu[0].thing.entry.Cnt = 0;
	AllInMenu[0].thing.entry.input[0] = '\0';
	AllInMenu[0].thing.entry.max = MAX;


	AllInMenu[1].OptionOrEntry = 1;
	AllInMenu[1].thing.entry.IsSelected = 0;
	AllInMenu[1].thing.entry.text = "last name";
	AllInMenu[1].thing.entry.Pos[0] = 5; AllInMenu[1];	AllInMenu[1].thing.entry.Pos[1] = 9;
	AllInMenu[1].thing.entry.Cnt = 0;
	AllInMenu[1].thing.entry.Cnt = 0;
	AllInMenu[1].thing.entry.input[0] = '\0';
	AllInMenu[1].thing.entry.max = MAX;

	AllInMenu[2].OptionOrEntry = 1;
	AllInMenu[2].thing.entry.IsSelected = 0;
	AllInMenu[2].thing.entry.text = "nick name";
	AllInMenu[2].thing.entry.Pos[0] = 5; AllInMenu[1];	AllInMenu[2].thing.entry.Pos[1] = 12;
	AllInMenu[2].thing.entry.Cnt = 0;
	AllInMenu[2].thing.entry.Cnt = 0;
	AllInMenu[2].thing.entry.input[0] = '\0';
	AllInMenu[2].thing.entry.max = MAX;

	AllInMenu[3].OptionOrEntry = 1;
	AllInMenu[3].thing.entry.IsSelected = 0;
	AllInMenu[3].thing.entry.text = "age";
	AllInMenu[3].thing.entry.Pos[0] = 5; AllInMenu[1];	AllInMenu[3].thing.entry.Pos[1] = 15;
	AllInMenu[3].thing.entry.Cnt = 0;
	AllInMenu[3].thing.entry.Cnt = 0;
	AllInMenu[3].thing.entry.input[0] = '\0';
	AllInMenu[3].thing.entry.max = 2;


	AllInMenu[4].OptionOrEntry = 1;
	AllInMenu[4].thing.entry.IsSelected = 0;
	AllInMenu[4].thing.entry.text = "password";
	AllInMenu[4].thing.entry.Pos[0] = 5; AllInMenu[1];	AllInMenu[4].thing.entry.Pos[1] = 18;
	AllInMenu[4].thing.entry.Cnt = 0;
	AllInMenu[4].thing.entry.Cnt = 0;
	AllInMenu[4].thing.entry.input[0] = '\0';
	AllInMenu[4].thing.entry.max = MAX;



	AllInMenu[5].OptionOrEntry = 0;
	AllInMenu[5].thing.option.IsSelected = 0;
	AllInMenu[5].thing.option.text = "SUBMIT";
	AllInMenu[5].thing.option.Pos[0] = 5; AllInMenu[1];	AllInMenu[5].thing.option.Pos[1] = 23;


	AllInMenu[6].OptionOrEntry = 0;
	AllInMenu[6].thing.option.IsSelected = 0;
	AllInMenu[6].thing.option.text = "BACK";
	AllInMenu[6].thing.option.Pos[0] = 5; AllInMenu[1];	AllInMenu[6].thing.option.Pos[1] = 26;

	Busy = 1;
	ClearTerminal();
	PrintRegisterMenu();
	PrintOptionOrEntry(AllInMenu[0], GREEN);
	for (int i = 1; i < OptionCnt; i++) {
		PrintOptionOrEntry(AllInMenu[i], BLUE);
	}





	Busy = 0;
}
void StatusMenu() {}
void SignInMenu() {
	CurrentState = SignIn;
	OptionCnt = 5;
	Selected = 0;
	AllInMenu = (OptionOrEntry*)realloc(AllInMenu, OptionCnt * sizeof(OptionOrEntry));

	AllInMenu[0].OptionOrEntry = 1;
	AllInMenu[0].thing.entry.IsSelected = 1;
	AllInMenu[0].thing.entry.text = "name";
	AllInMenu[0].thing.entry.Pos[0] = 5; AllInMenu[0];	AllInMenu[0].thing.entry.Pos[1] = 6;
	AllInMenu[0].thing.entry.Cnt = 0;
	AllInMenu[0].thing.entry.input[0] = '\0';
	AllInMenu[0].thing.entry.max = MAX;


	AllInMenu[1].OptionOrEntry = 1;
	AllInMenu[1].thing.entry.IsSelected = 0;
	AllInMenu[1].thing.entry.text = "last name";
	AllInMenu[1].thing.entry.Pos[0] = 5; AllInMenu[1];	AllInMenu[1].thing.entry.Pos[1] = 9;
	AllInMenu[1].thing.entry.Cnt = 0;
	AllInMenu[1].thing.entry.Cnt = 0;
	AllInMenu[1].thing.entry.input[0] = '\0';
	AllInMenu[1].thing.entry.max = MAX;


	AllInMenu[2].OptionOrEntry = 1;
	AllInMenu[2].thing.entry.IsSelected = 0;
	AllInMenu[2].thing.entry.text = "password";
	AllInMenu[2].thing.entry.Pos[0] = 5; AllInMenu[1];	AllInMenu[2].thing.entry.Pos[1] = 12;
	AllInMenu[2].thing.entry.Cnt = 0;
	AllInMenu[2].thing.entry.Cnt = 0;
	AllInMenu[2].thing.entry.input[0] = '\0';
	AllInMenu[2].thing.entry.max = MAX;



	AllInMenu[3].OptionOrEntry = 0;
	AllInMenu[3].thing.option.IsSelected = 0;
	AllInMenu[3].thing.option.text = "SUBMIT";
	AllInMenu[3].thing.option.Pos[0] = 5; AllInMenu[1];	AllInMenu[3].thing.option.Pos[1] = 17;

	AllInMenu[4].OptionOrEntry = 0;
	AllInMenu[4].thing.option.IsSelected = 0;
	AllInMenu[4].thing.option.text = "BACK";
	AllInMenu[4].thing.option.Pos[0] = 5; AllInMenu[1];	AllInMenu[4].thing.option.Pos[1] = 20;

	Busy = 1;
	ClearTerminal();
	PrintRegisterMenu();
	PrintOptionOrEntry(AllInMenu[0], GREEN);
	for (int i = 1; i < OptionCnt; i++) {
		PrintOptionOrEntry(AllInMenu[i], BLUE);
	}





	Busy = 0;

}
void GameMenu() {
	CurrentState = InGame;
	PrintGameMenu();
	UnClearChangingSizeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UnClearSizeChange, NULL, 0, NULL);
	StartWave();

}

#pragma endregion

WordKind  GenerateUnclear(char word[25]) {
	//ordinary---------------------------------------------
	char OrdinaryPermited[70];
	char SpecialCharacters[] = { '_','-','@','$','%','^','&','!' };
	const int SpecialCharactersSize=8;

	int OrdCnt = 0;
	for (int i = 48; i <= 57; i++) {
		OrdinaryPermited[OrdCnt] = i;
		OrdCnt++;
	}
	for (int i = 65; i <= 90; i++) {
		OrdinaryPermited[OrdCnt] = i;
		OrdCnt++;
	}
	for (int i = 97; i <= 122; i++) {
		OrdinaryPermited[OrdCnt] = i;
		OrdCnt++;
	}//hard------------------------------
	int randomNum = rand() % 3;
	//Ordinary-------------------------------------------------------

	if (randomNum == 0) {
		int lenght;

		lenght = rand() % 10;
		lenght = min((lenght + 3), 10);
		// because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdinaryPermited[rand() % OrdCnt];
		}
		word[lenght] = '\0';
		return OrdinaryWord;
	}
	//Long -------------------------------------------------------------
	if (randomNum == 1) {
		int lenght;

		lenght = rand() % 20;
		lenght = min((lenght + 3), 20);
		// because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdinaryPermited[rand() % OrdCnt];
		}
		word[lenght] = '\0';
		return LongWord;

	}
	//Hard------------------------------------------------------------------
	if (randomNum == 2) {
		int lenght;
		lenght = rand() % 20;
		lenght = min((lenght + 3), 20);
		// because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdinaryPermited[rand() % OrdCnt];
		}
		//we at least want 1 Special character and we dont want to make it so hard so max 4 characters would be fine
		int HowManySpecialCheraracters = rand() %min(lenght/2,4) +1;
		//it culd overwrite on the previous choice but its okay
		for (int i = 0; i < HowManySpecialCheraracters; i++) {
			word[rand() % lenght] = SpecialCharacters[rand() % SpecialCharactersSize];

		}
		word[lenght] = '\0';
		return HardWord;
	}

}
void CreatNewLinkedList(int ord, int Long, int hard, int Unclear,int HeadHeight) {
	char Chert[25];


	
	Heads = (Word*)realloc(Heads, (NumberOfHeads + 1) * sizeof(Word));
	NumberOfHeads++;
	Word * TempHead = Heads + NumberOfHeads - 1;
	//Opening FIles====================================================================
	FILE * fileOrd = fopen("ord.txt", "r");
	for (int i = 0; i < WhereInOrd; i++) {
		//OrdWords[0] is just for going through the file not that we need it;
		fgets(Chert, 1000, fileOrd);
	}

	FILE* fileLong = fopen("long.txt", "r");
	for (int i = 0; i < WherInLong; i++) {
		//OrdWords[0] is just for going through the file not that we need it;
		fgets(Chert, 1000, fileLong);
	}

	FILE* fileHard = fopen("hard.txt", "r");
	for (int i = 0; i < WhereInHard; i++) {
		//OrdWords[0] is just for going through the file not that we need it;
		fgets(Chert, 1000, fileHard);
	}
	//=====================================================================================



	for (int i = 0; i < ord; i++) {
		fgets(Chert, 1000, fileOrd);
		Chert[strlen(Chert) - 1] = '\0';
		TempHead->kind = OrdinaryWord;
		strcpy(TempHead->word, Chert);
		TempHead->IsItUnClear = 0;
		TempHead->next = (Word*)malloc(sizeof(Word));
		if (i == hard - 1&&Long==0&&hard==0&&Unclear==0) {
			TempHead->next = NULL;
		}
		TempHead = TempHead->next;
	}
	for (int i = 0; i < Long; i++) {
		fgets(Chert, 1000, fileLong);
		Chert[strlen(Chert) - 1] = '\0';
		TempHead->kind = HardWord;
		strcpy(TempHead->word, Chert);
		TempHead->IsItUnClear = 0;
		TempHead->next = (Word*)malloc(sizeof(Word));
		if (i == Long - 1 && hard == 0 && Unclear == 0) {
			TempHead->next = NULL;
		}
		TempHead = TempHead->next;
	}
	for (int i = 0; i < hard; i++) {
		fgets(Chert, 1000, fileHard);
		Chert[strlen(Chert) - 1] = '\0';
		TempHead->kind = LongWord;
		strcpy(TempHead->word, Chert);
		TempHead->IsItUnClear = 0;
		TempHead->next = (Word*)malloc(sizeof(Word));
		if (i == hard - 1 && Unclear == 0) {
			TempHead->next = NULL;
		}
		TempHead = TempHead->next;
	}
	
	UnClears = (Word**)realloc(UnClears, (UnCLearCnt + Unclear) * sizeof(Word*));
	for (int i = 0; i < Unclear; i++) {
		UnClears[UnCLearCnt]= TempHead;
		TempHead->kind = GenerateUnclear(Chert);
		strcpy(TempHead->word, Chert);
		TempHead->UnCLearLenght = rand()%15+4;
		TempHead->IsItUnClear = 1;
		
		TempHead->next = (Word*)malloc(sizeof(Word));
		
		if(i==Unclear-1) {
			TempHead->next = NULL;
		}else {
			TempHead = TempHead->next;
		}
		UnCLearCnt++;
	}
	


	fclose(fileHard);
	fclose(fileOrd);
	fclose(fileLong);
    Heads += NumberOfHeads - 1;
	shuffle( &Heads);
	Heads -= NumberOfHeads - 1;


	TempHead = (Heads + NumberOfHeads - 1);
	while (TempHead != NULL) {
		TempHead->height = HeadHeight;
		TempHead = TempHead->next;
		HeadHeight--;
	}


}
void ChangeOption(int to) {
	if (Selected == 0 && to == -1) {
		Log("There Are No Options Above\a",RED);
	}
	else if (Selected == OptionCnt-1 && to == 1) {
		Log("There Are No Options below\a" ,RED);

	}
	else {
		if (AllInMenu[Selected+to].OptionOrEntry == 0) {
			if (AllInMenu[Selected].OptionOrEntry == 0) {

				Option CurrentOption = AllInMenu[Selected].thing.option;
				DeleteSquare(strlen(CurrentOption.text) + 2, 3, CurrentOption.Pos[0] - 1, CurrentOption.Pos[1] - 1);
			}
			else {
				Entry CurrentOption = AllInMenu[Selected].thing.entry;

			}
			PrintOptionOrEntry(AllInMenu[Selected], BLUE);
			Selected += to;
			Option CurrentOption = AllInMenu[Selected].thing.option;
			PrintSquare(strlen(CurrentOption.text) + 2, 3, CurrentOption.Pos[0] - 1, CurrentOption.Pos[1] - 1,GREEN);
			
		}
		else {
			if (AllInMenu[Selected ].OptionOrEntry == 0) {
				Option CurrentOption = AllInMenu[Selected].thing.option;
				DeleteSquare(strlen(CurrentOption.text) + 2, 3, CurrentOption.Pos[0] - 1, CurrentOption.Pos[1] - 1);
			}
			PrintOptionOrEntry(AllInMenu[Selected], BLUE);
			Selected += to;
			Entry CurrentEntry = AllInMenu[Selected].thing.entry;
			gotoxy(CurrentEntry.Pos[0], CurrentEntry.Pos[1]);
			setcolor(GREEN);
			printf(CurrentEntry.text);
			setcolor(Color);
			gotoxy(CurserPos[0], CurserPos[1]);
			


		}
		
		
	}

}
void Fill(char c) {
	//Busy
	Entry Current = AllInMenu[Selected].thing.entry;
	if (Current.Cnt == Current.max && c!=8) {
		Log("You've Reached Max Lenght\a",RED);
		return;
	}
	else if (Current.Cnt == 0 && c == 8) {
		Log("You Cant Erase what is not there\a", RED);
		return;
	}
	if (c == 8) {
		gotoxy(Current.Pos[0] + strlen(Current.text) + strlen(">>> ") + strlen(Current.input), Current.Pos[1]);

		printf("\b \b");

		AllInMenu[Selected].thing.entry.input[Current.Cnt-1] = '\0';
		AllInMenu[Selected].thing.entry.Cnt--;
		return;

	}
	gotoxy(Current.Pos[0] + strlen(Current.text) + strlen(">>> ") + strlen(Current.input), Current.Pos[1]);
	setcolor(YELLOW);
	
	AllInMenu[Selected].thing.entry.input[Current.Cnt] = c;
	AllInMenu[Selected].thing.entry.input[Current.Cnt+1] = '\0';
	AllInMenu[Selected].thing.entry.Cnt++;
	if (CurrentState == Register&& Selected==4) {
		printf("*");
		
	}
	else {
		printf("%c", c);
	}

}
void FillFiles() {
	//ordinary---------------------------------------------
	int OrdinaryPermited[70];
	int OrdCnt = 0;
	char SpecialCharacters[] = { '_','-','@','$','%','^','&','!' };
	const int SpecialCharactersSize = 8;
	for (int i = 48; i <= 57; i++) {
		OrdinaryPermited[OrdCnt] = i;
		OrdCnt++;
	}
	for (int i = 65; i <= 90; i++) {
		OrdinaryPermited[OrdCnt] = i;
		OrdCnt++;
	}
	for (int i = 97; i <= 122; i++) {
		OrdinaryPermited[OrdCnt] = i;
		OrdCnt++;
	}
	//filling files----------------------------------------------------
	FILE* file = fopen("ord.txt", "w");
	int lenght;
	char word[26];
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10 ;
		lenght = min((lenght + 3), 10);
		// because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j ] = OrdinaryPermited[rand() % OrdCnt];
		}
		word[lenght] = '\0';
		fputs(strcat(word,"\n"), file);
	}
	fclose(file);
	//long-----------------------------------
	file = fopen("long.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10 + 11;//can take 11 to 20 length
		lenght = min((lenght + 3), 20);
		//j=3 because we dont want our word to have less than 3 letters;
		for (int j=0 ; j < lenght; j++) {
			word[j ] = OrdinaryPermited[rand() % OrdCnt];
		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);
	//hard----------------------

	file = fopen("hard.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 20 + 1;
		lenght = min((lenght + 3), 20);
		//j=3 because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j ] = OrdinaryPermited[rand() % OrdCnt];
		}
		int HowManySpecialCheraracters = rand() % min(lenght / 2, 4) + 1;
		//it culd overwrite on the previous choice but its okay
		for (int i = 0; i < HowManySpecialCheraracters; i++) {
			word[rand() % lenght] = SpecialCharacters[rand() % SpecialCharactersSize];

		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);


}
void shuffle(Word** head) {
	
	int size = 0;
	Word* temp = (*head);
	while (temp != NULL) {
		temp=temp->next;
		size++;
	}

	int NumberOfSwaps = rand() % 30+10;

	int Index1;
	int Index2;

	//head always will be a easy word;when index is 0 the index 1 will be changed;
	for (int i = 0; i < NumberOfSwaps; i++) {
		Index1 = max(rand() % (size),0);
		Index2 = max(rand() % (size),0);
		
		if (Index1 == Index2)
			continue;
		if(Index2<Index1){
			int temp;
			temp = Index1;
			Index1 = Index2;
			Index2 = temp;
		}
		Word* w1=*head;
		Word* w2=*head;
		for (int i = 0; i < Index2 - 1; i++) {
			w2 = w2->next;
		}

		if (Index1 == 0) {
			if (Index2 == 1) {
				temp = w2->next;
				w1->next = w1->next->next;
				temp->next = *head;
				*head = temp;
			}
			else {
				temp = w2->next->next;
				w2->next->next =( *head)->next;
				Word* temp2 = w2->next;
				w2->next =* head;
				(*head)->next = temp;
				(*head) = temp2;
				
			}
			
		}
		else {
			for (int i = 0; i < Index1 - 1; i++) {
				w1 = w1->next;
			}
			
			if (Index1 == Index2 - 1) {
				temp = w2->next;
				w1->next->next = w2->next->next;
				w1->next = temp;
				w1->next->next = w2;

			}
			else {
				temp = w1->next->next;
				w1->next->next = w2->next->next;
				w2->next->next = temp;
				temp = w1->next;
				w1->next = w2->next;
				w2->next = temp;
			}
		}
		
		

		
	}

	
}
void HashPass(char pass[20]) {
	int key = 2;
	int mode = 20;
	for (int i = 0; i < strlen(pass); i++) {
		pass[i] = ((int)pow(2, i % 3) * pass[i]) % 26+97;
		printf("%c", pass[i]);

	}
	
	

	
}
void StartWave() {
	CreatNewLinkedList(2, 3, 3, 2, 10);
	Busy = 1;
	PrintLinkedList(Heads, Heads->height, WHITE, 10);
	Busy = 0;
	RestGameMenu();
	
}
void GetDown() {
	for (int i = 0; i < NumberOfHeads; i++) {
		Word* temp = Heads + i;
		while (temp != NULL) {
			temp->height++;
			temp = temp->next;
		}
	}
}

