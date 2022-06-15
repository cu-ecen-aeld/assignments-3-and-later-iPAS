#include <stdio.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]) {  // https://stackoverflow.com/questions/3024197/what-does-int-argc-char-argv-mean
    openlog(NULL, LOG_NDELAY, LOG_USER);

    printf("%d %s %s %s\n", argc, argv[0], argv[1], argv[2]);

    if (argc != 3) {
        syslog(LOG_ERR, "Two arguments: <str> <filename> are needed!");
        return 1;
    }

    const char *str = argv[2];
    const char *filename = argv[1];
    FILE *pfile = fopen(filename, "w");

    fwrite(str, strlen(str), 1, pfile);
    syslog(LOG_DEBUG, "Writing %s to %s", str, filename);

    fclose(pfile);
    closelog();
    return 0;
}
