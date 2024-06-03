/****************************************************************************/
/*! @file
@brief DirectShow media sample wrapper for Windows media format

DirectShowのMedia SampleをラップしたWindows Media Format SDK用のBuffer
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/28
@note
*****************************************************************************/


#ifndef __WINDOWS_MEDIA_BUFFER__DSHOW_WRAPPER_H__
#define __WINDOWS_MEDIA_BUFFER__DSHOW_WRAPPER_H__

#include <assert.h>
#include <windows.h>
//#include <dshow.h>
#if 0
#include <atlcomcli.h>
#else
#include <strmif.h>
#include <comdef.h>
_COM_SMARTPTR_TYPEDEF(IMediaSample,__uuidof(IMediaSample));
#endif
#include <wmsdk.h>
#include <streams.h>
#include <dshow.h>

//----------------------------------------------------------------------------
//! @brief WMV用バッファ
//!
//! DirectShowではSampleにあたるもの。
//! 現在の実装はIMediaSampleのラッパーになっている。
//----------------------------------------------------------------------------
class CWMBuffer : public INSSBuffer, public CUnknown
{
	IMediaSamplePtr	m_Sample;

	IMediaSample *Sample()
	{
		assert( m_Sample );
		return m_Sample;
	}
public:
	CWMBuffer( IMediaSample *smaple ) : CUnknown(NAME("WM Buffer"),NULL), m_Sample(smaple) {}
	virtual ~CWMBuffer() {}

	void SetSample( IMediaSample *sample ) { m_Sample = sample; }
	IMediaSample *GetSample() { return m_Sample; }

	// Methods of IUnknown
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface( REFIID riid, void ** ppv );

	// Methods of INSSBuffer
	STDMETHODIMP GetBuffer( BYTE **ppdwBuffer );
	STDMETHODIMP GetBufferAndLength( BYTE **ppdwBuffer, DWORD *pdwLength );
	STDMETHODIMP GetLength( DWORD *pdwLength );
	STDMETHODIMP GetMaxLength( DWORD *pdwLength );
	STDMETHODIMP SetLength( DWORD dwLength );
};

#endif	// __WINDOWS_MEDIA_BUFFER__DSHOW_WRAPPER_H__
