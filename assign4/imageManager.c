#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>


void generate_html() {
	char array[] = 	{
	 "<HTML><HEAD><TITLE>Your Title Here</TITLE></HEAD><BODY BGCOLOR=\"FFFFFF\"><CENTER><IMG SRC=\"clouds.jpg\" ALIGN=\"BOTTOM\">"
	  "</CENTER><HR><a href=\"http://somegreatsite.com\">Link Name</a>is a link to another nifty site<H1>This is a Header</H1><H2>"
	  "This is a Medium Header</H2>Send me mail at <a href=\"mailto:support@yourcompany.com\">support@yourcompany.com</a>"
	  ".<P> This is a new paragraph!<P> <B>This is a new paragraph!</B><BR> <B><I>This is a new sentence without a paragraph break, in bold italics.</I></B><HR></BODY></HTML> " };
printf("%s",array);

 }

void generate_log() {

}

void scan_dir () {
	struct dirent **namelist;
    int n;

   n = scandir(".", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        while (n--) {
            printf("%s\n", namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }
}

void* update_html(void* arg){

}


int main_2(int argc, char** argv) {

	int n= 2;

	char folder[4][10] = {{"File1.jpg"},{"File2.bmp"},{"File1.jpg"},{"File2.bmp"}}; // replace with getFileNames

	FILE* fp;

	struct stat file_stat;

	fp = fopen("imageManager.log","w+");

	fprintf(fp, "FileID\t\tFileName\t\tSize\t\tModTime\t\tThreadId\n");

	for ( int i=0; i < n;i ++) {

	if (lstat(folder[i], &file_stat) == -1)
        fprintf(stderr, "%s\n", strerror(errno));
    else
        {
          fprintf(fp, "%u\t\t%s\t\t%u\t\t%u\t\t%u\n",
          file_stat.st_ino, folder[i],file_stat.st_size,file_stat.st_mtime,i);
        }
	}

	return 1;

} //end of main
