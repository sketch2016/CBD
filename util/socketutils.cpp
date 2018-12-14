#include "include/util/socketutils.h"
#include "include/util/logger.h"

#define TAG "Socket Utils"

//SocketUtils
SocketUtils::SocketUtils() {
    //TODO nothing
}

Socket *SocketUtils::getSocket(const char *ip, int port) {
    /* 创建socket */
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(fd < 0) {
        LOGE(TAG,"socket create failed!");
        return nullptr;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int trycnt = 5;
    while(connect(fd,(struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) < 0 && trycnt > 5) {
        LOGE(TAG,"connect fail !");
        trycnt --;
    }

    if (trycnt == 0) {
        return nullptr;
    }

    Socket *mysocket = new Socket(fd);
    mysocket->net_socket_set_nonblock(1);
    mysocket->set_socket_tcp_nodelay(1);
    LOGD(TAG,"connect ok!");
    return mysocket;
}


//Socket
Socket::Socket(int fd) {
    mSocketFd = fd;
}

Socket::~Socket() {
    close(mSocketFd);
}


int Socket::net_socket_set_nonblock(int enable) {
    int flags;
    if((flags = fcntl(mSocketFd, F_GETFL, 0)) < 0) {
        LOGE(TAG,"FCNTL(F_GETFL) error");
        return -1;
    }

    if(enable == 1) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    if(fcntl(mSocketFd, F_SETFL, flags) < 0) {
        LOGE(TAG,"FCNTL(F_SETFL) error");
        return -1;
    }

    return 0;
}

int Socket::set_socket_tcp_nodelay(int enable) {
    unsigned int flag = enable ? 1 : 0;
    if(setsockopt(mSocketFd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag))) {
        LOGE(TAG,"SETSOCKOPT IPPROTO_TCP TCP_NODELAY fail");
        return -1;
    }
    return 0;
}

int Socket::net_socket_send(void* p_data, int len, unsigned int timeout) {
    int try_cnt = 0;
    int offset = 0;

    while(offset < len) {
        int tlen = send(mSocketFd,(const char *) p_data+offset, len-offset, 0);
        if(tlen == 0) {
            LOGD(TAG,"net_socket_send fd<%1> tlen==0, errno=%2!",mSocketFd,errno);
            return -1;
        } else if (tlen < 0) {
            if (errno == EAGAIN || errno == EBUSY || errno == EWOULDBLOCK || errno == 0){
                if (try_cnt >= (int)timeout && timeout != 0xffffffff) {
                    LOGE(TAG,"net_socket_send fd<%1> tlen=%2, errno=%3,send failed because timout!",mSocketFd,tlen,errno);
                    return 0; //timeout
                } else {
                    usleep(10000);
                    try_cnt += 10;
                    continue;
                }
            } else {
                LOGE(TAG,"net_socket_send fd<%1> tlen=%2, errno=%3",mSocketFd,tlen,errno);
                return -1;
            }
        } else {
            try_cnt = 0;
            offset += tlen;
            if (offset < len) {
                usleep(10000);
            }
        }
    }
    return offset;
}

int Socket::net_socket_recv(void* data, int len, unsigned int timeout) {
    int rlen;
    int offset = 0;
    int try_cnt = 0;
    while(offset < len) {
        rlen = recv(mSocketFd,data+offset, len-offset, 0);
        if(rlen == 0) {
            LOGE(TAG,"fd<%1> rlen==0 errno=%2!\n",mSocketFd,errno);
            return -1;
        } else if (rlen < 0) {
            if (errno == EAGAIN || errno == EBUSY || errno == EWOULDBLOCK || errno == 0) {
                if (offset > 0) {
                    return offset;
                }

                if (try_cnt >= (int)timeout && timeout != 0xffffffff) {
                    //printf("client<%d> receive timeout, errno=%d\n",fd, errno);
                    break; //timeout
                } else {
                    usleep(10000);
                    try_cnt += 10;
                    continue;
                }
            } else {
                LOGE(TAG,"fd<%1> rlen=%2 errno=%3!\n",mSocketFd,rlen,errno);
                return -1;
            }
        } else {
            try_cnt = 0;
            offset += rlen;
            if (offset < len) {
                usleep(10000);
            }
        }
    }
    return offset;
}

int Socket::send_cmd(char* cmd,char *rsp, int rsp_len) {
    int tlen = net_socket_send(cmd,strlen(cmd)+1,5000);
    if (tlen < strlen(cmd)+1) {
        LOGE(TAG,"send_cmd failed!");
        return -1;
    }

    memset(rsp,0,rsp_len);
    int rlen = 0;
    int trycnt = 10;
    int offset = 0;
    while(trycnt > 0) {
        rlen = net_socket_recv(rsp+offset,rsp_len-offset,1000);
        if (rlen < 0) {
            LOGE(TAG,"server err!\n");
            return -1;
        }
        trycnt --;
        offset += rlen;
        if (rsp[offset-1] == 0) {
            break;
        }
    }

    if (offset <= 0) {
        LOGE(TAG,"recv timout!");
        return -1;
    }

    LOGD(TAG,"send cmd %1,fd is %2",rsp,mSocketFd);
    return 0;
}





