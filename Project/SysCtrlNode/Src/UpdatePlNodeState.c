#include "UpdatePlNodeState.h"

#include "RequestPlNode.h"
#include "RecordConnInfo.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define PLNODE_IP "127.0.0.1"
#define MAX_PLNODE 5

static const int ports[MAX_PLNODE] = {8001, 8002, 8003, 8004, 8005};
static unsigned int plNodeState = 0; //global varible

int detectPlNodeState(int port) {
    int run = 0;

    struct sockaddr_in plNodeAddr;
    memset(&plNodeAddr, 0, sizeof(plNodeAddr));
    plNodeAddr.sin_family = AF_INET;
    inet_pton(AF_INET, PLNODE_IP, &plNodeAddr.sin_addr);
    plNodeAddr.sin_port = htons(port);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, (struct sockaddr*)&plNodeAddr, sizeof(plNodeAddr)) != 0) {
//        printf("connect failed, so payloadnode in port %d is not running\n", port);
        run = 0;
    } else {
//        printf("connect ok, so payloadnode in port %d is running\n", port);
        run = 1;
    }
    close(sockfd);
    return run;
}

void* updatePlNodeState() {
    plNodeState = 0;
    while (1) { 
        int i = 0;
        for (i = 1; i <= MAX_PLNODE; i++) {
            if (detectPlNodeState(ports[i - 1]) == 1) {
                plNodeState |= ((unsigned int)1 << (MAX_PLNODE - i));
            } else {
                int trans = 0;
                unsigned int testBit = (unsigned int)1 << (MAX_PLNODE - i);
                if ((plNodeState & testBit) != 0) { // attention priority level
                    printf("will transfer the payload %d to other node\n", i);
                    trans = 1;
                }
                plNodeState &= (~((unsigned int)1 << (MAX_PLNODE - i)));
                
                if (trans) {
                    transfer(i);
                }
            }
        }
        printf("plNodeState: 0x%x\n", plNodeState);

        sleep(1);
    }
    return NULL;
}

int isRun(unsigned int plNode) {
    unsigned int testBit = (unsigned int)1 << (MAX_PLNODE - plNode); 
    if ((plNodeState & testBit) != 0) { // attention!not plNodeState & testBit
        return 1;
    }
    return 0;
}

int transfer(int plNode) {
    assert((plNode >= 1) && (plNode <= MAX_PLNODE));
    char users[300][30]; //attention! overflow
    getAllUsersByPlNode(plNode, users);

    int i = 0;
    while (strcmp(users[i], "")) {
        int minPlNode = getMinPlNode();
        if (minPlNode == -1) {
            printf("no alive plnode\n");
            return 0;
        }
        
        if (connectPlNode(users[i], minPlNode) == 1) {
            addUser(minPlNode, users[i]);
            printf("%s was transfered to plnode %d\n", users[i], minPlNode);
        }
        i++;
    }

    cleanRecordByPlNode(plNode);

    return 0;
}
