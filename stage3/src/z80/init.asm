	CPU Z80UNDOC
	listing purecode

	; Initialise interrupts.
	di
	im	1

	; Wipe RAM.
	xor	a
	ld	bc,2000h-.end-1
	ld	de,.end+1
	ld	hl,.end
	ld	sp,hl
	ld	(hl),a
	ldir

	; A, B, C, and E are 00 now.

	; Wipe special registers.
	pop	ix
	pop	iy
	ld	i,a
	ld	r,a

	; Clear D, F, H, and L.
	ld	d,a
	pop	hl
	pop	af

	; Wipe shadow registers.
	ex	af,af'
	exx
	pop	bc
	pop	de
	pop	hl

	; Wipe shadow A and the flags.
	pop	af

	; Wipe the stack pointer.
	ld	sp,hl

	; Jump back to address 0000 and stay there.
	ld	(hl),0E9h	; 0E9h is the machine code for 'jp (hl)'.
	jp	(hl)		

.end:
