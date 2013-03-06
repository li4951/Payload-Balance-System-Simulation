#include "PlNode.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define PAY_LIMIT 1000
#define MAX_PLNODE 5

static const int ports[MAX_PLNODE] = {8001, 8002, 8003, 8004, 8005};
static int plNodeId = 0;

static const char REQ_CONNECT[] = "connect";
static const char REQ_RELEASE[] = "release";

int getReqType(DataPack dataPack, char* reqType) {
    int tmp = strchr(dataPack, '$') - dataPack;
    strncpy(reqType, dataPack, tmp);
    reqType[tmp] = '\0';
    return 0;
}

int getUserId(DataPack dataPack, char* userId) {
    char* p = strchr(dataPack, '$') + 1;
    int tmp = strchr(p, '$') - p;
    strncpy(userId, p, tmp);
    userId[tmp] = '\0';
    return 0;
}

int PackResp(char* resp, char* content) {
    memset(resp, '$', sizeof(DataPack));
    resp[PACK_LEN - 1] = '\0';
    memcpy(resp, content, strlen(content));
    return 0;
}

int processConnectReq(char* userId, int connFd) {
    DataPack resp;
    PackResp(resp, "succeed");
    write(connFd, resp, PACK_LEN);
    printf("%s is connected in\n", userId);
    return 0;
}

int processReleaseReq(char* userId, int connFd) {
    DataPack resp;
    PackResp(resp, "succeed");
    write(connFd, resp, PACK_LEN);
    printf("%s is released\n", userId);
    return 0;
}

void process() {
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(ports[plNodeId - 1]);

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = SO_REUSEADDR;
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        fprintf(stderr,"setsockopt error: Payload process\n");
        exit(1);
    }
    if (bind(listenFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "bind error: Payload Process\n");
        exit(1);
    }
    listen(listenFd, PAY_LIMIT);
    
    while (1) {
        struct sockaddr_in connAddr;
        socklen_t connAddrLen = sizeof(connAddr);
        int connFd = accept(listenFd, (struct sockaddr*)&connAddr, &connAddrLen);
        assert(connFd != -1);

        DataPack req;
        memset(req, 0, sizeof(req));
        int n = read(connFd, req, PACK_LEN);
        if (n == 0) { //detect pack
            close(connFd);
            continue;
        }

        assert(n == PACK_LEN);

        char reqType[30] = "";
        getReqType(req, reqType);
        char userId[30] = "";
        getUserId(req, userId);
        if (!strcmp(reqType, REQ_CONNECT)) {
            processConnectReq(userId, connFd);
        } else if (!strcmp(reqType, REQ_RELEASE)) {
            processReleaseReq(userId, connFd);
        }

        close(connFd);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./PlNode PlNodeId\n");
        exit(1);
    }
    plNodeId = atoi(argv[1]);
    if (plNodeId < 1 || plNodeId > MAX_PLNODE) {
        fprintf(stderr, "PlNodeId should be in [1, %d]\n", MAX_PLNODE);
        exit(1);
    }
    printf("PLNODE ID: %d\tPORT: %d\n", plNodeId, ports[plNodeId - 1]);
    process();
    return 0;
}
