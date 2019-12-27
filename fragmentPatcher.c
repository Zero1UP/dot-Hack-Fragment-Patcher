#include <tamtypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <fileio.h>
#include <debug.h>
#include "libpad.h"
#include "malloc.h"
#include "libcdvd.h"
#include "r5900_regs.h"
#include <iopheap.h>
#include <sbv_patches.h>
#include "menu.h"


#define _RESIDENT_	__attribute__((section(".resident")))
#define DEBUG
#define ERROR_HAX0R				-4
#define ERROR_CDVDFSV_INIT		-5
#define ERROR_SYSTEMCNF_PARSE	-6
#define ERROR_SYSTEMCNF_FILEIO	-7
#define ERROR_SYSTEMCNF_MEMORY	-8


//PAD VARIABLES
//check for multiple definitions
#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#define ROM_PADMAN
#endif

#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif
//pad buffer
static char padBuf[256] __attribute__((aligned(64)));
//rumblers
static char actAlign[6];
static int actuators;
//button status
struct padButtonStatus buttons;
	u32 paddata;
	u32 old_pad;
	u32 new_pad;
int port, slot;

	#define MAKE_J(addr)   (u32)(0x08000000 | (0x03FFFFFF & ((u32)addr >> 2)))
	#define MAKE_JAL(addr)   (u32)(0x0C000000 | (0x03FFFFFF & ((u32)addr >> 2)))
	#define KSEG0(x)   ((void*)(((u32)(x)) | 0x80000000))

	static u32 vector;
	static u32 *p_vtbl;
	_RESIDENT_ u32 jal_cheats;

	extern void systemHook(void);
	extern void readPad(void);
	extern int j_defhandler;

	void initalise();
void initalise(void)
{
	int ret;

	SifInitRpc(0);
	// init debug screen
	init_scr();
	// load all modules
	loadModules();
	// init pad
	padInit(0);
	if((ret = padPortOpen(0, 0, padBuf)) == 0) {
		#if defined DEBUG
			scr_printf("padOpenPort failed: %d\n", ret);
		#endif
		SleepThread();
	}

	if(!initializePad(0, 0)) {
		#if defined DEBUG
			scr_printf("pad initalization failed!\n");
		#endif
		SleepThread();
	}
}


//loadModules
/////////////////////////////////////////////////////////////////////
static void loadModules(void)
{
    int ret;
#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
#endif
    if (ret < 0) {
        SleepThread();
    }
	

#ifdef ROM_PADMAN
    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
#else
    ret = SifLoadModule("rom0:XPADMAN", 0, NULL);
#endif
    if (ret < 0) {
        SleepThread();
    }
}

static int waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
        }
        lastState = state;
        state=padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {

    }
    return 0;
}

static int initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    waitPadReady(port, slot);
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    if (modes > 0) {
        for (i = 0; i < modes; i++) {
        }

    }
    if (modes == 0) {
        return 1;
    }

    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        return 1;
    }

    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        return 1;
    }
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
    padInfoPressMode(port, slot);

    waitPadReady(port, slot);
    padEnterPressMode(port, slot);

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);

    if (actuators != 0) {
        actAlign[0] = 0;
        actAlign[1] = 1;
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);

       padSetActAlign(port, slot, actAlign);
    }
    else {
        //printf("Did not find any actuators.\n");
    }
    return 1;
}


static void buttonStatts(int port, int slot)
{
	int ret;
		ret = padRead(port, slot, &buttons);

        if (ret != 0) {
            paddata = 0xffff ^ buttons.btns;

            new_pad = paddata & ~old_pad;
            old_pad = paddata;
		}
}


void checkPadConnected(void)
{
	int ret, i;
	ret=padGetState(0, 0);
	while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
		if(ret==PAD_STATE_DISCONN) {
			#if defined DEBUG
	           scr_printf("	Pad(%d, %d) is disconnected\n", 0, 0);
			#endif
		}
		ret=padGetState(0, 0);
	}
	if(i==1) {
		//scr_printf("	Pad: OK!\n");
	}
}


void pad_wait_button(u32 button)
{
	while(1)
	{
		buttonStatts(0, 0);
		if(new_pad & button) return;
	}
}

u32 getSyscallVTblAddr(void)
{
   const s32 syscall_num[2] = { 0xFE, 0xFF };
   const u32 magic[2] = { 0x1337C0DE, 0xB16B00B5 };
   u32 addr = -1;
   u32 i;

   SetSyscall(syscall_num[0], (void*)magic[0]);
   SetSyscall(syscall_num[1], (void*)magic[1]);
   ee_kmode_enter();

   for (i = 0x80000000; i < 0x80080000; i += 4) {
      if (!memcmp((u32*)i, magic, sizeof(magic))) {
         addr = i - syscall_num[0] * 4;
         break;
      }
   }

   ee_kmode_exit();
   SetSyscall(syscall_num[0], (void*)0);
   SetSyscall(syscall_num[1], (void*)0);

   return addr;
}


