

#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include "executor.h"
#include <sysexits.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

#define BUFFER_SIZE 1024



static int execute_aux(struct tree *t, int o_input_fd, int p_output_fd);



int execute(struct tree *t) {
   execute_aux(t, STDIN_FILENO, STDOUT_FILENO); /*processing the root node first*/

  /* print_tree(t);*/ 

   return 0;
}
/*
static void print_tree(struct tree *t) {
   if (t != NULL) {
      print_tree(t->left);

      if (t->conjunction == NONE) {
         printf("NONE: %s, ", t->argv[0]);
      } else {
         printf("%s, ", conj[t->conjunction]);
      }
      printf("IR: %s, ", t->input);
      printf("OR: %s\n", t->output);

      print_tree(t->right);
   }
}
*/
static int execute_aux(struct tree *t, int p_input_fd, int p_output_fd) {
   int pid, status, new_pid, fd, fd2, success_status = 0, failure_status = -1, new_status;
   int pipe_fd[2], pid_1, pid_2;


   /*none conjuction processing*/
   if (t->conjunction == NONE){

      /*process any entered linux commands*/
       if((pid = fork()) < 0){
          perror("fork");
          exit(-1);
       }
       /*parent processing*/
       if(pid != 0){
          wait(&status);   
          return status;  
       }

       /*child processing*/
       if (strcmp(t->argv[0], "date") == 0) {
      system("date");
      return 0;
   }
   
   if (strcmp(t->argv[0], "mkdir") == 0) {
      if (t->argv[1] != NULL) {
         if (mkdir(t->argv[1], 0777) == -1) {
            perror("mkdir");
            return -1;
         }
      } else {
         fprintf(stderr, "Missing directory name.\n");
         return -1;
      }
      return 0;
   }
   if (strcmp(t->argv[0], "rm") == 0) {
      if (t->argv[1] != NULL) {
         if (remove(t->argv[1]) == -1) {
            perror("rm");
            return -1;
         }
      } else {
         fprintf(stderr, "Missing file/directory name.\n");
         return -1;
      }
      return 0;
   }
   if (strcmp(t->argv[0], "pwd") == 0) {
      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
         printf("Current working directory: %s\n", cwd);
      } else {
         perror("getcwd");
         return -1;
      }
      return 0;
   }

   if (strcmp(t->argv[0], "echo") == 0) {
      int i = 1;
      while (t->argv[i] != NULL) {
         printf("%s ", t->argv[i]);
         i++;
      }
      printf("\n");
      return 0;
   }
if (strcmp(t->argv[0], "ls") == 0) {
    if (t->argv[1] != NULL) {
        const char* args[3];
        args[0] = "ls";
        args[1] = t->argv[1];
        args[2] = NULL;
        execvp(args[0], (char* const*)args);
    } else {
        const char* args[2];
        args[0] = "ls";
        args[1] = NULL;
        execvp(args[0], (char* const*)args);
    }
    perror("ls");
    return -1;
}

if (strcmp(t->argv[0], "cp") == 0) {
    if (t->argv[1] != NULL && t->argv[2] != NULL) {
        const char* args[4];
        args[0] = "cp";
        args[1] = t->argv[1];
        args[2] = t->argv[2];
        args[3] = NULL;
        execvp(args[0], (char* const*)args);
    } else {
        fprintf(stderr, "Missing source or destination file name for cp\n");
        return -1;
    }
    perror("cp");
    return -1;
}

if (strcmp(t->argv[0], "mv") == 0) {
    if (t->argv[1] != NULL && t->argv[2] != NULL) {
        const char* args[4];
        args[0] = "mv";
        args[1] = t->argv[1];
        args[2] = t->argv[2];
        args[3] = NULL;
        execvp(args[0], (char* const*)args);
    } else {
        fprintf(stderr, "Missing source or destination file name for mv\n");
        return -1;
    }
    perror("mv");
    return -1;
}

if (strcmp(t->argv[0], "touch") == 0) {
    if (t->argv[1] != NULL) {
        const char* args[3];
        args[0] = "touch";
        args[1] = t->argv[1];
        args[2] = NULL;
        execvp(args[0], (char* const*)args);
    } else {
        fprintf(stderr, "Missing file name for touch\n");
        return -1;
    }
    perror("touch");
    return -1;
}

if (strcmp(t->argv[0], "fahadsatti") == 0) {
    printf("what else?\n");
    return 0;
}

if (strcmp(t->argv[0], "eat") == 0) {
	int i;
	int position = 0;
    int maxPosition = 25; 

    while (position < maxPosition) {
    	if(position % 2 == 0) {
    	
        printf("%*s  _______\n", position, "");
        printf("%*s /       \\\n", position, "");
        printf("%*s/     O   \\\n", position, "");
        printf("%*s|         |\n", position, "");
        printf("%*s\\        /\n", position, "");
        printf("%*s \\______/\n", position, "");
    	} else {
        printf("%*s  _______\n", position, "");
        printf("%*s /       \\\n", position, "");
        printf("%*s/     O   \\\n", position, "");
        printf("%*s|        /\n", position, "");
        printf("%*s\\       \\\n", position, "");
        printf("%*s \\_______/\n", position, "");
        }

        usleep(200000);


        for (i = 0; i < 8; i++) {
            printf("\033[F"); 
            printf("\033[K");  
        }

        position++;  
    }
    return 0;

   }
   
   if (strcmp(t->argv[0], "disk-usage") == 0) {
        char buffer[1024];
        FILE* pipe_stream;
         pipe_stream = popen("du -h", "r");
        
        if (pipe_stream == NULL) {
            perror("disk-usage");
            return -1;
        }
        
        
        while (fgets(buffer, sizeof(buffer), pipe_stream) != NULL) {
            printf("%s", buffer);
        }
        
        pclose(pipe_stream);
    
    return 0;
   }

if (strcmp(t->argv[0], "file-metadata") == 0) {
   if (t->argv[1] != NULL) {
      struct stat fileStat;
      if (stat(t->argv[1], &fileStat) == 0) {
         printf("File: %s\n", t->argv[1]);
         printf("Size: %lld bytes\n", fileStat.st_size);
         printf("Owner User ID: %d\n", fileStat.st_uid);
         printf("Owner Group ID: %d\n", fileStat.st_gid);
         printf("Permissions: %o\n", fileStat.st_mode & 0777);
         printf("Last Access Time: %s", ctime(&fileStat.st_atime));
         printf("Last Modification Time: %s", ctime(&fileStat.st_mtime));
         printf("Last Status Change Time: %s", ctime(&fileStat.st_ctime));
      } else {
         perror("file-metadata");
         return -1;
      }
   } else {
      fprintf(stderr, "Missing file name.\n");
      return -1;
   }
   return 0;
}

if (strcmp(t->argv[0], "read") == 0) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    int fileDescriptor;

    if (t->argv[1] == NULL) {
        fprintf(stderr, "Missing file name.\n");
        return -1;
    }
    fileDescriptor = open(t->argv[1], O_RDONLY);

    if (fileDescriptor == -1) {
        perror("read");
        return -1;
    }


    while ((bytesRead = read(fileDescriptor, buffer, BUFFER_SIZE)) > 0) {
        
        write(STDOUT_FILENO, buffer, bytesRead);
    }

    if (bytesRead == -1) {
        perror("read");
        return -1;
    }

    close(fileDescriptor);

    return 0;
}

