
#include <stdio.h>

char *Compress_Check(path)
char *path;
{
   static char newpath[] = "/tmp/temp_bitmap";
   char command[100];
   FILE *fp;

   if ((fp = fopen(path,"r")) == NULL) return (path);
   else fclose(fp);

   if ((path[strlen(path)-2] == '.')&&(path[strlen(path)-1] == 'Z'))
     {
       printf("Uncompressing bitmap %s.\n",path);
       sprintf(command,"uncompress -c <%s>/tmp/temp_bitmap",path);
       system(command);
       return(newpath);
     }
   else return(path);
}