int getVTblAddr(void)
{
	//GET SYSCALL VECTOR ADDRESS
	p_vtbl = getSyscallVTblAddr();
	//check for errors
	if(p_vtbl == NULL || p_vtbl == 0){
		scr_printf("	ERROR! Could not get vector address!! Cheat device will not run!\n");
		return -1;
	}
	#if defined DEBUG
		printf("vector table: %0x\n", p_vtbl);
		//scr_printf("	vector table: %0x\n", p_vtbl);
	#endif
	return 0;
}


 int main(int argc, char *argv[]) 
{
	
	
	initalise();
	if(getVTblAddr() !=0){
		scr_printf("ERROR: Unknown\n");
		SleepThread();
	}	
	//scr_printf(GAMEID);
	menu_DisplayMain();
	//Main Loop
	while(1)
	{
		//check to see if the pad is still connected
		checkPadConnected();
		//read pad 0
		buttonStatts(0, 0);

		if(new_pad & PAD_CIRCLE)
		{
			
			scr_clear();
			menu_DisplayAbout();

		}
		
		if(new_pad & PAD_CROSS)
		{
			scr_clear();
			menu_DisplayMain();
		}
		
		if(new_pad & PAD_SQUARE)
		{
			scr_clear();
			menu_PatchInformation();
		}
		if(new_pad & PAD_START)
		{

			ee_kmode_enter();
			*(u32*)0x8007A000 = 0x27BDFFF0;
			*(u32*)0x8007A004 = 0x7FBF0000;
			*(u32*)0x8007A008 = 0x0C01E80A;
			*(u32*)0x8007A00C = 0x00000000;
			*(u32*)0x8007A010 = 0x7BBF0000;
			*(u32*)0x8007A014 = 0x27BD0010;
			*(u32*)0x8007A018 = 0x0340F809;
			*(u32*)0x8007A01C = 0x00000000;
			*(u32*)0x8007A020 = 0x080000C1;
			*(u32*)0x8007A024 = 0x00000000;
			*(u32*)0x8007A028 = 0x27BDFE00;
			*(u32*)0x8007A02C = 0x7FA10000;
			*(u32*)0x8007A030 = 0x7FA20010;
			*(u32*)0x8007A034 = 0x7FA30020;
			*(u32*)0x8007A038 = 0x7FA40030;
			*(u32*)0x8007A03C = 0x7FA50040;
			*(u32*)0x8007A040 = 0x7FA60050;
			*(u32*)0x8007A044 = 0x7FA70060;
			*(u32*)0x8007A048 = 0x7FA80070;
			*(u32*)0x8007A04C = 0x7FA90080;
			*(u32*)0x8007A050 = 0x7FAA0090;
			*(u32*)0x8007A054 = 0x7FAB00A0;
			*(u32*)0x8007A058 = 0x7FAC00B0;
			*(u32*)0x8007A05C = 0x7FAD00C0;
			*(u32*)0x8007A060 = 0x7FAE00D0;
			*(u32*)0x8007A064 = 0x7FAF00E0;
			*(u32*)0x8007A068 = 0x7FB000F0;
			*(u32*)0x8007A06C = 0x7FB10100;
			*(u32*)0x8007A070 = 0x7FB20110;
			*(u32*)0x8007A074 = 0x7FB30120;
			*(u32*)0x8007A078 = 0x7FB40130;
			*(u32*)0x8007A07C = 0x7FB50140;
			*(u32*)0x8007A080 = 0x7FB60150;
			*(u32*)0x8007A084 = 0x7FB70160;
			*(u32*)0x8007A088 = 0x7FB80170;
			*(u32*)0x8007A08C = 0x7FB90180;
			*(u32*)0x8007A090 = 0x7FBA0190;
			*(u32*)0x8007A094 = 0x7FBB01A0;
			*(u32*)0x8007A098 = 0x7FBE01B0;
			*(u32*)0x8007A09C = 0x7FBC01C0;
			*(u32*)0x8007A0A0 = 0x7FBF01D0;
			*(u32*)0x8007A0A4 = 0x0c01e88a;
			*(u32*)0x8007A0A8 = 0x00000000;
			*(u32*)0x8007A0AC = 0x7BA10000;
			*(u32*)0x8007A0B0 = 0x7BA20010;
			*(u32*)0x8007A0B4 = 0x7BA30020;
			*(u32*)0x8007A0B8 = 0x7BA40030;
			*(u32*)0x8007A0BC = 0x7BA50040;
			*(u32*)0x8007A0C0 = 0x7BA60050;
			*(u32*)0x8007A0C4 = 0x7BA70060;
			*(u32*)0x8007A0C8 = 0x7BA80070;
			*(u32*)0x8007A0CC = 0x7BA90080;
			*(u32*)0x8007A0D0 = 0x7BAA0090;
			*(u32*)0x8007A0D4 = 0x7BAB00A0;
			*(u32*)0x8007A0D8 = 0x7BAC00B0;
			*(u32*)0x8007A0DC = 0x7BAD00C0;
			*(u32*)0x8007A0E0 = 0x7BAE00D0;
			*(u32*)0x8007A0E4 = 0x7BAF00E0;
			*(u32*)0x8007A0E8 = 0x7BB000F0;
			*(u32*)0x8007A0EC = 0x7BB10100;
			*(u32*)0x8007A0F0 = 0x7BB20110;
			*(u32*)0x8007A0F4 = 0x7BB30120;
			*(u32*)0x8007A0F8 = 0x7BB40130;
			*(u32*)0x8007A0FC = 0x7BB50140;
			*(u32*)0x8007A100 = 0x7BB60150;
			*(u32*)0x8007A104 = 0x7BB70160;
			*(u32*)0x8007A108 = 0x7BB80170;
			*(u32*)0x8007A10C = 0x7BB90180;
			*(u32*)0x8007A110 = 0x7BBA0190;
			*(u32*)0x8007A114 = 0x7BBB01A0;
			*(u32*)0x8007A118 = 0x7BBE01B0;
			*(u32*)0x8007A11C = 0x7BBC01C0;
			*(u32*)0x8007A120 = 0x7BBF01D0;
			*(u32*)0x8007A124 = 0x03E00008;
			*(u32*)0x8007A128 = 0x27BD0200;
			*(u32*)0x8007A12C = 0x27BDFF80;
			*(u32*)0x8007A130 = 0xAFBF0000;
			*(u32*)0x8007A134 = 0xAFA20004;
			*(u32*)0x8007A138 = 0xAFA30008;
			*(u32*)0x8007A13C = 0xAFA4000C;
			*(u32*)0x8007A140 = 0xAFA50010;
			*(u32*)0x8007A144 = 0xAFA60014;
			*(u32*)0x8007A148 = 0xAFB00018;
			*(u32*)0x8007A14C = 0xAFB1001C;
			*(u32*)0x8007A150 = 0xAFB20020;
			*(u32*)0x8007A154 = 0xAFB30024;
			*(u32*)0x8007A158 = 0xAFB40028;
			*(u32*)0x8007A15C = 0xAFB5002C;
			*(u32*)0x8007A160 = 0xAFB60030;
			*(u32*)0x8007A164 = 0xAFB70034;
			*(u32*)0x8007A168 = 0x3C108007;
			*(u32*)0x8007A16C = 0x3610A1CC;
			*(u32*)0x8007A170 = 0x8E110000;
			*(u32*)0x8007A174 = 0x12200005;
			*(u32*)0x8007A178 = 0x00000000;
			*(u32*)0x8007A17C = 0x8E120004;
			*(u32*)0x8007A180 = 0xAE320000;
			*(u32*)0x8007A184 = 0x1000FFFA;
			*(u32*)0x8007A188 = 0x26100008;
			*(u32*)0x8007A18C = 0x8FBF0000;
			*(u32*)0x8007A190 = 0x8FA20004;
			*(u32*)0x8007A194 = 0x8FA30008;
			*(u32*)0x8007A198 = 0x8FA4000C;
			*(u32*)0x8007A19C = 0x8FA50010;
			*(u32*)0x8007A1A0 = 0x8FA60014;
			*(u32*)0x8007A1A4 = 0x8FB00018;
			*(u32*)0x8007A1A8 = 0x8FB1001C;
			*(u32*)0x8007A1AC = 0x8FB20020;
			*(u32*)0x8007A1B0 = 0x8FB30024;
			*(u32*)0x8007A1B4 = 0x8FB40028;
			*(u32*)0x8007A1B8 = 0x8FB5002C;
			*(u32*)0x8007A1BC = 0x8FB60030;
			*(u32*)0x8007A1C0 = 0x8FB70034;
			*(u32*)0x8007A1C4 = 0x03E00008;
			*(u32*)0x8007A1C8 = 0x27BD0080;

			*(u32*)0x8007A228 = 0x3535325F;
			*(u32*)0x8007A22C = 0x2E30305F;
			*(u32*)0x8007A230 = 0x2E31305F;
			*(u32*)0x8007A234 = 0x3C088008;
			*(u32*)0x8007A238 = 0x3C098007;
			*(u32*)0x8007A23C = 0x8D0AA228;
			*(u32*)0x8007A240 = 0x8D0BA22C;
			*(u32*)0x8007A244 = 0x8D0CA230;
			*(u32*)0x8007A248 = 0x3C0D8007;
			*(u32*)0x8007A24C = 0x35AD537C;
			*(u32*)0x8007A250 = 0x8DAE0000;
			*(u32*)0x8007A254 = 0x11C00009;
			*(u32*)0x8007A258 = 0x00000000;
			*(u32*)0x8007A25C = 0x114E0007;
			*(u32*)0x8007A260 = 0x00000000;
			*(u32*)0x8007A264 = 0x116E000E;
			*(u32*)0x8007A268 = 0x00000000;
			*(u32*)0x8007A26C = 0x118E0015;
			*(u32*)0x8007A270 = 0x00000000;
			*(u32*)0x8007A274 = 0x1000001B;
			*(u32*)0x8007A278 = 0x00000000;
			*(u32*)0x8007A27C = 0x3529A340;
			*(u32*)0x8007A280 = 0x8D2E0000;
			*(u32*)0x8007A284 = 0x8D2F0004;
			*(u32*)0x8007A288 = 0x11C00016;
			*(u32*)0x8007A28C = 0x00000000;
			*(u32*)0x8007A290 = 0xADCF0000;
			*(u32*)0x8007A294 = 0x1000FFFA;
			*(u32*)0x8007A298 = 0x25290008;
			*(u32*)0x8007A29C = 0x00000000;
			*(u32*)0x8007A2A0 = 0x3529A400;
			*(u32*)0x8007A2A4 = 0x8D2E0000;
			*(u32*)0x8007A2A8 = 0x8D2F0004;
			*(u32*)0x8007A2AC = 0x11C0000D;
			*(u32*)0x8007A2B0 = 0x00000000;
			*(u32*)0x8007A2B4 = 0xADCF0000;
			*(u32*)0x8007A2B8 = 0x1000FFFA;
			*(u32*)0x8007A2BC = 0x25290008;
			*(u32*)0x8007A2C0 = 0x00000000;
			*(u32*)0x8007A2C4 = 0x3529A700;
			*(u32*)0x8007A2C8 = 0x8D2E0000;
			*(u32*)0x8007A2CC = 0x8D2F0004;
			*(u32*)0x8007A2D0 = 0x11C00004;
			*(u32*)0x8007A2D4 = 0x00000000;
			*(u32*)0x8007A2D8 = 0xADCF0000;
			*(u32*)0x8007A2DC = 0x1000FFFA;
			*(u32*)0x8007A2E0 = 0x25290008;
			*(u32*)0x8007A2E4 = 0x03E00008;
			*(u32*)0x8007A340 = 0x00113cf0;
			*(u32*)0x8007A344 = 0x24020001;
			*(u32*)0x8007A400 = 0x002a87a8;
			*(u32*)0x8007A404 = 0x24020001;
			*(u32*)0x8007A408 = 0x005E03A0;
			*(u32*)0x8007A40C = 0x6F642E61;
			*(u32*)0x8007A410 = 0x005E03A4;
			*(u32*)0x8007A414 = 0x63616874;
			*(u32*)0x8007A418 = 0x005E03A8;
			*(u32*)0x8007A41C = 0x7372656B;
			*(u32*)0x8007A420 = 0x005E03AC;
			*(u32*)0x8007A424 = 0x67726F2E;
			*(u32*)0x8007A428 = 0x005E03B0;
			*(u32*)0x8007A42C = 0x00000000;
			*(u32*)0x8007A430 = 0x005E03B4;
			*(u32*)0x8007A434 = 0x00000000;
			*(u32*)0x8007A438 = 0x00113cf0;
			*(u32*)0x8007A43C = 0x24020001;
			*(u32*)0x8007A440 = 0x00177528;
			*(u32*)0x8007A444 = 0x00001021;
			*(u32*)0x8007A448 = 0x0028733c;
			*(u32*)0x8007A44C = 0x00003021;
			*(u32*)0x8007A450 = 0x002CB9F0;
			*(u32*)0x8007A454 = 0x00003021;
			*(u32*)0x8007A700 = 0x002a87a8;
			*(u32*)0x8007A704 = 0x24020001;




			*(u32*)0x800002FC = 0x0C01E800;


			ee_kmode_exit();
			
			
			startGame();
		}
	}
   return 0;
}
void startGame()
{
	scr_printf(" Loading... ");

	//wait for CD to spin up
	int cdWait = 6;

	// 6 = Ready
	// 0 = Error
	// 2 = Ready
	while (cdWait == 6)
	{
		cdWait = sceCdNCmdDiskReady();
	}

	if(cdWait == 2)
	{
		// Shutdown
		padPortClose(0, 0);

		SifExitRpc();
		LoadExecPS2("cdrom0:\\SLPS_255.27;1", 0, NULL);
	}

}
