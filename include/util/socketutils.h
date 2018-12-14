#ifndef SOCKET_H
#define SOCKET_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

class Socket
{
public:
    Socket(int fd);
    ~Socket();
    int net_socket_set_nonblock(int enable);
    int set_socket_tcp_nodelay(int enable);
    int net_socket_send(void* p_data, int len, unsigned int timeout);
    int net_socket_recv(void* data, int len, unsigned int timeout);
    int send_cmd(char* cmd,char *rsp, int rsp_len);
    //void destroy();

private:
    int mSocketFd;

};

class SocketUtils
{
public:
    SocketUtils();
    static Socket *getSocket(const char* ip, int port);
};

#endif // SOCKET_H
