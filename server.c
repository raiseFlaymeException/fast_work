#include <stdio.h>

#define FW_IMPL
#include "fast_work/fw.h"

#include "shared.h"

int main() {
    if (!fw_init()) {
        printf("Failed to init fw\n");
        return 1;
    }

    FwListener listener;
    if (!fw_listen(&listener, ADDR, PORT)) {
        printf("Failed to listen on the socker\n");
        return 1;
    }

    FwConn conn;
    if (!FwListener_accept(&listener, &conn)) {
        printf("Failed to accept connection\n");
        return 1;
    }

    char  *addr;
    size_t addr_size;
    if (!FwConn_get_addr(&conn, &addr, &addr_size)) {
        printf("Failed to get the address of client\n");
        return 1;
    }
    printf("Client connected from: %.*s:%d\n", addr_size, addr, FwConn_get_port(&conn));
    free(addr);

    const char *msg_send = "Hello";
    if (!FwConn_send(&conn, msg_send, strlen(msg_send))) {
        printf("Failed to send message %s\n", msg_send);
        return 1;
    }

    char  *msg_recv;
    size_t msg_recv_size;
    if (!FwConn_recv_all(&conn, &msg_recv, &msg_recv_size)) {
        printf("Failed to recv message\n");
        return 1;
    }

    //            or
    // char msg_recv[BUF_SIZE];
    // if (!FwConn_recv(&conn, msg_recv, sizeof(msg_recv)*sizeof(*msg_recv))) {
    //     printf("Failed to recv message\n");
    //     return 1;
    // }

    if (!FwConn_close(&conn)) {
        printf("Failed to close client's connection\n");
        return 1;
    }

    printf("client send: %.*s\n", msg_recv_size, msg_recv);
    free(msg_recv);

    if (!FwListener_close(&listener)) {
        printf("Failed to close listener's connection\n");
        return 1;
    }

    if (!fw_quit()) {
        printf("Failed to quit fw\n");
        return 1;
    }
    return 0;
}
