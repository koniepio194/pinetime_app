/* auto-generated by gen_syscalls.py, don't edit */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <syscalls/can.h>

extern int z_vrfy_can_send(struct device * dev, const struct zcan_frame * msg, s32_t timeout, can_tx_callback_t callback_isr, void * callback_arg);
uintptr_t z_mrsh_can_send(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2,
		uintptr_t arg3, uintptr_t arg4, uintptr_t arg5, void *ssf)
{
	_current_cpu->syscall_frame = ssf;
	(void) arg5;	/* unused */
	int ret = z_vrfy_can_send(*(struct device **)&arg0, *(const struct zcan_frame **)&arg1, *(s32_t*)&arg2, *(can_tx_callback_t*)&arg3, *(void **)&arg4)
;
	return (uintptr_t) ret;
}

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
