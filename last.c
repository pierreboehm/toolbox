#include <stdio.h>
#include <argp.h>

#include "last.h"

const char *argp_program_version = "last 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

/* This structure is used by main to communicate with parse_opt. */
struct arguments {
    char *args[2];            /* ARG1 and ARG2 */
    int verbose;              /* The -v flag */
    char *outfile;            /* Argument for -o */
    char *string1, *string2;  /* Arguments for -a and -b */
};

static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    /*{"alpha",   'a', "STRING1", 0, "Do something with STRING1 related to the letter A"},
    {"bravo",   'b', "STRING2", 0, "Do something with STRING2 related to the letter B"},*/
    {"output",  'o', "OUTFILE", 0, "Output to OUTFILE instead of to standard output"},
    {0}
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'v':
            arguments->verbose = 1;
            break;
        /*case 'a':
            arguments->string1 = arg;
            break;
        case 'b':
            arguments->string2 = arg;
            break;*/
        case 'o':
            arguments->outfile = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 2) {
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

//static char args_doc[] = "ARG1 ARG2";
static char doc[] = "last -- linux last command clone";
static struct argp argp = {options, parse_opt, NULL, doc};

int main(int argc, char **argv) {
    struct arguments arguments;
    //FILE *outstream;

    arguments.outfile = NULL;
    arguments.string1 = "";
    arguments.string2 = "";
    arguments.verbose = 0;

    /*
    FILE *file;
    struct utmp log;

    file = fopen("/var/log/wtmp", "rb");

    if (file) {
        while (fread(&log, sizeof(struct utmp), 1, file) == 1) {
            utmpprint(&log);
        }
    } else {
        perror("File not found");
    }

    fclose(file);
    */

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    /*
    if (arguments.outfile) {
        outstream = fopen(arguments.outfile, "w");
    } else {
        outstream = stdout;
    }

    if (arguments.verbose) {
        fprintf(outstream, "Verbose mode activated.");
    }
    */

    // FOR TEST PURPOSES ONLY
    /*
    if (is_host_reachable("192.168.1.2")) {
        printf("Host reachable\n");
    } else {
        printf("Host not reachable\n");
    }*/

    return 0;
}