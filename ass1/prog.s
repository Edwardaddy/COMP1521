# COMP1521 19t2 ... Game of Life on a NxN grid
#
# Written by <<YOU>>, June 2019

## Requires (from `boardX.s'):
# - N (word): board dimensions
# - board (byte[][]): initial board state
# - newBoard (byte[][]): next board state

## Provides:
	.globl	main
	#.globl	decideCell
	#.globl	neighbours
	#.globl	copyBackAndShow

	.data
msg1:	.asciiz "# Iterations: "
msg2:	.asciiz	"=== After iteration "
msg3:	.asciiz	"===\n"
dot:	.asciiz "."
hash:	.asciiz	"#"
nl:		.asciiz	"\n"


########################################################################
# .TEXT <main>
	.text
main:

# Frame:	$fp, $ra, $s0, $s1, $s2, $s3, $s4, $s5
# Uses:		$a0, $a1, $v0, $s0, $s1, $s2, $s3, $s4, $s5, $t0, $t1
# Clobbers:	$a0, $a1

# Locals:	
#		- 'maxiters' in $s0
#		- 'n' in $s1
#		- 'i' in $s2
#		- 'nn' in $s3
#		- 'j' in $s4
#		- 'n' in $s5

# Structure:
#	main
#	-> [prologue]
#	-> main_init
#	-> [epilogue]

#Code:

	sw		$fp, -4($sp)		# push $fp onto stack
	la		$fp, -4($sp)		# set up $fp for this function
	sw		$ra, -4($fp)		# save return address
	sw		$s0, -8($fp)		# save $s0 to use as maxiters
	sw		$s1, -12($fp)		# save $s1 to use as n
	sw		$s2, -16($fp)		# save $s2 to use as i
	sw 		$s3, -20($fp)		# save $s3 to use as nn
	sw		$s4, -24($fp)		# save $s4 to use as j
	sw		$s4, -28($fp)		# save $s5 to use as N
	addi	$sp, $sp, -32		# reset $sp to last pushed item
	
main_init:
	# printf ("# Iterations: ");	
	la 		$a0, msg1		
	li		$v0, 4		# $v0 = 4 
	syscall

	# scanf ("%d", &maxiters);
	li		$v0, 5		# $v0 =5 
	syscall

	move 	$s0, $v0	# $s0 = v0 = maxiters
	
	li		$s4, 0		# $s4 = 0 = j
	lw		$s5, N		# 
	
	
main_init_n:
	li		$s1, 1		# $s1 = 1 = n 

main_cond_n:
	# for (int n = 1; n <= maxiters; n++)
	bgt		$s1, $s0, main_n_false	# if n($1) > maxiters(v0) then goto main_n_false

main_init_i:
	li		$s2, 0		# $s2 = 0 = i

main_cond_i:
	# for (int i = 0; i < N; i++)
	bge		$s2, $s5, main_i_false	# if i($s2) >= N($s5) then goto main_i_false
	
main_init_j:
	li		$s4, 0		# $s4 = 0 = j
main_cond_j:
	# for (int j = 0; j < N; j++)
	bge		$s4, $s5, main_j_false 	# if j($s4) >= N($5) then goto main_j_false

main_j_step:
	# int nn = neighbours (i, j)
	move 	$a0, $s2				# $a0 = $s2 = i
	move 	$a1, $s4				# $a1 = $s4 = j
	jal		neighbours				# jump to neighbours and save position to $ra

	move 	$s3, $v0				# $s3 = nn = v0

	# newboard[i][j] = decideCell (board[i][j], nn)
	mul 	$t0, $s2, $s5			# i * N
	add		$t0, $t0, $s4			# $t0 = $t0 + $s4   i * N + j
	la		$t1, board				# load the address of board in $t1
	add		$t1, $t0, $t1			# $t1 = $t0 + $t1	address of board[i][j]
	lb		$a0, ($t1)				# $a0 = board[i][j]
	move 	$a1, $s3				# $a0 = $s3
	jal		decideCell				# jump to decideCell and save position to $ra
	
	move    $t6,$v0
	mul 	$t0, $s2, $s5			# i * N
	add		$t0, $t0, $s4			# $t0 = $t0 + $s4   i * N + j
	la		$t1, newBoard			# address of newboard
	add		$t1, $t1, $t0			# $t1 = $t1 + $t0 address of newboard[i][j]
	sb		$t6, ($t1)				# 

	addi	$s4, $s4, 1				# j++
	j		main_cond_j				# jump to main_cond_j

main_j_false:
	
main_i_step:
	

	addi	$s2, $s2, 1				# i++
	j main_cond_i					# jump to main_cond_i

main_i_false:
	
main_n_step:
# printf ("=== After iteration %d ===\n", n);
	la		$a0, msg2				
	li		$v0, 4		
	syscall
	move 	$a0, $s1				# $a0 = n
	li		$v0, 1
	syscall
	la		$a0, msg3
	li		$v0, 4
	syscall

	jal		copyBackAndShow			# jump to copyBackAndShow and save position to $ra
	addi	$s1, $s1, 1				# n++
	j		main_cond_n				# jump to main_cond_n
	
