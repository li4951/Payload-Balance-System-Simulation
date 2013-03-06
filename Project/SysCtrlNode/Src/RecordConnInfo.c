#include "RecordConnInfo.h"

#include "UpdatePlNodeState.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PLNODE 5

static const char* PATH_CONN_RECORD = "../PropertyDoc/ConnRecord/";
static int plNodeState[MAX_PLNODE] = {-1, -1, -1, -1, -1};//attention static

int openConnRecordFile(FILE** fp, char* mode, int plNode) {
    char filePath[100] = "";
    sprintf(filePath, "%s%d", PATH_CONN_RECORD, plNode);
    if ((*fp = fopen(filePath, mode)) == NULL) {
        fprintf(stderr, "fopen error: RecordConnInfo openFile");
        exit(1);
    }
    return 0;
}

int getMinPlNode() {
    int min = -1;
    int i = 0;
    for (i = 1; i <= MAX_PLNODE; i++) {
        if (!isRun(i)) {
            continue;
        }
        if ((getUserNumByPlNode(i) < getUserNumByPlNode(min)) || min == -1) {
            min = i;
        }
    }
    return min;
}

int getPlNodeByUser(char* userId) {
    int plNode = 1;
    for (plNode = 1; plNode <= MAX_PLNODE; plNode++) {
        FILE* fp = NULL;
        char mode[5] = "r";
        openConnRecordFile(&fp, mode, plNode);
        while (!feof(fp)) {
            char temp[50] = "";
            fscanf(fp, "%s", temp);
            if (!strcmp(temp, userId)) {
                fclose(fp);
                plNodeState[plNode - 1]--;
                return plNode;
            }
        }
        fclose(fp);
    }
    return -1;
}

void addUser(int plNode, char* userId) {
    FILE* fp = NULL;
    char mode[5] = "a";
    openConnRecordFile(&fp, mode, plNode);
    plNodeState[plNode - 1]++;    
    fprintf(fp, "%s\n", userId);
    fclose(fp);
}

void delUser(int plNode, char* userId) {
    if (plNode < 1 || plNode > MAX_PLNODE) {
        fprintf(stderr, "error: recordConnInfo delUser");
        exit(1);
    }

    FILE* fp = NULL;
    char mode[5] = "r";
    openConnRecordFile(&fp, mode, plNode);

    char buffer[1000][50];//define a buffer; attention overflow
    int count = 0;
    while (!feof(fp)) {
        char tempUserId[50];
        fscanf(fp, "%s", tempUserId);
        if (!strcmp(tempUserId, userId)) {
             continue;
        }
        strcpy(buffer[count++], tempUserId);
    }
    fclose(fp);

    strcpy(mode, "w");
    openConnRecordFile(&fp, mode, plNode);
    int i = 0;
    for (i = 0; i < count; i++) {
        fprintf(fp, "%s\n", buffer[i]);
    }
    fclose(fp);
}

int getUserNumByPlNode(int plNode) {
    if (plNodeState[plNode - 1] != -1) {
        return plNodeState[plNode - 1];
    }

    if (plNode < 1 || plNode > MAX_PLNODE) {
        fprintf(stderr, "Parameter error: RecordConnInfo getUserNumByPlNode plNode should be in [1, %d]\n", MAX_PLNODE);
        exit(1);
    }

    FILE* fp = NULL;
    char mode[5] = "r";
    openConnRecordFile(&fp, mode, plNode);
    
    int count = 0;
    while (!feof(fp)) {
        if (fgetc(fp) == '\n') {
            count++;
        }
    }
    fclose(fp);
    plNodeState[plNode - 1] = count;
    
    return count;
}

int getAllUsersByPlNode(int plNode, char (*users)[30]) {
    if (plNode < 1 || plNode > MAX_PLNODE) {
        fprintf(stderr, "error: recordConnInfo delUser");
        exit(1);
    }

    FILE* fp = NULL;
    char mode[5] = "r";
    openConnRecordFile(&fp, mode, plNode);

    int count = 0;
    while (!feof(fp)) {
        fscanf(fp, "%s", users[count]);
        count++;
    }
    strcpy(users[count], "");
    fclose(fp);
    return 0;
}

int cleanRecordByPlNode(int plNode) {
    FILE* fp = NULL;
    char mode[5] = "w";
    openConnRecordFile(&fp, mode, plNode);
    fclose(fp);
}
