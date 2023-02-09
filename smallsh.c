/*
 * Author: Tyler Snelgrove
 * CS 344
 * Program: Smallsh
 */

#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
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
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

char *search_replace(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub);
int parser(char **copies);
void exit_func(int exit_code);
int cd_func(char const *path);
int non_built_func(char *arguments[]);
void reset_globals();

//globals variabels for expansion
char *dolla_question=NULL;
char *dolla_exclamation = NULL;


//global variables for parser
int end_loc = -1;
int ampersand_loc = -1;
int less_loc = -1;
char *less_file;
int greater_loc = -1;
char *greater_file;
int token_start;

void handle_SIGINT(int signo){
fprintf(stderr, "Nothing");
}

int main(int argc, char *argv[])
  {
    struct sigaction ignore_action_sigint = {}, ignore_action_sigtstp={}, SIGINT_action={}, SIGINT_default={};
    ignore_action_sigint.sa_handler = SIG_IGN;
    ignore_action_sigtstp.sa_handler = SIG_IGN;
    SIGINT_default.sa_handler = SIG_DFL;
    SIGINT_action.sa_handler = handle_SIGINT;
    //sigfillset(&ignore_action_sigint.sa_mask);
    //sigfillset(&ignore_action_sigtstp.sa_mask);
    //sigfillset(&SIGINT_action.sa_mask);
    sigaction(SIGINT, &ignore_action_sigint, &SIGINT_default);
    sigaction(SIGTSTP, &ignore_action_sigtstp, NULL);
    /*
     * Author: Ryan Gambord
     * Date: Unkown
     * URL: https://canvas.oregonstate.edu/courses/1901764/assignments/9087347?module_item_id=22777104
     */ 
    char *line = NULL;
    size_t n = 0;
    char *ifs=getenv("IFS");
    for (;;) {
      char *word_copies[515]={NULL};
      
      pid_t x;
      int child_status;
      x=waitpid(0, &child_status, WUNTRACED | WNOHANG);
      if(x>0){
        if(WIFEXITED(child_status)){
          dolla_exclamation=realloc(dolla_exclamation, 10*sizeof(int));
          sprintf(dolla_exclamation, "%d", x);
          fprintf(stderr,"Child process %s done. Exit Status %d\n", dolla_exclamation ,WEXITSTATUS(child_status));
         
        }
        else if (WIFSTOPPED(child_status)){  
            kill(x,SIGCONT);
            dolla_exclamation=realloc(dolla_exclamation, 10*sizeof(int));
            sprintf(dolla_exclamation, "%d", x);
            fprintf(stderr,"Child process %s stopped. Continuing.\n", dolla_exclamation);  
        }
        else if (WIFSIGNALED(child_status)){
            fprintf(stderr, "HAPPY");
        }
      }

      char *pEnv;
      if(( pEnv=getenv("PS1"))!=NULL){
          fprintf(stderr,"%s", getenv("PS1"));
      }
      else{
          fprintf(stderr, "%s", "");
      }
      sigaction(SIGINT,&SIGINT_action, NULL);
      ssize_t line_length = getline(&line, &n, stdin);
      if (line_length==1) goto END_LOOP;
      
      if(line_length==-1){ // Error check for getline()
        err(errno, "getline()");
      }
      sigaction(SIGINT, &ignore_action_sigint, NULL);

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
        if(strcmp(copy_holder, "#")==0) break;//break out of the loop if # is found.
        if(!copy_holder) err(errno, "strdup()");//error check for strdup, returns NULL if failed
        word_copies[i]=copy_holder;
        token = strtok(NULL, ifs);

        i++;
      }
     
      /*
       * Checks for exit_func
       * Checks if too many arguments or if argument is a digit or not
       * Returns errors if need be or calls exit_func
       */
      if(strcmp(word_copies[0], "exit")==0){
        if(word_copies[2]) errx(1,"Too many arguments");
           
        int exit_val;
        if(word_copies[1]){
          if (!isdigit(*word_copies[1])) errx(1, "Argument not a digit");
          else exit_val = atoi(word_copies[1]);
        }
        else{ // if no argument provided, gets exit status of last foreground command
          int pid_t=getpid();
          char pid_buffer[50];
          sprintf(pid_buffer,"%d", pid_t);
          
          int child_status;
          waitpid(pid_t, &child_status, 0);
          char status_buffer[10];
          sprintf(status_buffer, "%d", child_status);
          
          exit_val=atoi(status_buffer);//FIX THIS*************************************
        }
        exit_func(exit_val);

      }

     
      if(strcmp(word_copies[0], "ls")==0 || strcmp(word_copies[0], "pwd")==0) goto NON_BUILT;
     
      /*
       * Used of expansion
       * Two for loops, to check each occurance with each word.
       * Calls search_replace and replaces copies in word_copies
       * if expansion needed.
       */
      char *home_get=getenv("HOME");

      int pid_t=getpid();
      char pid_buffer[50];
      sprintf(pid_buffer,"%d", pid_t);
      
      char *status_exit=NULL;
      if (!dolla_question){
        status_exit="0";
      }
      else{
        status_exit=dolla_question;
      }

      char *pid_background=NULL;
      if (dolla_exclamation == NULL){
        pid_background="";
      }
      else{
        pid_background=dolla_exclamation;
      }
            
      char *needle[]={"~", "$$", "$?", "$!"};
      char *sub[]={home_get, pid_buffer, status_exit, pid_background};
      for (int j = 0; j<i; j++){
        for (int needle_count =1; needle_count < 4; needle_count++){
          if(strncmp(word_copies[j],"~/", 2)==0){
            char *ret = search_replace(&word_copies[j], needle[0], home_get);
          }
          else{
            char *ret = search_replace(&word_copies[j], needle[needle_count], sub[needle_count]);
          }
        }

      }
      if(strcmp(word_copies[0], "cd")==0){
        int cd_ret = -1;
        if(word_copies[2]) errx(1, "Too many arguments");
        
        if(word_copies[1]){ 
          cd_ret =cd_func(word_copies[1]);
        }
        
        else{
          char *home_path=getenv("HOME");
          cd_ret = cd_func(home_path);
        }
        if(cd_ret == 0){
          goto END_LOOP;
        }
      }
     
    parser(word_copies);
NON_BUILT:
    
    non_built_func(word_copies);
    reset_globals();
END_LOOP:
    continue;
   }    

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
    if(strcmp(needle, "~") == 0) goto exit;
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