main_n_false:
	
main__post:
	# clean up stack frame
	lw 	$s5, -28($fp)				# restore $s5 value
	lw 	$s4, -24($fp)				# restore $s4 value
	lw 	$s3, -20($fp)				# restore $s3 value
	lw 	$s2, -16($fp)				# restore $s2 value
	lw 	$s1, -12($fp)				# restore $s1 value
	lw 	$s0, -8($fp)				# restore $s0 value
	lw 	$ra, -4($fp)				# restore $ra for return
	la 	$sp, 4($fp)					# restore $sp (remove stack frame)
	lw	$fp, ($fp)					# restore #fp (remove stack frame)
	jr	$ra 						# return
	# Put your other functions here

########################################################################

neighbours:

# Frame:	$fp, $ra, $s0, $s1, $s2, $t0, $t1
# Uses:		$a0, $a1, $v0, $s0, $s1, $s2, $t0, $t1
# Clobbers:	$a0, $a1

# Locals:	
#		- '' in $s0
#		- '' in $s1
#		- '' in $s2

# Structure:
#	main
#	-> [prologue]
#	-> ...
#	-> [epilogue]

# Code:

	# set up stack frame
    sw $fp, -4($sp)     # push $fp onto stack
    la $fp, -4($sp)     # set up $fp for this function
    sw $ra, -4($fp)     # save return address
    sw $s0, -8($fp)     # save $s0 to use as nn
    sw $s1, -12($fp)    # save $s1 to use as x
    sw $s2, -16($fp)    # save $s2 to use as y
    addi $sp, $sp, -20	# reset $sp to last pushed item

nei_init:
	li		$s0, 0		# $s0 = 0 = nn
	
nei_x_init:
	# for (int x = -1; x <= 1; x++)
	li		$s1, -1					# $s1 = -1 = x

nei_x_cond:
	bgt		$s1, 1, nei_x_false	# if $s1 > $t1 then nei_x_false

nei_y_init:
	li		$s2, -1		# $s2 = -1 = y
	
nei_y_cond:
	# int y = -1; y <= 1; y++
	bgt		$s2, 1, nei_y_false	# if $s2 > $t0 then nei_y_false
	# if (i + x < 0 ) continue;
	add		$t2, $a0, $s1			# $t2 = $a0 + $s1  i + x
	blt		$t2, 0, nei_y_step		# if $t2 < $0 then nei_y_step
	# i + x > N - 1 continue
	lw		$t4, N					# 
	addi	$t4, $t4, -1			# $t4 = $t4 + -1 = N-1
	bgt		$t2, $t4, nei_y_step	# if $t2 > $t4 then nei_y_step
	# if j + y < 0 continue
	add		$t5, $a1, $s2			# $t5 = $a0 + $s2 = j + y
	blt		$t5, 0, nei_y_step	# if $t5 < $t3 then nei_y_step
	# if j + y > N - 1 continue
	bgt		$t5, $t4, nei_y_step	# if $t5 > $t4 then nei_y_step
	# if (x == 0 && y == 0) continue
	# if x != 0 || y != 0, go to nei_if_init
	bne		$s1, 0, nei_if_init			# if x != 0 then nei_if_init
	bne		$s2, 0, nei_if_init			# if $s2 != 0 then nei_if_init
	
	j		nei_y_step				# jump to nei_y_step
	
nei_if_init:
	# if (board[i + x][j + y] == 1) nn++
	lw		$t4, N				#$t4 = N
	mul		$t4, $t4, $t2		# N * (i + x)
	add		$t4, $t4, $t5		# $t4 = $t4 + $t5 = N * (i + x) + j + y
	la		$t6, board
	add		$t6, $t4, $t6		# $t6 = $t4 + $t6 = address of board[i + x][j + y]
	lb		$t6, ($t6)			# 
	
nei_if_cond:
	bne		$t6, 1, nei_y_step	# if $t6 != $t0(1) then nei_y_step
	
	
nei_if_step:
	addi		$s0, $s0, 1		# $s0 = $s0 + 1  nn++

nei_y_step:
	addi	$s2, $s2, 1			# $s2 = $s2 + 1 y++
	j		nei_y_cond				# jump to nei_y_cond
	
nei_y_false:

nei_x_step:
	addi	$s1, $s1, 1			# x++
	j		nei_x_cond				# jump to nei_x_cond
	

nei_x_false:

nei_post:
	move $v0, $s0 # return nn
    # clean up stack frame
    lw $s2, -16($fp)    # restore $s2 value
    lw $s1, -12($fp)    # restore $s1 value
    lw $s0, -8($fp)     # restore $s0 value
    lw $ra, -4($fp)     # restore $ra for return
    la $sp, 4($fp)	    # restore $sp (remove stack frame)
    lw $fp, ($fp)       # restore $fp (remove stack frame)
    jr $ra              # return
########################################################################

decideCell:

# Frame:	$fp, $ra,
# Uses:		$a0, $a1, $t0
# Clobbers:	$a0, $a1

# Locals:	

