#include "client.h"

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        printf("command format should be: ./client -h [IP] -p [port]\n");
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
                            //send_request(buffer);
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
    return 0;
}
void send_request(char str)
{
    //int sockfd, portno;
}
