#include "main.h"

extern char input_string[100];

int insert_at_last(sll **head,pid_t pid)
{
        sll *new = malloc(sizeof(sll));

        if(!new)
        {
                printf("memory not allocated\n");
                return -1;
        }

        new->pid = pid;
        strncpy(new->cmd, input_string, sizeof(new->cmd) - 1);
        new->cmd[sizeof(new->cmd) - 1] = '\0';
        new->link = NULL;

        if(*head == NULL)
        {
                *head = new;
                return 1;
        }

        sll *temp = *head;
        while(temp->link)
        {
                temp = temp -> link;
        }
        temp->link = new;
        return 1;
}

void print_sll(sll *head)
{
  int i=1;
  while(head)
  {
          printf(" [%d]\t%d\t%s\n", i++, head->pid, head->cmd);  
          head = head->link;
  }
}

