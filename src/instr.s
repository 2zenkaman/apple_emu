.text
.globl asl
asl:
    salb    $1, (%rdi)
    setc    C(%rip)
    setz    Z(%rip)
    sets    N(%rip)
    ret

.globl adc
adc:
    movb    A(%rip), %al
    movb    C(%rip), %bl
    sarb    $1, %bl
    adcb    (%rdi), %al
    setc    C(%rip)
    seto    V(%rip)
    setz    Z(%rip)
    sets    N(%rip)
    movb    %al, A(%rip)
    ret

.globl dec
dec:
    decb    (%rdi)
    setz    Z(%rip)
    sets    N(%rip)
    ret

.globl inc
inc:
    incb    (%rdi)
    setz    Z(%rip)
    sets    N(%rip)
    ret

.globl loa
loa:
    movb    (%rsi), %al
    movb    %al, (%rdi)
    test    %al, %al
    setz    Z(%rip)
    sets    N(%rip)
    ret

.globl lsr
lsr:
    shrb    (%rdi)
    setc    C(%rip)
    setz    Z(%rip)
    sets    N(%rip)
    ret