#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILENAME "test.txt"

static int fd;

void open_file()
{
	printf("open file (%s) ... ", FILENAME);
	fflush(stdout);

	fd = open(FILENAME, O_RDWR);
	// fd = open(FILENAME, O_APPEND);
	if (fd < 0) {
		perror("failed");
		exit(1);
	}

	printf("OK\n");
}

void close_file()
{
	printf("close file ... ");
	fflush(stdout);

	int ret = close(fd);
	if (ret < 0) {
		perror("failed");
		exit(1);
	}

	printf("OK\n");
}

void write_file()
{
	const char m[] = "hello";

	ssize_t wrote = write(fd, m, sizeof(m));
	if (wrote < 0)
		perror("  write failed");
	else
		printf("  %zd bytes wrote.\n", wrote);
}

void write_many()
{
	const char m[] = "long message ..........................................\n";
	ssize_t write_size = 100 * 1024 * 1024;	 /* 100MB */
	// ssize_t write_size = 1 * 1024;	 /* 1KB */

	printf("  writing file ... ");
	fflush(stdout);

	ssize_t wrote;
	while (write_size > 0) {
		wrote = write(fd, m, sizeof(m));
		if (wrote < 0)
			perror("write failed");
		write_size -= wrote;
	}

	printf("OK\n");
}

void seek_head()
{
	off_t pos = lseek(fd, SEEK_SET, 0);
	if (pos < 0)
		perror("  seek failed");
	else
		printf("  seek succeed (pos = %ld)\n", pos); /* macのclangでは%lld */
}

void read_all(int print)
{
	seek_head();

	char buf[100];
	size_t nr_read = 0;
	ssize_t n;
	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		nr_read += n;
		if (print) {
			for (int i = 0; i < n; i++)
				putchar(buf[i]);
		}
	}
	if (n < 0)
		perror("  read failed:");
	fflush(stdout);
	printf("\n  %zu bytes read.\n", nr_read);
}

int command()
{
	char buf[10];
	printf("\n(w/r/h/c/W/i)> ");
	fflush(stdout);

	if (fgets(buf, sizeof(buf), stdin) == NULL) {
		printf("EOF\n");
		return 0;
	}

	switch (buf[0]) {
	case 'w':
		printf("  command: write\n");
		write_file();
		break;

	case 'r':
		printf("  command: read all\n");
		read_all(1);
		break;

	case 'c':
		printf("  command: read all but not print\n");
		read_all(0);
		break;

	case 'h':
		printf("  command: seek to head\n");
		seek_head();
		break;

	case 'W':
		printf("  command: write 100MB\n");
		write_many();
		break;

	case 'i':
		printf("  command: info\n");
		printf("  PID: %d\n", getpid());
		break;

	default:
		printf("  unknown command\n");
		break;
	}

	return 1;
}

int main(void)
{
	open_file();
	while (command())
		;
	close_file();
	return 0;
}
