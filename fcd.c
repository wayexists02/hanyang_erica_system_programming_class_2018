#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define QSIZE 4096
#define ARRSIZE 512
#define NAMELEN 256

char Result[ARRSIZE][NAMELEN];

void FindDir(char* dirname);
int BreadthFirstSearch(char* dirname);
char* FindFileName(char* path);
void MoveToDir(char* origin, char* path);

int main(int argc, char* argv[])
{
	char input_dir[257];
	char origin_path[256];
	char result[512];
	int pid;

	if (argc > 2) {
		fprintf(stderr, "Usage must be one of followings.\n");
		fprintf(stderr, "$ %s\n", argv[0]);
		fprintf(stderr, "$ %s [dirname]\n", argv[0]);
		exit(1);
	}
	else if (argc == 1) {
		printf("Enter name of directory you want to go.\n");
		for (;;) {
			printf("%s/> ", getcwd(NULL, 0));

			/*scanf("%s", input_dir);*/
			setbuf(stdin, NULL);
			fgets(input_dir, 257, stdin);
			input_dir[strlen(input_dir) - 1] = '\0';
			fflush(stdin);

			if (strcmp(input_dir, ".") == 0) break;
			else if (strcmp(input_dir, "") == 0) continue;

			FindDir(input_dir);
		}
	}
	else if (argc == 2) {
		sprintf(origin_path, "%s", getcwd(NULL, 0));
		FindDir(argv[1]);

		/*
		pid = fork();
	
		if (pid == 0) {
			//printf("%s\n", getcwd(NULL, 0));
			execlp("exec", "exec", "bash", NULL);
		}
		else {
			waitpid(pid, NULL, 0);
		}
		*/

		MoveToDir(origin_path, getcwd(NULL, 0));
	}

	return 0;
}

void FindDir(char* dir)
{
	int len, input_i;
	int i;

	len = BreadthFirstSearch(dir);

	if (len == 0) {
		printf("| The directory '%s' is not found.\n", dir);
		return;
	}
	else if (len == 1) {
		chdir(Result[0]);
	}
	else {
		printf("| Which directory do you want to go?\n");
		for (i = 0; i < len; i++) {
			printf("| %d. %s\n", (i + 1), Result[i]);
		}

		for (;;) {
			printf("|> ");

			input_i = 0;
			scanf("%d", &input_i);
			if (input_i < 1 || input_i > len) {
				fprintf(stderr, "| INVALID INDEX!\n");
				continue;
			}
			fflush(stdin);
			break;
		}

		chdir(Result[input_i - 1]);
	}
}

int BreadthFirstSearch(char* dirname)
{
	char Queue[QSIZE][NAMELEN];
	int in, out;
	int len;
	char* current;
	char* filename;
	DIR* dir;
	struct dirent* ent;

	len = 0;
	in = 0; out = 0;

	sprintf(Queue[in++], "/home");

	while (in != out) {
		current = Queue[out++];
		out = out % QSIZE;

		if ((dir = opendir(current)) == NULL) continue;

		filename = FindFileName(current);

		if (strcmp(filename, dirname) == 0) {
			sprintf(Result[len], "%s", current);
			len++;
		}

		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;

			if (strcmp(current, "/") == 0)
				sprintf(Queue[in++], "%s%s", current, ent->d_name);
			else
				sprintf(Queue[in++], "%s/%s", current, ent->d_name);
			in = in % QSIZE;
		}

		closedir(dir);
	}

	return len;
}

char* FindFileName(char* path)
{
	char* ptr = path;

	while (strstr(ptr, "/") != NULL) {
		ptr = strstr(ptr, "/") + 1;
	}

	return ptr;
}

void MoveToDir(char* origin, char* path)
{
	int fd;
	char temp_path[512];
	
	sprintf(temp_path, "%s/temp_result", origin);

	if ((fd = open(temp_path, O_WRONLY)) == -1) {
		fprintf(stderr, "Could not open a temp file.\n");
		exit(1);
	}

	write(fd, path, strlen(path));

	close(fd);
}