if(strcmp(t->argv[0], "write") == 0) {
	int fd;
    ssize_t bytesWritten;
    char buffer[BUFFER_SIZE];
    
    if (t->argv[1] == NULL) {
        fprintf(stderr, "Usage: %s <filename>\n", t->argv[0]);
        return 1;
    }
    
    fd = open(t->argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    
    printf("Enter text to write (press 'q' to end):\n");
    while ((bytesWritten = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
    if (buffer[0] == 'q')
            break;
        if (write(fd, buffer, bytesWritten) != bytesWritten) {
            perror("write");
            close(fd);
            return 1;
        }
    }
    if (bytesWritten == -1) {
        perror("read");
        close(fd);
        return 1;
    }
    
    close(fd);
    return 0;
}

if (strcmp(t->argv[0], "summary") == 0) {
    const char* filename;
    FILE* file;
    int characterCount = 0;
    int wordCount = 0;
    int lineCount = 0;
    int inWord = 0;
    int ch;

    if (t->argv[1] != NULL) {
        filename = t->argv[1];

        file = fopen(filename, "r");
        if (file == NULL) {
            perror("summary");
            return -1;
        }

        while ((ch = fgetc(file)) != EOF) {
            characterCount++;

            if (isspace(ch)) {
                inWord = 0;
            } else {
                if (!inWord) {	
                    wordCount++;
                    inWord = 1;
                }
            }

            if (ch == '\n') {
                lineCount++;
            }
        }

        fclose(file);

        printf("Summary for file: %s\n", filename);
        printf("Character Count: %d\n", characterCount);
        printf("Word Count: %d\n", wordCount);
        printf("Line Count: %d\n", lineCount);
    } else {
        fprintf(stderr, "Missing filename.\n");
        return -1;
    }

    return 0;
}


if(strcmp(t->argv[0], "find") == 0) {

            char line[256];
            int lineCount = 0;
            int wasFound = 0;
        if (t->argv[1] != NULL && t->argv[2] != NULL) {
            FILE* file = fopen(t->argv[1], "r");
            if (file == NULL) {
                perror("find");
                return -1;
            }

            while (fgets(line, sizeof(line), file)) {
                lineCount++;
                if (strstr(line, t->argv[2]) != NULL) {
                    printf("Found '%s' in %s (Line %d)\n", t->argv[2], t->argv[1], lineCount);
                    wasFound = 1;
                }
            }
            
            if(!wasFound) printf("Not Found.\n");

            fclose(file);
        } else {
            fprintf(stderr, "Missing filename or text to find.\n");
            return -1;
        }

    return 0;}
    

    if (strcmp(t->argv[0], "replace") == 0) {
    

    const char* filename;
    const char* searchText;
    const char* replaceText;
    FILE* file;
    FILE* tempFile;
    char buffer[BUFFER_SIZE];
    size_t searchTextLength;
    char* match;
    size_t prefixLength;
        if (t->argv[1] != NULL && t->argv[2] != NULL && t->argv[3] != NULL) {
            filename = t->argv[1];
            searchText = t->argv[2];
            replaceText = t->argv[3];

            file = fopen(filename, "r");
            if (file == NULL) {
                perror("replace");
                return -1;
            }

            tempFile = fopen("temp.txt", "w");
            if (tempFile == NULL) {
                perror("replace");
                fclose(file);
                return -1;
            }

            searchTextLength = strlen(searchText);

            while (fgets(buffer, sizeof(buffer), file)) {
                match = strstr(buffer, searchText);
                if (match != NULL) {
                    prefixLength = match - buffer;
                    fwrite(buffer, sizeof(char), prefixLength, tempFile);
                    fputs(replaceText, tempFile);
                    fputs(match + searchTextLength, tempFile);
                } else {
                    fputs(buffer, tempFile);
                }
            }

            fclose(file);
            fclose(tempFile);

            if (remove(filename) != 0) {
                perror("replace");
                return -1;
            }

            if (rename("temp.txt", filename) != 0) {
                perror("replace");
                return -1;
            }
        } else {
            fprintf(stderr, "Missing filename or texts for replacement.\n");
            return -1;
        }
        
    return 0;
    }




   }

   else if(t->conjunction == AND){

      /*Process left subtree, then right subtree if leftsubtree is processed correctly*/ 
      new_status = execute_aux(t->left, p_input_fd, p_output_fd);
      
      /*if the left_subtree was processed was succesfully, process the right subtree */
      if(new_status == 0){
        return execute_aux(t->right, p_input_fd, p_output_fd);
      }
      else {
         return new_status;
      }
   }

   /*of the current conjuction is of type pipe, process*/
   else if(t->conjunction == PIPE){
      
   if(t->left->output != NULL){
      printf("Ambiguous output redirect.\n");
      return failure_status;
   }

   if(t->right->input != NULL){
      printf("Ambiguous output redirect.\n");
      return failure_status;
   }

      /* create a pipe */
      if(pipe(pipe_fd) < 0){
         perror("pipe");
         exit(-1);
      }

      /*fork(creating child process)*/
      if((pid_1 = fork()) < 0){
         perror("fork");
      }
      

      if(pid_1 == 0) { /*child 1  process code*/
      
      close(pipe_fd[0]); /*close read end since were not using it*/

         if(t->input != NULL){

            if ((fd = open(t->input, O_RDONLY)) < 0) {
               perror("open");
               exit(-1);
            } 

            /*make the pipes write end the new standard output*/
            dup2(pipe_fd[1], STDOUT_FILENO);

            /*pass in input file if exists and the pipes write end for the output file descriptor*/
            execute_aux(t->left, fd, pipe_fd[1]);

            /*closed pipe write end*/
            if(close(pipe_fd[1]) < 0){
               perror("close");
               exit(-1);
            }

            /*close input file*/
            if(close(fd) < 0){
               perror("close");
               exit(-1);
            }
         }
         /*if no input file was provided, use parent file descriptor as the input file*/
         else {
            dup2(pipe_fd[1], STDOUT_FILENO);
            execute_aux(t->left, p_input_fd, pipe_fd[1]); /*process the left subtree*/
            if(close(pipe_fd[1] < 0)){
               perror("close");
               exit(-1);
            }
         } 
      }
      else {
         /*create second child to handle right subtree*/

         if((pid_2 = fork()) < 0){
            perror("fork");
            exit(-1);
          }

         if(pid_2 == 0){ /*child two code*/

            /*close write end of pipe, dont need it*/
            close(pipe_fd[1]);

            /*use output file if provided, else use parent output file descriptor*/
            if(t->output != NULL){

               /*open provided output file if exists*/
               if((fd = open(t->output, O_WRONLY | O_CREAT | O_TRUNC)) < 0){
                  perror("open");
                  exit(-1);
               }

               /*make the pipes read end the new standard input*/
               dup2(pipe_fd[0], STDIN_FILENO);
               execute_aux(t->right, pipe_fd[0], fd); /*process the right subtree*/
              
               /*close pipe read end*/
               if(close(pipe_fd[0]) < 0){
                  perror("close");
                  exit(-1);
                }

               /*closed output file*/
               if(close(fd) < 0){
                  perror("close");
                  exit(-1);
               }
            }

            else {
              dup2(pipe_fd[0], STDIN_FILENO);
              execute_aux(t->right, pipe_fd[0], p_output_fd); /*process the right subtree*/
            }

         } else {
           
           /* Parent has no need for the pipe */
           close(pipe_fd[0]);
           close(pipe_fd[1]);

           /* Reaping each child */
           wait(&status);
           wait(&status);
         }
      }
   }
   /*if current conjuction enumerator is of type subshell, process commands
     in a child process and return success/failure status after executing*/
   else if(t->conjunction == SUBSHELL){

     /*fork(creating child process)*/
      if((new_pid = fork()) < 0){
         perror("fork");
         exit(-1);
      }

     /*parent code*/
      if(new_pid != 0){
         wait(&status); /*wait for child*/
         exit(WEXITSTATUS(status)); /*return the status*/
      }
      else {
      /*child code*/

      /*get input from input file if it exists*/
      if(t->input != NULL){
         /*open input file*/
            if ((fd = open(t->input, O_RDONLY)) < 0) {
               perror("fd");
               exit(-1);
            } 
            /*changed standard input to use input from input file*/
            if(dup2(fd, STDIN_FILENO) < 0){
               perror("dup2");
               exit(-1);
            }
            /*close file descriptor(input file)*/
            if(close(fd) < 0){
               perror("fd");
               exit(-1);
            }
          }
          /*if there was no input file use provided parent file descriptor(the file parameter of the function)*/
          else {
             fd = p_input_fd;
          }

      /*use an output file if it exists*/
      if(t->output != NULL){
            if ((fd2 = open(t->output,  O_WRONLY | O_CREAT | O_TRUNC)) < 0) {
               perror("fd");
               exit(-1);
            } 

            /*change standard output to the output file(output will be written to output file)*/
            if(dup2(fd2, STDOUT_FILENO)){
               perror("dup2");
               exit(-1);
            }
            /*close the output file descriptor*/
            if(close(fd2) < 0){
               perror("fd");
               exit(-1);
            }
          }
          /*if no outputfile exists write output to provided parent output file */
          else {
            fd2 = p_output_fd;       
          }
         /*execute left subtree and get status*/
        status =  execute_aux(t->left, fd, fd2);
        /*return with value of statujs(0: success, -1: failure*/
        exit(status);
      }

   }
 
  return success_status;
}

