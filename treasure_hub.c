#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pid = -1;

void stop_monitor()
{
  if (pid <= 0)
    {
      printf("Monitorul nu este pornit.\n");
      return;
    }
  
  if (kill(pid, SIGTERM) == 0)
    {
      printf("Monitor oprit (PID %d).\n", pid);
      pid = -1;
    }
  else
    {
      perror("Eroare la oprirea monitorului");
    }
}



void start_monitor() {
    if (pid > 0)
      {
        printf("Monitorul ruleaza deja cu PID = %d\n", pid);
        return;
      }

    pid = fork();

    if (pid < 0)
      {
        perror("Eroare la rularea monitorului\n");
      }
    else
      {
	if(pid == 0)
	  {
	    printf("Monitor pornit cu PID = %d\n", getpid());
	    while(1)
	      {
		pause();
	      }
	  }
      }
}

void exit_monitor()
{
  if(pid >= 0)
    {
      stop_monitor();
    }
  
  printf("Programul se opreste :(\n");
  exit(0);
}


void process_command(const char command[100])
{
  if(strcmp(command, "start_monitor") == 0)
    {
      start_monitor();
      return;
    }

  if(strcmp(command, "stop_monitor") == 0)
    {
      stop_monitor();
      return;
    }

  if(strcmp(command, "exit") == 0)
    {
      exit_monitor();
      return;
    }

  if(strcmp(command, "list_treasures") == 0)
    {
      char id[10];
      scanf("%s", id);
      char path[50] = "./treasure_manager --list ";
      
      strcat(path, id);
      system(path);
  
      return;
    }
}


int main()
{
    char command[100];

    while(1)
      {
        printf(">>> ");
        scanf("%s", command);
	process_command(command);
	
      }
    
    return 0;
}
