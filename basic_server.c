#include "pipe_networking.h"

char computer[] = {0xf0, 0x9f, 0xa7, 0x91, 0xf0, 0x9f, 0x8f, 0xbb, 0xe2, 0x80, 0x8d, 0xf0, 0x9f, 0x92, 0xbb, '\0'};
int f = 0;

static void sighandler(int signo) {
    if (signo==SIGINT) {
        if (f) printf("\n%s Closing down server %s\n", computer, computer);
        exit(0);
    }
}

int main() {

    signal(SIGINT, sighandler);

    int to_client;
    int from_client;

    printf("%s Starting up server %s\n", computer, computer);

    while (1) {

        from_client = server_handshake( &to_client );

        f = fork();
        if (f) {
            close(from_client);
            close(to_client);
        } else {
            char buf[1000];
            for (int i = 0; i < 1000; i++) buf[i]=0;

            while (read(from_client, buf, 1000) != 0) {
                for (int i = 0; buf[i]; i++) {
                    if (buf[i] >= 'a' && buf[i] <= 'z') buf[i] = buf[i] - 'a' + 'A';
                }
                write(to_client, buf, 1000);
                for (int i = 0; buf[i]; i++) buf[i] = 0;
            }
            exit(0);
        }
    }
}
