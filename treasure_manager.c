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
  getchar(); //for the '\n' from last scanf()
  fgets(t.location, sizeof(t.location), stdin);
  t.location[strlen(t.location) - 1] = '\0';

  printf("Clue: ");
  fgets(t.clue, sizeof(t.clue), stdin);
  t.clue[strlen(t.clue) - 1] = '\0';

  printf("Value: ");
  scanf("%s", t.value);
  

  //writting
  write(f, &t, sizeof(treasure));

  close(f); //close the binary file
  
}

void list(const char* hunt_id)
{
  char path[50];
  strcpy(path, hunt_id);
  strcat(path, "/treasures.bin");

  int f = open(path, O_RDONLY);

  struct stat info;
  stat(path, &info);
  printf("Hunt: %s\n", hunt_id);
  printf("File size: %ld bytes\n", info.st_size);
  printf("Last modification: %ld\n", info.st_mtime);

  treasure t;
  printf("Treasures:\n");
  while(read(f, &t, sizeof(treasure)) == sizeof(treasure))
    {
      printf("ID: %s | ", t.id);
      printf("Username: %s | ", t.username);
      printf("Location: %s | ", t.location);
      printf("Clue: %s | ", t.clue);
      printf("Value:%s\n", t.value);
    }

  close(f);
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

  if(strcmp(argv[1], "--list") == 0)
    {
      const char* hunt_id = argv[2];
      list(hunt_id);
    }
  
  return 0;
}
