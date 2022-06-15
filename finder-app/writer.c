#include <stdio.h>
#include <syslog.h>

int main() {
    openlog(NULL, LOG_NDELAY, LOG_DAEMON);
    syslog(LOG_DEBUG, "Writing %s to %s", "<str>", "<filename>");
    closelog();
    return 0;
}
