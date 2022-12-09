#include <cstdio>
#include <sys/fcntl.h>
#include <syscall.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
	if (argc != 3) {
		perror("This command gets only 2 files paths!");
		printf("%d", -1);
		return -1;
	}
	int fd1 = open(argv[1], O_RDONLY);
	int fd2 = open(argv[2], O_RDONLY);

	long end_fd1 = lseek(fd1, 0, SEEK_END); // set to the end of the file
	long end2_fd2 = lseek(fd2, 0, SEEK_END);
	
	long curr_fd1 = lseek(fd1, 0, SEEK_SET); // set current to the beggining of the file
	long curr_fd2 = lseek(fd2, 0, SEEK_SET);

	size_t size_fd1 = end_fd1 - curr_fd1;
	size_t size_fd2 = end2_fd2 - curr_fd2;

	if (size_fd1 != size_fd2) {
		close(fd1);
		close(fd2);
		printf("%d", 1);
		return 1;
	}


	char c1;
	char c2;
	int r1 = read(fd1, &c1,1);
	int r2 = read(fd2, &c2,1);

	while (r1>0 && r2>0) {
		if (c1!=c2) {
			close(fd1);
			close(fd2); 
			printf("%d", 1);
			return 1;
		}
		r1 = read(fd1, &c1, 1);
		r2 = read(fd2, &c2, 1);
	}

	close(fd1);
	close(fd2);
	printf("%d", 2);
	return 2;
	
}