#include <stdio.h>

#define FW_IMPL
#include "fast_work/fw.h"

#include "shared.h"

int tcp() {
    if (!fw_init()) {
        printf("Failed to init fw\n");
        return 1;
    }

    FwConn listener;
    if (!FwConn_TCP_create_listener(&listener, ADDR, PORT)) {
        printf("Failed to listen on the socker\n");
        return 1;
    }

    FwConn conn;
    if (!FwConn_TCP_accept(&listener, &conn)) {
        printf("Failed to accept connection\n");
        return 1;
    }

    char  *addr;
    size_t addr_size;
    if (!FwAddr_get_addr(&conn.addr, &addr, &addr_size)) {
        printf("Failed to get the address of client\n");
        return 1;
    }
    printf("Client connected from: %.*s:%d\n", addr_size, addr, FwAddr_get_port(&conn.addr));
    free(addr);

    const char *msg_send = "Hello";
    if (!FwConn_TCP_send(&conn, msg_send, strlen(msg_send))) {
        printf("Failed to send message %s\n", msg_send);
        return 1;
    }

    char  *msg_recv;
    size_t msg_recv_size;
    if (!FwConn_TCP_recv_all(&conn, &msg_recv, &msg_recv_size)) {
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

    if (!FwConn_close(&listener)) {
        printf("Failed to close listener's connection\n");
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

    FwConn server;
    if (!FwConn_UDP_bind(&server, ADDR, PORT)) {
        printf("Failed to bind on the socket\n");
        return 1;
    }

    FwAddr client_addr;
    char  *msg_recv;
    size_t msg_recv_size;
    if (!FwConn_UDP_recv_all_cap_from(&server, &client_addr, &msg_recv, &msg_recv_size, 1)) {
        printf("error receiving message from client (error: %d)\n", fw_get_error_code());
        return 1;
    }

    //       or
    // FwAddr client_addr;
    // char   msg[BUF_SIZE] = {0};
    // if (!FwConn_UDP_recv_from(&server, &client_addr, msg, BUF_SIZE)) {
    //     printf("Failed to recv message (error: %d)\n", fw_get_error_code());
    //     return 1;
    // }

    char  *addr;
    size_t addr_size;
    if (!FwAddr_get_addr(&client_addr, &addr, &addr_size)) {
        printf("Failed to get the address of client\n");
        return 1;
    }
    printf("client %.*s:%d send: %.*s\n", addr_size, addr, FwAddr_get_port(&client_addr),
           msg_recv_size, msg_recv);
    free(addr);
    free(msg_recv);

    const char *msg_send = "World";
    if (!FwConn_UDP_send_to(&server, &client_addr, msg_send, strlen(msg_send))) {
        printf("Failed to send message %s\n", msg_send);
        return 1;
    }

    FwConn_close(&server);

    if (!fw_quit()) {
        printf("Failed to quit fw\n");
        return 1;
    }
    return 0;
}

int main() { return udp(); }
