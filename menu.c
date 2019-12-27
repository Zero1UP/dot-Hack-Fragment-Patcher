#include "menu.h"
void menu_DisplayMain(void)
{
	char *currentVersion = "Patch Version: 1.0.5\n";
	
	scr_printf(".hack//Fragment Patcher by 1UP\n");
	scr_printf(currentVersion);
	scr_printf("Visit 1UPsDevelopmment.com for more information.\n");
	scr_printf("Source code: https://github.com/Zero1UP/dot-Hack-Fragment-Patcher \n");	
	scr_printf("Press SQUARE for more information. \n");
	scr_printf("Press CIRCLE to view about. \n");
	scr_printf("Press CROSS to return to main screen. \n");
	scr_printf("Press START to load the patch and start the game.");
}

void menu_DisplayAbout(void)
{
	scr_printf("Fragment Patcher by 1UP\n");
	scr_printf("Special thanks to Harry62 for the idea and help\n");
	scr_printf("Press CROSS to return to main screen.");
}

void menu_PatchInformation(void)
{
	scr_printf("Supported Features:\n");
	scr_printf("DNAS Bypass\n");
	scr_printf("DNS Updated to connect to Alkalime's Lobby Server\n");
	scr_printf("No translations are available.\n");
	scr_printf("Disable item loss on crash. (ONLINE ONLY) \n");
	scr_printf("You will not have to have this setting set. \n");
	scr_printf("------------------------------------------- \n");
	scr_printf("1.0.5 Changes \n");
	scr_printf("Fixed a bug with the game crashing on an original version of the game. \n");
	scr_printf("Press CROSS to return to main screen.");

}
