#include "stdio.h"
#include <sys/stat.h>
#include "wrap.c"
#include "sys/epoll.h"
#include <fcntl.h>
#include "pub.c"
#include "dirent.h"
#include "signal.h"

#define PORT 8889


void sendhead(int cfd,int code,char *info,char *filetype,int length )
{
    //状态行
    char buf[1024];
    int len = 0;
    len = sprintf(buf,"HTTP/1.1 %d %s\r\n",code,info);
    send(cfd,buf,len,0);

    // 消息
    len=sprintf(buf,"Content-Type:%s\r\n",filetype);
    send(cfd,buf,len,0);

    if(length>0){

    //长度
        len=sprintf(buf,"Content-Type:%d\r\n",length);
        send(cfd,buf,len,0);
    }
    //空行
    send(cfd,"\r\n",2,0);

}

#if 0
// 发送文件
void send_file(int cfd, char *path,struct epoll_event *ev,int epfd,int flag)
{
        int fd = open(path,O_RDWR|777);
        int flags = fcntl(fd, F_GETFL);
        flag|=O_NONBLOCK;
        fcntl(fd,F_SETFL,flags);
		if(fd <0)
		{
			perror("");
			return ;
		}
		char buf[1024]="";
		int len =0;
        
    printf("path = %s,fd = %d\n",path,fd);
    if(fd<0){
        printf("err cant创建文件描述符\n");
        return ;
    }
    while (1)
    {
        len = read(fd,buf,sizeof(buf));
        if (len<0)
        {
            /* code */
            printf("send_file read err!\n");
            perror("read");
            close(fd);
            close(cfd);
            epoll_ctl(epfd,EPOLL_CTL_DEL,cfd,ev);
            break;
        }if (0==len)
        {   
            perror("read");
            printf("send_file read file down\n");
           
            break;
        }else{
            send(cfd,buf,len,0);
            printf("向客户端发送数据\n");
        }
        close(fd);
        if(1==flag){
        
        close(cfd);
        epoll_ctl(epfd,EPOLL_CTL_DEL,cfd,ev);
        }
    }
    

}

#endif

#if 1
void send_file(int cfd,char *path,struct epoll_event *ev,int epfd,int flag)
{
		int fd = open(path,O_RDWR);
        int flags = fcntl(fd, F_GETFL);
        flag|=O_NONBLOCK;
        fcntl(fd,F_SETFL,flags);
		if(fd <0)
		{
			perror("");
			return ;
		}
		char buf[1024]="";
		int len =0;
		while( 1)
		{

			len = read(fd,buf,sizeof(buf));
			if(len < 0)
			{
				perror("");
				break;

			}
			else if(len == 0)
			{
				break;
			}
			else
			{
				int n=0;
				n =  send(cfd,buf,len,0);
				printf("len=%d\n", n);

			}


		}
		close(fd);
		//关闭cfd,下树
		if(flag==1)
		{
			close(cfd);
			epoll_ctl(epfd,EPOLL_CTL_DEL,cfd,ev);
		}



}


