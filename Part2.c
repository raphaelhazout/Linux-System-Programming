#include <stdio.h>
#include <sys/fcntl.h>
#include <syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc,char* argv[])
{
    int fd1 = open("config.txt", O_RDONLY);
    char dir[50]="";
    char in[50]="";
    char out[50]="";
    char c;
    int count = 0;
    int i = 0;
    int rd1 = read(fd1, &c, 1);
    while (rd1>0) {                                                 //read the congif.txt file to 3 buffers.
        if (c == '\n') {
            i = 0;
            count++;
            rd1 = read(fd1, &c, 1);
        }
        if (count == 0) {
            dir[i++] = c;
        }
        if (count == 1) {
            in[i++] = c;
        }
        if (count == 2) {
            out[i++] = c;
        }
        rd1 = read(fd1, &c, 1);
    }
    close(fd1);
    
    char num1[10] = "";
    char num2[10] = "";
    int count3 = 0;
    char d_in;
    i = 0;
    int fd_in = open(in, O_RDONLY);
    int rd_in = read(fd_in, &d_in, 1);
    while (rd_in > 0) {
        if (d_in == ' ') {
            count3++;
            i = 0;
            rd1 = read(fd_in, &d_in, 1);
        }
        if (count3 == 0) {
            num1[i++] = d_in;
        }
        if (count3 == 1) {
            num2[i++] = d_in;
            break;
        }
        rd_in = read(fd_in, &d_in, 1);
    }
    pid_t pid = fork();                                                     // first fork!!!!!!
    if (pid == 0) {             //the child
        int fd2 = open("studentList.txt", O_CREAT | O_RDONLY | O_WRONLY,0777); //open file to the list of the students.
        close(1);
        dup(fd2);
        close(fd2);
        char* command = "ls";
        char* argument_list[] = {"ls",dir,NULL};
        int exe_ret = execvp(command,argument_list); // print all the students names to the destination file.
        if (exe_ret == -1) {
            printf("execvp faild!");
            _exit(-1);
        }
    }
    else {
        int waited1 = wait(NULL); //                           wait for the injection of the students to studList.

        int fd3 = open("studentList.txt", O_RDONLY); // for each student run the main.exe
        char** students = (char**)calloc(50, sizeof(char*));
        char buffer[50]="";
        int j = 0;
        i = 0;
        char d;
        int rd3 = read(fd3, &d, 1);
        while (rd3 > 0) {    //                          Create an array of the students;
            students[j] = (char*)calloc(50, sizeof(char));
            if (d == '\n') {
                strcpy(students[j++], buffer);
                i = 0;
                rd3 = read(fd3, &d, 1);
            }
            buffer[i++] = d;
            rd3 = read(fd3, &d, 1);
        }
        close(fd3);
        j = 0;
       int fd7 = open("grades.csv", O_CREAT | O_RDONLY | O_WRONLY, 0777);
       while (students[j] != nullptr) {            // for each student
           char path[256]="";
           strcpy(path, "./");
           strcat(path, dir);
           strcat(path, "/");
           strcat(path, students[j]);
           strcat(path, "/main.exe");
           pid_t pid2 = fork(); // for each student we will fork 
           if (pid2 < 0) {
               printf("error while creating child");
           }
           if (pid2 == 0) {
                int fd4 = open("out.txt", O_CREAT | O_RDONLY | O_WRONLY, 0777);
                close(1);
                dup(fd4);
                close(fd4);
                char* argument3_list[] = { path,num1,num2,NULL};
                int exe_ret2 = execvp(path, argument3_list); //          running the main.exe of the student and inject it to out.txt
                if (exe_ret2 == -1) {
                    _exit(-1);
                }
                exit(0);
            }
            else {
                int waited3 = waitpid(pid2,0,NULL);           // wait for the child!
                pid_t pid3 = fork();
                if (pid3 == 0) { // run the comp.out check
                    int fd5 = open("score.txt", O_RDONLY | O_CREAT | O_WRONLY, 0666);
                    close(1);
                    dup(fd5);
                    close(fd5);
                    char* arg_list[] = { "./comp.out",out,"out.txt",NULL };
                    int exe_ret3 = execvp("./comp.out", arg_list);
                    if (exe_ret3 == -1) {
                        _exit(-1);
                    }
                    exit(0);
                }
                else {
                    wait(NULL);
                    bool test = false;
                    int fd6 = open("score.txt", O_RDONLY);
                    char s;
                    int rd4 = read(fd6,&s, 1);
                    if (s == '2') {
                        test = true;
                    }
                    close(fd6);
           
                    char studentGrade[256]="";
                    strcpy(studentGrade, students[j]);
                    if (test) {
                        strcat(studentGrade, ",100\n");
                    }
                    else {
                        strcat(studentGrade, ",0\n");
                    }
                    write(fd7, studentGrade, sizeof(studentGrade));
                    free(students[j]);
                }
            }
            j++;
       }
       close(fd7);
       free(students);
    }
    remove("out.txt");
    remove("score.txt");
    remove("studentList.txt");
}
    