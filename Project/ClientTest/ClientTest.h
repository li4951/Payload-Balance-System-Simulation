#ifndef __CLIENT_TEST_H__
#define __CLIENT_TEST_H__

#define MAX_LEN 100
#define PACK_LEN 60

//declare user type
typedef struct {
    char Id[MAX_LEN];
}User;

//define the format of request or response
typedef char DataPack[PACK_LEN];

#endif

