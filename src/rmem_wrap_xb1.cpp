/*
 * Copyright (c) 2017 by Milos Tosic. All Rights Reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#define _CRT_SECURE_NO_WARNINGS 1

#include "rmem_platform.h"

#if RMEM_PLATFORM_XBOXONE

#include <xmem.h>

static inline uint32_t getOverhead(size_t /*_size*/)
{
	return 8;
}

PVOID WINAPI detour_XMEMALLOC_ROUTINE(SIZE_T dwSize, ULONGLONG dwAttributes)
{
	XALLOC_ATTRIBUTES att;
	att.dwAttributes = dwAttributes;
	PVOID ptr = XMemAllocDefault(dwSize, dwAttributes);
	rmemAlloc(att.s.dwAllocatorId, ptr, (uint32_t)dwSize, getOverhead(dwSize));
	return ptr;
}

VOID WINAPI detour_XMEMFREE_ROUTINE(PVOID lpAddress, ULONGLONG dwAttributes)
{
	XALLOC_ATTRIBUTES att;
	att.dwAttributes = dwAttributes;
	rmemFree(att.s.dwAllocatorId, lpAddress);
	XMemFreeDefault(lpAddress, dwAttributes);
}

void detour_exit(void);

extern "C"
{
	void rmemUnhookAllocs();

	void rmemHookAllocs(int /*_isLinkerBased*/)
	{
		XMemSetAllocationHooks(	&detour_XMEMALLOC_ROUTINE,
								&detour_XMEMFREE_ROUTINE );

		atexit(detour_exit);

		rmemInit(0);
	}

	void rmemUnhookAllocs()
	{
		XMemSetAllocationHooks(0, 0);
		rmemShutDown();
	}
};

void detour_exit(void)
{
	rmemUnhookAllocs();
}

#endif // RMEM_PLATFORM_XBOXONE