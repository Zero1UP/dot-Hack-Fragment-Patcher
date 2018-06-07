#include "menu.h"
void menu_DisplayMain(void)
{
	char *currentVersion = "Patch Version: 1.0\n";
	
	
	scr_printf(".hack//Fragment Patcher by 1UP\n");
	scr_printf(currentVersion);
	scr_printf("Visit 1UPsDevelopmment.com for more information.\n");	
	scr_printf("Discord: https://discord.gg/susPecv \n\n\\n");	
	scr_printf("Press SQUARE for more information. \n");
	scr_printf("Press CIRCLE to view about. \n");
	scr_printf("Press CROSS to return to main screen. \n");
	scr_printf("Press Select to check for updates. \n");
	scr_printf("Press START to load the patch and start the game. \n");
}

void menu_DisplayAbout(void)
{
	scr_printf("Fragment Patcher by 1UP\n");
	scr_printf("Special thanks to Harry62 for the idea and help\n");
}

void menu_PatchInformation(void)
{
	scr_printf("Supported Features:\n");
	scr_printf("DNAS Bypass\n");
	scr_printf("DNS Updated to connect to Alkalime's Lobby Server\n");
	scr_printf("No translations are available.\n");
}

void menu_GameInformation(void)
{
	
}