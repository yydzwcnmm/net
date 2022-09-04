#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

void func1(int no){
	printf("func1 %d\n",no);

}


int main(){

	//注册 Ctrl+C
	signal(SIGINT,func1);

	getchar();

	sigset_t set;
	sigset_t old;


	sigemptyset(&old);
	sigemptyset(&set);
	sigaddset(&set,SIGINT);

	sigprocmask(SIG_BLOCK,&set,&old);//这句之后 ctrl+c 就不会再调用 func1 了

	printf("sigprocmask success; ctrl+c no callback\n");

	getchar();

	//解除
	sigprocmask(SIG_SETMASK,&old,NULL);


	printf("unset ctrl+c success\n");

	getchar();

	return 0;

}

// 编译 gcc sigprocmask1.c -o sigprocmask1
// 运行 ./sigprocmask1   结果如下图
//