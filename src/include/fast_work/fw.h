#ifndef FW_H
#define FW_H

// TODO: UDP recv all

#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#else
#error "not implemented"
#endif

typedef struct sockaddr_in FwAddr;

typedef struct {
    SOCKET sock;
    FwAddr addr;
} FwConn;

#ifndef FW_ASSERT
#include <assert.h>
#define FW_ASSERT(cond, msg) assert(cond &&msg)
#endif // FW_ASSERT

#ifndef FW_MALLOC
#include <stdlib.h>
#define FW_MALLOC(size, elem_size) malloc(size *elem_size)
#endif // FW_MALLOC

#ifndef FW_REALLOC
#include <stdlib.h>
#define FW_REALLOC(addr, size, elem_size) realloc(addr, size *elem_size)
#endif // FW_MALLOC

#ifndef FW_FREE
#include <stdlib.h>
#define FW_FREE(addr) free(addr)
#endif // FW_MALLOC

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
/// @brief get the error code
///
/// @return the error code
///
int fw_get_error_code();
///
/// @brief get the error msg from the error code
///
/// @param error the error code
/// @param error_size the error size that we got
/// @return the error message THAT NEED TO BE DEALLOCATED, can be NULL if error
///
char *fw_get_error_msg(int error, size_t *error_size);

// NOLINTBEGIN

///
/// @brief bind and listen incomming connection
///
/// @param[out] listener the listener to create
/// @param[in] addr the address to connect to (ip address or host name)
/// @param[in] port the port to connect to
/// @return true if not failed otherwise false
///
bool FwConn_TCP_create_listener(FwConn *listener, const char *addr, unsigned short port);
///
/// @brief connect to a listener
///
/// @param[out] conn the connection to create
/// @param[in] addr the address to connect to (ip address or host name)
/// @param[in] port the port to connect to
/// @return true if not failed otherwise false
///
bool FwConn_TCP_create_conn(FwConn *conn, const char *addr, unsigned short port);

///
/// @brief accept the next incoming connection (blocking)
///
/// @param[in, out] listener the listener to accpet the connection from
/// @param[out] conn the connection we get
/// @return true if not failed otherwise false
///
bool FwConn_TCP_accept(FwConn *listener, FwConn *conn);

///
/// @brief send data to a socket
///
/// @param[in, out] conn the socket to send to
/// @param[in] msg the message to send
/// @param[in] msg_size the size of the message to send
/// @return true if not failed otherwise false
///
bool FwConn_TCP_send(FwConn *conn, const char *msg, size_t msg_size);

///
/// @brief check if we can call recv during max usec_timeout micro seconds
///
/// @param[in, out] conn the connection to check if we can recv from
/// @param[out] result the result of the operation
/// @param[in] usec_timeout the maximum time to wait in micro seconds
/// @return true if not failed othewise false
///
bool FwConn_TCP_check_timeout(FwConn *conn, bool *result, size_t usec_timeout);
///
/// @brief receive a message of a certain length from the connection (block)
///
/// @param[in, out] conn the connection to receive from
/// @param[in, out] msg the buffer to put the message in, must have at least msg_size allocated
/// @param[in] msg_size the maximum message size, all won't be filled except if the message is >= to
/// msg_size and the buffer won't be zeroed out
/// @return true if not failed otherwise false
///
bool FwConn_TCP_recv(FwConn *conn, char *msg, size_t msg_size);
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
bool FwConn_TCP_recv_all_cap(FwConn *conn, char **msg, size_t *msg_size, size_t cap);
/// @brief receive a message of any length from the connection (block)
/// call FwConnTCP_recv_all_cap with the capacity equal to FW_BUF_SIZE_RECV_ALL (see
/// FwConnTCP_recv_all_cap and FW_BUF_SIZE_RECV_ALL)
///
/// @param[in, out] conn the connection to receive from
/// @param[out] msg a pointer to a char * that will be allocated (and will need to be free) and will
/// hold the message received
/// @param[out] msg_size the message size read from the function
/// @return true if not failed othewise false
///
bool FwConn_TCP_recv_all(FwConn *conn, char **msg, size_t *msg_size);

bool FwConn_UDP_bind(FwConn *server, const char *addr, unsigned short port);

bool FwConn_UDP_create_conn(FwConn *conn);
bool FwConn_UDP_create_addr(FwAddr *addr, const char *address, unsigned short port);

bool FwConn_UDP_send_to(FwConn *conn, FwAddr *addr_to, const char *msg, size_t msg_size);

bool FwConn_UDP_recv_from(FwConn *conn, FwAddr *addr_from, char *msg, size_t msg_size);
bool FwConn_UDP_recv_all_cap_from(FwConn *conn, FwAddr *addr_from, char **msg, size_t *msg_size,
                                  size_t cap);
bool FwConn_UDP_recv_all_from(FwConn *conn, FwAddr *addr_from, char **msg, size_t *msg_size);

///
/// @brief close the listener
///
/// @param[in, out] listener the listener to close
/// @return true if not failed otherwise false
///
bool FwConn_close(FwConn *listener);

