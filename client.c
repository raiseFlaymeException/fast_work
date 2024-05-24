#include <stdio.h>

#define FW_IMPL
#include "fast_work/fw.h"

#include "shared.h"

int tcp() {
    if (!fw_init()) {
        printf("Failed to init fw\n");
        return 1;
    }

    FwConn conn;
    if (!FwConn_TCP_create_conn(&conn, ADDR, PORT)) {
        printf("Failed to connect to the server\n");
        return 1;
    }

    char msg_recv[BUF_SIZE];
    if (!FwConn_TCP_recv(&conn, msg_recv, sizeof(msg_recv) * sizeof(*msg_recv))) {
        printf("Failed to recv message\n");
        return 1;
    }

    printf("server send: %s\n", msg_recv);

    const char *msg_send = "World";
    if (!FwConn_TCP_send(&conn, msg_send, strlen(msg_send))) {
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

int udp() {
    if (!fw_init()) {
        printf("Failed to init fw\n");
        return 1;
    }

    FwConn client;
    if (!FwConn_UDP_create_conn(&client)) {
        printf("Failed to create the client connection\n");
        return 1;
    }

    FwAddr addr_server;
    if (!FwConn_UDP_create_addr(&addr_server, ADDR, PORT)) {
        printf("Failed to create the address to the server\n");
        return 1;
    }

    const char *msg = "Hello";
    if (!FwConn_UDP_send_to(&client, &addr_server, msg, strlen(msg))) {
        printf("Failed to send message %s\n", msg);
        return 1;
    }

    char msg_recv[BUF_SIZE] = {0};
    if (!FwConn_UDP_recv_from(&client, NULL, msg_recv, BUF_SIZE)) {
        printf("Failed to recv message\n");
        return 1;
    }

    printf("server send: %s\n", msg_recv);

    FwConn_close(&client);

    if (!fw_quit()) {
        printf("Failed to quit fw\n");
        return 1;
    }
    return 0;
}

int main() { return udp(); }
