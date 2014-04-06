/*
	@descr	MSW - Max Size Writer - Writes stdin to file, unless it exceeds specified size

	@author Johan Hedberg <mail@johan.pp.se>
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#ifdef __GNUC__
#include <getopt.h>
#endif
#define DEFMAXSIZE 5368709120
#define _STDC_C99

void usage(void);

int main(int argc, char**argv) {
        /* Option vars */
        char *fname = NULL;
        unsigned long long maxfsize = DEFMAXSIZE;
        int index;
        int opt;

        /* Program vars */
        char buf[4096];
        size_t written;
        size_t nread;
        size_t nwrit;
        int status = 0;
        int fh;
        struct stat sb;

        /* Parse arguments */
        opterr = 0;
        while((opt = getopt (argc, argv, "m:f:")) != -1) {
                switch(opt) {
                        case 'm':
                                maxfsize = atoll(optarg);
                                break;
                        case 'f':
                                fname = optarg;
                                break;
                        case '?':
                                if(optopt == 'm' || optopt == 'f')
                                        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                                else if (isprint(optopt))
                                        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                                else
                                        fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                                usage();
                                return 1;
                        default:
                                usage();
                                return 1;
                }
        }

        /* We has unknown crap on commandline? */
        for(index = optind; index < argc; ++index) {
                fprintf(stderr, "Unknown argument: '%s'.\n", argv[index]);
                usage();
                return 1;
        }

        /* We has filename? */
        if((NULL == fname) || (1 > strlen(fname))) {
                fprintf(stderr, "No filename given!\n");
                usage();
                return 1;
        }

        /* Open target file */
        fh = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG);
        if(-1 == fh) {
                perror("Cannot open file");
                return 1;
        }

        /* Read until EOF from stdin, nuke file and exit with error code if too large. */
        while(1) {
                nread = read(STDIN_FILENO, buf, sizeof buf);
                if(0 == nread) {
                        break;
                }

                stat(fname, &sb);
                if(sb.st_size > maxfsize) {
                        printf("File exceeded limit, emptying file and exiting.\n");
                        close(fh);
                        fh = open(fname, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG);
                        status = 1;
                        break;
                }

                written = 0;
                while(written < nread) {
                        nwrit = write(fh, buf + written, nread - written);
                        if (written < 1) break;
                        written += nwrit;
                }
        }
        close(fh);
        return status;
}

void usage(void) {
        fprintf(stderr, "MSW - Max Size Writer - Writes stdin to file, unless it exceeds specified size\n");
        fprintf(stderr, "Usage: msw -f <target file> [-m <max size in bytes>] (Default max size 5GB)\n");
}
