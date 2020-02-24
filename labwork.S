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

.data
.align 2

.text
.global enable_interrupt
.global disable_interrupt

 #översätter binärt tal på fyra bitar till ASCII
hexasc:
	andi	$t0, $a0, 0xF	# hämtar endast de relevanta bitarna från argumentet
	slti	$t1, $t0, 0xA	# kontrollerar om skickade siffran är mindre än 10
	beq	$t1, $0, teen	# om det inte är mindre än 10 gå till teen
	nop
	addi	$v0 ,$t0, 0x30	# lägg till position för ASCII-tecken '0'
	j	return
	nop
teen:
	addi	$v0 ,$t0, 0x37	# lägg till ASCII-tecken 10 positioner under 'A'
return:
	jr	$ra
	nop

# fördröjer med det angivna talet ms
delay:
	li	$t0, 4400		# konstant faktor för fördröjning
while:
	slti	$t1, $a0, 1
	bne	$t1, $0, stopw	# körs till t1 är mindre än 1
	nop
	addi	$a0, $a0, -1	# minksar a0
	li	$t2, 0		# initierar variabel för inre loopen
for:
	beq	$t2, $t0, stopf	# körs till variabel når konstant
	nop
	addi	$t2, $t2, 1	# ökar variabel
	j	for
	nop
stopf:
	j	while
	nop
stopw:
	jr	$ra
	nop

time2string:
	andi	$t1, $a1, 0xFF
	beq		$t1, $0, DING
	nop
	PUSH	$ra		# lägger returadress på stacken
	PUSH	$s0		# lägger möjligt värde i s0 på stacken
	move	$s0, $a0	# sparar address som ska skrivas till
	li	$t0, 0x3A	# skapar ASCII semi-kolon
	sb	$t0, 2($s0)	# skickar semi-kolon till addressen
	sb	$0, 5($s0)	# skickar null till addressen
	srl	$a0, $a1, 12	# flyttar första siffran till a0
	jal	hexasc		# översätter första siffran till ASCII
	nop
	sb	$v0, 0($s0)	# skickar tecken för första siffran till addressen
	srl	$a0, $a1, 8	# nästa siffra
	jal	hexasc		# översätter andra siffran till ASCII
	nop
	sb	$v0, 1($s0)	# skickar andra siffran till addressen
	srl	$a0, $a1, 4	# nästa siffra
	jal	hexasc		# översätt tredje siffra till ASCII
	nop
	sb	$v0, 3($s0)	# skicka tredje siffra till addressen
	move	$a0, $a1	# sista siffran
	jal	hexasc		#översätt sista siffran till ASCII
	nop
	sb	$v0, 4($s0)	# skicka sista siffran till addressen
	POP	$s0		# återställ s0
	POP	$ra		# återställ returaddress
	j	end
	nop
DING:
	li	$t0, 0x44	#D
	sb	$t0, 0($a0)
	li	$t0, 0x49	#I
	sb	$t0, 1($a0)
	li	$t0, 0x4E	#N
	sb	$t0, 2($a0)
	li	$t0, 0x47	#G
	sb	$t0, 3($a0)
	sb	$0, 4($a0)	#null
end:
	jr	$ra
	nop

# slår på interrupts globalt
enable_interrupt:
  ei
  jr $ra
  nop
  
disable_interrupt:
  di
  jr $ra
  nop
