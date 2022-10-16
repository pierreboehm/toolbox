#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <time.h>
#include <unistd.h>

void parent(pid_t pid);
void printerror(char* type);
void detach();

pid_t pid;
char buffer[256];

const int long_size = sizeof(long);

int main(int argc, char **argv) {

    pid = (pid_t)atoi(argv[1]);

    if (ptrace(PTRACE_ATTACH, pid, 0, 0) < 0) {
        printerror("PTRACE_ATTACH");
    }

    printf("process %d attached\n", pid);

    signal(SIGTERM, detach);
    signal(SIGINT, detach);

    parent(pid);

    return 0;
}

void getdata(pid_t child, long addr, char *str, int len) {
    char *laddr;
    int i, j;
    union u {
            long val;
            char chars[long_size];
    } data;

    i = 0;
    j = len / long_size;
    laddr = str;
    
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }

    j = len % long_size;
    
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(laddr, data.chars, j);
    }

    str[len] = '\0';
}

int is_entered(char *str) {
    for (; *str != '\0'; str++) {
        if (*str == '\r') {
            return 1;
        }
    }
    return 0;
}

const char *get_timestring() {
    char *time_string = (char *) malloc(20 * sizeof(char));
    time_t now;
    struct tm *tm_info;

    /* Get the current time. */
    time(&now);

    /* Convert it to local time representation. */
    tm_info = localtime(&now);

    sprintf(time_string, "%02i:%02i:%02i %02i.%02i.%04i", 
        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, 
        tm_info->tm_mday, tm_info->tm_mon, tm_info->tm_year + 1900);

    // DOES NOT WORK
    // strftime(time_string, sizeof(time_string), "%H:%M:%S %d.%m.%Y", tm_info);

    return time_string;
}

void parent(pid_t pid) {
	int status;
    char *cmd = NULL;

// TODO: re-define structs especially for aarch64: https://github.com/Distrotech/strace/blob/master/linux/aarch64/arch_regs.c
// NOTE: btw. it works too w/o register adaptions

    struct user_regs_struct regs;
    struct iovec iov = {
        .iov_base = &regs,
        .iov_len = sizeof(struct user_regs_struct),
    };

	while (1) {
        // TODO: check interruption conditions
        // TODO: add '|| WIFEXITED(status)'
		if (waitpid(pid, &status, 0) == -1 || WIFEXITED(status)) {
			// printerror("waitpid()");
            break;
        }

		// if (WIFEXITED(status)) {
        //     break;
		// }

        // NT_ARM_SYSTEM_CALL = ?, NT_PRSTATUS = 1
        if (ptrace(PTRACE_GETREGSET, pid, 1, &iov) == -1) {
			printerror("PTRACE_GETREGSET");
        }

// TODO: map correct registers! details here: https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md
// NOTE: btw. it works too w/o register adaptions

        if (regs.regs[7] == 4 || regs.regs[8] == 64) {      // r7 on arm (32-bit), r8 on arch64 (64-bit)
            if (regs.regs[0] == 1) {                       // (unsigned int) file descriptor (here: STDOUT = 1)
                size_t count = regs.regs[2];
                char *strbuf = (char *) malloc((count + 1) * sizeof(char));

                getdata(pid, regs.regs[1], strbuf, count);

                if (is_entered(strbuf)) {
                    printf("[%s]: %s\n", get_timestring(), cmd);
                    cmd = NULL;
                } else {
                    if (cmd == NULL) {
                        cmd = (char *) malloc((count + 1) * sizeof(char));
                        memcpy(cmd, strbuf, count);
                        cmd[count] = '\0';
                    } else {
                        strcat(cmd, strbuf);
                    }
                }
            }
        }

		if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1) {
			printerror("PTRACE_SYSCALL");
        }
	}
}

void printerror(char* type) {
	snprintf(buffer, 256, "Error %s", type);
	perror(buffer);
	exit(0);
}

void detach() {
	if (waitpid(pid, 0, 0) == -1) {
		printerror("waitpid()");
    }

	if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) {
		printerror("PTRACE_DETACH");
    }

	printf("\nprocess %d detached\n", pid);
	exit(0);
}