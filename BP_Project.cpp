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
const int MAxSHOWnHEIGHt = 22;
const int EASYTIME = 10;
const int MEDIUMTIME = 8;
const int HARDTIME = 10;
#pragma endregion

#pragma region structs, unions,Enums
typedef enum Mode { EasyMode, MediumMode, HardMode,RightEasy,LeftEasy,RightMedium,LeftMedium,RightHard,LeftHard } Mode;

typedef enum State { Main, Register, SignIn,InGame,Choice ,History,GameOverState} State;
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
	char time[100];
	int BeenPlayed;
	int score;
	Mode hardness;
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
	int TrueFalse[25];
	WordKind kind;
	Word* next;
	int height;
	int IsItUnClear;
	int UnCLearLenght;
	int color;
	int Faults;
}Word;


#pragma endregion

#pragma region public variables
State CurrentState;
OptionOrEntry* AllInMenu;
int CurserPos[2];
int Busy;
int Selected;
int Color;
int OptionCnt=0;
HANDLE BlinkThread;
HANDLE MusicPlayer;
Player player;

Word** Heads;
int NumberOfHeads;


int WhereInOrd=0, WhereInOrdRight=0, WhereInOrdLeft=0;
int WhereInHard=0,WhereInHardRight=0, WhereInHardLeft=0;
int WherInLong=0, WhereInLongRight=0, WhereInLongLeft=0;

int wave;
Word** UnClears;
int UnCLearCnt;
HANDLE UnClearChangingSizeThread;
Word* SelectedWord;
double RestTime;

int ListAndLock[2];
int Score;
Mode mode;
int DoColorize;
const char LevelNames[10][20] = { "Easy","Medium","Hard","Right Handed Easy","Left Handed Easy","Right Handed Medium","Left Handed Medium","Right Handed Hard","Left Handed Hard" };
const char* OrdrightPermited = "yuiophjklnmbYUIOPHJKLBNM78906";
const char* OrdLeftPermited = "qwertasdfgzxcvQWERTASDFGZXCV12345";
const char* RightspecialChars = "&_+^-";
const char* LeftspecialChars = "%$#@!";
int OrdinaryPermited[70];
int OrdCnt = 0;
char SpecialCharacters[] = { '_','-','@','$','%','^','&','!' };
const int SpecialCharactersSize = 8;



int RightLenght = strlen(OrdrightPermited);
int LeftLenght = strlen(OrdLeftPermited);

int RightSpecialLenght = strlen(RightspecialChars);
int LeftSpecialLenght = strlen(LeftspecialChars);

int LerftRightModeSelected = 0;
int SrandCnt = 0;
int PrevIndex = -1;
int cheetMode = 0;
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
void GameLog(const char* text, int color);
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
WordKind GenerateUnclear(char word[25], int None0_Right1_Left2);
void CreatNewLinkedList(int ord, int Long, int hard, int Unclear,int HeadHeight, int None0_Right1_Left2);
void PrintFace(int happy);
void PrintLinkedList(Word* head,int color);
void shuffle(Word** head);
void GetDown();
void ResetGameMenu();
void ShowScore();
void ShowWave();
void NextWord();
void GameOver(int stat);
void ChoiceMenu();
void PrintChoiceMenu(int space);
void PrintHistory(int n1, int n2, int n3);
void loose();
void win();
#pragma endregion

