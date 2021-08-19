#include "kernel/types.h"
#include "user/user.h"

// 定义读与写文件描述符的下标位置
#define READEND 0
#define WRITEEND 1

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2

// 要求:
// 1.父进程: 传一个字节给子进程;读取来自子进程的字节;打印<pid>: received pong
// 2.子进程: 打印<pid>: received ping;写一个字节给父进程;

int main(int argc, char *argv[]){
	
	int pfd[2];
	int cfd[2];

	char buf[10];
	
	int pid;

	pipe(pfd);
	pipe(cfd);

	pid = fork();

	if (pid < 0) {
		fprintf(STD_ERR, "fork happened error\n");
		exit(1);
	}
	else if (pid == 0) {
		// 子进程

		// 关闭父管道的写(因为父进程可能之前会传字节给子进程)
		close(pfd[WRITEEND]);
		// 关闭子管道的读()
		close(cfd[READEND]);
		
		// 从父管道的读里,读取父进程写的字节
		read(pfd[READEND], buf, 4);

		printf("%d: received %s\n", getpid(), buf);
		// 要求2:写字节进子管道,由父进程去读
		write(cfd[WRITEEND], "pong", 4);
		// 关闭
		close(cfd[WRITEEND]);
		exit(0);
	} else {
		// 父进程
		
		// 关闭父管道的读
		close(pfd[READEND]);
		// 关闭子管道的写(因为子进程可能之前会写字节给父进程)
		close(cfd[WRITEEND]);

		write(pfd[WRITEEND], "ping", 4);
		close(pfd[WRITEEND]);

		// 从子管道里读子进程写进去的字节
		read(cfd[READEND], buf, 4);
		printf("%d: received %s\n", getpid(), buf);
		
		exit(0);
	}

	exit(0);

}

