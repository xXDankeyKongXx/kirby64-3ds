.arm

.global _InvalidateAndFlushCaches

_ClearCacheKernel:
	cpsid aif
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 0	@ Clean entire data cache
	mcr p15, 0, r0, c7, c10, 5	@ Data Memory Barrier
	mcr p15, 0, r0, c7, c5, 0	@ Invalidate entire instruction cache / Flush BTB
	mcr p15, 0, r0, c7, c10, 4	@ Data Sync Barrier
	bx lr

@ Kirby 64 3DS: use Luma3DS's cache SVCs (also implemented by Azahar/Citra)
@ instead of running kernel code through svcCustomBackdoor (0x80), which
@ emulators don't support. Same effect: clean D-cache, invalidate I-cache.
_InvalidateAndFlushCaches:
	push {r4, lr}
	svc 0x92					@ svcFlushEntireDataCache
	svc 0x94					@ svcInvalidateEntireInstructionCache
	pop {r4, pc}