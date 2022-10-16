#include <stdio.h>
#include <utmp.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>
#include <sys/time.h>

void utmpprint(struct utmp *log) {
    const char *addr_string;
	char buffer[INET6_ADDRSTRLEN];
    char time_string[40];
    struct tm* tm_info;
	struct timeval tv;

    if (log->ut_addr_v6[1] || log->ut_addr_v6[2] || log->ut_addr_v6[3]) {
		addr_string = inet_ntop(AF_INET6, &(log->ut_addr_v6), buffer, sizeof(buffer));
    } else {
		addr_string = inet_ntop(AF_INET, &(log->ut_addr_v6), buffer, sizeof(buffer));
    }

    tv.tv_sec = log->ut_tv.tv_sec;
	tv.tv_usec = log->ut_tv.tv_usec;

    tm_info = localtime(&tv.tv_sec);
    strftime(time_string, sizeof(time_string), "%H:%M:%S %d.%m.%Y", tm_info);

    fprintf(stdout, "%d %05d %-4.4s %20s %20s %20s %20s %20s\n", 
        log->ut_type, log->ut_pid, log->ut_id, log->ut_line, log->ut_user, log->ut_host, addr_string, time_string);
}
