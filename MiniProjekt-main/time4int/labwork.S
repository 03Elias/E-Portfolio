  # labwork.S
  # Written 2015-2017 by F Lundevall	

#enable_interrupt från labb 3.
.global enable_interrupt

enable_interrupt:
	ei
	jr $ra	
	nop