void Printl(Word* head) {
	Word* temp = head;
	while (temp != NULL) {
		printf(temp->word);
		printf("->");
		temp = temp->next;
	}
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
int main(void) {
	/*
	the left and right words kind will be still Between Hard Ord Long and doesnt include "right" or "left";
	
	
	*/
	//initilizinfg public varaibkes-------------
	srand(time(NULL));
	DoColorize = 1;
	Score = 0; 
	mode = EasyMode;
	ListAndLock[0] = 0;
	ListAndLock[1] = 0;
	
	Color = WHITE;
	UnCLearCnt = 0;
	wave = 0;
	NumberOfHeads = 0;
	WhereInOrd=0;
	WhereInHard=0;
	WherInLong=0;
	
	//------------------------------------------
	
	FillFiles();
	//Remember To Uncomment;
	MusicPlayer = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Music, NULL, 0, NULL);
	
	Busy = 0;
	CurserPos[0] = 0;
	CurserPos[1] = 0;
	system("cls");
	//initializing////////
	HANDLE BlinkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Colorize, NULL, 0, NULL);
	HANDLE thread_id = start_listening(my_callback_on_key_arrival);
	//------------------------------------------------------------------------------------------------
	welcome();
	int Cnt = 0;
	while (1) {
		if (CurrentState == InGame) {
		
			Sleep((RestTime / 10) * 1000);

			while (Busy);
			Busy = 1;
			if (Cnt == 11) {
				StartWave();
				int n = 10;
				switch (mode) {
				case EasyMode:
					RestTime *= 80.0 / 100.0;
					break;
				case MediumMode:
					RestTime *= 70.0 / 100.0;

				case HardMode:
					RestTime *= 60.0 / 100.0;
				default:
					break;

				}
				if (RestTime <= 1) {
					GameOver(1);
				}


				//while (n--) {
				//	NextWord();
				//}
				/*system("cls");
				Printl(*Heads);
				
				printf("\n\n");
				Printl(*(Heads + 1));
				while (1);*/

			
				Cnt = 0;
			}
			GetDown();
			
			Cnt++;
			
			Busy = 0;
		}
	}





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
			//checking that it is stee
			if (UnClears[i]->IsItUnClear == 1) {
				if (UnClears[i]->height >= MInSHOWNhEIGHT) {
					int color = rand() % 10 + 1;
					setcolor(rand() % 10 + 1);
					UnClears[i]->color = color;

					gotoxy((GAMEWIDTH - UnClears[i]->UnCLearLenght) / 2 + 2, UnClears[i]->height);
					for (int j = 0; j < UnClears[i]->UnCLearLenght+2; j++) {
						printf(" ");
					}
					UnClears[i]->UnCLearLenght = (UnClears[i]->UnCLearLenght + 2) % 20;
					gotoxy((GAMEWIDTH - UnClears[i]->UnCLearLenght) / 2 + 2, UnClears[i]->height);
					for (int j = 0; j < UnClears[i]->UnCLearLenght; j++) {
						printf("*");
					}





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
		if (DoColorize) {
			while (Busy);
			Busy = 1;




			if (OptionCnt!=0&&AllInMenu[Selected].OptionOrEntry == 0) {
				CurrentOption = AllInMenu[Selected].thing.option;
				gotoxy(CurrentOption.Pos[0], CurrentOption.Pos[1]);
				setcolor(GREEN);
				printf(CurrentOption.text);

				setcolor(Color);
			}
			Busy = 0;
		}
		Sleep(400);
		if(DoColorize){
			
			Busy = 1;

			if (OptionCnt != 0&&AllInMenu[Selected].OptionOrEntry == 0) {
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

	srand(time(NULL));
		
	
	while (Busy);
	Busy = 1;
	
	if (CurrentState == InGame) {
		//Deleting
		if (NumberOfHeads>ListAndLock[0]&&(*(Heads + ListAndLock[0]))->height>=MInSHOWNhEIGHT) {
			if (c == 8) {
				if (ListAndLock[1] == 0) {

					GameLog("Nothing To Erase", RED);
				}
				else {

					gotoxy((GAMEWIDTH - strlen((*(Heads + ListAndLock[0]))->word)) / 2 + 2 + (ListAndLock[1] - 1), (*(Heads + ListAndLock[0]))->height);
					setcolor(WHITE);
					printf("%c", (*(Heads + ListAndLock[0]))->word[ListAndLock[1] - 1]);
					if ((*(Heads + ListAndLock[0]))->TrueFalse[ListAndLock[1] - 1] == 0) {
						(*(Heads + ListAndLock[0]))->Faults--;
					}
					(*(Heads + ListAndLock[0]))->TrueFalse[ListAndLock[1] - 1] = -1;
					ListAndLock[1]--;
					if ((*(Heads + ListAndLock[0]))->Faults == 0) {
						PrintFace(1);
					}

				}
			}
			else {
				if (c == (*(Heads + ListAndLock[0]))->word[ListAndLock[1]]) {


					setcolor(YELLOW);
					(*(Heads + ListAndLock[0]))->TrueFalse[ListAndLock[1]] = 1;
				}
				else {


					setcolor(RED);
					(*(Heads + ListAndLock[0]))->TrueFalse[ListAndLock[1]] = 0;
					(*(Heads + ListAndLock[0]))->Faults++;
				}
				gotoxy((GAMEWIDTH - strlen((*(Heads + ListAndLock[0]))->word)) / 2 + 2 + (ListAndLock[1]), (*(Heads + ListAndLock[0]))->height);
				printf("%c", (*(Heads + ListAndLock[0]))->word[ListAndLock[1]]);
				ListAndLock[1]++;
				if ((*(Heads + ListAndLock[0]))->Faults == 1) {//means that user just made is first fault;
					PrintFace(0);
				}
				if (ListAndLock[1] == strlen((*(Heads + ListAndLock[0]))->word)) {
					NextWord();
					//going next------------------------------------------------------------------------

				}
				//-----------------------------------------------------------


			}


		}
	}
	else {
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
			if (OptionCnt != 0 && AllInMenu[Selected].OptionOrEntry == 1) {
				ChangeOption(1);
			}
			else {
				//Choice Menu-----------------------------------------------------
				if (CurrentState == Choice) {
					if (OptionCnt != 0 && AllInMenu[Selected].thing.option.text[0] == '|') {
						int Games[3];
						

						for (int i = 0; i < 3; i++) {
							if (player.PrevGames[i].BeenPlayed == 1) {
								Games[i] = player.PrevGames[i].score;
							}
							else {
								Games[i] = 0;

							}
						}
						PrintHistory(Games[0], Games[1], Games[2]);

					}
				


					else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "New Easy GAME")==0) {
						mode = EasyMode;
						GameMenu();
						
					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "New Medium GAME")==0) {
						mode = MediumMode;
					
						GameMenu();
						
					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "New Hard GAME")==0) {
						mode = HardMode;
						GameMenu();

						
					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "New Right Handed GAME")==0) {
						mode = (Mode)(RightEasy + (LerftRightModeSelected * 2));
						GameMenu();

						
					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "New Left Handed GAME")==0) {
						mode = (Mode)(LeftEasy + (LerftRightModeSelected * 2));
						GameMenu();

					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "GAME 1")==0) {
							Score = player.PrevGames[0].score;
							wave= player.PrevGames[0].LastWave;
							mode = player.PrevGames[0].hardness;
							PrevIndex = 0;
							GameMenu();

					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "GAME 2")==0) {
						Score = player.PrevGames[1].score;
						wave = player.PrevGames[1].LastWave;
						mode = player.PrevGames[1].hardness;
						PrevIndex = 1;
						GameMenu();

					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "GAME 3")==0) {
						Score = player.PrevGames[2].score;
						wave = player.PrevGames[2].LastWave;
						mode = player.PrevGames[2].hardness;
						PrevIndex = 2;
						GameMenu();

					}else if (OptionCnt != 0 && strcmp(AllInMenu[Selected].thing.option.text, "EXIT")==0) {
						system("cls");
						exit(0);
					}
					
				}
				//Main Menu------------------------------------------------------------------
				if (CurrentState == Main) {
					if (OptionCnt != 0 && AllInMenu[Selected].thing.option.text[0] == 'R') {
						RegisterMenu();
					}
					else if (OptionCnt != 0 && AllInMenu[Selected].thing.option.text[0] == 'E') {
						system("cls");
						exit(0);
					}
					else if (OptionCnt != 0 && AllInMenu[Selected].thing.option.text[0] == 'S') {
						SignInMenu();
					}
				}
				//Register menu------------------------------------------------------------------
				else if (CurrentState == Register) {
					if (AllInMenu[Selected].thing.option.text[0] == 'B') {
						MainMenu();
					}
					else if (AllInMenu[Selected].thing.option.text[0] == 'S') {
						RegisterEval();
					}
				}
				//Sign In menu---------------------------------------------------------------------
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
		else if (OptionCnt!=0&&AllInMenu[Selected].OptionOrEntry == 1 && c != -32) {
			if (c == 32) {
				Log("No Spaces", RED);
				
			}
			else {
				Fill(c);
			}

		}
		else if (c == 77) {
			if (CurrentState == Choice && AllInMenu[Selected].OptionOrEntry == 0 && \
				(strcmp(AllInMenu[Selected].thing.option.text, "New Right Handed GAME") == 0 || strcmp(AllInMenu[Selected].thing.option.text, "New Left Handed GAME") == 0)) {
				setcolor(WHITE);
				gotoxy(28, 13);
				printf("      ");
				gotoxy(28, 13);
				LerftRightModeSelected = (LerftRightModeSelected + 1) % 3;
				printf("%s", LevelNames[LerftRightModeSelected]);
			}

		}else if (c == 75) {
			if (CurrentState == Choice && AllInMenu[Selected].OptionOrEntry == 0 && \
				(strcmp(AllInMenu[Selected].thing.option.text, "New Right Handed GAME") == 0 || strcmp(AllInMenu[Selected].thing.option.text, "New Left Handed GAME") == 0)) {
				setcolor(WHITE);
				gotoxy(28, 13);
				printf("      ");
				gotoxy(28, 13);
				LerftRightModeSelected = (LerftRightModeSelected -1+3) % 3;
				printf("%s", LevelNames[LerftRightModeSelected]);
			}

		}

	}
	setcolor(Color);
	gotoxy(0, 0);
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
	gotoxy(77, 10);
	setcolor(RED);
	printf("_____ _ _-__ _-__ ---  _-- --- _-_ _-__ -__");
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
		fclose(file);
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
	fwrite(&player, sizeof(Player), 1, file);
	fclose(file);
	ChoiceMenu();



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
		if (strcmp(player.password, "hello world")) {
			cheetMode = 1;
		}
		else {
			Log("Password Is Not Correct\a", RED);
			return;
		}
	}
	ChoiceMenu();
}

