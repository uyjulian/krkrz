//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Base Layer Bitmap implementation
//---------------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include "tjsCommHead.h"

#include <memory>
#include <stdlib.h>
#include <math.h>

#include "LayerBitmapIntf.h"
#include "LayerBitmapImpl.h"
#include "MsgIntf.h"
#include "ComplexRect.h"
#include "tvpgl.h"
#include "tjsHashSearch.h"
#include "EventIntf.h"
#include "SysInitImpl.h"
#include "StorageIntf.h"
#include "DebugIntf.h"
#ifdef _WIN32
#include "WindowFormUnit.h"
#endif
#include "UtilStreams.h"

#include "StringUtil.h"
#include "BitmapBitsAlloc.h"

#ifdef __ANDROID__
#include "VirtualKey.h"
#endif

//---------------------------------------------------------------------------
// tTVPBitmap : internal bitmap object
//---------------------------------------------------------------------------
/*
	important:
	Note that each lines must be started at tjs_uint32 ( 4bytes ) aligned address.
	This is the default Windows bitmap allocate behavior.
*/
tTVPBitmap::tTVPBitmap(tjs_uint width, tjs_uint height, tjs_uint bpp, bool unpadding)
{
	// tTVPBitmap constructor
#ifdef _WIN32
	TVPInitWindowOptions(); // ensure window/bitmap usage options are initialized
#endif
	RefCount = 1;

	Allocate(width, height, bpp, unpadding); // allocate initial bitmap
}
//---------------------------------------------------------------------------
tTVPBitmap::tTVPBitmap(tjs_uint width, tjs_uint height, tjs_uint bpp, void* bits)
{
	// tTVPBitmap constructor
#ifdef _WIN32
	TVPInitWindowOptions(); // ensure window/bitmap usage options are initialized
#endif
	RefCount = 1;

	BitmapInfo = new BitmapInfomation( width, height, bpp );
	Width = width;
	Height = height;
	PitchBytes = BitmapInfo->GetPitchBytes();
	PitchStep = -PitchBytes;

	// set bitmap bits
	try
	{
		Bits = bits;
		if( bpp == 8 ) {
			Palette = new tjs_uint[DEFAULT_PALETTE_COUNT];
			ActualPalCount = 0;
		} else {
			Palette = NULL;
			ActualPalCount = 0;
		}
	}
	catch(...)
	{
		delete BitmapInfo;
		BitmapInfo = NULL;
		throw;
	}
}
//---------------------------------------------------------------------------
tTVPBitmap::~tTVPBitmap()
{
	tTVPBitmapBitsAlloc::Free(Bits);
	delete BitmapInfo;
	if( Palette ) delete Palette;
}
//---------------------------------------------------------------------------
tTVPBitmap::tTVPBitmap(const tTVPBitmap & r)
{
	// constructor for cloning bitmap
#ifdef _WIN32
	TVPInitWindowOptions(); // ensure window/bitmap usage options are initialized
#endif
	RefCount = 1;

	// allocate bitmap which has the same metrics to r
	Allocate(r.GetWidth(), r.GetHeight(), r.GetBPP());

	// copy BitmapInfo
	*BitmapInfo = *r.BitmapInfo;

	// copy Bits
	if(r.Bits) memcpy(Bits, r.Bits, r.BitmapInfo->GetImageSize() );
	if(r.Palette) {
		memcpy(Palette, r.Palette, sizeof(tjs_uint)*DEFAULT_PALETTE_COUNT );
		ActualPalCount = r.ActualPalCount;
	}

	// copy pitch
	PitchBytes = r.PitchBytes;
	PitchStep = r.PitchStep;
}
//---------------------------------------------------------------------------
void tTVPBitmap::Allocate(tjs_uint width, tjs_uint height, tjs_uint bpp, bool unpadding)
{
	// allocate bitmap bits
	// bpp must be 8 or 32

	// create BITMAPINFO
	BitmapInfo = new BitmapInfomation( width, height, bpp, unpadding );

	Width = width;
	Height = height;
	PitchBytes = BitmapInfo->GetPitchBytes();
	PitchStep = -PitchBytes;

	// allocate bitmap bits
	try
	{
		Bits = tTVPBitmapBitsAlloc::Alloc(BitmapInfo->GetImageSize(), width, height);
		if( bpp == 8 ) {
			Palette = new tjs_uint[DEFAULT_PALETTE_COUNT];
			ActualPalCount = 0;
		} else {
			Palette = NULL;
			ActualPalCount = 0;
		}
	}
	catch(...)
	{
		delete BitmapInfo;
		BitmapInfo = NULL;
		throw;
	}
}
//---------------------------------------------------------------------------
void * tTVPBitmap::GetScanLine(tjs_uint l) const
{
	if((tjs_int)l>=BitmapInfo->GetHeight() )
	{
		TVPThrowExceptionMessage(TVPScanLineRangeOver, ttstr((tjs_int)l),
			ttstr((tjs_int)BitmapInfo->GetHeight()-1));
	}

	return (BitmapInfo->GetHeight() - l -1 ) * PitchBytes + (tjs_uint8*)Bits;
}
//---------------------------------------------------------------------------
void tTVPBitmap::SetPaletteCount( tjs_uint count ) {
	if( !Is8bit() ) TVPThrowExceptionMessage(TVPInvalidOperationFor32BPP);
	if( count >= DEFAULT_PALETTE_COUNT )
		TVPThrowExceptionMessage(TJSRangeError);

	ActualPalCount = count;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tTVPNativeBaseBitmap
//---------------------------------------------------------------------------
tTVPNativeBaseBitmap::tTVPNativeBaseBitmap(tjs_uint w, tjs_uint h, tjs_uint bpp, bool unpadding)
{
	Bitmap = new tTVPBitmap(w, h, bpp, unpadding);
}
//---------------------------------------------------------------------------
tTVPNativeBaseBitmap::tTVPNativeBaseBitmap(const tTVPNativeBaseBitmap & r)
{
	Bitmap = r.Bitmap;
	Bitmap->AddRef();
}
//---------------------------------------------------------------------------
tTVPNativeBaseBitmap::~tTVPNativeBaseBitmap()
{
	Bitmap->Release();
}
//---------------------------------------------------------------------------
tjs_uint tTVPNativeBaseBitmap::GetWidth() const
{
	return Bitmap->GetWidth();
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::SetWidth(tjs_uint w)
{
	SetSize(w, Bitmap->GetHeight());
}
//---------------------------------------------------------------------------
tjs_uint tTVPNativeBaseBitmap::GetHeight() const
{
	return Bitmap->GetHeight();
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::SetHeight(tjs_uint h)
{
	SetSize(Bitmap->GetWidth(), h);
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::SetSize(tjs_uint w, tjs_uint h, bool keepimage)
{
	if(Bitmap->GetWidth() != w || Bitmap->GetHeight() != h)
	{
		// create a new bitmap and copy existing bitmap
		tTVPBitmap *newbitmap = new tTVPBitmap(w, h, Bitmap->GetBPP());

		if(keepimage)
		{
			tjs_int pixelsize = Bitmap->Is32bit() ? 4 : 1;
			tjs_int lh = h < Bitmap->GetHeight() ?
				h : Bitmap->GetHeight();
			tjs_int lw = w < Bitmap->GetWidth() ?
				w : Bitmap->GetWidth();
			tjs_int cs = lw * pixelsize;
			tjs_int i;
			for(i = 0; i < lh; i++)
			{
				void * ds = newbitmap->GetScanLine(i);
				void * ss = Bitmap->GetScanLine(i);

				memcpy(ds, ss, cs);
			}
			if( pixelsize == 1 )
				memcpy(newbitmap->GetPalette(), Bitmap->GetPalette(), sizeof(tjs_uint)*tTVPBitmap::DEFAULT_PALETTE_COUNT);
		}

		Bitmap->Release();
		Bitmap = newbitmap;
	}
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::SetSizeAndImageBuffer( tjs_uint width, tjs_uint height, void* bits )
{
	// create a new bitmap and copy existing bitmap
	tTVPBitmap *newbitmap = new tTVPBitmap(width, height, Bitmap->GetBPP(), bits );
	Bitmap->Release();
	Bitmap = newbitmap;
}
//---------------------------------------------------------------------------
tjs_uint tTVPNativeBaseBitmap::GetBPP() const
{
	return Bitmap->GetBPP();
}
//---------------------------------------------------------------------------
bool tTVPNativeBaseBitmap::Is32BPP() const
{
	return Bitmap->Is32bit();
}
//---------------------------------------------------------------------------
bool tTVPNativeBaseBitmap::Is8BPP() const
{
	return Bitmap->Is8bit();
}
//---------------------------------------------------------------------------
bool tTVPNativeBaseBitmap::Assign(const tTVPNativeBaseBitmap &rhs)
{
	if(this == &rhs || Bitmap == rhs.Bitmap) return false;

	Bitmap->Release();
	Bitmap = rhs.Bitmap;
	Bitmap->AddRef();

	return true; // changed
}
//---------------------------------------------------------------------------
bool tTVPNativeBaseBitmap::AssignBitmap(const tTVPNativeBaseBitmap &rhs)
{
	// assign only bitmap
	if(this == &rhs || Bitmap == rhs.Bitmap) return false;

	Bitmap->Release();
	Bitmap = rhs.Bitmap;
	Bitmap->AddRef();

	return true;
}
//---------------------------------------------------------------------------
const void * tTVPNativeBaseBitmap::GetScanLine(tjs_uint l) const
{
	return Bitmap->GetScanLine(l);
}
//---------------------------------------------------------------------------
void * tTVPNativeBaseBitmap::GetScanLineForWrite(tjs_uint l)
{
	Independ();
	return Bitmap->GetScanLine(l);
}
//---------------------------------------------------------------------------
tjs_int tTVPNativeBaseBitmap::GetPitchBytes() const
{
	return Bitmap->GetPitch();
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::Independ()
{
	// sever Bitmap's image sharing
	if(Bitmap->IsIndependent()) return;
	tTVPBitmap *newb = new tTVPBitmap(*Bitmap);
	Bitmap->Release();
	Bitmap = newb;
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::IndependNoCopy()
{
	// indepent the bitmap, but not to copy the original bitmap
	if(Bitmap->IsIndependent()) return;
	Recreate();
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::Recreate()
{
	Recreate(Bitmap->GetWidth(), Bitmap->GetHeight(), Bitmap->GetBPP());
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::Recreate(tjs_uint w, tjs_uint h, tjs_uint bpp, bool unpadding)
{
	Bitmap->Release();
	Bitmap = new tTVPBitmap(w, h, bpp, unpadding);
}
//---------------------------------------------------------------------------
tjs_uint tTVPNativeBaseBitmap::GetPalette( tjs_uint index ) const {
	if( !Is8BPP() ) TVPThrowExceptionMessage(TVPInvalidOperationFor32BPP);
	if( index >= Bitmap->GetPaletteCount() )
		TVPThrowExceptionMessage(TJSRangeError);

	return Bitmap->GetPalette()[index];
}
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::SetPalette( tjs_uint index, tjs_uint color ) {
	if( !Is8BPP() ) TVPThrowExceptionMessage(TVPInvalidOperationFor32BPP);
	if( index >= Bitmap->GetPaletteCount() ) {
		Bitmap->SetPaletteCount( index+1 );
	}
	Bitmap->GetPalette()[index] = color;
}
