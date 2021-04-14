.THUMB						@ turn on thumb mode
.ALIGN  2					@ align code correctly in memory 
.GLOBL  someFunction		@ name of function goes here


@ ==================================
@ ==================================

.THUMB_FUNC			@ we are about to declare a thumb function
someFunction:		@ function start

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway
	
	@ YOUR ASSEMBLY CODE GOES HERE
	@ r0-r3 will automatically contain any parameters sent when calling the function (any more are pushed to stack for you).
	
	ldr r4, [r0]		@location
	ldr r5, [r1]		@place
	
	add r6, r5, r4		@r6 = r5 + r4
	
	str r6, [r2]		@r2 = r6 value finalplace

pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr in thumb mode(?) so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3

@ ==================================
@ ==================================