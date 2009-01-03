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
#pragma once

struct SUmdFileData
{
	char name[MAX_PATH];
	s32 size;
	s32 lba;
	BOOL isDir;
};

struct SUmdFileFind
{
	s32 dirOffset;
	_tISO_DirEnt tP_DirEnt;
};

class CUMD_App
{
	CWQSG_UMD m_umd;
	CString m_strLastErr;

	BOOL GetPathDirEnt( _tISO_DirEnt& tDirEnt , const CStringA path );
	BOOL zzz_CreateDir( CStringW strPathName , CStringA strName , CStringA strPath );
	BOOL zzz_导入文件夹( CStringW strPathName , CStringA path );
	BOOL zzz_WriteFile( CStringW strPathName , CStringA strName , CStringA strPath , const s32 offset , const BOOL isNew  );
	BOOL zzz_GetFileData( CStringA pathA , CStringA nameA , _tISO_DirEnt& tDirEnt );
public:
	CUMD_App(void);
	~CUMD_App(void);

	bool OpenISO( CStringW ISO_PathName , const BOOL bCanWrite );

	void CloseISO()
	{
		m_umd.CloseISO( );
	}

	BOOL IsOpen()
	{
		return m_umd.IsOpen();
	}

	CString GetErrStr()
	{
		return m_strLastErr;
	}

	BOOL 导入文件( CStringW strPathName , CStringA path , const s32 offset );
	BOOL 写文件偏移( CStringA pathA , CStringA nameA , s32 oldOffset , CStringW inFileName );
	BOOL 替换文件( CStringA pathA , CStringA nameA , CStringW inFileName );
	BOOL 导出文件( CStringW strPathName , CStringA path , CStringA nameA );

	BOOL GetFileData( CStringA pathA , CStringA nameA , SUmdFileData& data );

	SUmdFileFind* FindUmdFile( CStringA pathA );
	BOOL FindNextUmdFile( SUmdFileFind* handle , SUmdFileData& data );
	void CloseFindUmdFile( SUmdFileFind* handle );
};
