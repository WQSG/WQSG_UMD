/*
	WQSG_UMD Console Command Mode

	Author:		kid, K_I_D@126.com,dark.z.kid@gmail.com
	Function:	ParseCommandParam

	History:
		2009.2.18	
		2009.2.15	first create

	Readme:
		--iso			umd iso文件名
		--file			要导入的文件或目录名（包含目录本身）
		--importdir		导入的目的文件夹
		--quiet			不显示输出
		--log			输出写到文件
*/

#include "stdafx.h"
#include "consolecommand.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>


bool quiet = false;
static const wchar_t * logfilename = _T("wqsg_umd_log.txt");
static FILE * logfile;
static CStringW file;
static CStringW iso;
static CStringW importDir;


#define STRLEN(x) sizeof(x) - 1
#define dprint(fmt,...) if (!quiet) {_tprintf(fmt,__VA_ARGS__);} \
						if (logfile) {fwprintf(logfile, fmt ,__VA_ARGS__);}


// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{
	int hConHandle;

	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	BOOL ret = AllocConsole();


	// set the screen buffer to be big enough to let us scroll text

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), 

	&coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), 

	coninfo.dwSize);

	// redirect unbuffered STDOUT to the console

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;

	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, NULL, _IONBF, 0 );


	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	// point to console as well
	std::ios::sync_with_stdio();

	if (ret) {SetConsoleTitle(_T("WQSG_UMD Console Command Mode"));}
}

void InitConsole()
{
	if (!quiet)
	{
		AttachConsole(ATTACH_PARENT_PROCESS);
		RedirectIOToConsole();
		/*if (!AttachConsole(ATTACH_PARENT_PROCESS))
		{
			AllocConsole();
		}*/
	}
	setlocale(LC_ALL, "CHS");		//support chinese
	//std::wcout.imbue(std::locale("ZHI"));
	if (logfile)
	{
		CTime tmp = CTime::GetCurrentTime();
		fwprintf(logfile, _T("%s"), tmp.Format(_T("\n========== %A, %B %d, %Y \t %H:%M:%S ==========\n")));
		fwprintf(logfile, _T("WQSG_UMD Console Command Mode\n"));
		fwprintf(logfile, _T("use '%s'\n"), GetCommandLineW());
	}
}

void CloseConsole()
{
	if (logfile) {fclose(logfile);}
	FreeConsole();
}

void PrintHelp()
{
	InitConsole();
	dprint(_T("WQSG_UMD Console Command Mode\n"));
	dprint(_T("Add by kid\t2009.2.15\n\n"));
	dprint(_T("WQSG_UMD.exe <--iso=a.iso> <--file=file|dir> [--importdir=pspdir] [--quiet] [--log]\n"));
	dprint(_T("example:\n"));
	dprint(_T("    WQSG_UMD.exe --iso=c:\\umd0001.iso --file=.\\EBOOT.BIN --importdir=PSP_GAME/SYSDIR --log\n"));
	dprint(_T("    WQSG_UMD.exe --iso=umd0002.iso --file=.\\PSP_GAME --log --quiet\n"));

	CloseConsole();
	ExitProcess(0);
}

bool ParseCommandParam(const LPWSTR* const szArglist, int nArgs)
{
	bool parseOK = true;
	std::wstring errInfo;
	for (int i = 1; i < nArgs; ++i)
	{
		if (!StrCmpNI(szArglist[i],_T("--quiet"), sizeof("--quiet")))
		{
			quiet = true;
		}
		else if (!StrCmpNI(szArglist[i], _T("--iso="), STRLEN("--iso=")))
		{
			iso.Format(_T("%s"),szArglist[i] + STRLEN("--iso="));
		}
		else if (!StrCmpI(szArglist[i], _T("--log")))
		{
			if (_wfopen_s(&logfile, logfilename, _T("a")))
			{
				parseOK = false;
				errInfo += _T("Open log file error.\n");
				break;
			}
		}
		else if (!StrCmpNI(szArglist[i], _T("--file="), STRLEN("--file=")))
		{
			file.Format(_T("%s"),szArglist[i] + STRLEN("--file="));
		}
		else if (!StrCmpNI(szArglist[i], _T("--importdir="), STRLEN("--importdir=")))
		{
			importDir.Format(_T("%s"),szArglist[i] + STRLEN("--importdir="));
		}
		else if (!StrCmpNI(szArglist[i], _T("--help"), STRLEN("--help")))
		{
			PrintHelp();
		}
		else
		{
			parseOK = false;
			errInfo += szArglist[i];
			errInfo += _T(": unknow param.\n");
			break;
		}
	}

	InitConsole();
	if (parseOK && !file.IsEmpty() && !iso.IsEmpty())
	{
		CISO_App m_umd;
		if( !m_umd.OpenISO(iso , true , E_WIT_UMD ) )
		{
			parseOK = false;
			dprint(_T("Open iso file %s error.\n"), iso);
		}
		else if (!m_umd.导入文件(file, importDir.IsEmpty() ? "" : CW2A(importDir.GetBuffer()), 0))
		{
			parseOK = false;
			dprint(_T("Import error: %s\n"), m_umd.GetErrStr());
		}
		else
		{
			dprint(_T("Import ok.\n"));
		}
	}
	else
	{
		parseOK = false;
		if (iso.IsEmpty())
		{
			errInfo += _T("Missing required argument: --iso.\n");
		}
		if (file.IsEmpty())
		{
			errInfo += _T("Missing required argument: --file.\n");
		}
		dprint(_T("%s\n"), errInfo.c_str());
	}
	CloseConsole();
	return parseOK;
}
