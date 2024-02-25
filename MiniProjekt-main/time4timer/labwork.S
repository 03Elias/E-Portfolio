  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #

.global hexasc
.global delay
.global time2string
hexasc: 
	add $t7, $a0, $0
	andi $t7, $t7, 0x0000000F
	slti $t6, $t7, 10
	beq  $t6, $0, char
	nop
	addi $v0, $t7, 0x00000030
	
	jr $ra  
	nop
char:	
	addi $t7, $t7, -9
	addi $v0, $t7, 0x00000040

	jr $ra  
	nop

########################################################################################## Assignment 4

delay:    

	addiu $t5, $0, 4370    #här ändras enkelt konstanten 
			      #OBS - Detta var alltså 4711 i C - men det tog oerhört lång tid, så nu är den 1.

while:
	beqz $a0, endloop
	nop
		addi $a0, $a0, -1
				
	addi $t1, $0, 0		# t1 = i = 0
for:	
	beq $t1, $t5, while	# om i== konstanten, hoppar till while
	nop
	addi $t1, $t1, 1  # i++
	j for			# fortsätt for-loop.
	nop

endloop: 
jr $ra
nop
  
time2string:
	PUSH $ra 	#push return address
	PUSH $s0	#Denna ska vara oförändrad när vi lämnar funktionen (enligt konvention)
	PUSH $s1
	
	add     $s1,$0,$a1 
	add     $s0,$0,$a0        # temporär - adressen där stringen är 
				  #(vi ger s0 värdet då vi ska använda a0 som input på vår hexasc-funk.

	sra  $a0, $s1, 12 	  #Flyttar så att vänstra värdet i minuter står längst till höger (här 0x5) och sparar i a0
	jal hexasc		  # a0 är nu en byte med vänstra siffran i minuter, så vi 'kallar' på hexasc
	nop
	sb $v0, 0($s0)		  # v0 är det som returnerades, vi sparar en byte av den (ascii) till adressen. 

	# Vi gör samma sak som ovan, fast här högra minut-siffran. Alltså 0X:00
	sra  $a0, $s1, 8 	  #Flyttar så att högra värdet i minuter står längst till höger
	jal hexasc
	nop
	sb $v0, 1($s0)   	  #ökar bara med 1 it eftersom varje tal är 1 byte och varje ord har 32 bits.


	#Här ska vi fixa colon. - - - - - - - - - - - Fixat	
	addi $t6, $0, 0x3A 
	sb $t6, 2($s0)

	# Samma sak, men här Alltså 00:X0
	sra  $a0, $s1, 4 	  #Flyttar byte längst till höger.
	jal hexasc
	nop
	sb $v0, 3($s0)	

	# Samma sak TYP, men här 00:0X (vi behöver inte flytta byte, men annars samma.
	add $a0, $s1, $0 #LSB sekunder
	jal hexasc
	nop
	sb $v0, 4($s0)

next:
	#Vi ska ha null på nästa byte.
	addi $t7, $0, 0x00
	sb $t7, 6($s0)
	
	#Vi poppar gamla retur-adressen och hoppar till den!
	POP $s1
	POP $s0 
	POP $ra
	jr $ra
	nop


