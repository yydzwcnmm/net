#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
    
    
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    struct  sockaddr_in addr;
    struct  sockaddr_in clr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET,"192.168.12.128",&addr.sin_addr.s_addr);
    bind(lfd,(struct sockaddr *)&addr,sizeof(addr));
    listen(lfd,128);

    socklen_t len = sizeof(clr);
    char ip[16];
    int cfd = accept(lfd,(struct scokadrr *)&clr,&len);
    printf("new ip = %s port = %d\n",inet_ntop(AF_INET,&clr.sin_addr.s_addr,
				ip,16),	ntohs(clr.sin_port));

    
    while (1)
    {
        char buf[1024]="";
        sleep(2);
        write(cfd,"hello",5);
        read(cfd,buf,sizeof(buf));
        printf("%s\n",buf);
    }
    
    //关闭
	close(lfd);
	close(cfd);
	return 0;

    
    
}