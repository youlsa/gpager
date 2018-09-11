/*
 * cmdline.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef CMDLINE_H_
#define CMDLINE_H_
#define CMDLINE_H_
#ifdef __cplusplus
 extern "C" {
#endif

#define CMDLINE_OK              	(0)
#define CMDLINE_BLE_OK				(1)
#define CMDLINE_ACCOUNT_ID			(2)
#define CMDLINE_ACCOUNT_PASSWORD	(3)
#define CMDLINE_BAD_CMD         	(-1)
#define CMDLINE_TOO_MANY_ARGS   	(-2)
#define CMDLINE_TOO_FEW_ARGS    	(-3)
#define CMDLINE_INVALID_ARG     	(-4)
#define CMDLINE_ERROR           	(-5)

typedef int (*pfnCmdLine)(int argc, char *argv[]);

typedef struct
{
    const char *pcCmd;
    pfnCmdLine pfnCmd;
    const char *pcHelp;
} tCmdLineEntry;

typedef struct
{
	const char *pcCmd;
	pfnCmdLine pfnCmd;
} tCmdInvisibleEntry;

extern tCmdLineEntry g_psCmdTable[];
extern tCmdInvisibleEntry g_psCmdInvisibleTable[];

void CheckForUserCommands(void);

#ifdef __cplusplus
}
#endif
#endif /* CMDLINE_H_ */
