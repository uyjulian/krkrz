/****************************************************************************/
/*! @file
@brief DirectShowを利用したムービーのレイヤー描画再生

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/25
@note
			2004/08/25	T.Imoto		
*****************************************************************************/

#ifndef __DSLAYERD_H__
#define __DSLAYERD_H__

#include "dsmovie.h"
#include "IRendererBufferAccess.h"
#include "IRendererBufferVideo.h"
#if 1
#include <comdef.h>
_COM_SMARTPTR_TYPEDEF(IRendererBufferAccess,IID_IRendererBufferAccess);
_COM_SMARTPTR_TYPEDEF(IRendererBufferVideo,IID_IRendererBufferVideo);
#endif

class tTVPBaseBitmap;
//----------------------------------------------------------------------------
//! @brief レイヤー描画ビデオクラス
//----------------------------------------------------------------------------
class tTVPDSLayerVideo : public tTVPDSMovie
{
private:
	IRendererBufferAccessPtr	m_BuffAccess;
	IRendererBufferVideoPtr	m_BuffVideo;

	BYTE			*m_BmpBits[2];
	//----------------------------------------------------------------------------
	//! @brief	  	IRendererBufferAccessを取得する
	//! @return		IRendererBufferAccessインターフェイス
	//----------------------------------------------------------------------------
	IRendererBufferAccess *BufferAccess()
	{
		assert( m_BuffAccess );
		return m_BuffAccess;
	}
	//----------------------------------------------------------------------------
	//! @brief	  	IRendererBufferVideoを取得する
	//! @return		IRendererBufferVideoインターフェイス
	//----------------------------------------------------------------------------
	IRendererBufferVideo *BufferVideo()
	{
		assert( m_BuffVideo );
		return m_BuffVideo;
	}

public:
	tTVPDSLayerVideo();
	virtual ~tTVPDSLayerVideo();

	virtual void __stdcall BuildGraph( HWND callbackwin, IStream *stream,
		const tjs_char * streamname, const tjs_char *type, unsigned __int64 size );

	virtual void __stdcall ReleaseAll();

	virtual void __stdcall GetFrontBuffer( BYTE **buff );
	virtual void __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size );

	virtual void __stdcall GetVideoSize( long *width, long *height );
	virtual HRESULT __stdcall GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame );
};

#endif	// __DSLAYERD_H__
