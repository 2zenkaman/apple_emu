	.text
	.globl	carry
carry:
	adcb 	%sil, %dil
	setc	%al
	ret