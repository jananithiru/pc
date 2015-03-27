#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>

FILE* fp;
int g_num_files;
int g_t_count;
pthread_mutex_t mutex_lock;
pthread_t tid[10];

typedef struct {
	int num_of_files;
	char* p_folder_name;
	char folder_name[128];
} thread_data;

void generate_html2() {
	char array[] =
			{
					"<HTML><HEAD><TITLE>Your Title Here</TITLE></HEAD><BODY BGCOLOR=\"FFFFFF\"><CENTER><IMG SRC=\"clouds.jpg\" ALIGN=\"BOTTOM\">"
							"</CENTER><HR><a href=\"http://somegreatsite.com\">Link Name</a>is a link to another nifty site<H1>This is a Header</H1><H2>"
							"This is a Medium Header</H2>Send me mail at <a href=\"mailto:support@yourcompany.com\">support@yourcompany.com</a>"
							".<P> This is a new paragraph!<P> <B>This is a new paragraph!</B><BR> <B><I>This is a new sentence without a paragraph break, in bold italics.</I></B><HR></BODY></HTML> " };
	printf("%s", array);

}

void scan_dir_helper(void* arg) {

	// Get list of files in a directory in top level directory
	char* files[] = { { "File1.jpg" }, { "File2.bmp" }, {
			"File2.bmp" }, { "File2.bmp" }, };

	thread_data* td = (thread_data*) arg;

	for (int i = 0; i < td->num_of_files; i++) { // if directory, create a new thread

		if (td->num_of_files == 5) {

			thread_data t_dir;
			t_dir.p_folder_name = files[i];
			strcpy(t_dir.folder_name, files[i]);
			t_dir.num_of_files = 4;

			int err = pthread_create(&tid[g_t_count], NULL, &scan_dir_helper,
					&t_dir);
			if (err != 0)
				printf("\ncan't create thread :[%s]", strerror(err));
			g_t_count++;

		} else {

			struct stat file_stat;

			printf("%s",files[i]);
			printf("\nflush");
			printf("\nflush");

			if (lstat(files[i], &file_stat) == -1)
				fprintf(stderr, "%s\n", strerror(errno));
			else {
				fprintf(fp, "%u\t\t%s\t\t%u\t\t%u\t\t%u\n", file_stat.st_ino,
						files[i], file_stat.st_size, file_stat.st_mtime,
						tid[g_t_count]);
			}

		}
	}
}

int main(int argc, char** argv) {

	fp = fopen("imageManager.log", "w+");

	fprintf(fp, "FileID\t\tFileName\t\tSize\t\tModTime\t\tThreadId\n");

	// get mutex
	if (pthread_mutex_init(&mutex_lock, NULL) != 0) {
		printf("\n mutex init failed\n");
		return 1;
	}

	thread_data t_directory;
	strcpy(t_directory.folder_name,"Folder");
	t_directory.num_of_files = 5;

	int err = pthread_create(&tid[0], NULL, &scan_dir_helper, &t_directory);
	if (err != 0)
		printf("\ncan't create thread :[%s]", strerror(err));
	g_t_count++;

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	pthread_mutex_destroy(&mutex_lock);

	return 0;
} //eom
