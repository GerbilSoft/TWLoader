// Based on ctrulib's ptmu.c.
#include <stdlib.h>
#include <3ds/types.h>
#include <3ds/result.h>
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/services/ptmu.h>
#include <3ds/ipc.h>

#include "ns_x.h"

static Handle nsxHandle;
static int nsxRefCount;

Result nsxInit(void)
{
	if (AtomicPostIncrement(&nsxRefCount)) return 0;
	Result res = srvGetServiceHandle(&nsxHandle, "ns:s");
	if (R_FAILED(res)) AtomicDecrement(&nsxRefCount);
	return res;
}

void nsxExit(void)
{
	if (AtomicDecrement(&nsxRefCount)) return;
	svcCloseHandle(nsxHandle);
}

Result NSSX_SetTWLBannerHMAC(const u8 *sha1_hmac)
{
	Result ret=0;
	u32 *cmdbuf = getThreadCommandBuffer();

	const u32 *sha1_u32 = (const u32*)sha1_hmac;
	cmdbuf[0] = IPC_MakeHeader(0xD,5,0); // 0xD0140
	cmdbuf[1] = sha1_u32[0];
	cmdbuf[2] = sha1_u32[1];
	cmdbuf[3] = sha1_u32[2];
	cmdbuf[4] = sha1_u32[3];
	cmdbuf[5] = sha1_u32[4];

	if(R_FAILED(ret = svcSendSyncRequest(nsxHandle)))return ret;

	return (Result)cmdbuf[1];
}
