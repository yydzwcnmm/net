#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "wrap.c"
#include "wrap.h"

int main(void)
{

int lfd = tcp4bind(8000,"127.0.0.1");

Listen(lfd,128);

struct sockaddr_in clr;
socklen_t len = sizeof(clr);

while (1)
{
    char ip[16]= "";
    int cfd = Accept(lfd,(struct sockaddr *)&clr,&len);
    printf("new ip=%s,port = %d\n",inet_ntop(AF_INET,&clr.sin_addr.s_addr,ip,16),
    ntohs(clr.sin_port));
    pid_t pid;
    pid = fork();
    if( 0==pid ){

#if 1
        while (1)
        {
        
        close(lfd);
        printf("进入子进程循环\n");
        char buf[1024] = "";
        int n = read(cfd,buf,sizeof(buf));
        printf("%s\n",buf);
        write(cfd,buf,n);
        if(0==n){
            printf("client close\n");
            close(cfd);
            break;}
        }
#endif
      

    }


    if(pid>0){
        close(cfd);

    }
    
    close(lfd);
    return 0;


}



}