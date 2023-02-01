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

char *search_replace(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub);

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
      for (int j = 0; j<i; j++){
        char *needle[]={"~/", "$$", "$?", "$!"};
        for (int needle_count =0; needle_count < 4; needle_count++){
          char *ret = search_replace(&word_copies[j], needle[needle_count], "p");
        }
      }
      int w=0;
      while(word_copies[w]){
        printf("%s\n", word_copies[w]);
        w++;
      }
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
