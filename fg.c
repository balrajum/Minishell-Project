#include "main.h"

void fg_bg(sll *head,int flag)
{
	   if(head == NULL)
	   {
		   printf("Error: No process is running in foreground && background\n");
		   return;
	   }
       // Traverse to the last node
       while (head->link)
       {
	      head = head ->link;
       }

       // Foreground
       if (flag)
       {
	      printf("%s\n", head->cmd);
	      kill(head->pid, SIGCONT);
	      waitpid(head->pid, NULL, WUNTRACED);
       }

       // Background
       else
       {
	      printf("%s\t&\n", head->cmd);
	      kill(head->pid, SIGCONT);
       }
}
