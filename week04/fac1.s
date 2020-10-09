# $s0 = i
# $s1 = j

	lw $t0, n
init_loop:
	li $s0,0
condition:
	bge $s0, $t0 loop_finish

	li $t1, 2

	la $t2, $a1 # address of a1
	li $t3, 4 #t3 = 4
	mult $t3, $t3, $s0
	add $t2, $t2, $t3 #t2 = address of a1 + ith offset
	lw $t2, ($t2)

	div		$t2, $t1			# $t2 / $t1
	# mflo	$t5					# $t5 = floor($t2 / $t1) 
	mfhi	$t1					# $t1 = $t2 mod $t1 
	



	addi $s0, 1
	jal condition
loop_finish:


# $s0 = i
# $s1 = j

for_init:
	li	$s0, 0
	li	$s1, 0
	li	$t1, 2
for_cond:
	bge		$s0, $a1, for_false	# if $s0 >= $a1 then for_false
for_step:
# src[i]%2 = 0
	move $t0, $s0
	mul $t0, $t0, 4
	add $t0, $a0, $t0
	lw	$t0, ($t0) # src[i]
	div	$t0, $t1
	mfhi $t1
	bnez $t1 if_odd

even:
	mul $t2, $s1, 4
	add $t2, $t2, $a2
	sw  $t2, ($t0) #dest[j]
	addi $s1, $s1, 1

if_odd:
	

	addi	$s0, $s0, 1			# $s0 = $s0 + 1
	

for_false:
	move $v0, $s1