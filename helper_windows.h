
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <conio.h>
#include <windows.h>
#include <stdio.h>
using namespace std;

DWORD WINAPI keypress_callback( LPVOID args ){
  void (*callback)(char) = (void (*)(char))((void *)(args));

  while (1) {
	  
	callback(_getch());
	
  }
 
  return 0;
}

HANDLE start_listening(void (*callback)(char)){
  HANDLE thread = CreateThread(NULL, 0, keypress_callback, (void *)(callback), 0, NULL);
  return thread;
}