#pragma endregion

#pragma region printing functions
void ShowScore() {
	setcolor(BLUE);
	gotoxy(39, 5);
	printf("    ");
	gotoxy(39, 5);
	printf("%d", Score);
}
void ShowWave() {
	setcolor(BLUE);
	gotoxy(39, 3);
	printf("    ");
	gotoxy(39, 3);
	printf("%d", wave);
}
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
	gotoxy(2, 2);
	printf("                                      ");
	gotoxy(2, 2);
	setcolor(color);
	printf(text);
	gotoxy(CurserPos[0], CurserPos[1]);
	setcolor(Color);
}
void GameLog(const char* text, int color) {
	setcolor(color);
	gotoxy(39, 7);
	printf("                         ");
	gotoxy(39, 7);
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
	printf(" \n\n \n___________________________________PRESS Double ENTER TO CONTINUE____________________________________\n");
	getchar();
	MainMenu();

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

void PrintLinkedList(Word* head,int color) {
	Word* temp = head;
	setcolor(color);
	
	while (temp!=NULL) {
		if (temp->height < MInSHOWNhEIGHT)
			break;

		if (temp->color != 0) {
			setcolor(temp->color);
		}
		else {
			setcolor(Color);
		}
		if (temp->IsItUnClear == 0) {
			gotoxy((GAMEWIDTH - strlen(temp->word)) / 2 + 2, temp->height);
			for (int i = 0; i < strlen(temp->word); i++) {
				if (temp->TrueFalse[i] == -1) {
					setcolor(WHITE);
				}
				else if (temp->TrueFalse[i] == 0) {
					setcolor(RED);
				}
				else {
					setcolor(YELLOW);

				}
				printf("%c", temp->word[i]);
			}
		}
		else {

			gotoxy((GAMEWIDTH - temp->UnCLearLenght) / 2 +2, temp->height);
			for (int i = 0; i < temp->UnCLearLenght; i++) {
				printf("*");
			}
			int t=3;
		}
		temp = temp->next;
		
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
void PrintHistory(int n1, int n2, int n3) {
	CurrentState = History;
	system("cls");
	DoColorize = 0;
	
	while (max(max(n1, n1), n3) >= 50) {
		n1 /= 2;
		n2 /= 2;
		n3 /= 2;
	}
	setcolor(WHITE);
	int mini = min(min(n1, n2), n3);
	n1 -= mini;
	n2 -= mini;
	n3 -= mini;
	int ns[3] = { n1,n2,n3 };
	int maxH = (max(max(n1, n2), n3))+1;
	int maxdown = 0;
	int pos[3];
	pos[0] = 1;
	pos[1] = 51;
	pos[2] = 101;
	for (int i = 0; i < maxH+1 ; i++) {
		printf("|                                                                                           \n");
		maxdown++;
	}
	printf("-----------------------------------------------------------------------------------------------------------  \n");
	printf("Game1                                             Game2                                       Game3\n\n");
	printf("_________________________________________PRESS ENTER KEY TO CONTINUE_________________________________________");

	int PartInt;
	int height = maxH - ns[0];
	for (int j = 0; j < 2; j++) {

		gotoxy(pos[j], maxH-ns[j]);
		setcolor(WHITE);
		printf("**");
		pos[j] += (j + 1) * 2;
		if ((ns[j + 1] - ns[j]) == 0) {
			PartInt = 0;
		}
		else {
			PartInt = (pos[j + 1] - pos[j]) / ((ns[j + 1] - ns[j]) * 1.0);
		}
		if (PartInt > 0) {
			setcolor(GREEN);
		}
		else {
			setcolor(RED);
		}
		int x = pos[j];
		int y = maxH-ns[j];
		for (int i = 0; i < abs(ns[j + 1] - ns[j]); i++) {
			
			if (height <= 0) {
				int t = 0;
			}
			gotoxy(x, y);

			for (int p = 0; p < abs(PartInt)-1; p++) {
				printf("_");
			}
			
			if (PartInt < 0) {
				printf("\\");
				y++;
			}
			else {
				printf("/");
				y--;
			}

			x += abs(PartInt);
		}
						
		if (PartInt > 0) {
			gotoxy(x, y);
			while (y > maxH - ns[j + 1]) {
				printf("/");
				y--;
				gotoxy(x, y);
			}
		}
		else {
			gotoxy(x, y);
			while (y < maxH - ns[j + 1]) {
				printf("\\");
				y++;
				gotoxy(x, y);
			}

		}
		gotoxy(x, y);
		while (x < pos[j + 1]) {
			printf("_");
			x++;
		}

	}
	setcolor(WHITE);
	printf("**");
	gotoxy(CurserPos[0], CurserPos[1]);
	while (getchar() != 10);


	DoColorize = 1;
	ChoiceMenu();


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
void ResetGameMenu() {
	int n = 21;
	gotoxy(0, 1);
	setcolor(WHITE);
	while (n--) {
		printf("||                          ||\n");
	}
	setcolor(Color);

}
void PrintChoiceMenu(int space) {
	setcolor(WHITE);
	gotoxy(0, 0);

	printf("____________________________________________________________________________________________\n");
	printf("||logs                                            || Previos Games:                       || \n");
	printf("||                                                ||                                      || \n");
	printf("||------------------------------------------------||Time:                                 || \n");
	printf("|| Options:                                       ||Score:           ||--------------------- \n");
	printf("||                                                ||Wave:            ||--------------------- \n");
	printf("||                                                ||Level:           ||--------------------- \n");
	printf("||                                                ||--------------------------------------|| \n");
	printf("||                                                ||Time:                                 || \n");
	printf("||                                                ||Score:           ||--------------------- \n");
	printf("||                                                ||Wave:            ||--------------------- \n");
	printf("||                                                ||Wave:            ||--------------------- \n");
	printf("||                       |                        ||--------------------------------------|| \n");
	printf("||                       |  Easy     (<-|->)      ||Level:           ||--------------------- \n");
	printf("||                       |                        ||Time:                                 || \n");
	printf("||                                                ||Wave:            ||--------------------- \n");
	printf("||                                                ||Score:           ||--------------------- \n");
	printf("||                                                ||--------------------------------------|| \n");
	printf("||Restart Game:                                   ||\n");
	printf("||                                                ||\n");
										                      
	while (space--) {					                      
	printf("||                                                ||\n");
	}									                      
										                      
	printf("||                                                ||\n");
										                      
										                      
	printf("||                                                ||\n");
	printf("||                                                ||\n");
	printf("||                                                ||\n");

	printf("||-------------------------------------------------||\n");


	                   


	setcolor(Color);
	gotoxy(CurserPos[0], CurserPos[1]);
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
	gotoxy(67, 6);
	setcolor(0);
	printf("dedicated to my mother");
	setcolor(Color);
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
	Busy = 1;
	DoColorize = 0;
	if (mode == EasyMode||mode==RightEasy||mode==LeftEasy)
		RestTime = EASYTIME;
	else if (mode == MediumMode||mode==LeftMedium||mode==RightMedium)
		RestTime = MEDIUMTIME;
	else if(mode == HardMode||mode==LeftHard||mode==RightHard)
		RestTime = HARDTIME;
	CurrentState = InGame;
//	RestTime = 1;
	system("cls");
	ClearTerminal();
	
	PrintGameMenu();
	UnClearChangingSizeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UnClearSizeChange, NULL, 0, NULL);
	ShowScore();
	
	gotoxy(39, 1);
	setcolor(11);
	printf("%s.%c", player.name, player.LastName[0]);
	setcolor(12);
	printf(" AKA ");
	setcolor(11);
	printf("%s", player.NickName);

	StartWave();

	
	Busy = 0;

	

}
void ChoiceMenu() {
	CurrentState = Choice;
	OptionCnt = 7;
	Selected = 0;
	
	AllInMenu = (OptionOrEntry*)realloc(AllInMenu, 11 * sizeof(OptionOrEntry));

	AllInMenu[0].OptionOrEntry = 0;
	AllInMenu[0].thing.option.IsSelected = 1;
	AllInMenu[0].thing.option.text = "New Easy GAME";

	AllInMenu[0].thing.option.Pos[0] = 3; AllInMenu[1];	AllInMenu[0].thing.option.Pos[1] = 6;


	AllInMenu[1].OptionOrEntry = 0;
	AllInMenu[1].thing.option.IsSelected = 0;
	AllInMenu[1].thing.option.text = "New Medium GAME";
	AllInMenu[1].thing.option.Pos[0] = 3; AllInMenu[1];	AllInMenu[1].thing.option.Pos[1] = 8;

	AllInMenu[2].OptionOrEntry = 0;
	AllInMenu[2].thing.option.IsSelected = 0;
	AllInMenu[2].thing.option.text = "New Hard GAME";
	AllInMenu[2].thing.option.Pos[0] = 3; AllInMenu[1];	AllInMenu[2].thing.option.Pos[1] = 10;

	AllInMenu[3].OptionOrEntry = 0;
	AllInMenu[3].thing.option.IsSelected = 0;
	AllInMenu[3].thing.option.text = "New Right Handed GAME";
	AllInMenu[3].thing.option.Pos[0] =3; AllInMenu[1];	AllInMenu[3].thing.option.Pos[1] = 12;

	AllInMenu[4].OptionOrEntry = 0;
	AllInMenu[4].thing.option.IsSelected = 0;
	AllInMenu[4].thing.option.text = "New Left Handed GAME";
	AllInMenu[4].thing.option.Pos[0] = 3; AllInMenu[1];	AllInMenu[4].thing.option.Pos[1] = 14;

	AllInMenu[5].OptionOrEntry = 0;
	AllInMenu[5].thing.option.IsSelected = 0;
	AllInMenu[5].thing.option.text = "|Chart Of Previos Games|";
	AllInMenu[5].thing.option.Pos[0] = 3; AllInMenu[1];	AllInMenu[5].thing.option.Pos[1] = 16;
	char name[10];
	///creating a player
	/*
	player.PrevGames[0].BeenPlayed = 1;
	player.PrevGames[1].BeenPlayed = 1;
	player.PrevGames[2].BeenPlayed = 1;
	player.PrevGames[0].LastWave = 3;
	player.PrevGames[1].LastWave = 3;
	player.PrevGames[2].LastWave = 3;
	player.PrevGames[0].score = 5;
	player.PrevGames[1].score = 5;
	player.PrevGames[2].score = 5;
	time_t tm = time(NULL);
	strcpy(player.name, "Erfan");
	strcpy(player.LastName, "Geramizadeh");
	strcpy(player.NickName, "The Master");
	strcpy(player.PrevGames[0].time, asctime(localtime(&tm)));
	strcpy(player.PrevGames[1].time, asctime(localtime(&tm)));
	strcpy(player.PrevGames[2].time, asctime(localtime(&tm)));*/
	//--------------------------------

	
	for (int i = 0; i < 3; i++) {

		if (player.PrevGames[i].BeenPlayed == 1) {
			OptionCnt++;
			
			AllInMenu[6+i].OptionOrEntry = 0;
			AllInMenu[6+i].thing.option.IsSelected = 0;
			if (i == 0) {
				AllInMenu[i + 6].thing.option.text = "GAME 1";
			}
			else if(i==1){
				AllInMenu[i + 6].thing.option.text = "GAME 2";


			}else{
				AllInMenu[i + 6].thing.option.text = "GAME 3";
			}
			AllInMenu[i+6].thing.option.Pos[0] = 5; AllInMenu[1];	AllInMenu[i+6].thing.option.Pos[1] = 18+2*(OptionCnt-7);
			


		}
	}



	AllInMenu[OptionCnt-1].OptionOrEntry = 0;
	AllInMenu[OptionCnt - 1].thing.option.IsSelected = 0;
	AllInMenu[OptionCnt - 1].thing.option.text = "EXIT";
	AllInMenu[OptionCnt - 1].thing.option.Pos[0] = 3; AllInMenu[1];	AllInMenu[OptionCnt - 1].thing.option.Pos[1] = 27- 2*(10-OptionCnt);



	Busy = 1;
	ClearTerminal();
	PrintChoiceMenu((OptionCnt-7)*2);
	PrintOptionOrEntry(AllInMenu[0], GREEN);
	for (int i = 1; i < OptionCnt; i++) {
		PrintOptionOrEntry(AllInMenu[i], BLUE);

	}
	if (OptionCnt == 7) {
		gotoxy(3, 19);
		setcolor(YELLOW);
		printf("No Games Has Been Played");
		Log("No Games Has Been Played", YELLOW);
	}
	for (int i = 0; i < 3; i++) {

		if (player.PrevGames[i].BeenPlayed == 1) {
			
			gotoxy(59, 5 * i + 3);
			setcolor(10);
			printf("%s", player.PrevGames[i].time);
			gotoxy(59, 5 * i + 4);
			printf("%d", player.PrevGames[i].score);
			gotoxy(59, 5 * i + 5);
			printf("%d", player.PrevGames[i].LastWave);
			gotoxy(59, 5 * i + 6);

			
			printf("%s",LevelNames[player.PrevGames[i].hardness] );

		}
	}
	Option CurrentOption = AllInMenu[0].thing.option;
	PrintSquare(strlen(CurrentOption.text) + 2, 3, CurrentOption.Pos[0] - 1, CurrentOption.Pos[1] - 1, GREEN);

	Busy = 0;

}

#pragma endregion

WordKind  GenerateUnclear(char word[25],int None0_Right1_Left2) {
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
			switch (None0_Right1_Left2) {
			case 0:
				word[j] = OrdinaryPermited[rand() % OrdCnt];
				break;
			case 1:
				word[j] = OrdrightPermited[rand() % RightLenght];
				break;
			case 2:
				word[j] = OrdLeftPermited[rand() % LeftLenght];
				break;
			
			}
			
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
			switch (None0_Right1_Left2) {
			case 0:
				word[j] = OrdinaryPermited[rand() % OrdCnt];
				break;
			case 1:
				word[j] = OrdrightPermited[rand() % RightLenght];
				break;
			case 2:
				word[j] = OrdLeftPermited[rand() % LeftLenght];
				break;

			}
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
			switch (None0_Right1_Left2) {
			case 0:
				word[j] = OrdinaryPermited[rand() % OrdCnt];
				break;
			case 1:
				word[j] = OrdrightPermited[rand() % RightLenght];
				break;
			case 2:
				word[j] = OrdLeftPermited[rand() % LeftLenght];
				break;

			}
		}
		//we at least want 1 Special character and we dont want to make it so hard so max 4 characters would be fine
		int HowManySpecialCheraracters = rand() %min(lenght/2,4) +1;
		//it culd overwrite on the previous choice but its okay
		for (int i = 0; i < HowManySpecialCheraracters; i++) {
			switch (None0_Right1_Left2) {
			case 0:
				word[rand() % lenght] = SpecialCharacters[rand() % SpecialCharactersSize];
				break;
			case 1:
				word[rand() % lenght] = RightspecialChars[rand() % RightSpecialLenght];
				break;
			case 2:
				word[rand() % lenght] = LeftspecialChars[rand() % LeftSpecialLenght];
				break;

			}
			

		}
		word[lenght] = '\0';
		return HardWord;
	}

}
void CreatNewLinkedList(int ord, int Long, int hard, int Unclear,int HeadHeight,int None0_Right1_Left2) {
	char Chert[25];


	
	
	NumberOfHeads++;
	Heads = (Word**)realloc(Heads, (NumberOfHeads ) * sizeof(Word*));
	*(Heads + NumberOfHeads - 1) = (Word*)malloc(sizeof(Word));


	
	Word * TempHead = *(Heads + NumberOfHeads - 1);
	//Opening FIles====================================================================
	int* WhereIn[9] = { &WhereInOrd,&WherInLong,&WhereInHard,&WhereInOrdRight,&WhereInLongRight,&WhereInHardRight ,\
		& WhereInOrdLeft,&WhereInLongLeft,&WhereInHardLeft };
	const char* OrdFIleName="\0";
	const char* HardFIleName= "\0";
	const char* LongFIleName= "\0";
	switch (None0_Right1_Left2) {
	case 0:
		OrdFIleName = "ord.txt";
		LongFIleName = "long.txt";
		HardFIleName = "hard.txt";
		break;
	case 1:
		OrdFIleName = "Rightord.txt";
		LongFIleName = "Rightlong.txt";
		HardFIleName = "Righthard.txt";

		break;
	case 2:
		OrdFIleName = "Leftord.txt";
		LongFIleName = "Leftlong.txt";
		HardFIleName = "Lefthard.txt";
		break;
	default:
		break;
	}
	
	FILE* fileOrd = fopen(OrdFIleName, "r");
	for (int i = 0; i < (*(WhereIn[None0_Right1_Left2 * 3 + 0])); i++) {
		//OrdWords[0] is just for going through the file not that we need it;
		fgets(Chert, 1000, fileOrd);

	}
	FILE* fileLong = fopen(LongFIleName, "r");
	for (int i = 0; i < (*(WhereIn[None0_Right1_Left2 * 3 + 1])); i++) {
		//OrdWords[0] is just for going through the file not that we need it;
		fgets(Chert, 1000, fileLong);
	}

	FILE* fileHard = fopen(HardFIleName, "r");
	for (int i = 0; i < (*(WhereIn[None0_Right1_Left2 * 3 + 2])); i++) {
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
		if (i == ord - 1&&Long==0&&hard==0&&Unclear==0) {
			TempHead->next = NULL;
		}
		TempHead = TempHead->next;
		(*(WhereIn[None0_Right1_Left2 * 3 + 0]))++;
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
		(*(WhereIn[None0_Right1_Left2 * 3 + 1]))++;
		

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
		(*(WhereIn[None0_Right1_Left2 * 3 + 2]))++;

	}
	
	UnClears = (Word**)realloc(UnClears, (UnCLearCnt + Unclear) * sizeof(Word*));
	for (int i = 0; i < Unclear; i++) {
		UnClears[UnCLearCnt]= TempHead;
		TempHead->kind = GenerateUnclear(Chert, None0_Right1_Left2);
		(*(WhereIn[None0_Right1_Left2 * 3 + TempHead->kind]))++;
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
	

	
	
	shuffle(Heads + NumberOfHeads - 1);



	TempHead = *(Heads + NumberOfHeads - 1);
	while (TempHead != NULL) {
		TempHead->color = 0;
		TempHead->height = HeadHeight;
		TempHead->Faults = 0;
		for (int i = 0; i < strlen(TempHead->word); i++) {
			TempHead->TrueFalse[i] = -1;
		}
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

	

	int RightLenght = strlen(OrdrightPermited);
	int LeftLenght = strlen(OrdLeftPermited);

	int RightSpecialLenght = strlen(RightspecialChars);
	int LeftSpecialLenght = strlen(LeftspecialChars);

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
	//filling files NoneHand----------------------------------------------------
	FILE* file = fopen("ord.txt", "w");
	int lenght;
	char word[26];
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10;
		if (lenght < 3) {
			lenght += 3;
		}
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
		lenght = rand() % 10 + 11;
		
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



	//filling files Right Hand----------------------------------------------------
	file = fopen("Rightord.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10;
		if (lenght < 3) {
			lenght += 3;
		}
	
		// because we dont want our word to have less than 1 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdrightPermited[rand() % RightLenght];
		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);
	//long-----------------------------------
	file = fopen("Rightlong.txt" , "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10 + 11;//can take 11 to 20 length
		
		//j=3 because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdrightPermited[rand() % RightLenght];
		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);
	//hard----------------------

	file = fopen("Righthard.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10 + 11;
		
		//j=3 because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdrightPermited[rand() % RightLenght];
		}
		int HowManySpecialCheraracters = rand() % min(lenght / 2, 4) + 1;
		//it culd overwrite on the previous choice but its okay
		for (int i = 0; i < HowManySpecialCheraracters; i++) {
			word[rand() % lenght] = RightspecialChars[rand() % RightSpecialLenght];

		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);




	//filling files Left Hand----------------------------------------------------

	file = fopen("Leftord.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10;
		if (lenght < 3) {
			lenght += 3;
		}
		// because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdLeftPermited[rand() % LeftLenght];
		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);
	//long-----------------------------------
	
	file = fopen("Leftlong.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10 + 11;//can take 11 to 20 length
		
		//j=3 because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdLeftPermited[rand() % LeftLenght];
		}
		word[lenght] = '\0';
		fputs(strcat(word, "\n"), file);
	}
	fclose(file);
	//hard----------------------

	file = fopen("Lefthard.txt", "w");
	for (int i = 0; i < 100; i++) {
		lenght = rand() % 10+ 11;
	
		//j=3 because we dont want our word to have less than 3 letters;
		for (int j = 0; j < lenght; j++) {
			word[j] = OrdLeftPermited[rand() % LeftLenght];
		}
		int HowManySpecialCheraracters = rand() % min(lenght / 2, 4) + 1;
		//it culd overwrite on the previous choice but its okay
		for (int i = 0; i < HowManySpecialCheraracters; i++) {
			word[rand() % lenght] = LeftspecialChars[rand() % LeftSpecialLenght];

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

	int NumberOfSwaps = rand() % 30+50;

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
		

	}
	
	

	
}
void StartWave() {
	wave++;
	int maxOrd = 11 - wave;
	int maxHard= 2*wave;
	int maxLong = 2*wave;
	int maxUnClear = 2*wave;
	int all = 0;
	int ord=0, hard=0, _long=0, UnClear=0;
	if (maxOrd != 0) {
		ord = rand() % min(maxOrd, 10);
	}
	else {
		ord = 0;
	}
	all += ord;

	if (all < 10) {
		hard = rand() % min(10 -all, maxHard);
		all += hard;
	}

	if (all < 10) {

		 _long = rand() % min(10 - all, maxLong);
		all += _long;
	}



	if (all < 10) {

		 UnClear = rand() % min(10 - all, maxUnClear);
		all += UnClear;
	}
	ord += 10 - all;

	
	if (((int)mode) > 2) {
		CreatNewLinkedList(ord, hard, _long, UnClear, 0, ((int)mode-1)%2+1);
	}
	else {
		CreatNewLinkedList(ord, hard, _long, UnClear, 0, 0);

	}
	if ((*(Heads + NumberOfHeads - 1))->IsItUnClear == 1) {
		(*(Heads + NumberOfHeads - 1))->IsItUnClear = 0;
	}
	

	ShowWave();
	

	
}
int cnt = 0;
void GetDown() {
	ResetGameMenu();
	for (int i = 0; i < NumberOfHeads; i++) {
		Word* temp =*( Heads + i);
		if (temp != NULL) {
			if (temp->height >= MAxSHOWnHEIGHt) {
				GameOver(0);
			}
			while (temp != NULL) {
				(temp->height)++;
				temp = temp->next;
			}
		}
	}
	for (int i = 0; i < NumberOfHeads; i++) {
		if (NumberOfHeads == 2) {
			int t = 1;
		}
		PrintLinkedList(*(Heads + i), WHITE);

	}
	if (cheetMode) {
		cnt++;
		if (cnt > 15) {
			NextWord();
		}
	}
}
void NextWord() {
	if (NumberOfHeads <= ListAndLock[0]) {
		return;
	}
	gotoxy((GAMEWIDTH - strlen((*(Heads + ListAndLock[0]))->word)) / 2 + 2,(*(Heads + ListAndLock[0]))->height);
	for (int i = 0; i < strlen((*(Heads + ListAndLock[0]))->word)+2; i++) {
		printf(" ");
	}
	if ((*(Heads + ListAndLock[0]))->Faults == 0) {

		if ((*(Heads + ListAndLock[0]))->kind == HardWord) {
			Score += HARDSCORE;
		}
		else if ((*(Heads + ListAndLock[0]))->kind == OrdinaryWord) {
			Score += ORDSCORE;
		}
		else if ((*(Heads + ListAndLock[0]))->kind == LongWord) {
			Score += LONGSCORE;

		}
		if ((*(Heads + ListAndLock[0]))->IsItUnClear == 1) {
			Score++;
			ShowScore();

		}
		GameLog("CORRECT", GREEN);

	}
	else {
		GameLog("WRONG", RED);


	}
	PrintFace(1);




	Word* temp;
	temp = *(Heads + ListAndLock[0]);

	if ( ((*(Heads + ListAndLock[0]))->next) == NULL) {
		*(Heads + ListAndLock[0]) = NULL;
		free(*(Heads + ListAndLock[0]));
		ListAndLock[0]++;
		
	}
	else {
		*(Heads + ListAndLock[0])  =   (*(Heads + ListAndLock[0]))->next;


	}
	ListAndLock[1] = 0;
	ShowScore();

	if (NumberOfHeads > ListAndLock[0]) {
		Word chosen = *(*(Heads + ListAndLock[0]));
		if (chosen.IsItUnClear == 1) {
			//
			
			//
			(*(Heads + ListAndLock[0]))->IsItUnClear = 0;
			if (chosen.height >= MInSHOWNhEIGHT) {
				setcolor(WHITE);
				gotoxy((GAMEWIDTH - chosen.UnCLearLenght) / 2 + 2, chosen.height);
				for (int i = 0; i < chosen.UnCLearLenght+2; i++) {
					printf(" ");
				}
				gotoxy((GAMEWIDTH - strlen(chosen.word)) / 2 + 2, chosen.height);
				printf(chosen.word);
			}
		}
	}

	

}
void GameOver(int stat) {
	CurrentState = GameOverState;
	TerminateThread(UnClearChangingSizeThread,0);
	
	DoColorize = 0;

	if (PrevIndex == -1) {
		player.PrevGames[2] = player.PrevGames[1];
		player.PrevGames[1] = player.PrevGames[0];
	
		player.PrevGames[0].BeenPlayed = 1;
		player.PrevGames[0].score = Score;
		player.PrevGames[0].hardness = mode;
		player.PrevGames[0].LastWave = wave;
	}
	else {
		if (PrevIndex == 0) {
			//nothing
		}
		else if (PrevIndex == 1) {
			player.PrevGames[1] = player.PrevGames[0];
		}
		else {
			
			player.PrevGames[2] = player.PrevGames[1];
			player.PrevGames[1] = player.PrevGames[0];
		}

		player.PrevGames[0].BeenPlayed = 1;
		player.PrevGames[0].score = Score;
		player.PrevGames[0].hardness = mode;
		player.PrevGames[0].LastWave = wave;

		

	}




	time_t second = time(NULL);
	strcpy(player.PrevGames[0].time,asctime(localtime(&second))) ;
	char FileName[45];

	FileName[0] = '\0';

	strcat(FileName, "Users/");
	strcat(FileName, player.name);
	strcat(FileName, " ");
	strcat(FileName, player.LastName);
	strcat(FileName, ".bin");
	
	FILE* file = fopen(FileName, "rb");


	file = fopen(FileName, "wb");
	fwrite(&player, sizeof(Player), 1, file);
	fclose(file);
	if (stat == 1) {
		win();
	}
	else {
		loose();
	}
	system("cls");
	exit(0);

}
void loose() {
	//ClearTerminal with red color--------------------
	setcolor(RED);
	gotoxy(0, 0);
	//system("cls");
	for (int i = 1; i < 50; i++) {
		for (int i = 1; i < 40; i++) {
			printf("---");

		}
		printf("\n");
	}
	gotoxy(CurserPos[0], CurserPos[1]);
	//-------------------------------------------------
	int n = 5;
	float second = 5;
	while (n--) {
		
		gotoxy(0, 0);
		setcolor(YELLOW);


		printf("#################################################################################################################\n");
		printf("##                                                                                                             ##\n");
		printf("##        LLLLLLLLLLL                                                                                          ##\n");
		printf("##        L:::::::::L                                                                                          ##\n");
		printf("##        L:::::::::L                                                                                          ##\n");
		printf("##        LL:::::::LL                                                                                          ##\n");
		printf("##          L:::::L                  ooooooooooo      ooooooooooo       ssssssssss       eeeeeeeeeeee          ##\n");
		printf("##          L:::::L                oo:::::::::::oo  oo:::::::::::oo   ss::::::::::s    ee::::::::::::ee        ##\n");
		printf("##          L:::::L               o:::::::::::::::oo:::::::::::::::oss:::::::::::::s  e::::::eeeee:::::ee      ##\n");
		printf("##          L:::::L               o:::::ooooo:::::oo:::::ooooo:::::os::::::ssss:::::se::::::e     e:::::e      ##\n");
		printf("##          L:::::L               o::::o     o::::oo::::o     o::::o s:::::s  ssssss e:::::::eeeee::::::e      ##\n");
		printf("##          L:::::L               o::::o     o::::oo::::o     o::::o   s::::::s      e:::::::::::::::::e       ##\n");
		printf("##          L:::::L               o::::o     o::::oo::::o     o::::o      s::::::s   e::::::eeeeeeeeeee        ##\n");
		printf("##          L:::::L         LLLLLLo::::o     o::::oo::::o     o::::ossssss   s:::::s e:::::::e                 ##\n");
		printf("##        LL:::::::LLLLLLLLL:::::Lo:::::ooooo:::::oo:::::ooooo:::::os:::::ssss::::::se::::::::e                ##\n");
		printf("##        L::::::::::::::::::::::Lo:::::::::::::::oo:::::::::::::::os::::::::::::::s  e::::::::eeeeeeee        ##\n");
		printf("##        L::::::::::::::::::::::L oo:::::::::::oo  oo:::::::::::oo  s:::::::::::ss    ee:::::::::::::e        ##\n");
		printf("##        LLLLLLLLLLLLLLLLLLLLLLLL   ooooooooooo      ooooooooooo     sssssssssss        eeeeeeeeeeeeee        ##\n");
		printf("##                                                                                                             ##\n");
		printf("##__________________________________________%.1f seconds until closing__________________________________________##\n", second);
		printf("##                                                                                                             ##\n");
		printf("#################################################################################################################\n");

		second -= 0.5;
		Sleep(500);
		gotoxy(0, 0);
		setcolor(RED);
		
                                                                                               
		printf("#################################################################################################################\n");
		printf("##                                                                                                             ##\n");
		printf("##        LLLLLLLLLLL                                                                                          ##\n");
		printf("##        L:::::::::L                                                                                          ##\n");
		printf("##        L:::::::::L                                                                                          ##\n");
		printf("##        LL:::::::LL                                                                                          ##\n");
		printf("##          L:::::L                  ooooooooooo      ooooooooooo       ssssssssss       eeeeeeeeeeee          ##\n");
		printf("##          L:::::L                oo:::::::::::oo  oo:::::::::::oo   ss::::::::::s    ee::::::::::::ee        ##\n");
		printf("##          L:::::L               o:::::::::::::::oo:::::::::::::::oss:::::::::::::s  e::::::eeeee:::::ee      ##\n");
		printf("##          L:::::L               o:::::ooooo:::::oo:::::ooooo:::::os::::::ssss:::::se::::::e     e:::::e      ##\n");
		printf("##          L:::::L               o::::o     o::::oo::::o     o::::o s:::::s  ssssss e:::::::eeeee::::::e      ##\n");
		printf("##          L:::::L               o::::o     o::::oo::::o     o::::o   s::::::s      e:::::::::::::::::e       ##\n");
		printf("##          L:::::L               o::::o     o::::oo::::o     o::::o      s::::::s   e::::::eeeeeeeeeee        ##\n");
		printf("##          L:::::L         LLLLLLo::::o     o::::oo::::o     o::::ossssss   s:::::s e:::::::e                 ##\n");
		printf("##        LL:::::::LLLLLLLLL:::::Lo:::::ooooo:::::oo:::::ooooo:::::os:::::ssss::::::se::::::::e                ##\n");
		printf("##        L::::::::::::::::::::::Lo:::::::::::::::oo:::::::::::::::os::::::::::::::s  e::::::::eeeeeeee        ##\n");
		printf("##        L::::::::::::::::::::::L oo:::::::::::oo  oo:::::::::::oo  s:::::::::::ss    ee:::::::::::::e        ##\n");
		printf("##        LLLLLLLLLLLLLLLLLLLLLLLL   ooooooooooo      ooooooooooo     sssssssssss        eeeeeeeeeeeeee        ##\n");
		printf("##                                                                                                             ##\n");
		printf("##__________________________________________%.1f seconds until closing__________________________________________##\n", second);
		printf("##                                                                                                             ##\n");
		printf("#################################################################################################################\n");
                                                                   

		Sleep(500);
		second -= 0.5;

	}



}
void win() {
	//ClearTerminal with green color--------------------

	gotoxy(0, 0);
	setcolor(GREEN);
	//system("cls");
	for (int i = 1; i < 50; i++) {
		for (int i = 1; i < 40; i++) {
			
			printf("---");

		}
		printf("\n");
	}

	gotoxy(CurserPos[0], CurserPos[1]);
	//-------------------------------------------------
	int n = 5;
	float second = 5;
	while (n--) {
		gotoxy(0, 0);
		setcolor(YELLOW);

		printf("###############################################################################################\n");
		printf("##                                                                                           ##\n");
		printf("##      WWWWWWWW                           WWWWWWWWIIIIIIIIIINNNNNNNN        NNNNNNNN        ##\n");
		printf("##      W::::::W                           W::::::WI::::::::IN:::::::N       N::::::N        ##\n");
		printf("##      W::::::W                           W::::::WI::::::::IN::::::::N      N::::::N        ##\n");
		printf("##      W::::::W                           W::::::WII::::::IIN:::::::::N     N::::::N        ##\n");
		printf("##       W:::::W           WWWWW           W:::::W   I::::I  N::::::::::N    N::::::N        ##\n");
		printf("##        W:::::W         W:::::W         W:::::W    I::::I  N:::::::::::N   N::::::N        ##\n");
		printf("##         W:::::W       W:::::::W       W:::::W     I::::I  N:::::::N::::N  N::::::N        ##\n");
		printf("##          W:::::W     W:::::::::W     W:::::W      I::::I  N::::::N N::::N N::::::N        ##\n");
		printf("##           W:::::W   W:::::W:::::W   W:::::W       I::::I  N::::::N  N::::N:::::::N        ##\n");
		printf("##            W:::::W W:::::W W:::::W W:::::W        I::::I  N::::::N   N:::::::::::N        ##\n");
		printf("##             W:::::W:::::W   W:::::W:::::W         I::::I  N::::::N    N::::::::::N        ##\n");
		printf("##              W:::::::::W     W:::::::::W          I::::I  N::::::N     N:::::::::N        ##\n");
		printf("##               W:::::::W       W:::::::W         II::::::IIN::::::N      N::::::::N        ##\n");
		printf("##                W:::::W         W:::::W          I::::::::IN::::::N       N:::::::N        ##\n");
		printf("##                 W:::W           W:::W           I::::::::IN::::::N        N::::::N        ##\n");
		printf("##                  WWW             WWW            IIIIIIIIIINNNNNNNN         NNNNNNN        ##\n");
		printf("##                                                                                           ##\n");
		printf("##_______________________________%.1f seconds until closing____________________________________##\n",second);
		printf("##                                                                                           ##\n");
		printf("###############################################################################################\n");

		Sleep(500);
		second -= 0.5;
		setcolor(GREEN);
		gotoxy(0, 0);
		printf("###############################################################################################\n");
		printf("##                                                                                           ##\n");
		printf("##      WWWWWWWW                           WWWWWWWWIIIIIIIIIINNNNNNNN        NNNNNNNN        ##\n");
		printf("##      W::::::W                           W::::::WI::::::::IN:::::::N       N::::::N        ##\n");
		printf("##      W::::::W                           W::::::WI::::::::IN::::::::N      N::::::N        ##\n");
		printf("##      W::::::W                           W::::::WII::::::IIN:::::::::N     N::::::N        ##\n");
		printf("##       W:::::W           WWWWW           W:::::W   I::::I  N::::::::::N    N::::::N        ##\n");
		printf("##        W:::::W         W:::::W         W:::::W    I::::I  N:::::::::::N   N::::::N        ##\n");
		printf("##         W:::::W       W:::::::W       W:::::W     I::::I  N:::::::N::::N  N::::::N        ##\n");
		printf("##          W:::::W     W:::::::::W     W:::::W      I::::I  N::::::N N::::N N::::::N        ##\n");
		printf("##           W:::::W   W:::::W:::::W   W:::::W       I::::I  N::::::N  N::::N:::::::N        ##\n");
		printf("##            W:::::W W:::::W W:::::W W:::::W        I::::I  N::::::N   N:::::::::::N        ##\n");
		printf("##             W:::::W:::::W   W:::::W:::::W         I::::I  N::::::N    N::::::::::N        ##\n");
		printf("##              W:::::::::W     W:::::::::W          I::::I  N::::::N     N:::::::::N        ##\n");
		printf("##               W:::::::W       W:::::::W         II::::::IIN::::::N      N::::::::N        ##\n");
		printf("##                W:::::W         W:::::W          I::::::::IN::::::N       N:::::::N        ##\n");
		printf("##                 W:::W           W:::W           I::::::::IN::::::N        N::::::N        ##\n");
		printf("##                  WWW             WWW            IIIIIIIIIINNNNNNNN         NNNNNNN        ##\n");
		printf("##                                                                                           ##\n");
		printf("##_______________________________%.1f seconds until closing____________________________________##\n", second);
		printf("##                                                                                           ##\n");
		printf("###############################################################################################\n");
		Sleep(500);
		second -= 0.5;


	}

}