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
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#define MAX_LEN 2000

int search(char *filename, char *str);
LIST_HEAD(listhead, entry) head;
LIST_HEAD(dir_listhead, dir_entry);
struct entry
{
    char request[129];
    LIST_ENTRY(entry) entries;
} *rear;
struct dir_entry
{
    char dir_name[100];
    LIST_ENTRY(dir_entry) dir_entries;
};
void push_back(char *str);
void pop_front();
void parse_message(char *buffer);
void *worker(void *arg);

#endif
