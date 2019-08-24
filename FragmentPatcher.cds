// Fragment ELF Detection and Code Loader

address $8007a228
print "_255_00._01."

lui t0, $8008
lui t1, $8007

// _255.27 < Main ELF
lw t2, $a228(t0) 
// _00.ELF < Online Elf
lw t3, $a22c(t0)  
//  _01.ELF < Offline ELF
lw t4, $a230(t0) 
// currently loaded ELF
setreg t5,$8007537C
// Load the currently loaded elf text
lw t6 zero(t5) 

// if t6 isn't currently anything terminate
beq t6 zero, :__EOF 
nop

//If the main elf is loaded jump to the code loader
beq t2,t6,:__MainElfCodeLoader 
nop

// If the online elf is loaded jump to code loader
beq t3,t6 :__OnlineElfCodeLoader
nop

// If the offline elf is loaded jump to code loader
beq t4,t6 :__OfflineElfCodeLoader 
nop

beq zero, zero :__EOF
nop
// ---------------------------------------------------------------------------
__MainElfCodeLoader:
ori t1,t1, $a340

__MainElfCodeLoaderAddress:
// Gets the address line to our code from the Code stack
lw t6,$000(t1)
// Gets the data line to our code from the Code stack
lw t7,0004(t1) 

// Jump back to EOF if t6 is zero (t6 would contain the next adddress)
beq t6,zero :__EOF
nop
// Store the data line to the actual address
sw t7, zero(t6)
beq zero,zero :__MainElfCodeLoaderAddress
// We do not want to nop here for the delay slot
// Add 8 to t5 so it will grab the next address data pair
addiu t1,t1,$0008
nop


// ---------------------------------------------------------------------------
__OnlineElfCodeLoader:
ori t1,t1, $a400

__OnlineElfCodeLoaderAddress:
// Gets the address line to our code from the Code stack
lw t6,$000(t1)
// Gets the data line to our code from the Code stack
lw t7,0004(t1) 

// Jump back to EOF if t6 is zero (t6 would contain the next adddress)
beq t6,zero :__EOF
nop
// Store the data line to the actual address
sw t7, zero(t6)
beq zero,zero :__OnlineElfCodeLoaderAddress
// We do not want to nop here for the delay slot
// Add 8 to t5 so it will grab the next address data pair
addiu t1,t1,$0008
nop

// ---------------------------------------------------------------------------
__OfflineElfCodeLoader:
ori t1,t1, $a700

__OfflineElfCodeLoaderAddress:
// Gets the address line to our code from the Code stack
lw t6,$000(t1)
// Gets the data line to our code from the Code stack
lw t7,0004(t1) 

// Jump back to EOF if t6 is zero (t6 would contain the next adddress)
beq t6,zero :__EOF
nop
// Store the data line to the actual address
sw t7, zero(t6)
beq zero,zero :__OfflineElfCodeLoaderAddress
// We do not want to nop here for the delay slot
// Add 8 to t5 so it will grab the next address data pair
addiu t1,t1,$0008
__EOF:
jr ra

// ----------------------------- Begin of Main ELF Code stack
// Controller specific data
address $8007a340 
// hexcode $0012cd14
// hexcode $0803f414
// hexcode $000fd054
// hexcode $0804b347
// hexcode $0012cd7c
// hexcode $0803f417
hexcode $00113cf0 // Skip FMV
hexcode $24020001


// ----------------------------- Begin of the Online ELF Code stack
// Controller specific data
address $8007a400 
// hexcode $001317c4 
// hexcode $0803f414
// hexcode $000fd054
// hexcode $0804c5f3
// hexcode $0013182c
// hexcode $0803f417

// DNAS Bypass
hexcode $002a87a8 
hexcode $24020001

// DNS Change to Alk's Server
hexcode $005E03A0 
hexcode $6F642E61
hexcode $005E03A4
hexcode $63616874
hexcode $005E03A8
hexcode $7372656B
hexcode $005E03AC
hexcode $67726F2E
hexcode $005E03B0
hexcode $00000000
hexcode $005E03B4
hexcode $00000000

// Disable Item loss on characters if the game isn't saved.
hexcode $00113cf0 
hexcode $24020001
hexcode $00177528
hexcode $00001021
hexcode $0028733c
hexcode $00003021
hexcode $002CB9F0
hexcode $00003021

// ----------------------------- Begin of Offline ELF Code stack
address $8007a700 
// DNAS Bypass
hexcode $002a87a8 
hexcode $24020001
// hexcode $00128584
// hexcode $0803f414
// hexcode $000fd054
// hexcode $0804a163
// hexcode $001285ec
// hexcode $0803f417

// Other Code that isn't specific to anything. This will always be loaded
// address $8007a800 