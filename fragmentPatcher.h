#include "libcdvd.h"
#include "libpad.h"
#include <fileio.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <kernel.h>
#include <loadfile.h>
#include <malloc.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <stdio.h>
#include <tamtypes.h>
#include <string.h>
#include "math.h"
#include "menu.h"



/////////////////////////////////////////////////////////////////////
/////////////// patcher.C DEFINES //////////////////////////////////////
/////////////////////////////////////////////////////////////////////
int http_Download(const char *address);
int http_Cleanup();
int http_ParseAddress(const char *address);
int http_DownloadSave(const char *address, const char *path);