///
/// @brief get the ip address of the connection
///
/// @param[in, out] address the address to get the ip from
/// @param[out] addr a pointer to a char * that will be allocated (and will need to be free) and
/// will hold the ip adress
/// @param[out] addr_size the size of the allocated char *
/// @return true if not failed otherwise false
///
bool FwAddr_get_addr(FwAddr *address, char **addr, size_t *addr_size);
///
/// @brief get the port of the connection
///
/// @param[in, out] address the connection to get the port from
/// @return the port (can't failed so no error code)
///
unsigned short FwAddr_get_port(FwAddr *address);

// NOLINTEND

#endif // FW_H
// #define FW_IMPL
#ifdef FW_IMPL

#ifndef FW_BUF_SIZE_RECV_ALL
#define FW_BUF_SIZE_RECV_ALL 512
#endif // FW_BUF_SIZE_RECV_ALL

#define FW_USEC_TO_SEC_DIV (size_t)1e6

static bool impl_fw_resolve(const char *addr, unsigned long *result) {
    struct hostent *host = gethostbyname(addr);
    if (host == NULL || *host->h_addr_list == NULL) { return false; }
    *result = (*(struct in_addr *)(host->h_addr)).s_addr;
    return true;
}
static bool impl_fw_TCP_create_socket(FwConn *conn, const char *addr, // NOLINT
                                      unsigned short port) {
    conn->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (conn->sock == INVALID_SOCKET) { return false; }

    memset(&conn->addr, 0, sizeof(conn->addr));

    if (!impl_fw_resolve(addr, &conn->addr.sin_addr.s_addr)) { return false; }

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port   = htons(port);
    return true;
}

bool fw_init(void) {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
bool fw_quit(void) { return WSACleanup() == 0; }

int   fw_get_error_code() { return WSAGetLastError(); }
char *fw_get_error_msg(int error, size_t *error_size) {
    char *local_res = NULL;
    *error_size     = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *)&local_res, 0, NULL);

    // convert allocated pointer of window to allocated pointer of C
    size_t local_res_size = strlen(local_res);
    char  *res            = FW_MALLOC(local_res_size, sizeof(*res));
    memcpy(res, local_res, local_res_size);

    LocalFree(local_res);

    return res;
}

