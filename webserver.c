#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT      3000
#define BUF_SIZE  4096

const char *HTML_HOME =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html><html><body>"
    "<h1> Home Page</h1>"
    "<p>Welcome to my C web server!</p>"
    "<a href='/about'>About</a>"
    "</body></html>";

const char *HTML_ABOUT =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html><html><body>"
    "<h1> about</h1>"
    "<p>This server is written in pure C.</p>"
    "<a href='/'>Home</a>"
    "</body></html>";

const char *HTML_404 =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html><html><body>"
    "<h1>404 - Not Found</h1>"
    "</body></html>";

void handle_client(int client_fd) {
    char buffer[BUF_SIZE] = {0};

    read(client_fd, buffer, sizeof(buffer) - 1);

    char method[8], path[256];
    sscanf(buffer, "%7s %255s", method, path);
    printf("[%s] %s\n", method, path);
    if (strcmp(path, "/") == 0)
        write(client_fd, HTML_HOME,  strlen(HTML_HOME));
    else if (strcmp(path, "/about") == 0)
        write(client_fd, HTML_ABOUT, strlen(HTML_ABOUT));
    else
        write(client_fd, HTML_404,   strlen(HTML_404));

    close(client_fd);
}

/*Main */
int main(void) {
    /* Create TCP socket */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    /* Allow port reuse after restart */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind to port */
    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }

    /* Listen */
    listen(server_fd, 10);
    printf("Server running → http://localhost:%d\n", PORT);
    printf("Routes: /  |  /about\n\n");

    /* Accept loop */
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) { perror("accept"); continue; }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);        /* child doesn't need listener */
            handle_client(client_fd);
            exit(0);
        }
        close(client_fd);            /* parent closes client socket */
        waitpid(-1, NULL, WNOHANG);  /* reap finished children      */
    }

    close(server_fd);
    return 0;
}