int parser(char *copies[])
  { 
    int i=0;
    while(copies[i]){
        i++;
      }
    end_loc=i;//end of array
    
    i=end_loc;
    if ((i-2) >= 0){
    
      if (strcmp(copies[i-1], "&") == 0 ){// if ampersand, that index point becomes the check for < amd > this i decreased to match apmersand 
        ampersand_loc=i-1;
        i=i-1;
      }
      else{
        if(strcmp(copies[i-2], ">") != 0 && strcmp(copies[i-2], "<") != 0){

          token_start=-1;
          goto exit;
        }
      
      }
      if(strcmp(copies[i-2],"<")==0){
        less_loc = i-2;
        less_file = copies[i-1];
      }
      if(strcmp(copies[i-2],">")==0){
        greater_loc = i-2;
        greater_file = copies[i-1];
      }
    }
    if ((i-4) >= 0){//checking for further < or > after initial < or >
    
      if(strcmp(copies[i-4],"<")==0){
        less_loc = i-4;
        less_file = copies[i-3];
      }
      if(strcmp(copies[i-4],">")==0){
        greater_loc = i-4;
        greater_file = copies[i-3];
      }
    }
   if(less_loc==-1 && greater_loc!=-1) token_start=greater_loc;
   else if (less_loc!=-1 && greater_loc==-1) token_start=less_loc;
   else{
     token_start= (less_loc>greater_loc) ? greater_loc : less_loc;
    
   }
exit:
   if (greater_loc>-1){

       free(copies[greater_loc]);
       copies[greater_loc]=NULL;
     }
   if (less_loc>-1){
       free(copies[less_loc]);
       copies[less_loc]=NULL;
       
   }
  if (ampersand_loc>-1){
      free(copies[ampersand_loc]);
      copies[ampersand_loc]=NULL;
  } 


   return 0;

  }

/*
 * Function Name: exit_func
 * Arguments: Int
 * Description: Sends SIGINT to child processes, prints to stderr, 
 * exits with argument int
 * Returns: None
 */
void exit_func(int exit_code)
{
  fprintf(stderr,"\nexit\n");
  kill(0, SIGINT);
  exit(exit_code);
}

int cd_func(char const *path)
{
  if(chdir(path) == -1) err(errno, "chdir");
  return 0;

}
/*
 * 
 * Description: Used for commands that aren't used in non built functions
 * Based off the example from canvas Process API page, Processes and I/O
 * Author: Unknown
 * Date: Unknown
 * Sources:https://canvas.oregonstate.edu/courses/1901764/pages/exploration-process-api-executing-a-new-program?module_item_id=22777102 
 *       https://canvas.oregonstate.edu/courses/1901764/pages/exploration-processes-and-i-slash-o?module_item_id=22777110
 * */
int non_built_func(char *arguments[])
{
  int child_status;
  pid_t spawnPid = fork();
   
  switch(spawnPid){
  case -1:
    perror("fork()\n");
    exit(1);
    break;
  case 0:
    if(greater_loc>-1){
      int targetFD = open(greater_file, O_RDWR | O_CREAT | O_TRUNC, 0777);
      if (targetFD == -1){
        perror("target open()");
        exit(1);
      }

      int result=dup2(targetFD, 1);
      if (result == -1){
        perror("target dup2()");
        exit(2);
      }
    }

    if(less_loc>-1){
      int sourceFD = open(less_file, O_RDWR);
      if (sourceFD == -1){
        perror("source open()");
        exit(1);
      }

      int result = dup2(sourceFD,0);
      if (result == -1){
        perror("source dup2()");
        exit(2);
      }      
    }
    if(ampersand_loc > -1){
       
    }
    execvp(arguments[0],arguments);
    perror("execvp()");
    exit(2);
    break;
  default:
    if(ampersand_loc==-1){
      spawnPid =waitpid(spawnPid, &child_status, 0);
      //int x = spawnPid;
      if(WIFEXITED(child_status)){
          dolla_question=realloc(dolla_question, 10*sizeof(int));
          asprintf(&dolla_question, "%d", WEXITSTATUS(child_status));
     } else{
          printf("Child %d exited abnormally due to signal %d\n", spawnPid, WTERMSIG(child_status));
     }
      break;
    }else{ 
      dolla_exclamation=realloc(dolla_exclamation, 10*sizeof(int));
      sprintf(dolla_exclamation, "%d", spawnPid);
      spawnPid=waitpid(spawnPid, &child_status, WNOHANG); 
     
      break;
   }
  
 
  }
  return 0;
}

/*
 * Desricption: Resets glbals variables used in parsing back to -1 for new loop
 * Arguments: None
 * Reutnrs: None
 */
void reset_globals()
{
    end_loc = -1;
    ampersand_loc = -1;
    less_loc = -1;
    free(less_file);
    less_file=NULL;
    greater_loc = -1;
    free(greater_file);
    greater_file=NULL;
    token_start = -1;
}
