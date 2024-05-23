#ifndef FW_H
#define FW_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#if defined(_WIN32) || defined (_WIN64)
#include <winsock2.h>
#else
#error "not implemented"
#endif

// listener and conn not on the same struct to differenciate them and maybe later seperate them due
// to internal reason
typedef struct {
    SOCKET             sock;
    struct sockaddr_in addr;
} FwListener;
typedef struct {
    SOCKET             sock;
    struct sockaddr_in addr;
} FwConn;

///
/// @brief initialize the library
///
/// @return true if not failed otherwise false
///
bool fw_init(void);
///
/// @brief deinitialize the library
///
/// @return true if not failed otherwise false
///
bool fw_quit(void);

///
/// @brief listen for an incomming connection
///
/// @param[out] listener the listener to create
/// @param[in] addr the address to connect to (ip address or host name)
/// @param[in] port the port to connect to
/// @return true if not failed otherwise false
///
bool fw_listen(FwListener *listener, const char *addr, unsigned short port);
///
/// @brief connect to a listener
///
/// @param[out] conn the connection to create
/// @param[in] addr the address to connect to (ip address or host name)
/// @param[in] port the port to connect to
/// @return true if not failed otherwise false
///
bool fw_connect(FwConn *conn, const char *addr, unsigned short port);

// NOLINTBEGIN
///
/// @brief accept the next incoming connection (blocking)
///
/// @param[in, out] listener the listener to accpet the connection from
/// @param[out] conn the connection we get
/// @return true if not failed otherwise false
///
bool FwListener_accept(FwListener *listener, FwConn *conn);

///
/// @brief close the listener
///
/// @param[in, out] listener the listener to close
/// @return true if not failed otherwise false
///
bool FwListener_close(FwListener *listener);

///
/// @brief send data to a socket
///
/// @param[in, out] conn the socket to send to
/// @param[in] msg the message to send
/// @param[in] msg_size the size of the message to send
/// @return true if not failed otherwise false
///
bool FwConn_send(FwConn *conn, const char *msg, size_t msg_size);

///
/// @brief check if we can call recv during max usec_timeout micro seconds
///
/// @param[in, out] conn the connection to check if we can recv from
/// @param[out] result the result of the operation
/// @param[in] usec_timeout the maximum time to wait in micro seconds
/// @return true if not failed othewise false
///
bool FwConn_check_timeout(FwConn *conn, bool *result, size_t usec_timeout);

///
/// @brief receive a message of a certain length from the connection (block)
///
/// @param[in, out] conn the connection to receive from
/// @param[in, out] msg the buffer to put the message in, must have at least msg_size allocated
/// @param[in] msg_size the maximum message size, all won't be filled except if the message is >= to
/// msg_size and the buffer won't be zeroed out
/// @return true if not failed otherwise false
///
bool FwConn_recv(FwConn *conn, char *msg, size_t msg_size);
///
/// @brief receive a message of any length from the connection (block)
///
/// @param[in, out] conn the connection to receive from
/// @param[out] msg a pointer to a char * that will be allocated (and will need to be free) and will
/// hold the message received
/// @param[out] msg_size the message size read from the function
/// @param[in] cap the capacity of the vector (put an estimate of the size of the data received)
/// @return true if not failed othewise false
///
bool FwConn_recv_all_cap(FwConn *conn, char **msg, size_t *msg_size, size_t cap);
///
/// @brief receive a message of any length from the connection (block)
/// call FwConn_recv_all_cap with the capacity equal to FW_BUF_SIZE_RECV_ALL (see
/// FwConn_recv_all_cap and FW_BUF_SIZE_RECV_ALL)
///
/// @param[in, out] conn the connection to receive from
/// @param[out] msg a pointer to a char * that will be allocated (and will need to be free) and will
/// hold the message received
/// @param[out] msg_size the message size read from the function
/// @return true if not failed othewise false
///
bool FwConn_recv_all(FwConn *conn, char **msg, size_t *msg_size);

///
/// @brief get the ip address of the connection
///
/// @param[in, out] conn the connection to get the ip from
/// @param[out] addr a pointer to a char * that will be allocated (and will need to be free) and
/// will hold the ip adress
/// @param[out] addr_size the size of the allocated char *
/// @return true if not failed otherwise false
///
bool FwConn_get_addr(FwConn *conn, char **addr, size_t *addr_size);
///
/// @brief get the port of the connection
///
/// @param[in, out] conn the connection to get the port from
/// @return the port (can't failed so no error code)
///
unsigned short FwConn_get_port(FwConn *conn);

///
/// @brief close the connection
///
/// @param[in, out] conn the connection to close
/// @return true if not failed otherwise false
///
bool FwConn_close(FwConn *conn);
// NOLINTEND