# Structure:
#	main
#	-> [prologue]
#	-> ...
#	-> [epilogue]

# Code:


	sw		$fp, -4($sp)		# push $fp onto stack
	la		$fp, -4($sp)		# set up $fp for this function
	sw		$ra, -4($fp)		# save return address
	addi	$sp, $sp, -8		# reset $sp to last pushed item

dec_init:
	# a0 = old, nn = a1, t0 = ret
	li		$v0, 0		# $v0 = 0
	
dec_if_cond:
	# if (old == 1)
	li		$t0, 1		# $t0 = 1
	bne		$a0, $t0, dec_else_if	# if $a0 != $t0 then dec_else_if
	
dec_if_step:
	# only if nn = 2 || nn = 3, ret = 1
	li		$t0, 2		# $t0 = 2
	beq		$a1, $t0, ret_1	# if $a1 == $t0 then ret_1
	li		$t0, 3		# $t0 = 3
	beq		$a1, $t0, ret_1	# if $a1 == $t0 then ret_1
	# else ret = 0
	j		dec_post				# jump to dec_post

dec_else_if:
	# else if (nn == 3) ret = 1;
	li		$t0, 3		# $t0 = 3
	bne		$a1, $t0, dec_post	# if $a1 != $t0 then dec_post
	j		ret_1				# jump to ret_1
		
ret_1:
	li		$v0, 1		# $v0 = 1

dec_post:
	# clean up stack frame
	lw 		$ra, -4($fp)		# redsore $ra for return
	la		$sp, 4($fp)			# restore $sp (remove stack frame)
	lw		$fp, ($fp)			# restore $fp (remove stack frame)
	jr		$ra					# return

########################################################################


copyBackAndShow:

# Frame:	$fp, $ra, $s0, $s1, $s2, $s3, $s4
# Uses:		$a0, $a1, $v0, $s0, $s1, $s2, $s3, $s4, $t0, $t1
# Clobbers:	$a0, $a1

# Locals:	
#		- '' in $s0
#		- '' in $s1
#		- '' in $s2
#		- '' in $s3
#		- '' in $s4

# Structure:
#	main
#	-> [prologue]
#	-> ...
#	-> [epilogue]

# Code:
	# Set up stack frame
    sw $fp, -4($sp)	    # push $fp onto stack
    la $fp, -4($sp)	    # set up $fp for this function
    sw $ra, -4($fp)	    # save return address
    sw $s0, -8($fp)	    # save $s0 to use as i
    sw $s1, -12($fp)    # save $s1 to use as j
    sw $s2, -16($fp)    # save $s2 to use as N
    addi $sp, $sp, -20	# reset $sp to last pushed item

copy_init:
	lw		$s2, N		# $s0 = N

copy_i_init:
	li		$s0, 0		# $s0 = 0 = i
	
copy_i_cond:
	bge		$s0, $s2, copy_i_false	# if $s0 > $s2 then copy_i_false

copy_j_init:
	li		$s1, 0		# $s1 = 0 

copy_j_cond:
	bge		$s1, $s2, copy_j_false	# if $s1 > $s2 then copy_j_false

copy_j_step:
    # newBoard[i][j]
    mul 	$t0, $s0, $s2 			# i * N
    add  	$t0, $t0, $s1 			# $t0 = i * N + j
    la 		$t1, newBoard 			# load the address of newboard
    add 	$t1, $t0, $t1 			# address of newBoard[i][j]
    #lb 		$t1, ($t1) 				# newBoard[i][j]

    # board[i][j]
    la 		$t2, board 				# load the address of board
    add 	$t2, $t0, $t2 			# address of board[i][j]
    lb 		$t1, ($t1) 				# board[i][j] = newboard[i][j]
    sb 		$t1, ($t2) 				# $t2 = board[i][j]
	lb $t2,($t2)

dot_cond:
	# if (board[i][j] == 0) putchar ('.');
	li $t8, 0
	beq	$t2, $t8, print_dot
	# putchar ('#')
	la $a0, hash 					
    li $v0, 4 						
    syscall
	j		dot_end				# jump to dot_end

print_dot:
	# putchar ('.')
	la $a0, dot 
    li $v0, 4
    syscall
	

dot_end:
	addi	$s1, $s1, 1			# $s1 = s1 + 1 = j++
	j		copy_j_cond				# jump to copy_j_cond
	
	
copy_j_false:
	
copy_i_step:
	# putchar ('\n')
	la 		$a0, nl
    li 		$v0, 4 
    syscall 
	addi	$s0, $s0, 1			# $s0 = $s0 + 1 = i++
	
	j		copy_i_cond				# jump to copy_i_cond
	
copy_i_false:

copy_post:
	# clean up stack frame
    lw $s2, -16($fp)    # restore $s2 value
    lw $s1, -12($fp)    # restore $s1 value
    lw $s0, -8($fp)     # restore $s0 value
    lw $ra, -4($fp)     # restore $ra for return
    la $sp, 4($fp)	    # restore $sp (remove stack frame)
    lw $fp, ($fp)       # restore $fp (remove stack frame)
    jr $ra              # return
