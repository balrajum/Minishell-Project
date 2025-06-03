#include "main.h"

void n_pipe(char *argv[])
{
	int i=0,pipe_count =0;
	while(argv[i])
	{
		if(!strcmp(argv[i],"|"))
		{
			pipe_count++;
		}
		i++;
	}
	i=0;

	int arr[pipe_count+1],index=1;
	arr[0] =0;

	while(argv[i])
	{
		if(!strcmp(argv[i],"|"))
		{
			arr[index++] = i+1;
		}
		i++;
	}

	int fd[2];

	for(int i=0;i<pipe_count;i++)
	{
		if(pipe(fd) == -1)
		{
			perror("Pipe error\n");
			return;
		}

		pid_t pid = fork();

		if(pid == -1)
		{
			perror("Fork\n");
			return;
		}

		if(pid > 0)
		{
			//parent

			close(fd[1]);

			dup2(fd[0],0);

			close(fd[0]);

			wait(NULL);
		}
		else if(pid == 0)
		{
			//child
			if(i != pipe_count -1)
			{
				close(fd[0]);

				dup2(fd[1],1);
			}
			execvp(argv[arr[i]],argv + arr[i]);

			exit(0);
		}
	}
}
