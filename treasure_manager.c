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


void logg(const char* hunt_id, char* message)
{
  char path[50];
  strcpy(path, hunt_id);
  strcat(path, "/logged-hunt.bin");

  int f = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

  write(f, message, strlen(message));
  close(f);

  char link_path[50];
  strcpy(link_path, "logged_hunt-");
  strcat(link_path, hunt_id);
  symlink(path, link_path);
}


void add_treasure(const char* hunt_id)
{
  //creare director
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
	  char message[50] = "A fost creat hunt-ul ";
	  strcat(message, hunt_id);
	  strcat(message, "\n");
	  logg(hunt_id, message);
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

  close(f);
  logg(hunt_id, "A fost adaugat un treasure\n");
  
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
  logg(hunt_id, "Au fost listate treasure-urile\n");
}


void view(const char* hunt_id, char* treasure_id)
{
  char path[50];
  strcpy(path, hunt_id);
  strcat(path, "/treasures.bin");

  int f = open(path, O_RDONLY);

  treasure t;
  while(read(f, &t, sizeof(treasure)) == sizeof(treasure))
    {
      if(strcmp(t.id, treasure_id) == 0)
	{
	  printf("ID: %s | ", t.id);
	  printf("Username: %s | ", t.username);
	  printf("Location: %s | ", t.location);
	  printf("Clue: %s | ", t.clue);
	  printf("Value:%s\n", t.value);
	  break;
	}
    }

  char message[50] = "A fost vizualizat treasure-ul ";
  strcat(message, treasure_id);
  strcat(message, "\n");
  logg(hunt_id, message);

  close(f);
}


void remove_treasure(const char* hunt_id, char* treasure_id)
{
  char path[50];
  strcpy(path, hunt_id);
  strcat(path, "/treasures.bin");

  char path_new[50];
  strcpy(path_new, hunt_id);
  strcat(path_new, "/treasures_new.bin");

  int f = open(path, O_RDONLY);
  int f_new = open(path_new, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

  treasure t;
  while(read(f, &t, sizeof(treasure)) == sizeof(treasure))
    {
      if(strcmp(t.id, treasure_id) != 0)
	{
	  write(f_new, &t, sizeof(treasure));
	}
    }
  close(f);
  close(f_new);
  remove(path);
  rename(path_new, path);

  char message[50] = "Treasure ";
  strcat(message, treasure_id);
  strcat(message, " has been removed\n");
  logg(hunt_id, message);
}


int main(int argc, char** argv)
{
  if((argc < 3) || (argc > 4))//pt cand fac si alea cu 2 id-uri
    {
      printf("Usage: %s --add/list <hunt_id>\n", argv[0]);
      return 1;
    }

  if(strcmp(argv[1], "--add") == 0)
    {
      const char* hunt_id = argv[2];
      add_treasure(hunt_id);
    }

  if(strcmp(argv[1], "--list") == 0)
    {
      const char* hunt_id = argv[2];
      list(hunt_id);
    }

  if(strcmp(argv[1], "--view") == 0)
    {
      const char* hunt_id = argv[2];
      char* treasure_id = argv[3];
      view(hunt_id, treasure_id);
    }

  if(strcmp(argv[1], "--remove") == 0)
    {
      const char* hunt_id = argv[2];
      char* treasure_id = argv[3];
      remove_treasure(hunt_id, treasure_id);
    }
  
  return 0;
}
