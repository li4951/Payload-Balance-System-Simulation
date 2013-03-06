#include "ClientTest.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 5150 

//declare four type of request
static const char REQ_CONNECT[] = "connect";
static const char REQ_RELEASE[] = "release";
static const char REQ_GET_PLNODE_BY_USER[] = "getplnodebyuser";
static const char REQ_GET_USERNUM_BY_PLNODE[] = "getusernumbyplnode";

int packUserReq(const char* reqType, char* parameter, DataPack req) {
    if (reqType == NULL || req == NULL) {
        printf("packUserReq parameter error\n");
        return -1;
    }

    memset(req, '$', sizeof(DataPack));
    req[PACK_LEN - 1] = '\0';
    char*p = req;
    memcpy(p, reqType, strlen(reqType));
    p = p + strlen(reqType) + 1;
    memcpy(p, parameter, strlen(parameter));
    
    return 0;
}

int getServConn(int* sockfd) {
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &servAddr.sin_addr);
    servAddr.sin_port = htons(SERV_PORT);

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int err = connect(*sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr));
    if (err < 0) {
        fprintf(stderr, "connect error: ClientTest.c getServConn\n");
        exit(1);
    }
    return 0;
}

void connectReq(User user){
    int sockfd = -1;
    getServConn(&sockfd);
    assert(sockfd != -1);

    DataPack req;
    packUserReq(REQ_CONNECT, user.Id, req);
    
    write(sockfd, req, PACK_LEN);
    
    DataPack resp;
    int n = read(sockfd, resp, PACK_LEN);
    if (n != PACK_LEN) {
        printf("resp maybe wrong in ConnectReq\n");
    }

    *strchr(resp, '$') = '\0';
    int payloadId = atoi(resp);

    printf("%s is assigned on payloadNode %d\n", user.Id, payloadId);
    
    close(sockfd);
}

int releaseReq(User user) {
    int sockfd = -1;
    getServConn(&sockfd);
    assert(sockfd != -1);

    DataPack req;
    packUserReq(REQ_RELEASE, user.Id, req);
    
    write(sockfd, req, PACK_LEN);
    
    DataPack resp;
    int n = read(sockfd, resp, PACK_LEN);
    if (n != PACK_LEN) {
        printf("resp maybe wrong in ConnectReq\n");
    }

    *strchr(resp, '$') = '\0';
    int payloadId = atoi(resp);

    printf("%s is released from payloadNode %d\n", user.Id, payloadId);
    
    close(sockfd);

}

int getPlNodeByUser(User user) {
    int sockfd = -1;
    getServConn(&sockfd);
    assert(sockfd != -1);

    DataPack req;
    packUserReq(REQ_GET_PLNODE_BY_USER, user.Id, req);
    write(sockfd, req, PACK_LEN);

    DataPack resp;
    int n = read(sockfd, resp, PACK_LEN);
    if (n != PACK_LEN) {
        printf("resp maybe wrong in ConnectReq\n");
    }

    *strchr(resp, '$') = '\0';
    int payloadId = atoi(resp);

    printf("%s is on payloadnode %d\n", user.Id, payloadId);
    
    return 0;
}

unsigned int getUserNumByPlNode(int payloadId) {
    int sockfd = -1;
    getServConn(&sockfd);
    assert(sockfd != -1);

    DataPack req;
    char payloadIdStr[10] = "";
    sprintf(payloadIdStr, "%d", payloadId);
    packUserReq(REQ_GET_USERNUM_BY_PLNODE, payloadIdStr, req);

    write(sockfd, req, PACK_LEN);

    DataPack resp;
    int n = read(sockfd, resp, PACK_LEN);
    if (n != PACK_LEN) {
        printf("resp maybe wrong in ConnectReq\n");
    }

    *strchr(resp, '$') = '\0';
    int userNum = atoi(resp);

    printf("The number of users on  payloadnode %d is %d\n", payloadId, userNum);
    
    return 0;
}

int printUI() {
    printf("Simulation of Payload Balance System\n");
    printf("Please input four types instructor below:\n");
    printf("1)ConnectReq UserName----e.g. ConnectReq liyan\n");
    printf("2)ReleaseReq UserName----e.g. ReleaseReq liyan\n");
    printf("3)GetPnodeByUser UserName----e.g. GetPnodeByUser liyan\n");
    printf("4)GetUserNumByPnode PayloadId----e.g. GetUserNumByPnode 1\n");
    printf("5)help\n");
}

int shell() {
    printUI();
    while (1) {
        char order[100] = "";
        fgets(order, 100, stdin);

        char orderType[20] = "";
        char parameter[50] = "";
        sscanf(order, "%s%s", orderType, parameter);

        if (!strcmp("", orderType) || !strcmp("", parameter)) {
            printf("Instructor error\n");
            printUI();
        }
 
        if (!strcmp("ConnectReq", orderType)) {
            User user;
            strcpy(user.Id, parameter);
            connectReq(user);
        } else if (!strcmp("ReleaseReq", orderType)) {
            User user;
            strcpy(user.Id, parameter);
            releaseReq(user);
        } else if (!strcmp("GetPnodeByUser", orderType)) {
            User user;
            strcpy(user.Id, parameter);
            getPlNodeByUser(user);
        } else if (!strcmp("GetUserNumByPnode", orderType)) { //the only one that the instructor different from function name
            int payloadId = atoi(parameter);
            getUserNumByPlNode(payloadId);
        } else if (!strcmp("help", orderType)) {
            printUI();
        } else {
            printf("Instructor error\n");
            printUI();
        }
    }
}

int main() {
    shell();
    return 0;
}
