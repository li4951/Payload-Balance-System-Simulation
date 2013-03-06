#include "SysCtrlNode.h"

#include "ProcessReqFromClient.h"
#include "UpdatePlNodeState.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define SERV_PORT 5150

//declare four type of request from client
static const char REQ_CONNECT[] = "connect";
static const char REQ_RELEASE[] = "release";
static const char REQ_GET_PLNODE_BY_USER[] = "getplnodebyuser";
static const char REQ_GET_USERNUM_BY_PLNODE[] = "getusernumbyplnode";

int getReqType(DataPack dataPack, char* reqType) {
    int tmp = strchr(dataPack, '$') - dataPack;
    strncpy(reqType, dataPack, tmp);
    reqType[tmp] = '\0';
    return 0;
}

void process() {
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERV_PORT);

    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = SO_REUSEADDR;
    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
        fprintf(stderr, "setsockopt error: SysCtrlNode process\n");
        exit(1);
    }
    if (bind(listenFd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "bind error: SysCtrlNode process\n");
        exit(1);
    }
    listen(listenFd, 10000);
    
    while (1) {
        struct sockaddr_in connAddr;
        socklen_t connAddrLen = sizeof(connAddr);
        int connFd = accept(listenFd, (struct sockaddr*)&connAddr, &connAddrLen);

        assert(connFd != -1); 

        DataPack req;
        memset(req, 0, sizeof(req));
        int n = read(connFd, req, PACK_LEN);
        assert(n == PACK_LEN);
        
        char reqType[30];
        getReqType(req, reqType);
        if (!strcmp(reqType, REQ_CONNECT)) {
            connectReq(req, connFd);
        } else if (!strcmp(reqType, REQ_RELEASE)) {
            releaseReq(req, connFd);
        } else if (!strcmp(reqType, REQ_GET_PLNODE_BY_USER)) {
            getPlNodeByUserReq(req, connFd);
        } else if (!strcmp(reqType, REQ_GET_USERNUM_BY_PLNODE)) {
            getUserNumByPlNodeReq(req, connFd);
        }
        close(connFd);
    }
}

int main() {
    /*create thread to detect PlNode state*/
    pthread_t threadDetectPlNode;
    int err = 0;
    err =  pthread_create(&threadDetectPlNode, NULL, updatePlNodeState, NULL);
    if (err != 0) {
        fprintf(stderr, "create thread error: %s\n", strerror(err));
        exit(1);
    }

    /*listen the client and response*/
    process();
    return 0;
}
