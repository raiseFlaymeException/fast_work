#include <stdio.h>

#define FW_IMPL
#include "fast_work/fw.h"

#include "shared.h"

int main() {
    if (!fw_init()) {
        printf("Failed to init fw\n");
        return 1;
    }

    FwConn conn;
    if (!fw_connect(&conn, ADDR, PORT)) {
        printf("Failed to connect to the server\n");
        return 1;
    }

    char msg_recv[BUF_SIZE];
    if (!FwConn_recv(&conn, msg_recv, sizeof(msg_recv)*sizeof(*msg_recv))) {
        printf("Failed to recv message\n");
        return 1;
    }

    printf("server send: %s\n", msg_recv);

    const char *msg_send = "World";
    if (!FwConn_send(&conn, msg_send, strlen(msg_send))) {
        printf("Failed to send message %s\n", msg_send);
        return 1;
    }

    if (!FwConn_close(&conn)) {
        printf("Failed to close the connection\n");
        return 1;
    }

    if (!fw_quit()) {
        printf("Failed to quit fw\n");
        return 1;
    }
    return 0;
}
