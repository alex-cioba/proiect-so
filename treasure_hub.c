#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

pid_t pid = -1;


void handler(int sig)
{
    if (sig == SIGUSR1)
      {
        int f = open("command.txt", O_RDONLY);
	if(f < 0)
	  {
	    perror("Eroare deschidere command.txt");
	    return;
	  }
	
        char buffer[100];
        int len = read(f, buffer, sizeof(buffer) - 1);
	if(len < 0)
	  {
	    perror("Eroare citire command.txt");
	    close(f);
	    return;
	  }
        close(f);

        buffer[len] = '\0';

        char cmd[35], param1[35], param2[35];
        int args = sscanf(buffer, "%s %s %s", cmd, param1, param2);
	

	pid_t nepotul = fork();
	if(nepotul == 0)
	  {
	    if (strcmp(cmd, "list_treasures") == 0 && args == 2)
	      {
		execl("./treasure_manager", "treasure_manager", "--list", param1, (char *)NULL);
		perror("Eroare list_treasures");
		_exit(1);
	      }
	    
	    if (strcmp(cmd, "view_treasure") == 0 && args == 3)
	      {
		execl("./treasure_manager", "treasure_manager", "--view", param1, param2, (char *)NULL);
		perror("Eroare view_treasure");
		_exit(1);
	      }
	    
	    if (strcmp(cmd, "list_hunts") == 0 && args == 1)
	      {
		execl("./treasure_manager", "treasure_manager", "--list_hunts", (char *)NULL);
		perror("Eroare list_hunts");
		_exit(1);
	      }

	    printf("Comanda necunoscuta sau parametri lipsa: %s\n", buffer);
	    _exit(1);
	  }
	else
	  {
	    if(nepotul > 0)
	      {
		waitpid(nepotul, NULL, 0);
		unlink("command.txt");
	      }
	    else
	      {
		perror("Eroare la fork()");
	      }
	  }
    }
    
    if (sig == SIGTERM)
      {
	write(STDOUT_FILENO, "Monitorul se opreste\n", 22);
	exit(0);
      }
}




void process_command(const char* command_line)
{
  if(strlen(command_line) == 0)
    {
      return;
    }
  if (strcmp(command_line, "stop_monitor") == 0) {
    if (pid > 0) {
      kill(pid, SIGTERM);
      waitpid(pid, NULL, 0);
      pid = -1;
    } else {
      printf("Monitorul e deja oprit\n");
    }
        return;
  }
  
  if (strcmp(command_line, "exit") == 0) {
    if (pid > 0) {
      kill(pid, SIGTERM);
      waitpid(pid, NULL, 0);
            pid = -1;
    }
    
    unlink("command.txt");
    printf("Programul se opreste :(\n");
    exit(0);
    }
  
  if (pid > 0)
    {
      int f = open("command.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      if (f < 0)
	{
	  perror("Eroare la open()");
	  return;
	}
      write(f, command_line, strlen(command_line)+1);
      fsync(f);
      close(f);
      kill(pid, SIGUSR1);
  }
  else
    {
      printf("Monitorul nu este pornit\n");
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
	if(pid == 0) //copil
	  {
	    struct sigaction sa;
	    sa.sa_handler = handler;
	    sigemptyset(&sa.sa_mask);
	    sa.sa_flags = SA_RESTART;

	    sigaction(SIGUSR1, &sa, NULL);
	    sigaction(SIGTERM, &sa, NULL);
	    
	    while(1)
	      {
		pause();
	      }
	  }
	else //parinte
	  {
	    printf("Monitor pornit cu PID = %d\n", pid);
	    
	    char command[100];
	    while(1)
	      {
		fgets(command, sizeof(command), stdin);
		command[strcspn(command, "\n")] = '\0';
		process_command(command);
	      }
	  }
      }
}



int main()
{
    char command[50];

    while(1)
      {
        fgets(command, sizeof(command), stdin);
	command[strcspn(command, "\n")] = '\0';
        if(strcmp(command, "start_monitor") == 0)
	  {
	    start_monitor();
	    break;
	  }
	else
	  {
	    printf("Monitorul nu este pornit (start_monitor)\n");
	  }
	
      }
    
    return 0;
}
