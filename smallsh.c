/*
 * Author: Tyler Snelgrove
 * CS 344
 * Program: Smallsh
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *search_replace(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub);
int parser(char **copies);

//global variables for parser
int pound_loc = -1;
int ampersand_loc = -1;
int less_loc = -1;
int less_file = -1;
int greater_loc = -1;
int greater_file = -1;
int token_start;

int main(int argc, char *argv[])
  {
    char *word_copies[515]={NULL};
    char *line = NULL;
    size_t n = 0;
    /*
     * Author: Ryan Gambord
     * Date: Unkown
     * URL: https://canvas.oregonstate.edu/courses/1901764/assignments/9087347?module_item_id=22777104
     */
    char *ifs=getenv("IFS");
    for (;;) {
      printf("%s", getenv("PS1"));
      //printf("%s", getenv("IFS"));
      ssize_t line_length = getline(&line, &n, stdin);
      
      if(line_length==-1){ // Error check for getline()
        err(errno, "getline()");
      }
    /* Reallocates line */
      //printf("%s", line);
  /* ... */
      if (!ifs){
        ifs=" \t\n";
      }
    /*
     * Loop that tokens line and also sends copies of the words
     * to word_copies array for later use.
     */ 
      char* token =strtok(line, ifs); 
      int i=0;
      while(token != NULL){
        char *copy_holder=strdup(token);
        if(!copy_holder) err(errno, "strdup()");//error check for strdup, returns NULL if failed
        word_copies[i]=copy_holder;
        token = strtok(NULL, ifs);
        i++;
      }
      /*
       * Used of expansion
       * Two for loops, to check each occurance with each word.
       * Calls search_replace and replaces copies in word_copies
       * if expansion needed.
       */
      char *home_get=getenv("HOME");
      strcat(home_get,"/");
      
      int pid_t=getpid();
      char pid_buffer[50];
      sprintf(pid_buffer,"%d", pid_t);
      
      int child_status;
      waitpid(pid_t, &child_status, 0);
      char status_buffer[10];
      sprintf(status_buffer, "%d", child_status);

      char *needle[]={"~/", "$$", "$?", "$!"};
      char *sub[]={home_get, pid_buffer, status_buffer, "holder"};
      for (int j = 0; j<i; j++){
        for (int needle_count =0; needle_count < 4; needle_count++){
          char *ret = search_replace(&word_copies[j], needle[needle_count], sub[needle_count]);
        }
      }
     // int w=0;
      //while(word_copies[w]){
        //printf("%s ", word_copies[w]);
        //w++;
      //}
    parser(word_copies);
   }    

  free(line);
  }

/*
 * Author: Ryan Gambord
 * Date: Nov 13, 2022
 * https://www.youtube.com/watch?v=-3ty5W_6-IQ
 */
char *search_replace(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub)
  {
    char *str = *haystack;
    size_t haystack_len = strlen(str);
    size_t const needle_len = strlen(needle), sub_len = strlen(sub);

    for (; (str = strstr(str, needle));){
      ptrdiff_t off = str - *haystack;
      if (sub_len > needle_len) {
        str = realloc(*haystack, sizeof **haystack * (haystack_len + sub_len - needle_len + 1));
        if (!str) goto exit;
        *haystack = str;
        str = *haystack + off;
      }
    memmove(str + sub_len, str + needle_len, haystack_len + 1 - off - needle_len);
    memcpy(str, sub, sub_len);
    haystack_len = haystack_len + sub_len - needle_len;
    str += sub_len;

    }
    str = *haystack;
    if (sub_len < needle_len) {
      str = realloc(*haystack, sizeof **haystack * (haystack_len +1));
      if (!str) goto exit;
      *haystack = str;
    }

exit:
    return str;
  }

int parser(char **copies)
  { 
    int i=0;
    while(copies[i]){
        if (strcmp(copies[i], "#") == 0) pound_loc=i;
        i++;
      }
    if(pound_loc<0) pound_loc=i;//if end of array and not # hit
    
    i=pound_loc;
    if ((i-2) >= 0){
    
      if (strcmp(copies[i-1], "&") == 0 ){// if ampersand, that index point becomes the check for < amd > this i decreased to match apmersand 
        ampersand_loc=i-1;
        i=i-1;
      }
      else{
        if(strcmp(copies[i-2], ">") != 0 && strcmp(copies[i-2], "<") != 0){

          token_start=-1;
          printf("%d", token_start);
          return 0;
        }
      
      }
      if(strcmp(copies[i-2],"<")==0){
        less_loc = i-2;
        less_file = i-1;
      }
      if(strcmp(copies[i-2],">")==0){
        greater_loc = i-2;
        greater_file = i-1;
      }
    }
    if ((i-4) >= 0){//checking for further < or > after initial < or >
    
      if(strcmp(copies[i-4],"<")==0){
        less_loc = i-4;
        less_file = i-3;
      }
      if(strcmp(copies[i-4],">")==0){
        greater_loc = i-4;
        greater_file = i-3;
      }
    }
   if(less_loc==-1 && greater_loc!=-1) token_start=greater_loc;
   else if (less_loc!=-1 && greater_loc==-1) token_start=less_loc;
   else{
     token_start= (less_loc>greater_loc) ? greater_loc : less_loc;
    
   }
   printf("#=%d, &=%d, < = %d, less_tock = %d, >=%d, greater_toc=%d token_start= %d",pound_loc, ampersand_loc, less_loc, less_file, greater_loc, greater_file, token_start);
   return 0;
  }
