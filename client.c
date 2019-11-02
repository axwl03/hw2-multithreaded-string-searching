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
        printf("ERROR opening socket\n");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[4]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = inet_addr(argv[2]);
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
        exit(1);
    }
    char buffer[129], ch;
    while(scanf(" %128[^ \n]", buffer))
    {
        if(strcmp(buffer, "Query") == 0)
        {
            while(1)
            {
                ch = getchar();
                if(ch == '"')
                {
                    memset(buffer, 0, sizeof(buffer));
                    if(scanf("%128[^\"\n]", buffer))
                    {
                        ch = getchar();
                        if(ch == '"')
                        {
                            //send
                            send(sockfd, buffer, sizeof(buffer), 0);////
                            printf("%s\n", buffer);
                        }
                        else if(ch == '\n')
                        {
                            printf("QUERY_STRING should be enclose by \"\"\n");
                            break;
                        }
                        else
                        {
                            printf("QUERY_STRING size > 128\n");
                            while((ch = getchar()) != '\n');
                            break;
                        }
                    }
                }
                else if(ch == ' ')
                    continue;
                else if(ch == '\n')
                    break;
                else
                {
                    printf("QUERY_STRING should be enclose by \"\"\n");
                    while((ch = getchar()) != '\n');
                    break;
                }
            }
        }
        else if(strcmp(buffer, "exit") == 0)
            break;
        else
        {
            printf("command not recognized\n");
            while((ch = getchar()) != '\n');
        }
        memset(buffer, 0, sizeof(buffer));
    }
    close(sockfd);
    return 0;
}
