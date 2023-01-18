#include <stdio.h>
#include <string.h>

int main(void){
    int a=0;
    char salam[10]="salam";
    char * t=salam;
    t+=1;
    t=strstr("salam","al");
    printf("%s",*t);
}