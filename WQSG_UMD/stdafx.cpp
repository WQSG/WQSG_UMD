/*  WQSG_UMD
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
// stdafx.cpp : 只包括标准包含文件的源文件
// WQSG_UMD.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


#include<ISO/ISO_cpp.h>

class CWQSG_LCID
{
	static BOOL CALLBACK InitLCID_CallBack(LPWSTR a_str);
public:
	std::map<std::wstring,LCID> m_LocaleNameToLCID;

	CWQSG_LCID()
	{
		if( !EnumSystemLocalesW( InitLCID_CallBack , LCID_SUPPORTED ) )
			exit(-1);
	}
};

static CWQSG_LCID gs_wqsg_lcid;

BOOL CALLBACK CWQSG_LCID::InitLCID_CallBack(LPWSTR a_str)
{
	int iTmp = 0;
	swscanf( a_str , L"%x" , &iTmp );

	WCHAR buf[1024];
	if( WQSG_LCIDToLocaleName( (LCID)iTmp , buf , 1024 ) )
	{
		CStringW str( buf );
		str.MakeLower();

		gs_wqsg_lcid.m_LocaleNameToLCID[str.GetString()] = (LCID)iTmp;
	}

	return TRUE;	
}

BOOL WQSG_LCIDToLocaleName(
						   __in LCID     Locale,
						   __out_ecount_opt(cchName) LPWSTR  lpName,
						   __in int      cchName)
{
#if 1
	if( GetLocaleInfo( Locale , LOCALE_SNAME , lpName , 1024 ) )
	{
		return TRUE;
	}
	else if( GetLocaleInfo( Locale , LOCALE_SISO639LANGNAME , lpName , cchName ) &&
		GetLocaleInfo( Locale , LOCALE_SISO3166CTRYNAME , lpName + wcslen(lpName) + 1 , cchName - wcslen(lpName) - 1 ) )
	{
		lpName[wcslen(lpName)] = L'-';
		return TRUE;
	}

	return FALSE;
#else
	return LCIDToLocaleName( Locale , lpName , cchName , 0 );
#endif
}

BOOL InitLCID()
{
	
}

LCID WQSG_LocaleNameToLCID( LPCWSTR  lpName )
{
	CStringW str( lpName );
	str.MakeLower();

	std::map<std::wstring,LCID>::iterator it = gs_wqsg_lcid.m_LocaleNameToLCID.find( str.GetString() );
	return (it == gs_wqsg_lcid.m_LocaleNameToLCID.end())?0:it->second;
}
