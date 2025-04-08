#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>


typedef struct treasure
{
  char id[50];
  char username[50];
  char location[50];
  char clue[100];
  char value[50];
}treasure;


void add_treasure(const char* hunt_id)
{
  struct stat st;

  if(stat(hunt_id, &st) == -1) //checks if hunt_id exists
    {
      if(mkdir(hunt_id, 0777) == -1)
	{
	  perror("Eroare creare director\n");
	  exit(1);
	}
      else
	{
	  printf("Directorul %s a fost creat\n", hunt_id);
	}
    }

  //file path (hunt_id/treasures.bin)
  char path[50];
  strcpy(path, hunt_id);
  strcat(path, "/treasures.bin");

  //file creation
  int f = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

  //reading
  treasure t;
  printf("ID: ");
  scanf("%s", t.id);

  printf("Username: ");
  scanf("%s", t.username);

  printf("Coordinates: ");
  scanf("%s", t.location);

  printf("Clue: ");
  fgets(t.clue, sizeof(t.clue), stdin);

  printf("Value: ");
  scanf("%s", t.value);
  

  //writting
  write(f, &t, sizeof(treasure));

  close(f); //close the binary file
  
}

void view(const char* hunt_id)
{
  char path[50];
  strcpy(path, hunt_id);
  strcat(path, "/treasures.bin");

  open(path, O_RDONLY);

  
}


int main(int argc, char** argv)
{
  if(argc < 3)
    {
      printf("Usage: %s --add <hunt_id>\n", argv[0]);
      return 1;
    }

  if(strcmp(argv[1], "--add") == 0)
    {
      const char* hunt_id = argv[2];
      printf("Am primit comanda add pentru hunt-ul: %s\n", hunt_id);
      add_treasure(hunt_id);
    }
  else
    {
      printf("Comanda necunoscuta: %s\n", argv[1]);
    }
  return 0;
}
