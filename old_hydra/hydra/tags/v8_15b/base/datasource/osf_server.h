/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <rpc/types.h>


typedef struct {
	u_int rpcevt_len;
	char *rpcevt_val;
} rpcevt;
bool_t xdr_rpcevt();

#define DAQPROG ((u_int)0x20000001)
#define DAQVERS ((u_int)1)
#define ONLINEEVT ((u_int)1)
extern rpcevt *onlineevt_1();