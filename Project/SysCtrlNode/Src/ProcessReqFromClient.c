#include "ProcessReqFromClient.h"

#include "CommonDef.h"
#include "RecordConnInfo.h"
#include "RequestPlNode.h"

#include <stdio.h>
#include <string.h>

int getUserId(DataPack dataPack, char* userId) {
    char* p = strchr(dataPack, '$') + 1;
    int tmp = strchr(p, '$') - p;
    strncpy(userId, p, tmp);
    userId[tmp] = '\0';
    return 0;
}

int connectReq(DataPack req, int clientFd) {
    char userId[20] = "";
    getUserId(req, userId);

    int plNode = getMinPlNode();
    if (plNode == -1) {
        printf("no alive plnode\n");
        return 0;
    }    
    printf("%s will be assigned on plnode %d\n", userId, plNode);

    DataPack resp;
    memset(resp, '$', sizeof(resp));

    if (connectPlNode(userId, plNode) == 1) { 
        addUser(plNode, userId);
        resp[0] = '0' + plNode;
    } else {
        resp[0] = '0';
    }
    resp[PACK_LEN - 1] = '\0';
    write(clientFd, resp, PACK_LEN);
    return 0;
}

int releaseReq(DataPack req, int clientFd) {
    char userId[20] = "";
    getUserId(req, userId);

    int plNode = getPlNodeByUser(userId);
    printf("%s will be release from plnode %d\n", userId, plNode);

    DataPack resp;
    memset(resp, '$', sizeof(resp));

    if (releasePlNode(userId, plNode) == 1) {
        delUser(plNode, userId);
        resp[0] = '0' + plNode;
    } else {
        resp[0] = '0';
    }
    resp[PACK_LEN - 1] = '\0';
    write(clientFd, resp, PACK_LEN);
    return 0;
}

int getPlNodeByUserReq(DataPack req, int clientFd) {
    char userId[20] = "";
    getUserId(req, userId);
    int plNode = getPlNodeByUser(userId);

    DataPack resp;
    memset(resp, '$', sizeof(resp));

    char plNodeStr[10] = "";
    sprintf(plNodeStr, "%d", plNode);
    strncpy(resp, plNodeStr, strlen(plNodeStr));
    resp[PACK_LEN - 1] = '\0';

    write(clientFd, resp, PACK_LEN);
    return 0;
}

unsigned int getUserNumByPlNodeReq(DataPack req, int clientFd) {
    char plNodeStr[10] = "";
    char* p = strchr(req, '$');
    p++;
    int temp = strchr(p, '$') - p;
    strncpy(plNodeStr, p, temp);
    plNodeStr[temp] = '\0';

    int plNode = atoi(plNodeStr);
    int num = getUserNumByPlNode(plNode);
    
    DataPack resp;
    memset(resp, '$', sizeof(resp));
    char numStr[10] = "";
    sprintf(numStr, "%d", num);
    strncpy(resp, numStr, strlen(numStr));
    resp[PACK_LEN - 1] = '\0';

    write(clientFd, resp, PACK_LEN);
    return 0;
}
