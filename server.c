#include "server.h"

int sockfd, newsockfd, portno;
struct sockaddr_in serv_addr, cli_addr;
socklen_t cli_len = sizeof(cli_addr);

int main(int argc, char *argv[])
{
    /*test
    LIST_INIT(&head);
    push_back("hello");
    push_back("hey");
    struct entry *p;
    for(p = head.lh_first; p != NULL; p = p->entries.le_next)
    	printf("%s\n", p->request);
    pop_front();
    pop_front();
    if(head.lh_first == NULL)
    	printf("remove success\n");
    push_back("hi");
    printf("%s\n", rear->request);
    //test*/

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
    ////
    char buffer[MAX_LEN];
    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        if(recv(newsockfd, buffer, sizeof(buffer)-1, 0) == 0)
            break;
        printf("%s\n", buffer);
        parse_message(buffer);
    }
    ////
    close(sockfd);
    close(newsockfd);
    return 0;
}
void search(char *str)
{
    FILE *fp = fopen("test.txt", "r");
    if(!fp)
    {
        printf("Error opening file.\n");
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
    printf("%d\n", match_count);
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
    if(rear == NULL)
        LIST_INSERT_HEAD(&head, np, entries);
    else
        LIST_INSERT_AFTER(rear, np, entries);
    rear = np;
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
