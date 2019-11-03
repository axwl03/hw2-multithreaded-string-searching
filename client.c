#include "client.h"

int sockfd, portno;
struct sockaddr_in serv_addr;

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        printf("command format should be: ./client -h [IP] -p [port]\n");
        exit(1);
    }
    else
    {
        if(strcmp(argv[1], "-h") != 0 && strcmp(argv[3], "-p") != 0)
        {
            printf("command format should be: ./client -h [IP] -p [port]\n");
            exit(1);
        }
    }
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
    serv_addr.sin_addr.s_addr = inet_addr(argv[2]);
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }
    int ret;
    char buffer[MAX_LEN], test_str[10];
    pthread_t pth;
    pthread_create(&pth, NULL, recv_result, NULL);
    while(1)
    {
        memset(buffer, 0, MAX_LEN);
        memset(test_str, 0, sizeof(test_str));
        readline(buffer);
        ret = format_check(buffer);
        if(ret == 1) 		// send Query
        {
            send(sockfd, buffer, strlen(buffer), 0);
        }
        else if(ret == 2)	// exit
            break;
    }
    close(sockfd);
    return 0;
}
void readline(char *str)
{
    char ch;
    for(int i = 0; i < MAX_LEN-1; ++i)
    {
        ch = getchar();
        if(ch == '\n')
        {
            str[i] = '\0';
            return;
        }
        str[i] = ch;
    }
}
int format_check(char *buffer)
{
    if(strcmp(buffer, "exit") == 0)
        return 2;
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
            return -1;
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
                    return -1;
                }
                if(buffer[i] == '\0')
                {
                    printf("QUERY_STRING should be enclosed by \"\"\n");
                    return -1;
                }
                else if(buffer[i] == '"')
                    ++i;
            }
            else if(buffer[i] == '\0')
                break;
            else if(buffer[i] == ' ')
                ++i;
            else
            {
                printf("QUERY_STRING should be enclosed by \"\"\n");
                return -1;
            }
        }
        return 1;
    }
    else
    {
        printf("command not recognized\n");
        return -1;
    }
}
void *recv_result(void *arg)
{
    char buffer[1000];
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        if(recv(sockfd, buffer, sizeof(buffer), 0) == 0)
        {
            printf("connection break, type \"exit\" to quit\n");
            break;
        }
        printf("%s", buffer);
    }
    pthread_exit(NULL);
}
