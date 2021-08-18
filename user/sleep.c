#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]) {
	int time;
	if (argc < 2) {
		fprintf(2, "Error: sleep required a time\n");
		exit(1);
	}
	// 字符串转换成int
	time = atoi(argv[1]);
	sleep(time);
	
	exit(0);
}

