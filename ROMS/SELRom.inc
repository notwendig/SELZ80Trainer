SELNOTUSED equ 4000h
SELRAMSTART equ 1000h
SELRAMEND equ 0FFFFh
IC8279_Data equ 0
IC8279_Ctrl equ 1
IC8279_Data_mirrow equ 2
IC8279_Ctrl_mirrow equ 3
PIO_8255_PortA equ 4
PIO_8255_PortB equ 5
PIO_8255_PortC equ 6
PIO_8255_CntrlStatus equ 7
FIFOMSK equ 7			; Maske	für Zeichenzahl
LSD equ 0Fh		; Last signifikant digit
KBMASK equ 1Fh		; Maske	auf Maximalcode
WRDIS equ 80h		; Schreibkommando zum Display
CLFIFO equ 0C2h
LS73_90_P8 equ 8
LS73_90_P9 equ 9
LS73_90_PA equ 0Ah
LS73_90_PB equ 0Bh
BinIO_PC equ 0Ch
BinIO_PD equ 0Dh
BinIO_PE equ 0Eh
BinIO_PF equ 0Fh
UserDisplay_1 equ 10h		; MSD
UserDisplay_2 equ 11h
UserDisplay_3 equ 12h
UserDisplay_4 equ 13h
UserDisplay_5 equ 14h
UserDisplay_6 equ 15h		; LSD
BlankPort_16 equ 16h
BlankPort_16mirrow equ 17h
BUSDecoder_1c equ 1Ch
BUSDecoder_1D equ 1Dh
BUSDecoder_1E equ 1Eh
BUSDecoder_1F equ 1Fh
seg7E equ 1
seg7F equ 10b
seg7G equ 100b
seg7P equ 1000b
seg7A equ 10000b
seg7B equ 100000b
seg7C equ 1000000b
seg7D equ 10000000b
SIO8251_Dataport equ 18h
SIO8251_CntrPort equ 19h
KeyCode_F equ 0
KeyCode_E equ 1
KeyCode_D equ 2
KeyCode_C equ 3
KeyCode_MM equ 4
KeyCode_RN equ 5
KeyCode_SS equ 6
KeyCode_?? equ 7
KeyCode_B equ 8
KeyCode_A equ 9
KeyCode_9 equ 0Ah
KeyCode_8 equ 0Bh
KeyCode_TR equ 0Ch
KeyCode_TW equ 0Dh
KeyCode_BP equ 0Eh
KeyCode_EX equ 0Fh
KeyCode_7 equ 10h
KeyCode_6 equ 11h
KeyCode_5 equ 12h
KeyCode_4 equ 13h
KeyCode_PLUS equ 14h
KeyCode_MINUS equ 15h
KeyCode_IN equ 16h
KeyCode_RL equ 17h
KeyCode_3 equ 18h
KeyCode_2 equ 19h
KeyCode_1 equ 1Ah
KeyCode_0 equ 1Bh
KeyCode_SD equ 1Ch
KeyCode_SA equ 1Dh
KeyCode_EN equ 1Eh
KeyCode_RG equ 1Fh
TxRDY equ 0			; Transmitter Ready
RxRDY equ 1			; Receiver Ready
TxEMPTY equ 2			; Tranmitter empty
PE equ 3			; Parity Error
OE equ 4			; Overun Error
FE equ 5			; Framing Error
BRK equ 6			; Sync/Break detect
DSRD equ 7			; Data set ready
TxEN equ 0			; Transmit enabled
DTR equ 1			; Data Terminal	Ready
RxEN equ 2			; Receiver Enabled
SBRK equ 3			; Send Break
ER equ 4			; Error	Reset
RTS equ 5			; Request to send
IR equ 6			; Internal reset
EH equ 7			; Enter	HUNT Mode
