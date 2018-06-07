.include "r5900_regs.h"
.set noreorder

.global j_defhandler
.extern dump_memory
.extern jal_cheat
.section .resident, "a" # really gay that i have to compile this specific part at 0x00080000!!

	.global systemHook
	.ent	systemHook
systemHook:
	addiu $sp, $sp,	-240
	sq $ra,		0($sp)
	sq $s0,		16($sp)
	sq $s1,		32($sp)
	sq $s2,		48($sp)
	sq $s3,		64($sp)
	sq $s4,		80($sp)
	sq $s5,		96($sp)
	sq $s6,		112($sp)
	sq $s7,		128($sp)
	sq $a0,		144($sp)
	sq $a1,		160($sp)
	sq $a2,		176($sp)
	sq $v0,		192($sp)
	sq $v1,		208($sp)
	
	
	# --- FOR CHEATING ---
	li $v0,		0x001bfa68
	li $v1,		0x0c0655f4
	sw $v1,		_orig_jal
	lw $v1,		jal_cheats
	#li $v1,		0x00090000
	sw $v1,		0($v0)
	
	# --- FOR CHEATING ---
	
	lq $v1,		208($sp)
	lq $v0,		192($sp)
	lq $a2,		176($sp)
	lq $a1,		160($sp)
	lq $a0,		144($sp)
	lq $s7,		128($sp)
	lq $s6,		112($sp)
	lq $s5,		96($sp)
	lq $s4,		80($sp)
	lq $s3,		64($sp)
	lq $s2,		48($sp)
	lq $s1,		32($sp)
	lq $s0,		16($sp)
	lq $ra,		0($sp)
	addiu $sp, $sp,	240
	j_defhandler:
	nop
	nop
	jr $ra
	nop
	.end systemHook

	.global readPad
	.ent	readPad
readPad:
	addiu $sp, $sp,	-240
	sq $ra,		0($sp)
	sq $s0,		16($sp)
	sq $s1,		32($sp)
	sq $s2,		48($sp)
	sq $s3,		64($sp)
	sq $s4,		80($sp)
	sq $s5,		96($sp)
	sq $s6,		112($sp)
	sq $s7,		128($sp)
	sq $a0,		144($sp)
	sq $a1,		160($sp)
	sq $a2,		176($sp)
	sq $v0,		192($sp)
	sq $v1,		208($sp)
	
	# -- read joker --
	lui		$t7,	0x0044
	lh		$t8,	0xd81c($t7)
	# -- read joker --
	
	# L2 + RIGHT
	addiu	$t7,	$zero,	0xFEDF
	bne		$t7,	$t8,	end
	nop
	# DI
	mfc0	$v1,	$12
	lui		$v0,	0x0001
	and		$v1,	$v1, $v0
	daddu	$a0,	$zero, $zero
	beq		$v1,	$zero, end
	sltu	$a1,	$zero, $v1
	lui		$v0,	0x0001
	nop
	loop:
	di
	sync.p
	mfc0	$v1,	$12
	nop
	and		$v1,	$v1, $v0
	nop
	bne		$v1,	$zero, loop
	nop
	daddu	$a0,	$a1, $zero
	
	
	#jal		dump_memory
	nop
	
	# EI
	mfc0	$v0,	$12
	lui		$v1,	0x0001
	and		$v0,	$v0, $v1
	ei
	
end:
	lq $v1,		208($sp)
	lq $v0,		192($sp)
	lq $a2,		176($sp)
	lq $a1,		160($sp)
	lq $a0,		144($sp)
	lq $s7,		128($sp)
	lq $s6,		112($sp)
	lq $s5,		96($sp)
	lq $s4,		80($sp)
	lq $s3,		64($sp)
	lq $s2,		48($sp)
	lq $s1,		32($sp)
	lq $s0,		16($sp)
_orig_jal:
	nop
	nop
	lq $ra,		0($sp)
	jr $ra
	addiu $sp, $sp, 240
	.end	readPad