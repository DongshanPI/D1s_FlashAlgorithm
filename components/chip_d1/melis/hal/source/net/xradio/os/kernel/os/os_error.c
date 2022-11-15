#include <kernel/os/os_errno.h>
// int rt_get_errno(void);
// void rt_set_errno(int error);

int XR_OS_GetErrno(void)
{
	// return (int)rt_get_errno();
	return 0;
}

void XR_OS_SetErrno(int err)
{
	// rt_set_errno((int)err);
}


