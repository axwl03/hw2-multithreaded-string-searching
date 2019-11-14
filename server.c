#include "server.h"

int sockfd, newsockfd, portno;
struct sockaddr_in serv_addr, cli_addr;
socklen_t cli_len = sizeof(cli_addr);
pthread_mutex_t lock;
char *root;

int main(int argc, char *argv[])
{
    if(argc != 7)
    {
        printf("command format should be: ./server -r [ROOT] -p [port] -n THREAD_NUMBER\n");
        exit(1);
    }
    else
    {
        if(strcmp(argv[1], "-r") != 0 || strcmp(argv[3], "-p") != 0 || strcmp(argv[5], "-n") != 0)
        {
            printf("command format should be: ./server -r [ROOT] -p [port] -n THREAD_NUMBER\n");
            exit(1);
        }
    }
    if(atoi(argv[6]) < 1 || atoi(argv[6]) > 50)
    {
        printf("the number of threads is limited between 1~50\n");
        exit(1);
    }
    root = argv[2];
    pthread_t pth[atoi(argv[6])];
    pthread_mutex_init(&lock, NULL);
    LIST_INIT(&head);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[4]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR binding socket");
        exit(1);
    }
    if(listen(sockfd, 5) < 0)
    {
        perror("ERROR listening");
        exit(1);
    }
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
    if(newsockfd < 0)
    {
        perror("ERROR accepting");
        exit(1);
    }
    for(int i = 0; i < atoi(argv[6]); ++i)
    {
        if(pthread_create(&pth[i], NULL, worker, NULL))
        {
            printf("ERROR pthread create\n");
            exit(1);
        }
    }
    char buffer[MAX_LEN];
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        if(recv(newsockfd, buffer, sizeof(buffer)-1, 0) == 0)
            break;
        printf("%s\n", buffer);
        parse_message(buffer);
    }
    close(sockfd);
    close(newsockfd);
    return 0;
}
int search(char *filename, char *str)
{
    FILE *fp = fopen(filename, "r");
    if(!fp)
    {
        printf("Error opening %s\n", filename);
        exit(1);
    }
    char ch;
    fpos_t file_pos;
    int match_count = 0, i;

    while((ch = fgetc(fp)) != EOF)
    {
        if(ch == str[0])
        {
            fgetpos(fp, &file_pos);
            for(i = 1; i < strlen(str); ++i)
            {
                ch = fgetc(fp);
                if(ch != str[i])
                    break;
            }
            if(i == strlen(str))
                match_count++;
            fsetpos(fp, &file_pos);
        }
    }
    fclose(fp);
    return match_count;
}
void push_back(char *str)
{
    struct entry *np = malloc(sizeof(struct entry));
    if(np == NULL)
    {
        printf("malloc error\n");
        exit(1);
    }
    memset(np->request, 0, sizeof(np->request));
    strcpy(np->request, str);
    pthread_mutex_lock(&lock);
    if(rear == NULL)
        LIST_INSERT_HEAD(&head, np, entries);
    else
        LIST_INSERT_AFTER(rear, np, entries);
    rear = np;
    pthread_mutex_unlock(&lock);
}
void pop_front()
{
    if(head.lh_first != NULL)
        LIST_REMOVE(head.lh_first, entries);
    if(head.lh_first == NULL)
        rear = NULL;
}
void parse_message(char *buffer)
{
    int i, j;
    char str[129];
    for(i = 0; i < 129; ++i)
    {
        if(buffer[i] == ' ' || buffer[i] == '\0' || i == 128)
        {
            str[i] = '\0';
            break;
        }
        else
            str[i] = buffer[i];
    }
    if(strcmp(str, "Query") == 0)
    {
        while(i < MAX_LEN && buffer[i] == ' ')++i;
        if(buffer[i] == '\0')
        {
            printf("query format: Query \"QUERY_STRING\"\n");
            return;
        }
        while(i < MAX_LEN)
        {
            if(buffer[i] == '"')
            {
                ++i;
                for(j = 0; j < 129; ++j, ++i)
                {
                    if(buffer[i] != '"' && buffer[i] != '\0')
                        str[j] = buffer[i];
                    else
                    {
                        str[j] = '\0';
                        break;
                    }
                }
                if(j == 129)
                {
                    printf("QUERY_STRING is longer than 128 bytes\n");
                    return;
                }
                if(buffer[i] == '\0')
                {
                    printf("QUERY_STRING should be enclosed by \"\"\n");
                    return;
                }
                else if(buffer[i] == '"')
                    ++i;
                printf("***%s\n", str);
                push_back(str);
            }
            else if(buffer[i] == '\0')
                break;
            else if(buffer[i] == ' ')
                ++i;
        }
    }
    else
    {
        printf("command not recognized\n");
    }
}
void *worker(void *arg)
{
    char request[129], reply[500], path[100], current_dir[100], temp[100];
    int result, found;
    DIR *d;
    struct dirent *dir;
    struct stat buf;
    struct dir_listhead dir_head;
    LIST_INIT(&dir_head);
    struct dir_entry *np;
    memset(request, 0, sizeof(request));
    memset(reply, 0, sizeof(reply));
    while(1)
    {
        pthread_mutex_lock(&lock);
        if(head.lh_first != NULL)
        {
            strcpy(request, head.lh_first->request);
            pop_front();
            pthread_mutex_unlock(&lock);
            sprintf(reply, "String: \"%s\"\n", request);
            found = 0;
            memset(current_dir, 0, sizeof(current_dir));
            strncpy(current_dir, root, sizeof(current_dir)-1);
            while(1)
            {
                d = opendir(current_dir);
                if(d)
                {
                    while((dir = readdir(d)) != NULL)
                    {
                        memset(&buf, 0, sizeof(buf));
                        strcpy(path, current_dir);
                        strncat(path, "/", sizeof(path)-strlen(path)-1);
                        stat(strncat(path, dir->d_name, sizeof(path)-strlen(path)-1), &buf);
                        if(S_ISDIR(buf.st_mode) && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
                        {
                            strcpy(path, current_dir);
                            strncat(path, "/", sizeof(path)-strlen(path)-1);
                            strncat(path, dir->d_name, sizeof(path)-strlen(path)-1);
                            np = malloc(sizeof(struct dir_entry));
                            if(!np)
                            {
                                printf("malloc failed\n");
                                exit(1);
                            }
                            strcpy(np->dir_name, path);
                            LIST_INSERT_HEAD(&dir_head, np, dir_entries);
                        }
                        else if(S_ISREG(buf.st_mode))
                        {
                            // read file and search
                            printf("search %s\n", path);
                            result = search(path, request);
                            printf("%d\n", result);
                            if(result != 0)
                            {
                                sprintf(temp, "File: %s, Count: %d\n", path, result);
                                strncat(reply, temp, sizeof(reply)-strlen(reply)-1);
                                //printf("send: %ld bytes\n", send(newsockfd, reply, strlen(reply), 0));
                                //memset(reply, 0, sizeof(reply));
                            }
                            found += result;
                        }
                    }
                }
                else
                {
                    printf("could not open %s\n", current_dir);
                    break;
                }
                closedir(d);
                if(dir_head.lh_first != NULL)
                {
                    strcpy(current_dir, dir_head.lh_first->dir_name);
                    LIST_REMOVE(dir_head.lh_first, dir_entries);
                }
                else break;
            }
            if(found == 0)
            {
                strncat(reply, "Not found\n", sizeof(reply)-strlen(reply)-1);
                //memset(reply, 0, sizeof(reply));
            }
            printf("send: %ld bytes\n", send(newsockfd, reply, strlen(reply), 0));
            memset(reply, 0, sizeof(reply));
            memset(request, 0, sizeof(request));
        }
        else
            pthread_mutex_unlock(&lock);
    }
}