#endif // FW_H
#ifdef FW_IMPL

#ifndef FW_BUF_SIZE_RECV_ALL
#define FW_BUF_SIZE_RECV_ALL 256
#endif // FW_BUF_SIZE_RECV_ALL

#define FW_USEC_TO_SEC_DIV (size_t)1e6


static bool impl_fw_resolve(const char *addr, unsigned long *result) {
    struct hostent *host = gethostbyname(addr);
    if (host == NULL || *host->h_addr_list == NULL) { return false; }
    *result = (*(struct in_addr *)(host->h_addr)).s_addr;
    return true;
}

bool fw_init(void) {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
bool fw_quit(void) { return WSACleanup() == 0; }

bool fw_listen(FwListener *listener, const char *addr, unsigned short port) {
    listener->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener->sock == INVALID_SOCKET) { return false; }

    memset(&listener->addr, 0, sizeof(listener->addr));

    if (!impl_fw_resolve(addr, &listener->addr.sin_addr.s_addr)) { return false; }

    listener->addr.sin_family = AF_INET;
    listener->addr.sin_port   = htons(port);

    if (bind(listener->sock, (struct sockaddr *)&listener->addr, sizeof(listener->addr)) != 0) {
        closesocket(listener->sock);
        return false;
    }

    if (listen(listener->sock, SOMAXCONN) != 0) {
        closesocket(listener->sock);
        return false;
    }
    return true;
}
bool fw_connect(FwConn *conn, const char *addr, unsigned short port) {
    conn->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn->sock == INVALID_SOCKET) { return false; }

    memset(&conn->addr, 0, sizeof(conn->addr));

    if (!impl_fw_resolve(addr, &conn->addr.sin_addr.s_addr)) { return false; }

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port   = htons(port);

    if (connect(conn->sock, (struct sockaddr *)&conn->addr, sizeof(conn->addr)) != 0) {
        closesocket(conn->sock);
        return false;
    }
    return true;
}

bool FwListener_accept(FwListener *listener, FwConn *conn) {
    int addrlen = sizeof(conn->addr);
    conn->sock  = accept(listener->sock, (struct sockaddr *)&conn->addr, &addrlen);
    return conn->sock != INVALID_SOCKET;
}
bool FwListener_close(FwListener *listener) { return closesocket(listener->sock) == 0; }

bool FwConn_send(FwConn *conn, const char *msg, size_t msg_size) {
    return send(conn->sock, msg, msg_size, 0) != SOCKET_ERROR;
}

bool FwConn_check_timeout(FwConn *conn, bool *result, size_t usec_timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(conn->sock, &readfds);

    TIMEVAL timeout;
    timeout.tv_sec  = usec_timeout / FW_USEC_TO_SEC_DIV;
    timeout.tv_usec = usec_timeout % FW_USEC_TO_SEC_DIV;

    int n_sock_read = select(0, &readfds, NULL, NULL, &timeout);
    if (n_sock_read == SOCKET_ERROR) { return false; }
    *result = n_sock_read == 1;
    return true;
}
bool FwConn_recv(FwConn *conn, char *msg, size_t msg_size) {
    return recv(conn->sock, msg, msg_size, 0) != SOCKET_ERROR;
}
bool FwConn_recv_all_cap(FwConn *conn, char **msg, size_t *msg_size, size_t cap) {
    assert(cap > 0);
    *msg      = malloc(cap);
    *msg_size = 0;

    char cur_char;
    int  byte_read;
    while ((byte_read = recv(conn->sock, &cur_char, 1, 0)) > 0) {
        if ((*msg_size) + 1 > cap) {
            cap *= 2;
            *msg = realloc(*msg, cap);
        }
        (*msg)[(*msg_size)++] = cur_char;
    }

    return byte_read == 0;
}
bool FwConn_recv_all(FwConn *conn, char **msg, size_t *msg_size) {
    return FwConn_recv_all_cap(conn, msg, msg_size, FW_BUF_SIZE_RECV_ALL);
}

bool FwConn_get_addr(FwConn *conn, char **addr, size_t *addr_size) {
    char *internal_addr = inet_ntoa(conn->addr.sin_addr);
    if (internal_addr == NULL) { return false; }

    *addr_size = strlen(internal_addr);
    *addr      = malloc(*addr_size);
    memcpy(*addr, internal_addr, *addr_size);

    return true;
}
unsigned short FwConn_get_port(FwConn *conn) { return ntohs(conn->addr.sin_port); }

bool FwConn_close(FwConn *conn) { return closesocket(conn->sock) == 0; }

#endif // FW_IMPL
