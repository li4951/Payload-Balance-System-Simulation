#ifndef __RECORD_CONN_INFO_H__
#define __RECORD_CONN_INFO_H_

int getMinPlNode();

int getPlNodeIdByUser(char* userId);

void addUser(int plNode, char* userId);

void delUser(int plNode, char* userId);

int getUserNumByPlNode(int plNode);

int getAllUsersByPlNode(int plNode, char (*users)[30]);

int cleanRecordByPlNode(int plNode);

#endif
