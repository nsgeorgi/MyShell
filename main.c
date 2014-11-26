#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
void parsing_redirection(char *cmd,char *buffer); // //apokwdikopoiei tin entoli tou xristi kai kali tin execve gia tin ektelesi tis entolis(apli entoli)
void parsing(char *cmd, char *buffer); //apokwdikopoiei tin entoli tou xristi kai kali tin execve gia tin ektelesi tis entolis(redirection)
void parsing_pipe(char *cmd,char *buffer);//apokwdikopoiei tin entoli tou xristi kai kali tin execve gia tin ektelesi tis entolis(pipe)
    int main()
    {

        char buffer[100];  // tin xrisimopoioume ws orisma gia na vroume to current directory
        char *cmd;
        cmd=(char *)malloc(100);  //i entoli tou xristi


        while(1)
        {
                printf("MYSHELL$:");
                fflush(stdout);
                getcwd(buffer,100); //current directory
                printf("%s:",buffer);
                fflush(stdout);

                //AUTA TA VALAME SE SXOLIA GIATI DEN LEITOURGOUSE TO SHELL SWSTA
                cmd=fgets(cmd,100,stdin); //ekxwreitai i entoli tou xristi
                //if (strcmp(cmd,"==>")==0)
                //parsing_redirection(cmd,buffer);
                //else if (strcmp(cmd,"|")==0)
                //parsing_pipe(cmd,buffer);
                //else
                if(strcmp(cmd,"exit")==0)
                {
                    exit(EXIT_SUCCESS);
                }
                pid_t pid=fork(); //id process
                if (pid==0)  //child process
                {




                  if(cmd[strlen(cmd)-2]=='&')
                  {

                    cmd[strlen(cmd)-2]='\0';

                  }

                  parsing(cmd,buffer);

                }

                else if(pid>0)             //parent process
                //elegxos gia background process an periexei i entoli tou xristi &
                {


                  if(cmd[strlen(cmd)-2]=='&')
                    cmd[strlen(cmd)-2]='\0';
                    else
                    wait(NULL);


                }
                else
                perror("fork failed");

        };
        return 0;
    }



void parsing(char *cmd,char *buffer) //apokwdikopoisi kai ektelesi tis entolis execve gia aplo input
{

    int i ;

    char *tmp[5]; //o dusdiastastos pou tha periexei tin entoli tou xristi xwris ta kena
    for (i=0; i<5; i++)
    tmp[i]=(char*)malloc(100);

    char *cmd2;
    cmd2=(char*)malloc(100);  //boithitikos pinakas gia to parsing


    cmd2=strtok(cmd," \t\r\n"); //spaw ton pinaka me tin strtok se kathe keno

    if (cmd2==NULL)
    perror("command not found ! ");

    while(cmd2!=NULL)
    {


    if (strcmp(cmd,"cd")==0)
    {
        cmd2=strtok(NULL," \t\r\n"); //krataw to path meta to i keno

        if (chdir(cmd2)==0) //ektelesi tis entolis chdir() gia na paw se ena directory
        {


        printf("Path found\n");
        fflush(stdout);
        break;
        }
        else
        perror("Invalid path");


    }


    else if (strcmp(cmd,"pwd")==0) //entoli gia na brw to current directory
    {

            getcwd(buffer,100); //current directory
            printf("%s:\n",buffer);
            fflush(stdout);
            break;
    }

    else              // gia opoiadipote alli entoli ektos tis pwd kai tis cd
    {
        int i=0;
        for (i=0; i<5; i++)
        {
            if(i==0)
            {
            strcpy(tmp[0],cmd2);
            continue;
            }
             tmp[i]=strtok(NULL," \t\r\n");

        }

        execvp(tmp[0],tmp);
        if(execvp(tmp[0],tmp)==-1) perror("execvp failed");

        break;
    }

    };


}


