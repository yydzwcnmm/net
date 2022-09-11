

// 服务器 

#include <stdio.h>
#include "wrap.h"
#include <event.h>





void cfdcb(int lfd,short event,void *arg)
{
    #if 0
    struct event_base * base = (struct event_base *)arg;

    int cfd = Accept(lfd,NULL,NULL);
    struct event *ev = event_new(base,cfd,EV_READ|EV_PERSIST,cfdcb,NULL);
    event_add(ev,NULL);
    #endif
    
	int n = Read(cfd,buf,sizeof(buf));
	if(n <= 0)
	{
		perror("err or close\n");
		//event_del();//下树
	
	}
	else
	{
		printf("%s\n",buf);
		Write(cfd,buf,n);
	
	}



}

void lfdcb(int lfd,short event,void *arg)
{
	struct event_base *base = (struct event_base *)arg; 
	//提取新的cfd
	int cfd = Accept(lfd,NULL,NULL);
	//将cfd上树
	struct event *ev = event_new(base,cfd,EV_READ | EV_PERSIST,cfdcb,NULL);//初始化上树节点
	event_add(ev,NULL);
	
}

int main(int argc, char *argv[])
{
	
	//绑定
	int lfd = tcp4bind(8000,NULL);
	//监听
	Listen(lfd,128);
	//创建event_base根节点
	struct event_base * base = event_base_new();

	//初始化lfd上树节点
	 struct event *ev = event_new(base,lfd,EV_READ|EV_PERSIST,lfdcb,NULL);
	//上树
	event_add(ev,NULL);

	//循环监听
	event_base_dispath(base);
    //阻塞

	//收尾
	close(lfd);
	event_base_free(base);
	return 0;
}