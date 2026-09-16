#include <sys/types.h>
2 #include <unistd.h>
3 #include <stdlib.h>
4 #include <stdio.h>
5 static const int Bignumber = 10000;
6
7 main(int argc, char *argv[ ]) /* demonstrate fork(2) */
8 {
9 char ch, first, last;
10 pid_t pid;
11 int i;
12
13 if ((pid = fork()) > 0) { /* parent */
14 first = 'A';
15 last = 'Z';
16 }
17 else if (pid == 0) { /* child */
18 first = 'a';
19 last = 'z';
20 }
21 else { /* cannot fork(2) */
22 perror(argv[0]);
23 exit(1);
24 }
25 for (ch = first; ch <= last; ch++) {
26 /* delay loop */
27 for (i = 0; i < Bignumber; i++)
28 ; /* null */
29 write(1, &ch, 1);
30 }
31
32 exit(0);
33 }