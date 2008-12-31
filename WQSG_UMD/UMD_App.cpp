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
#include "StdAfx.h"
#include "UMD_App.h"

CUMD_App::CUMD_App(void)
{
}

CUMD_App::~CUMD_App(void)
{
	CloseISO();
}

bool CUMD_App::OpenISO( CStringW ISO_PathName , const BOOL bCanWrite )
{
	CloseISO();
	if( !m_umd.OpenISO( ISO_PathName , bCanWrite ) )
	{
		m_strLastErr = ISO_PathName + L"\r\n" + m_umd.GetErrStr();
		return false;
	}
	return true;
}

BOOL CUMD_App::GetPathDirEnt( _tISO_DirEnt& tDirEnt , const CStringA a_path )
{
	CStringA path( a_path );

	if( !m_umd.GetRootDirEnt( tDirEnt ) )
	{
		m_strLastErr = L"读取根目录失败";
		return FALSE;
	}

	if( path.Right( 1 ) != L'/' )
		path += L'/';

	_tISO_DirEnt tmp;

	while( path[0] == '/' )
		path.Delete( 0 , 1 );

	int pos;
	while( (pos = path.Find( '/' )) > 0 )
	{
		CStringA name( path.Left( pos ) );

		path.Delete( 0 , pos );
		while( path[0] == '/' )
			path.Delete( 0 , 1 );

		if( m_umd.FindFile( tDirEnt , name.GetBuffer() , tmp ) < 0 )
		{
			CStringW str;str = a_path;
			m_strLastErr =  str + L"\r\n路经错误";
			return FALSE;
		}

		memcpy( &tDirEnt , &tmp , sizeof(tDirEnt) );
	}
	return TRUE;
}

inline static BOOL TestName( CStringA& strName )
{
	for( int i = 0; (i>=0) && (strName[i]) ; ++i )
	{
		u8 ch = strName[i];
		if( ( (ch < 'a') || (ch > 'z') ) &&
			( (ch < 'A') || (ch > 'Z') ) &&
			( (ch < '0') || (ch > '9') ) &&
			( ch != '.' ) && ( ch != '_' ) )
		{
			//			CStringW strNameW;
			//			MessageBox( L"文件名只能用 字母 数字 点 下划线" , strNameW = strName );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUMD_App::zzz_CreateDir( CStringW strPathName , CStringA strName , CStringA strPath )
{
	if( !TestName(strName) )
	{
		CStringW strNameW;strNameW = strName;
		m_strLastErr = strNameW + L"\r\n目录名只能用 字母 数字 点 下划线" ;
		return FALSE;
	}

	_tISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , strPath ) )
	{
		CStringW strNameW;strNameW = strPath;
		m_strLastErr = strNameW + L"\r\nISO路径错误?";
		return FALSE;
	}

	if( !m_umd.CreateDir( tP_DirEnt , strName ) )
	{
		CStringW strNameW;strNameW = strName;
		m_strLastErr = strNameW + L"\r\n创建目录失败" ;
		return FALSE;
	}

	return TRUE;
}

BOOL CUMD_App::zzz_导入文件夹( CStringW a_strPathName , CStringA path )
{
	CStringW strPathName( a_strPathName );
	{
		while( strPathName.Right(1) == L'\\' )
			strPathName.Delete( strPathName.GetLength() -1 );

		const int pos = strPathName.ReverseFind( L'\\' );
		if( pos <= 0 )
		{
			m_strLastErr = a_strPathName + L"\r\n目录路径错误" ;
			return FALSE;
		}

		CString nameX = strPathName.Mid( pos + 1 );

		CStringA nameA;nameA = nameX;

		if( !zzz_CreateDir( strPathName , nameA , path ) )
			return FALSE;

		path += ('/' + nameA);

		strPathName += L'\\';
	}

	BOOL rt = TRUE;
	WIN32_FIND_DATAW data;

	const HANDLE handle = ::FindFirstFileW( strPathName + L"*.*" , &data );
	if( INVALID_HANDLE_VALUE != handle )
	{
		do{
			if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				CStringW nameW( data.cFileName );
				if( nameW != L"." && nameW != L".." )
				{
					if( !导入文件( strPathName + data.cFileName , path , 0 ) )
					{
						rt = FALSE;
						break;
					}
				}
			}
			else
			{
				if( !导入文件( strPathName + data.cFileName , path , 0 ) )
				{
					rt = FALSE;
					break;
				}
			}

		}while( FindNextFileW( handle , &data ) );

		FindClose( handle );
	}

	return rt;
}

BOOL CUMD_App::导入文件( CStringW strPathName , CStringA path , const s32 offset )
{
	CStringA strNameA;
	{
		const int pos = strPathName.ReverseFind( '\\' );
		if( pos <= 0 )
			return FALSE;

		CString strNameW( strPathName.Mid( pos + 1 ) );
		strNameA = strNameW;
	}

	if( ::WQSG_IsFile( strPathName.GetBuffer() ) )
	{
		if( !zzz_WriteFile( strPathName , strNameA , path , offset , TRUE ) )
			return FALSE;
	}
	else if( ::WQSG_IsDir( strPathName.GetBuffer() ) )
	{
		if( !zzz_导入文件夹( strPathName , path ) )
			return FALSE;
	}
	else
	{
		m_strLastErr = strPathName + L"\r\n文件不存在" ;
		return FALSE;
	}
	return TRUE;
}

