#ifndef __PROCESS_REQ_FROM_CLIENT_H__
#define __PROCESS_REQ_FROM_CLIENT_H__

#include "CommonDef.h"

int connectReq(DataPack req, int clientFd);

int releaseReq(DataPack req, int clientFd);

int getPlNodeByUserReq(DataPack req, int clientFd);

unsigned int getUserNumByPlNodeReq(DataPack req, int clientFd);

#endif
