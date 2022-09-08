#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    //创建套接字
    int fd = socket(AI_INET,SOCK_DGRAM,0);
    //绑定
    struct sockaddr_in myaddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(9000);
    myaddr.sin_addr.s_addr = int_addr("127.0.0.1");
    bind(fd,(struct sockaddr *)&myaddr,sizeof(myaddr));
    // read and wriet
    char buf[1500] = "";
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    while (1)
    {
        /* code */
        int n = recvfrom(fd,buf,sizeof(buf),0,(struct sockaddr *)&cliaddr,&len)
        printf("%s\n",buf);
        
        sendto(fd,buf,n,0,(struct sockaddr *)&cliaddr,len);



    }
    




}