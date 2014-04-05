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
        int *tempchar;
        int stdinstat;
        int status = 0;
        FILE *fh;
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
        fh = fopen(fname, "w+");
        if(fh == NULL) {
                perror("Cannot open file");
                free(fname);
                return 1;
        }

        /* Read until EOF from stdin, nuke file and exit with error code if too large. */
        while(1) {
                stdinstat = scanf("%c", tempchar);
                if(EOF == stdinstat) break;

                stat(fname, &sb);
                if(sb.st_size > maxfsize) {
                        free(tempchar);
                        printf("File exceeded limit, emptying file and exiting.\n");
                        fclose(fh);
                        fh = fopen(fname, "w+");
                        status = 1;
                        break;
                }

                fprintf(fh, "%c", *tempchar);
                free(tempchar);
        }
        free(fname);
        fclose(fh);
        return status;
}

void usage(void) {
        fprintf(stderr, "MSW - Max Size Writer - Writes stdin to file, unless it exceeds specified size\n");
        fprintf(stderr, "Usage: msw -f <target file> [-m <max size in bytes>] (Default max size 5GB)\n");
}