BOOL CUMD_App::zzz_WriteFile( CStringW strPathName , CStringA strName , CStringA strPath , const s32 offset , const BOOL isNew  )
{
	if( !TestName(strName) )
	{
		CStringW strNameW ; strNameW = strName;
		m_strLastErr = strNameW + L"\r\n文件名只能用 字母 数字 点 下划线" ;
		return FALSE;
	}

	_tISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , strPath ) )
	{
		CStringW strNameW ; strNameW = strPath;
		m_strLastErr = strNameW + L"\r\nISO路径错误?" ;
		return FALSE;
	}

	CWQSG_File fp;
	if( !fp.OpenFile( strPathName.GetBuffer() , 1 , 3 ) )
	{
		m_strLastErr = strPathName + L"\r\n打开文件失败";
		return FALSE;
	}
	const s64 srcFileSize = fp.GetFileSize();
	if( ( srcFileSize < 0 ) || ( srcFileSize > (((u32)-1))>>1) )
	{
		m_strLastErr = strPathName + L"\r\n文件大小错误" ;
		return FALSE;
	}

	if( !m_umd.WriteFile( tP_DirEnt , strName , fp , (s32)srcFileSize , offset , isNew , FALSE ) )
	{
		m_strLastErr = strPathName + L"\r\n写文件到ISO出错" ;
		return FALSE;
	}

	return TRUE;
}

BOOL CUMD_App::导出文件( CStringW strPathName , CStringA pathA , CStringA nameA )
{
	::DeleteFile( strPathName );

	_tISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , pathA ) )
		return FALSE;

	_tISO_DirEnt tDirEnt;
	if( m_umd.FindFile( tP_DirEnt , nameA.GetBuffer() , tDirEnt ) < 0 )
		return FALSE;

	CWQSG_File fp;
	if( !fp.OpenFile( strPathName.GetBuffer() , 4 , 3 ) )
	{
		m_strLastErr = strPathName + L"\r\n打开文件失败" ;
		return FALSE;
	}

	if( !m_umd.ReadFile( tP_DirEnt , nameA , fp , tDirEnt.size_le , 0 ) )
	{
		fp.Close();
		::DeleteFile( strPathName );
		m_strLastErr = strPathName + L"\r\n导出文件失败" ;
		return FALSE;
	}
	return TRUE;
}

BOOL CUMD_App::写文件偏移( CStringA pathA , CStringA nameA , s32 oldOffset , CStringW inFileName )
{
	_tISO_DirEnt tDirEnt;
	if( !zzz_GetFileData( pathA , nameA , tDirEnt ) )
		return FALSE;

	if( tDirEnt.attr & 2 )
	{
		CStringW str;str = pathA + nameA;
		m_strLastErr = str + L"\r\n不能写文件夹";
		return FALSE;
	}

	return zzz_WriteFile( inFileName , nameA , pathA , oldOffset , FALSE );
}

BOOL CUMD_App::替换文件( CStringA pathA , CStringA nameA , CStringW inFileName )
{
	return zzz_WriteFile( inFileName , nameA , pathA , 0 , TRUE );
}

BOOL CUMD_App::zzz_GetFileData( CStringA pathA , CStringA nameA , _tISO_DirEnt& tDirEnt )
{
	_tISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , pathA ) )
		return FALSE;

	if( nameA[0] != 0 )
	{
		if( m_umd.FindFile( tP_DirEnt , nameA.GetBuffer() , tDirEnt ) < 0 )
		{
			CStringW str;str = pathA + nameA;
			m_strLastErr = str + L"\r\nISO没有此文件";
			return FALSE;
		}
	}
	else
		tDirEnt = tP_DirEnt;

	return TRUE;
}

BOOL CUMD_App::GetFileData( CStringA pathA , CStringA nameA , SUmdFileData& data )
{
	_tISO_DirEnt tmp;
	if( zzz_GetFileData( pathA , nameA , tmp ) )
	{
		data.isDir = ( (tmp.attr & 2) == 2 );
		data.size = tmp.size_le;
		data.lba = tmp.lba_le;

		WQSG_strcpy( nameA.GetBuffer() , data.name );

		return TRUE;
	}

	return FALSE;
}


SUmdFileFind* CUMD_App::FindUmdFile( CStringA pathA )
{
	_tISO_DirEnt tP_DirEnt;
	if( !GetPathDirEnt( tP_DirEnt , pathA ) )
		return NULL;

	SUmdFileFind* pRt = new SUmdFileFind;
	pRt->dirOffset = 0x60;

	return pRt;
}

BOOL CUMD_App::FindNextUmdFile( SUmdFileFind* handle , SUmdFileData& data )
{
	_tISO_DirEnt tDirEnt;

	if( handle->dirOffset < handle->tP_DirEnt.size_le )
	{
		{
			const s32 newOffset( m_umd.ReadDirEnt( handle->tP_DirEnt , handle->dirOffset , tDirEnt , data.name ) );
			if( newOffset < 0 )
			{
				m_strLastErr = L"目录表出错?";
				return FALSE;
			}
			handle->dirOffset = newOffset;
		}

		if( tDirEnt.len == 0 )
		{
			m_strLastErr = "OK";
			return FALSE;
		}

		data.size = tDirEnt.size_le;
		data.lba = tDirEnt.lba_le;

		handle->dirOffset += tDirEnt.len;

		return TRUE;
	}
	return FALSE;
}

void CUMD_App::CloseFindUmdFile( SUmdFileFind* handle )
{
	delete handle;
}
