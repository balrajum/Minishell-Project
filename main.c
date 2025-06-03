/*
Name: Balraju M
Description: MINISHELL PROJECT
 
 */


#include "main.h"


//global variable declarations

char *external_command[200];
char input_string[100];
char prompt[100] = "MiniShell$";

int main()
{

	system("clear");
	scan_input(prompt,input_string);

}

