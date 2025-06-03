/*
Name: Balraju M
Description: Minishell functions 
 */

#include "main.h"

extern char *external_command[200];
int wstatus;
pid_t pid;
extern char prompt[100];
sll *head;

void handle_cd(char *arg) 
{
    if (arg == NULL) 
    {
        // 'cd' with no arguments: go to home directory
        char *home_dir = getenv("PWD");
        if (home_dir == NULL) 
        {
            fprintf(stderr, "Environment variable not set.\n");
            return;
        }
        if (chdir(home_dir) != 0) 
        {
            perror("chdir");
        } 
    } 
    else if (strcmp(arg, "..") == 0) 
    {
        // 'cd ..' to move to parent directory
        if (chdir("..") != 0) 
        {
            perror("chdir");
        }
    } 
    else 
    {
        // 'cd <directory>' to change to the specified directory
        if (chdir(arg) != 0) 
        {
            perror("chdir");
        }
	}
}

void my_handler ( int signum )
{
	if ( signum == SIGINT )
	{
		printf(ANSI_COLOR_GREEN"%s\n"ANSI_COLOR_RESET,prompt);
	}
	if(signum == SIGTSTP)
	{
		insert_at_last(&head,pid);
	}
}
void execute_external_command(char *input_string)
{
    char *argv[100], ptr[100];
    int len = strlen(input_string);
    int count = 0, j = 0;

    for (int i = 0; i <= len; i++)
    {
        if (input_string[i] == ' ' || input_string[i] == '\0')
        {
            if (count > 0)
            {
                ptr[count] = '\0';
                argv[j] = malloc(strlen(ptr) + 1);
                strcpy(argv[j++], ptr);
                count = 0;
            }
        }
        else
        {
            ptr[count++] = input_string[i];
        }
    }
    argv[j] = NULL;

    int pipe_present = 0;
    for (int i = 0; argv[i] != NULL; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            pipe_present = 1;
            break;
        }
    }

    if (pipe_present)
    {
        int pipe_count = 0;
        for (int i = 0; argv[i] != NULL; i++)
        {
            if (strcmp(argv[i], "|") == 0)
                pipe_count++;
        }

        int arr[pipe_count + 2], index = 1;
        arr[0] = 0;
        for (int i = 0; argv[i] != NULL; i++)
        {
            if (strcmp(argv[i], "|") == 0)
            {
                argv[i] = NULL;
                arr[index++] = i + 1;
            }
        }
        arr[index] = -1;

        int fd[2], in_fd = 0;
        for (int i = 0; i <= pipe_count; i++)
        {
            if (arr[i + 1] != -1 && pipe(fd) == -1)
            {
                perror("Pipe error");
                return;
            }

            pid_t pid = fork();
            if (pid == -1)
            {
                perror("Fork error");
                return;
            }

            if (pid == 0)
            {
                if (in_fd != 0)
                {
                    dup2(in_fd, 0);
                    close(in_fd);
                }

                if (arr[i + 1] != -1)
                {
                    close(fd[0]);
                    dup2(fd[1], 1);
                    close(fd[1]);
                }

                execvp(argv[arr[i]], &argv[arr[i]]);

                exit(0);
            }
            else
            {
                wait(NULL);
                if (in_fd != 0)
                    close(in_fd);
                if (arr[i + 1] != -1)
                {
                    close(fd[1]);
                    in_fd = fd[0];
                }
                else
                {
                    // Close the last read end to ensure stdout is used in the last iteration
                    if (fd[0] != 0)
                        close(fd[0]);
                }
            }
        }
    }
    else
    {
		//if pipe is not present
            execvp(argv[0], argv);
    }

    for (int i = 0; argv[i] != NULL; i++)
    {
        free(argv[i]);
    }
}