#endif
//读取解析读取套接字内容
void read_client_request(int epfd,struct epoll_event *ev)
{   
    #if 0
    // 切换到web文件所在的目录
    char pwd_path[256] = "";
    char *paddr = getenv("PWD");
    strcpy(pwd_path,paddr);
    strcat(pwd_path,"/web-http");
    chdir(pwd_path);
    printf("path = %s",pwd_path);
    #endif

    char buf[2048]="";
    char tmp[1024]="";
    int ret;
	int len =0;
  

    int n = Readline(ev->data.fd, buf, sizeof(buf));
    printf("inside read_client_request \n");
    if(n<=0){
        printf("close or err\n");
        epoll_ctl(epfd,EPOLL_CTL_DEL,ev->data.fd,ev);
        close(ev->data.fd);
    }

    printf("[%s]\n", buf);

    while ((ret = Readline(ev->data.fd,tmp,sizeof(tmp))) >0 );
            
     
    //解析请求行
   
    char method[256] = "";
    char content[256] = "";
    char protocol[256] ="";
    sscanf(buf,"%[^ ] %[^ ] %[^ \r\n]",method,content,protocol);
    printf("[%s] [%s] [%s]\n",method,content,protocol);
    if(strcasecmp(method,"get")==0){
        
        char *strfile= content+1;
        
        char st1[] = "";
        printf("strfile = %s\n",strfile);
        //如果没有请求文件，默认请求当前目录
        if(strcmp(st1,strfile)==0)
            {strcpy(strfile,"./");}
        
            struct stat s;
           //文件不存在
            if(stat(strfile,&s)<0){
                printf("file not found\n");
                sendhead(ev->data.fd,404,"NOT FOUND",get_mime_type("*.html"),0);
                send_file(ev->data.fd,"404.jpg",ev,epfd,1);
            }else{
            // 请求的是一个文件
                if(S_ISREG(s.st_mode)){
                    printf("请求文件\n");
                    //发送报头     
                    sendhead(ev->data.fd,200,"ok",get_mime_type("strfile"),s.st_size);
                    send_file(ev->data.fd,"strfile",ev,epfd,1);



                } 
                
            //请求的是一个文件夹
                if (S_ISDIR(s.st_mode))
                {
                    int len = 0;
                    char buf[] = "";
                    printf("请求文件夹\n");
                    // 发送列表网页
                    sendhead(ev->data.fd,200,"ok",get_mime_type("*.html"),0);
                    //发送header.html
                    printf("服务器发送网页头\n");
                    send_file(ev->data.fd,"dir_header.html",ev,epfd,0);
                   
                    struct dirent **mylist = NULL;
                    int n = scandir(strfile,&mylist,NULL,alphasort);
                    if(n<0){
                        printf("scandir err!\n");
                    }
                    for(int i=0;i<n;i++){
                    
                        if(mylist[i]->d_type = DT_DIR){
                            //如果是目录
                            len = sprintf(buf,"<li><a href=%s/ >%s</a></li>",mylist[i]->d_name,mylist[i]->d_name);
                        }else{
                            //如果是文件
                            len = sprintf(buf,"<li><a href=%s >%s</a></li>",mylist[i]->d_name,mylist[i]->d_name);
                        }
                        //列表发送
                        send(ev->data.fd,buf,len,0);

                        free(mylist[i]);
                        }

                        free(mylist);
                        
                        //printf("当前目录%s\n",strfile);
                        printf("服务器发送网页尾\n");
                        send_file(ev->data.fd,"dir_tail.html",ev,epfd,1);
                        printf("发送网页完毕\n");
                }
                
            }

    }

}


int main(int argc,char const *argv[])
{

    // 切换到web文件所在的目录, 方便将WEB文件穿入读写套接字
    char pwd_path[256] = "";
    char *paddr = getenv("PWD");
    strcpy(pwd_path,paddr);
    strcat(pwd_path,"/web-http");
    // 切换到WEB目录      
    chdir(pwd_path);
    printf("path = %s",pwd_path);
    
    
    int lfd = tcp4bind(PORT,NULL);
    Listen(lfd,128);
    int epfd = epoll_create(1);
    struct epoll_event ev,evs[1024];
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);

    while (1)
    {
        /* code */
        int nread = epoll_wait(epfd,evs,1024,-1);
        if(nread<0){
            perror("");
            break;
        }
        else{

            for(int i = 0;i<nread; i++){
            //lfd
            if(evs[i].data.fd == lfd&&evs[i].events & EPOLLIN){
                struct sockaddr_in cliaddr;
                char ip[16] = "";
                socklen_t len = sizeof(cliaddr);
                int cfd = Accept(lfd,(struct sockaddr*)&cliaddr, &len );

                printf("new client ip=%s port=%d\n",
						inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ip,16),
						ntohs(cliaddr.sin_port));
                #if 1
                //设置cfd为非阻塞
					int flag = fcntl(cfd,F_GETFL);
					flag |= O_NONBLOCK;
					fcntl(cfd,F_SETFL,flag);
                #endif

                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);


            }

            //cfd
            else if (evs[i].events & EPOLLIN)
            {
                
                read_client_request(epfd, &evs[i]);

            }           

            }
        }

    }
    


    return 0;
}