bool FwConn_TCP_create_listener(FwConn *listener, const char *addr, unsigned short port) {
    if (!impl_fw_TCP_create_socket(listener, addr, port)) { return false; }

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
bool FwConn_TCP_create_conn(FwConn *conn, const char *addr, unsigned short port) {
    if (!impl_fw_TCP_create_socket(conn, addr, port)) { return false; }

    if (connect(conn->sock, (struct sockaddr *)&conn->addr, sizeof(conn->addr)) != 0) {
        closesocket(conn->sock);
        return false;
    }
    return true;
}

bool FwConn_TCP_accept(FwConn *listener, FwConn *conn) {
    int addrlen = sizeof(conn->addr);
    conn->sock  = accept(listener->sock, (struct sockaddr *)&conn->addr, &addrlen);
    return conn->sock != INVALID_SOCKET;
}

bool FwConn_TCP_send(FwConn *conn, const char *msg, size_t msg_size) {
    return send(conn->sock, msg, msg_size, 0) != SOCKET_ERROR;
}

bool FwConn_TCP_check_timeout(FwConn *conn, bool *result, size_t usec_timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(conn->sock, &readfds);

    struct timeval timeout;
    timeout.tv_sec  = usec_timeout / FW_USEC_TO_SEC_DIV;
    timeout.tv_usec = usec_timeout % FW_USEC_TO_SEC_DIV;

    int n_sock_read = select(0, &readfds, NULL, NULL, &timeout);
    if (n_sock_read == SOCKET_ERROR) { return false; }
    *result = n_sock_read == 1;
    return true;
}
bool FwConn_TCP_recv(FwConn *conn, char *msg, size_t msg_size) {
    return recv(conn->sock, msg, msg_size, 0) != SOCKET_ERROR;
}
bool FwConn_TCP_recv_all_cap(FwConn *conn, char **msg, size_t *msg_size, size_t cap) {
    FW_ASSERT(cap > 0, "cap must be > 0 so the msg vector can grow");
    *msg = FW_MALLOC(cap, sizeof(**msg));
    if (!*msg) { return false; }
    *msg_size = 0;

    char    buf[FW_BUF_SIZE_RECV_ALL];
    ssize_t bytes_read = recv(conn->sock, buf, sizeof(buf), 0);

    if (bytes_read != sizeof(buf)) {
        if (bytes_read == SOCKET_ERROR) {
            FW_FREE(*msg);
            return false;
        } else if (bytes_read > 0) {
            if ((*msg_size) + bytes_read > cap) {
                while ((*msg_size) + bytes_read > cap) { cap <<= 1; }
                *msg = FW_REALLOC(*msg, cap, sizeof(**msg));
                if (!*msg) { return false; }
            }
            memcpy((*msg) + *msg_size, buf, bytes_read);
            *msg_size += bytes_read;
        }
    }
    while (bytes_read == sizeof(buf)) {
        if (bytes_read == SOCKET_ERROR) {
            FW_FREE(*msg);
            return false;
        } else if (bytes_read > 0) {
            if ((*msg_size) + bytes_read > cap) {
                while ((*msg_size) + bytes_read > cap) { cap <<= 1; }
                *msg = FW_REALLOC(*msg, cap, sizeof(**msg));
                if (!*msg) { return false; }
            }
            memcpy((*msg) + *msg_size, buf, bytes_read);
            *msg_size += bytes_read;
        }
        bool res;
        if (!FwConn_TCP_check_timeout(conn, &res, 0)) { return false; }
        if (!res) { break; }
        bytes_read = recv(conn->sock, buf, sizeof(buf), 0);
    }

    return true;
}
bool FwConn_TCP_recv_all(FwConn *conn, char **msg, size_t *msg_size) {
    return FwConn_TCP_recv_all_cap(conn, msg, msg_size, FW_BUF_SIZE_RECV_ALL);
}

bool FwConn_UDP_bind(FwConn *server, const char *addr, unsigned short port) {
    if (!FwConn_UDP_create_conn(server)) { return false; }
    if (!FwConn_UDP_create_addr(&server->addr, addr, port)) { return false; }

    if (bind(server->sock, (struct sockaddr *)&server->addr, sizeof(server->addr)) != 0) {
        closesocket(server->sock);
        return false;
    }
    return true;
}
bool FwConn_UDP_create_conn(FwConn *conn) {
    conn->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (conn->sock == INVALID_SOCKET) { return false; }
    return true;
}
bool FwConn_UDP_create_addr(FwAddr *address, const char *addr, unsigned short port) {
    memset(address, 0, sizeof(*address));

    if (!impl_fw_resolve(addr, &address->sin_addr.s_addr)) { return false; }

    address->sin_family = AF_INET;
    address->sin_port   = htons(port);

    return true;
}

bool FwConn_UDP_send_to(FwConn *conn, FwAddr *addr_to, const char *msg, size_t msg_size) {
    return sendto(conn->sock, msg, msg_size, 0, (struct sockaddr *)addr_to, sizeof(*addr_to)) !=
           SOCKET_ERROR;
}

bool FwConn_UDP_recv_from(FwConn *conn, FwAddr *addr_from, char *msg, size_t msg_size) {
    if (addr_from == NULL) {
        return recvfrom(conn->sock, msg, msg_size, 0, NULL, NULL) != SOCKET_ERROR;
    }
    int fromlen = sizeof(*addr_from);
    return recvfrom(conn->sock, msg, msg_size, 0, (struct sockaddr *)addr_from, &fromlen) !=
           SOCKET_ERROR;
}
bool FwConn_UDP_recv_all_cap_from(FwConn *conn, FwAddr *addr_from, char **msg, size_t *msg_size,
                                  size_t cap) {
    FW_ASSERT(cap > 0, "cap must be > 0 so the msg vector can grow");
    *msg = FW_MALLOC(cap, sizeof(**msg));
    if (!*msg) { return false; }
    *msg_size = 0;

    int fromlen = sizeof(*addr_from);

    int recv_from_len = -1;
    while (recv_from_len == -1) {
        recv_from_len =
            recvfrom(conn->sock, *msg, cap, MSG_PEEK, (struct sockaddr *)addr_from, &fromlen);

        int err = WSAGetLastError();
        if (err && err != WSAEMSGSIZE) {
            WSASetLastError(err); // put pack the error
            return false;
        }
        WSASetLastError(err);     // put pack the error

        if (recv_from_len == -1) {
            cap <<= 1;
            *msg = FW_REALLOC(*msg, cap, sizeof(**msg));
            if (!*msg) { return false; }
        }
    }
    *msg_size = recv_from_len;
    return recvfrom(conn->sock, *msg, *msg_size, 0, (struct sockaddr *)addr_from, &fromlen) !=
           SOCKET_ERROR;
}
bool FwConn_UDP_recv_all_from(FwConn *conn, FwAddr *addr_from, char **msg, size_t *msg_size) {
    return FwConn_UDP_recv_all_cap_from(conn, addr_from, msg, msg_size, FW_BUF_SIZE_RECV_ALL);
}

bool FwConn_close(FwConn *conn) { return closesocket(conn->sock) == 0; }

bool FwAddr_get_addr(FwAddr *address, char **addr, size_t *addr_size) {
    char *internal_addr = inet_ntoa(address->sin_addr);
    if (internal_addr == NULL) { return false; }

    *addr_size = strlen(internal_addr);
    *addr      = FW_MALLOC(*addr_size, sizeof(**addr));
    if (!*addr) { return false; }
    memcpy(*addr, internal_addr, *addr_size);

    return true;
}
unsigned short FwAddr_get_port(FwAddr *address) { return ntohs(address->sin_port); }

#endif //