//internal commands execution
void execute_internal_commands(char *input_string)
{
    char buff[100];
    char *cmd = get_command(input_string);

    if (strcmp("exit", input_string) == 0)
    {
        exit(0);
    }
    else if (strcmp("pwd", cmd) == 0)
    {
        getcwd(buff, sizeof(buff));
        printf("%s\n", buff);
    }
	else if (strncmp(input_string, "cd ", 3) == 0 || strcmp(input_string, "cd\n") == 0)
    {
        char *command = strtok(input_string, " \n");  
        char *arg = strtok(NULL, " \n");     
        handle_cd(arg);
    }
	else if(strcmp("echo",cmd)==0)
	{
	  if(strcmp(input_string,"echo $$")==0)
	  {
		  printf("%d\n",getpid());
	  }
	  else if(strcmp(input_string,"echo $?")==0)
	  {
		  printf("%d\n",WEXITSTATUS(wstatus));
	  }
	  else if(strcmp(input_string,"echo $SHELL")==0)
	  {
		  printf("%s\n",getenv("SHELL"));
	  }
	  else if(strcmp(input_string,"echo $HOME")==0)
	  {
		  printf("%s\n",getenv("HOME"));
	  }
	  else 
	  {
		  printf("%s\n",input_string + 5);
	  }
	}
	else if(!strcmp("fg",cmd))        //foreground
	{
		fg_bg(head,1);
	}
	else if(!strcmp("bg",cmd))
	{
		fg_bg(head,0);
	}
	else if(!strcmp("jobs",cmd))
	{
		print_sll(head);
	}
}
//loading external commands in 2d array
void extract_external_commands(char **external_commands)
{
    int fd = open("commands.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("File is not present\n");
        return;
    }
    else
    {
        char ch;
        for (int i = 0; i < 200; i++)
        {
            external_commands[i] = malloc(20 * sizeof(char));
        }
        int i = 0, j = 0;
        while (read(fd, &ch, 1) > 0)
        {
            if (ch == '\n')
            {
                external_commands[i][j] = '\0';
                i++;
                j = 0;
            }
            else
            {
                external_commands[i][j] = ch;
                j++;
            }
        }
        external_commands[i] = NULL;
    }
    close(fd);
}
//reading input from the user
void scan_input(char *prompt, char *input_string)
{
    extract_external_commands(external_command);

	signal(SIGINT,my_handler);
	signal(SIGTSTP,my_handler);

    while (1)
    {
        printf(ANSI_COLOR_GREEN" %s "ANSI_COLOR_RESET, prompt);
        scanf(" %[^\n]", input_string);
		//getchar();
		__fpurge(stdin);
		 // If PS1 is present
	      if (strstr(input_string, "PS1") != NULL)
	      {
		     char *address = strchr(input_string, ' ');

		     if (address)
		     {
			  
			    printf("PS1 : command not found\n");
			    continue;
		     }
		     else
		     {
			   
			    address = strchr(input_string, '=');
			    address++;
			    strcpy(prompt, address);
		     }
	      }
		  else
		  {
        
        char *cmd = get_command(input_string);
        int ret = check_command_type(cmd);

        if (ret == EXTERNAL)
        {
            pid = fork();
            if (pid == -1)
            {
                printf("Fork error\n");
                return;
            }
            else if (pid == 0)
            {
				signal(SIGINT,SIG_DFL);
				signal(SIGTSTP,SIG_DFL);
                execute_external_command(input_string);
            }
            else
            {
                waitpid(pid, &wstatus, WUNTRACED);
            }
        }
        else if (ret == BUILTIN)
        {
           execute_internal_commands(input_string);
        }
        else
        {
            printf("Error: Command is not found\n");
        }
		  }
    }
}

//getting command from the input string
char *get_command(char *input_string)
{
    char *array = malloc(strlen(input_string)* sizeof(char));
    if (array == NULL)
    {
        printf("Malloc memory is not allocated\n");
        return NULL;
    }
	int i;
    for (i = 0; i < strlen(input_string); i++)
    {
        if (input_string[i] == ' ' || input_string[i]=='\0')
        {
            break;
        }
        array[i] = input_string[i];
    }
	array[i] = '\0';
    return array;
}

//checking command type
int check_command_type(char *cmd)
{
    char *builtin[] = {"echo", "printf", "fg", "bg", "jobs", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};

    int i = 0;
    while (builtin[i])
    {
        if (!strcmp(builtin[i], cmd))
		{
            return BUILTIN;
		}
        i++;
    }
    i = 0;
    while (external_command[i])
    {
        if (!strcmp(external_command[i], cmd))
            return EXTERNAL;
        i++;
    }
    
	if(strcmp(cmd,"\0")==0)
	{
	  printf("%s",prompt);  
	}

	return NO_COMMAND;
}

