#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


// 获取文件名
char* fmtname(char * path) {
	static char buf[DIRSIZ+1];
	char *p;

	// 从路径字符串的最后一个字符倒序遍历,直到'/',中间的部分就是文件名
	for (p = path + strlen(path); p >= path && *p != '/'; p--);

	p ++;

	if (strlen(p) >= DIRSIZ) return p;

	memmove(buf, p, strlen(p));


	buf[strlen(p)] = 0;

	return buf;
}


// 查找
void find(char* path, char *name) {
	char buf[512], *p;

	int fd;

	struct dirent de;
	// 文件信息
	struct stat st;


	// 判断文件能否打开
	if ((fd = open(path, 0)) < 0) {
		printf("find open %s error!", path);
		exit(1);
	}

	// 获取文件信息
	if (fstat(fd, &st) < 0) {
		printf("fstat %s error", path);
		close(fd);
		exit(1);
	}

	switch(st.type) {
		case T_FILE:
			if (strcmp(fmtname(path), name) == 0) {
				printf("%s\n", path);
			}
			break;
		case T_DIR:
			// 判断目录名称是否超过长度限制
			if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
				printf("find: %s path too long\n", path);
				break;
			}

			// 给目录加上'/'
			// 生成新路径,赋值给buf
			strcpy(buf, path);
			p = buf + strlen(buf);
			*p++ = '/';


			// 这里为什么用while?
			while (read(fd, &de, sizeof(de)) == sizeof(de)) {
				// 无效目录
				if (de.inum == 0) {
					continue;
				}

				// add de.name to path?
				memmove(p, de.name, DIRSIZ);


				p[DIRSIZ] = 0;

				// 跳过.和..路径
				if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
					continue;
				}

				find(buf, name);
			}
			break;	
	}

	close(fd);
}


int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("usage: find <path> <name>\n");
		exit(1);
	}

	find(argv[1], argv[2]);

	exit(0);
}

