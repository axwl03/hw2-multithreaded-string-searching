#include "server.h"

int sockfd, newsockfd, portno;
struct sockaddr_in serv_addr, cli_addr;
socklen_t cli_len = sizeof(cli_addr);

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
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("ERROR binding socket\n");
        exit(1);
    }
    if(listen(sockfd, 5) < 0)
    {
        printf("ERROR listening\n");
        exit(1);
    }
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
    if(newsockfd < 0)
    {
        printf("ERROR accepting\n");
        exit(1);
    }
    ////
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    printf("recv %ld bytes\n", recv(newsockfd, buffer, sizeof(buffer)-1, 0));
    printf("%s\n", buffer);
    ////




    close(sockfd);
    close(newsockfd);
    return 0;
}
