#include <SDL2/SDL_gamecontroller.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
   char ch, file_name[25];
   char buffer[10];
   FILE *fp;


   fp = fopen("hex_test.bin", "rb"); // read mode

   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }

   fread(buffer, sizeof(buffer), 4, fp);


   for(int i = 0; i < sizeof(buffer); i++) { 

      printf("%x\n", buffer[i]);


   }
   fclose(fp);
   return 0;
}