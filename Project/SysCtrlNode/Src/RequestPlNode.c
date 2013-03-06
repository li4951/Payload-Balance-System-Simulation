#include "RequestPlNode.h"

#include "CommonDef.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_PLNODE 5
#define PLNODE_IP "127.0.0.1"

#define REQ_CONNECT "connect"
#define REQ_RELEASE "release"

static const int ports[MAX_PLNODE] = {8001, 8002, 8003, 8004, 8005};

int packUserReq(const char* reqType, char* userId, DataPack req) {
    if (reqType == NULL || req == NULL) {
        fprintf(stderr, "Parameter error: RequestPlNode packUserReq\n");
        exit(1);
    }

    memset(req, '$', sizeof(DataPack));
    req[PACK_LEN - 1] = '\0';
    char* p = req;
    memcpy(p, reqType, strlen(reqType));
    p = p + strlen(reqType) + 1;
    memcpy(p, userId, strlen(userId));

    return 0;
}

int getPlNodeConn(int* sockfd, int plNode) {
    struct sockaddr_in plNodeAddr;
    memset(&plNodeAddr, 0, sizeof(plNodeAddr));
    plNodeAddr.sin_family = AF_INET;
    inet_pton(AF_INET, PLNODE_IP, &plNodeAddr.sin_addr);
    plNodeAddr.sin_port = htons(ports[plNode - 1]);

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(*sockfd, (struct sockaddr*)&plNodeAddr, sizeof(plNodeAddr)) != 0) {
        fprintf(stderr, "connect error: RequestPlNode connect\n");
        exit(1);
    }
    return 0;
}

int connectPlNode(char* userId, int plNode) {
    int sockfd = -1;
    getPlNodeConn(&sockfd, plNode);
    assert(sockfd != -1);

    DataPack req;
    packUserReq(REQ_CONNECT, userId, req);
    write(sockfd, req, PACK_LEN);

    DataPack resp;
    int n = read(sockfd, resp, PACK_LEN);
    close(sockfd);

    char* p = strchr(resp, '$');
    *p = '\0';
    if (!strcmp(resp, "succeed")) {
        return 1;
    }
    return 0;

}

int releasePlNode(char* userId, int plNode) {
    int sockfd = -1;
    getPlNodeConn(&sockfd, plNode);
    assert(sockfd != -1);

    DataPack req;
    packUserReq(REQ_RELEASE, userId, req);
    write(sockfd, req, PACK_LEN);

    DataPack resp;
    int n = read(sockfd, resp, PACK_LEN);
    close(sockfd);

    char* p = strchr(resp, '$');
    *p = '\0';
    if (!strcmp(resp, "succeed")) {
        return 1;
    }
    return 0;

}

