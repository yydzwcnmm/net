


#include <stdio.h>
#include <fcntl.h>
#include "wrap.h"
#include <sys/epoll.h>
int main(int argc, char *argv[])
{
	//创建套接字 绑定
	int lfd = tcp4bind(8000,NULL);
	//监听
	Listen(lfd,128);
	//创建树
	int epfd = epoll_create(1);
	//将lfd上树
	struct epoll_event ev,evs[1024];
	ev.data.fd = lfd;
	ev.events = EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
	//while监听
	while(1)
	{
		int nready = epoll_wait(epfd,evs,1024,-1);//监听
		printf("epoll wait _________________\n");
		if(nready <0)
		{
			perror("");
			break;
		}
		else if( nready == 0)
		{
			continue;
		}
		else//有文件描述符变化
		{
			for(int i=0;i<nready;i++)
			{
				//判断lfd变化,并且是读事件变化
				if(evs[i].data.fd == lfd && evs[i].events & EPOLLIN)
				{
					struct sockaddr_in cliaddr;
                    char ip[16] = "";
                    socklen_t len = sizeof(cliaddr);
                    int cfd =   Accept(lfd,(struct sockaddr *)&cliaddr,&len);
                    printf("new client ip=%s port =%d\n",inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ip,16)
							,ntohs(cliaddr.sin_port));
                    //将cfd上树
                    ev.data.fd = cfd;
                    ev.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,cdf,&ev);


				
				}
				else if( evs[i].events & EPOLLIN)//cfd变化 ,而且是读事件变化
				{
					char buf[1024] = "";
                    int n = read(evs[i].data.fd,buf,sizeof(buf));
                    if (n<0){
                        perror("");
                        epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,&evs[i]);
                    }
                    if (n==0){
                        printf("client close");
                        Close(evs[i].data.fd);
                        epoll_ctl(epfd,EPOLL_CTL_DEL,evs[i].data.fd,&evs[i]);
                    }
                    else{
                        printf("%s\n",buf);
                        Write(evs[i].data.fd,buf,sizeof(buf));
                    }


				}
			
			
			}
		
		}
	
	
	}
	
	return 0;
}


