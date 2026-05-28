#include"process.h"
#include<string.h>
#include<unistd.h>

#define NUM 101
#define STYLE '='

void process_v1()
{
  char buffer[NUM];
  memset(buffer, 0, sizeof(buffer));
  int cnt = 0;
  const char* lables="|/-\\";
  int len = strlen(lables);

  while(cnt<=100)
  {
    printf("[%-100s][%d%%][%c]\r",buffer,cnt,lables[cnt%len]);
    fflush(stdout);
    buffer[cnt]=STYLE;
    cnt++;
    usleep(50000);
  }

  printf("\n");
}
