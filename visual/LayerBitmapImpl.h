//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Base Layer Bitmap implementation
//---------------------------------------------------------------------------
#ifndef LayerBitmapImplH
#define LayerBitmapImplH

#include "ComplexRect.h"

#include "BitmapInfomation.h"




//---------------------------------------------------------------------------
// tTVPBitmap : internal bitmap object
//---------------------------------------------------------------------------
class tTVPBitmap
{
public:
	static const tjs_int DEFAULT_PALETTE_COUNT = 256;
private:
	tjs_int RefCount;

	void * Bits; // pointer to bitmap bits
	BitmapInfomation *BitmapInfo; // DIB information

	tjs_int PitchBytes; // bytes required in a line
	tjs_int PitchStep; // step bytes to next(below) line
	tjs_int Width; // actual width
	tjs_int Height; // actual height

	tjs_int ActualPalCount;
	tjs_uint* Palette;

public:
	tTVPBitmap(tjs_uint width, tjs_uint height, tjs_uint bpp, bool unpadding=false);
	// for async load
	// @param bits : tTVPBitmapBitsAlloc::Allocで確保したものを使用すること
	tTVPBitmap(tjs_uint width, tjs_uint height, tjs_uint bpp, void* bits);

	tTVPBitmap(const tTVPBitmap & r);

	~tTVPBitmap();

	void Allocate(tjs_uint width, tjs_uint height, tjs_uint bpp, bool unpadding=false);

	void AddRef(void)
	{
		RefCount ++;
	}

	void Release(void)
	{
		if(RefCount == 1)
			delete this;
		else
			RefCount--;
	}

	tjs_uint GetWidth() const { return Width; }
	tjs_uint GetHeight() const { return Height; }

	tjs_uint GetBPP() const { return BitmapInfo->GetBPP(); }
	bool Is32bit() const { return BitmapInfo->Is32bit(); }
	bool Is8bit() const { return BitmapInfo->Is8bit(); }


	void * GetScanLine(tjs_uint l) const;

	tjs_int GetPitch() const { return PitchStep; }

	bool IsIndependent() const { return RefCount == 1; }

	const BitmapInfomation* GetBitmapInfomation() const { return BitmapInfo; }
#ifdef _WIN32
	const BITMAPINFO * GetBITMAPINFO() const { return BitmapInfo->GetBITMAPINFO(); }
	const BITMAPINFOHEADER * GetBITMAPINFOHEADER() const { return (const BITMAPINFOHEADER*)( BitmapInfo->GetBITMAPINFO() ); }
#endif

	const void * GetBits() const { return Bits; }
	const tjs_uint* GetPalette() const { return Palette; };
	tjs_uint* GetPalette() { return Palette; };
	tjs_uint GetPaletteCount() const { return ActualPalCount; };
	void SetPaletteCount( tjs_uint count );
};
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// tTVPNativeBaseBitmap
//---------------------------------------------------------------------------
class tTVPBitmap;
class tTVPComplexRect;
class tTVPNativeBaseBitmap
{
public:
	tTVPNativeBaseBitmap(tjs_uint w, tjs_uint h, tjs_uint bpp, bool unpadding=false);
	tTVPNativeBaseBitmap(const tTVPNativeBaseBitmap & r);
	virtual ~tTVPNativeBaseBitmap();

	/* metrics */
	tjs_uint GetWidth() const ;
	void SetWidth(tjs_uint w);
	tjs_uint GetHeight() const;
	void SetHeight(tjs_uint h);

	void SetSize(tjs_uint w, tjs_uint h, bool keepimage = true);
	// for async load
	// @param bits : tTVPBitmapBitsAlloc::Allocで確保したものを使用すること
	void SetSizeAndImageBuffer( tjs_uint width, tjs_uint height, void* bits );

	/* color depth */
	tjs_uint GetBPP() const;

	bool Is32BPP() const;
	bool Is8BPP() const;

	/* assign */
	bool Assign(const tTVPNativeBaseBitmap &rhs) ;
	bool AssignBitmap(const tTVPNativeBaseBitmap &rhs) ; // assigns only bitmap

	/* scan line */
	const void * GetScanLine(tjs_uint l) const;
	void * GetScanLineForWrite(tjs_uint l);
	tjs_int GetPitchBytes() const;


	/* object lifetime management */
	void Independ();
	void IndependNoCopy();
	void Recreate();
	void Recreate(tjs_uint w, tjs_uint h, tjs_uint bpp, bool unpadding=false);

	bool IsIndependent() const { return Bitmap->IsIndependent(); }

	/* other utilities */
	tTVPBitmap * GetBitmap() const { return Bitmap; }

	tjs_uint GetPalette( tjs_uint index ) const;
	void SetPalette( tjs_uint index, tjs_uint color );

protected:
private:
	tTVPBitmap *Bitmap;
public:
	void operator =(const tTVPNativeBaseBitmap &rhs) { Assign(rhs); }
};
//---------------------------------------------------------------------------


#endif
