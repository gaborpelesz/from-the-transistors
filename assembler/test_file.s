and r1, r2, r3, LSL #5
eor r1, r6, r2
sub r2, r1, r3
rsb r5, r1, r2
label:
    add r1, r2, r3, LSL #24
adc r5, r2, r3
sbc r1, r6, r2, LSL #2
rsc r5, r2, r3
tst r5, r3
teq r5, r2
cmp r5, r2
cmn r5, r2
ORR r5, r2, r3
mov r5, r2
bic r5, r2, r3
mvn r5, r2
b label
ldr r1, [r2]
ldr r1, [r2]!
str r1, [r2, #+10]
str r1, [r2, #-10]!
str r1, [r2], #-10
ldr r1, [r2, #13]
ldr r1, [r2, r3, LSL #30]!
ldr r1, [r2], r3, LSL #28