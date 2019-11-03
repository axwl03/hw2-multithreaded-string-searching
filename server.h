#ifndef SERVER_H
#define SERVER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/queue.h>
#define MAX_LEN 2000

void search(char *str);
LIST_HEAD(listhead, entry) head;
struct entry
{
    char request[129];
    LIST_ENTRY(entry) entries;
} *rear;
void push_back(char *str);
void pop_front();
void parse_message(char *buffer);

#endif
