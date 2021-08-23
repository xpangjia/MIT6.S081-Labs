
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PRIME_NUMS 35

// 子进程方法,筛除非质数
void child(int* pl);

int main(void){
	int pd[2];
	
	// 开启管道
	pipe(pd);

	// 开启子进程
	int pid = fork();

	if (pid < 0) { 
		printf("Error, fork failed");
		exit(1);
	} else if (pid > 0) {
		// 父进程
		
		close(pd[0]);

		for (int i=2; i<PRIME_NUMS+1; i++) {
			write(pd[1], &i, sizeof(int));	
		}
		close(pd[1]);

		// 等待子进程结束
		wait((int *) 0);
	} else if (pid == 0) {
		// 子进程
		child(pd);
	}

	exit(0);
}


void child(int* pl) {
	// 右边管道的文件描述符
	int pr[2];

	close(pl[1]);

	int prime;
	// 读取最左边的数
	int result = read(pl[0], &prime, sizeof(int));

	if (result == 0) {
		exit(0);
	}

	// 创建右边管道,作为下一步的筛子
	pipe(pr);


	int pid = fork();
	if (pid < 0) {
		printf("Error, fork faild");
		exit(1);
	} else if (pid > 0) {
		close(pr[0]);

		// 最左边的数,属于质数(思考为什么?)
		printf("prime %d\n", prime);
		
		int n;
		while (read(pl[0], &n, sizeof(int)) != 0) {
			// 不能被最左边的质数整除的,有可能不是质数,
			// 继续放入管道,再次筛查	
			if (n % prime != 0) {
				write(pr[1], &n, sizeof(int));
			}
		}
		
		close(pr[1]);

		// 等待子进程结束
		wait((int *) 0);

		exit(0);
	} else if (pid == 0) {
		child(pr);
	}
}
