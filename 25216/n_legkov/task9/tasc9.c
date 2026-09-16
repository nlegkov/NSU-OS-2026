#include <sys/types.h>
#include <unistd.h>
 #include <stdlib.h>
 #include <stdio.h>
 static const int Bignumber = 10000;

 main(int argc, char *argv[ ]) /* demonstrate fork(2) */
 {
 char ch, first, last;
 pid_t pid;
 int i;

 if ((pid = fork()) > 0) { /* parent */
 first = 'A';
 last = 'Z';
 }
 else if (pid == 0) { /* child */
 first = 'a';
 last = 'z';
 }
 else { /* cannot fork(2) */
 perror(argv[0]);
 exit(1);
 }
 for (ch = first; ch <= last; ch++) {
 /* delay loop */
 for (i = 0; i < Bignumber; i++)
 ; /* null */
 write(1, &ch, 1);
 }

 exit(0);
 }