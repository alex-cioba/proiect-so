#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>

pid_t pid = -1;
int pipe_fd[2];
int closing = 0;



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
	    dup2(pipe_fd[1], STDOUT_FILENO);
	    dup2(pipe_fd[1], STDERR_FILENO);
	    close(pipe_fd[0]);
	    close(pipe_fd[1]);

	    
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
	exit(0); //iese din procesul copil
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
	exit(1);
      }
    else
      {
	if(pid == 0) //copil
	  {
	    close(pipe_fd[0]);
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
	    printf("Parintele are PID = %d\n", getpid());
	    return;
	  }
      }
}


int is_hunt(const char *hunt_id)
{
  struct stat statbuf;
  if(stat(hunt_id, &statbuf) != 0 || !S_ISDIR(statbuf.st_mode))
    {
      return 0;
    }

  char path[256];
  snprintf(path, sizeof(path), "%s/treasures.bin", hunt_id);

  if(access(path, F_OK) == 0)
    {
      return 1;
    }

  return 0;
}


void calculate_score()
{
  DIR *dir = opendir(".");
  struct dirent *entry;

  if(!dir)
    {
      perror("Eroare deschidere director");
      return;
    }

  while((entry = readdir(dir)) != NULL)
    {
      if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
	{
	  continue;
	}

      if(!is_hunt(entry->d_name))
	{
	  continue;
	}

      const char *name = entry->d_name;
      int fd[2];
      if(pipe(fd) < 0)
	{
	  perror("pipe");
	  continue;
	}

      pid_t pid = fork();
      if(pid < 0)
	{
	  perror("fork");
	  close(fd[0]);
	  close(fd[1]);
	  continue;
	}

      if(pid == 0) //copil
	{
	  close(fd[0]);
	  dup2(fd[1], STDOUT_FILENO);
	  close(fd[1]);

	  execl("./calculate_score", "calculate_score", name, NULL);
	  perror("execl");
	  _exit(1);
	}
      else //parinte
	{
	  close(fd[1]);
	  char buffer[256];
	  ssize_t n;
	  printf("[Score - %s]\n", name);
	  while((n = read(fd[0], buffer, sizeof(buffer)-1)) > 0)
	    {
	      buffer[n] = '\0';
	      printf("%s", buffer);
	    }

	  close(fd[0]);
	  waitpid(pid, NULL, 0);
	}
    }
  closedir(dir);
}



void *stop_monitor(void *arg)
{
  printf("Monitorul se opreste...\n");
  sleep(5);

  kill(pid, SIGTERM);
  waitpid(pid, NULL, 0);
  pid = -1;
  closing = 0;
  printf("Monitorul a fost oprit\n");
  printf(">>> ");
  fflush(stdout);
  return NULL;
}





void process_command(const char* command_line)
{
  if(strlen(command_line) == 0)
    {
      return;
    }

  if(closing == 1)
    {
      printf("Monitorul e in proces de oprire si nu se pot trimite comenzi\n");
      return;
    }

  if(strcmp(command_line, "start_monitor") == 0)
    {
      start_monitor();
      return;
    }
  
  if (strcmp(command_line, "stop_monitor") == 0)
    {
      if (pid > 0)
	{
	  closing = 1;
	  pthread_t oprire;
	  pthread_create(&oprire, NULL, stop_monitor, NULL);
	  pthread_detach(oprire);
	}
      else
	{
	  printf("Monitorul e deja oprit\n");
	}
      return;
    }
  
  if (strcmp(command_line, "exit") == 0)
    {
      if (pid > 0)
	{
	  printf("Monitorul e inca pornit (stop_monitor)\n");
	  return;
	}
      
      unlink("command.txt");
      printf("Programul se opreste :(\n");
      exit(0);
    }

  if(strcmp(command_line, "calculate_score") == 0)
    {
      calculate_score();
      return;
    }
  
  if (pid > 0)
    {
      int f = open("command.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      if (f < 0)
	{
	  perror("Eroare la open()");
	  return;
	}
      write(f, command_line, strlen(command_line));
      fsync(f);
      close(f);
      kill(pid, SIGUSR1);
  }
  else
    {
      printf("Monitorul nu este pornit (start_monitor)\n");
    }
}




void *citire_monitor(void *arg)
{
  char buffer[256];
  while(1)
    {
      ssize_t n = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
      if(n > 0)
	{
	  buffer[n] = '\0';
	  printf("[Parinte, PID = %d]:\n%s", getpid(), buffer);
	  if(closing == 0)
	    {
	      printf(">>> ");
	      fflush(stdout);
	    }
	}
    }
  return NULL;
}

void *citire_stdin(void *arg)
{
  char command[100];
  while(1)
    {
      if(closing == 0)
	{
	  printf(">>> ");
	  fflush(stdout);
	}
      if(fgets(command, sizeof(command), stdin))
	{
	  command[strcspn(command, "\n")] = '\0';
	  process_command(command);
	}
    }
  return NULL;
}



int main() //procesul parinte
{
    if (pipe(pipe_fd) < 0)
      {
        perror("Eroare la pipe()");
        exit(1);
      }

    pthread_t t1, t2;
    pthread_create(&t1, NULL, citire_stdin, NULL);
    pthread_create(&t2, NULL, citire_monitor, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
