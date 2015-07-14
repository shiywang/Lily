#ifndef GETOPT_H
#define GETOPT_H

extern int getopt (int argc, char *const argv[], const char *opts);
extern int  opterr;
extern int  optind;
extern int  optopt;
extern char *optarg;

#endif
