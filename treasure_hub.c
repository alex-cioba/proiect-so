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
      char hunt_id[10];
      scanf("%s", hunt_id);
      
      char path[50] = "./treasure_manager --list ";
      strcat(path, hunt_id);
      
      system(path);
      return;
    }

  if(strcmp(command, "view_treasure") == 0)
    {
      char hunt_id[10];
      char treasure_id[10];
      scanf("%s", hunt_id);
      scanf("%s", treasure_id);

      char path[50] = "./treasure_manager --view ";
      strcat(path, hunt_id);
      strcat(path, " ");
      strcat(path, treasure_id);

      system(path);
      return;
    }

  if(strcmp(command, "list_hunts") == 0)
    {
      char path[32] = "./treasure_manager --list_hunts";
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