void parsing_redirection(char *cmd,char *buffer)
{
        int i;
        char *tmp1[5]; // edw ekxwreitai to prwto command prin to ==>
        char *tmp2[5]; // edw ekxwreitai to command meta to      ==>
        for (i=0; i<5; i++)
        tmp1[i]=(char*)malloc(100);
        tmp2[i]=(char*)malloc(100);
        char *cmd2;
        cmd2=(char*)malloc(100);  //boithitikos pinakas gia to parsing


        cmd2=strtok(cmd," \t\r\n");

        if (cmd2==NULL)
        perror("command not found ! ");
        // edw ginetai i ekxwrisi

        strcpy(tmp1[0],cmd2);
        tmp1[1]=strtok(NULL," \t\r\n");
        tmp1[2]='\0';
        strtok(NULL," \t\r\n");
        tmp2[0]=strtok(NULL," \t\r\n");
        tmp2[1]='\0';


        int fds[2]; // file descriptors
        int count;  // xrisimopoieitai gia na diavasei to stdout
        int fd;     // perigrafeas arxeiou
        char c;    //xrisimopoiitai gia grapsei kai na diavasei xaraktira
        pid_t pid;   // xrisimopoieitai gia tin fork()

        pipe(fds);

        // child process 1
        if (fork() == 0) {

        fd = open(tmp1[0], O_RDWR | O_CREAT, 0666); // i open epistrefei to perigrafea tou arxeiou
        printf("2");
        fflush(stdout);
        // i open tha epistrepsei -1 an uparxei sfalma stin klisi tis
        if (fd < 0) {
        printf("Error: %s\n", strerror(errno));
        return;
    }

        dup2(fds[0], 0); //antigrafei to stdin sto  prwto akro tou swlina pou einai mono read

        //to akro pou einai gia write kleinei giati den to xreiazomaste
        close(fds[1]);

        //Diavazei apo to stdout
        while ((count = read(0, &c, 1)) > 0)
        write(fd, &c, 1); // kai sti sunexeia grafei sto arxeio.


        execlp("echo", "echo", NULL);

        // child process 2
        } else if ((pid = fork()) == 0) {
        dup2(fds[1], 1); // antistoixei to stdout sto deksi akro tou pipe
         printf("3");
        fflush(stdout);
        // kleinoume to akro pou einai gia read giati den to xreiazomaste
        close(fds[0]);

        //emfanizei to outpout sto stdout
        execvp(tmp1[0], tmp1);
        perror("execvp failed");

        // parent process
        } else {
       printf("1\n");
        fflush(stdout);
        waitpid(pid,NULL,0); //perimenei i parent mexri na enimerwthei oti termatise to child
        close(fds[0]); //kleinoume kai ta 2 akra
        close(fds[1]);
  }

void parsing_pipe(char *cmd,char *buffer)
{
        int i;
        char *tmp1[5]; // edw ekxwreitai to prwto command prin to  |
        char *tmp2[5]; // edw ekxwreitai to command meta to      |
        for (i=0; i<5; i++)
        tmp1[i]=(char*)malloc(100);
        tmp2[i]=(char*)malloc(100);
        char *cmd2;
        cmd2=(char*)malloc(100);  //boithitikos pinakas gia to parsing


        cmd2=strtok(cmd," \t\r\n");

        if (cmd2==NULL)
        perror("command not found ! ");
        // edw ginetai i ekxwrisi

        strcpy(tmp1[0],cmd2);
        tmp1[1]=strtok(NULL," \t\r\n");
        tmp1[2]='\0';
        strtok(NULL," \t\r\n");
        tmp2[0]=strtok(NULL," \t\r\n");
        tmp2[1]='\0';


    int fds[2]; // perigrafeis arxeiwn
  pipe(fds);
  pid_t pid;

  // child process #1
  if (fork() == 0) {
    // antistoixei to stdin sto aristero akro tou pipe
    dup2(fds[0], 0);

    // den prokeitai na grapsoume sto deksi akro tou pipe opote kai to kleunoume
    close(fds[1]);

    // ektelesi tis deuteris entolis
    execvp(tmp2[0], tmp2);
    perror("execvp failed");

  // child process 2
  } else if ((pid = fork()) == 0) {
    // antistoixei to stdout sto deksi akro tou pipe
    dup2(fds[1], 1);

    // den prokeitai na diavasoume apo to ena akro tou pipe opote kai to kleinoume
    close(fds[0]);

    // ektelesi tis prwtis entolis
    execvp(tmp1[0], tmp1);
    perror("execvp failed");

  // parent process
  } else
    waitpid(pid, NULL, 0);
}

}








