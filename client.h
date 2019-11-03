#ifndef CLIENT_H
#define CLIENT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#define MAX_LEN 2000

void readline(char *str);
int format_check(char *buffer);
void *recv_result(void *arg);

#endif
