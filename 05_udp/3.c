#include <stdio.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <arpa/inet.h>
int main(int argc, char *argv[])
{

    unlink("sock.s");
    int lfd = socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un myaddr;
    myaddr.sun_family = AF_UNIX;
    strcpy(myaddr.sum_path,"sock.s");
    bind(lfd,(struct sockaddr *)&myaddr,sizeof(myaddr));
    listen(lfd,128);

    //ti qu
    struct sockaddr_un cliaddr;
    socklen_t lenc = sizeof(cliaddr);
    int cfd = accpet(lfd,(struct sockaddr *)&cliaddr,&lenc);

    printf("new cilent file = %s\n",cliaddr.sun_path);
    //read and writer
    char buf[1500]="";
    while (1)
    {
        
        int n = recv(cfd,buf,sizeof(buf),0);
        if (n<0){
            printf("erron\n");
            break;
        }
        else{
            printf("%s\n",buf);
            send(cfd,buf,n,0)
        }
        
    }
    close(lfd);
    close(cfd);



}