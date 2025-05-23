#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct treasure
{
  char id[50];
  char username[50];
  char location[50];
  char clue[100];
  char value[50];
}treasure;

struct user_score
{
  char username[50];
  int score;
};

int find_user(struct user_score* users, int count, const char* name)
{
  for (int i = 0; i < count; ++i)
    {
      if (strcmp(users[i].username, name) == 0)
	{
	  return i;
        }
    }
  return -1;
}


int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      fprintf(stderr, "Usage: %s <hunt_id>\n", argv[0]);
      return 1;
    }

  char path[128];
  snprintf(path, sizeof(path), "%s/treasures.bin", argv[1]);
  
  int fd = open(path, O_RDONLY);
  if (fd == -1)
    {
      perror("Eroare la deschiderea fisierului treasures.bin");
      return 1;
    }
  
  struct user_score users[100];
  int user_count = 0;
  
  treasure t;
  int value;
  int idx;
  while (read(fd, &t, sizeof(treasure)) == sizeof(treasure))
    {
      value = atoi(t.value);
      idx = find_user(users, user_count, t.username);
      
      if (idx == -1)
	{
	  strcpy(users[user_count].username, t.username);
	  users[user_count].score = value;
	  user_count++;
        }
      else
	{
	  users[idx].score += value;
        }
    }
  
  close(fd);
  
  for (int i = 0; i < user_count; ++i)
    {
      printf("User: %s | Score: %d\n", users[i].username, users[i].score);
    }
  
  return 0;
}
