//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Base Layer Bitmap implementation
//---------------------------------------------------------------------------
#ifndef LayerBitmapIntfH
#define LayerBitmapIntfH


#include "ComplexRect.h"
#include "tvpgl.h"
#include "argb.h"

/*[*/
//---------------------------------------------------------------------------
// tTVPBBBltMethod and tTVPBBStretchType
//---------------------------------------------------------------------------
enum tTVPBBBltMethod
{
	bmCopy,
	bmCopyOnAlpha,
	bmAlpha,
	bmAlphaOnAlpha,
	bmAdd,
	bmSub,
	bmMul,
	bmDodge,
	bmDarken,
	bmLighten,
	bmScreen,
	bmAddAlpha,
	bmAddAlphaOnAddAlpha,
	bmAddAlphaOnAlpha,
	bmAlphaOnAddAlpha,
	bmCopyOnAddAlpha,
	bmPsNormal,
	bmPsAdditive,
	bmPsSubtractive,
	bmPsMultiplicative,
	bmPsScreen,
	bmPsOverlay,
	bmPsHardLight,
	bmPsSoftLight,
	bmPsColorDodge,
	bmPsColorDodge5,
	bmPsColorBurn,
	bmPsLighten,
	bmPsDarken,
	bmPsDifference,
	bmPsDifference5,
	bmPsExclusion
};

enum tTVPBBStretchType
{
	stNearest = 0, // primal method; nearest neighbor method
	stFastLinear = 1, // fast linear interpolation (does not have so much precision)
	stLinear = 2,  // (strict) linear interpolation
	stCubic = 3,    // cubic interpolation
	stSemiFastLinear = 4,
	stFastCubic = 5,
	stLanczos2 = 6,    // Lanczos 2 interpolation
	stFastLanczos2 = 7,
	stLanczos3 = 8,    // Lanczos 3 interpolation
	stFastLanczos3 = 9,
	stSpline16 = 10,	// Spline16 interpolation
	stFastSpline16 = 11,
	stSpline36 = 12,	// Spline36 interpolation
	stFastSpline36 = 13,
	stAreaAvg = 14,	// Area average interpolation
	stFastAreaAvg = 15,
	stGaussian = 16,
	stFastGaussian = 17,
	stBlackmanSinc = 18,
	stFastBlackmanSinc = 19,

	stTypeMask = 0x0000ffff, // stretch type mask
	stFlagMask = 0xffff0000, // flag mask

	stRefNoClip = 0x10000 // referencing source is not limited by the given rectangle
						// (may allow to see the border pixel to interpolate)
};
/*]*/



//---------------------------------------------------------------------------
// FIXME: for including order problem
//---------------------------------------------------------------------------
#include "LayerBitmapImpl.h"
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
#define TVP_BB_COPY_MAIN 1
#define TVP_BB_COPY_MASK 2
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
extern tTVPGLGammaAdjustData TVPIntactGammaAdjustData;
extern tjs_int TVPDrawThreadNum;
extern tjs_int TVPGetProcessorNum(void);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTVPBaseBitmap
//---------------------------------------------------------------------------
class tTVPNativeBaseBitmap;
class tTVPBaseBitmap : public tTVPNativeBaseBitmap
{
public:
	tTVPBaseBitmap(tjs_uint w, tjs_uint h, tjs_uint bpp = 32, bool unpadding=false);
	tTVPBaseBitmap(const tTVPBaseBitmap & r) :
		tTVPNativeBaseBitmap(r) {}
	~tTVPBaseBitmap();

public:

	void operator =(const tTVPBaseBitmap & rhs) { Assign(rhs); }

	// metrics
	void SetSizeWithFill(tjs_uint w, tjs_uint h, tjs_uint32 fillvalue);

	// point access
	tjs_uint32 GetPoint(tjs_int x, tjs_int y) const;
	bool SetPoint(tjs_int x, tjs_int y, tjs_uint32 value);
	bool SetPointMain(tjs_int x, tjs_int y, tjs_uint32 color); // for 32bpp
	bool SetPointMask(tjs_int x, tjs_int y, tjs_int mask); // for 32bpp

	// drawing stuff
	bool Fill(tTVPRect rect, tjs_uint32 value);

	bool FillColor(tTVPRect rect, tjs_uint32 color, tjs_int opa);

private:
	bool BlendColor(tTVPRect rect, tjs_uint32 color, tjs_int opa, bool additive);

        struct PartialFillParam {
          tTVPBaseBitmap *self;
          tjs_uint8 *dest;
          tjs_int x;
          tjs_int y;
          tjs_int w;
          tjs_int h;
          tjs_int pitch;
          tjs_uint32 value;
          bool is32bpp;
        };
        static void TJS_USERENTRY PartialFillEntry(void *param);
        void PartialFill(const PartialFillParam *param);

        struct PartialFillColorParam {
          tTVPBaseBitmap *self;
          tjs_uint8 *dest;
          tjs_int x;
          tjs_int y;
          tjs_int w;
          tjs_int h;
          tjs_int pitch;
          tjs_uint32 color;
          tjs_int opa;
        };
        static void TJS_USERENTRY PartialFillColorEntry(void *param);
        void PartialFillColor(const PartialFillColorParam *param);

        struct PartialBlendColorParam {
          tTVPBaseBitmap *self;
          tjs_uint8 *dest;
          tjs_int x;
          tjs_int y;
          tjs_int w;
          tjs_int h;
          tjs_int pitch;
          tjs_uint32 color;
          tjs_int opa;
          bool additive;
        };
        static void TJS_USERENTRY PartialBlendColorEntry(void *param);
        void PartialBlendColor(const PartialBlendColorParam *param);

        struct PartialRemoveConstOpacityParam {
          tTVPBaseBitmap *self;
          tjs_uint8 *dest;
          tjs_int x;
          tjs_int y;
          tjs_int w;
          tjs_int h;
          tjs_int pitch;
          tjs_int level;
        };
        static void TJS_USERENTRY PartialRemoveConstOpacityEntry(void *param);
        void PartialRemoveConstOpacity(const PartialRemoveConstOpacityParam *param);
  
        struct PartialFillMaskParam {
          tTVPBaseBitmap *self;
          tjs_uint8 *dest;
          tjs_int x;
          tjs_int y;
          tjs_int w;
          tjs_int h;
          tjs_int pitch;
          tjs_int value;
        };
        static void TJS_USERENTRY PartialFillMaskEntry(void *param);
        void PartialFillMask(const PartialFillMaskParam *param);

        struct PartialCopyRectParam {
          tTVPBaseBitmap *self;
          tjs_int pixelsize;
          tjs_uint8 *dest;
          tjs_int dpitch;
          tjs_int dx;
          tjs_int dy;
          tjs_int w;
          tjs_int h;
          const tjs_int8 *src;
          tjs_int spitch;
          tjs_int sx;
          tjs_int sy;
          tjs_int plane;
          bool backwardCopy;
        };
        static void TJS_USERENTRY PartialCopyRectEntry(void *param);
        void PartialCopyRect(const PartialCopyRectParam *param);

        struct PartialBltParam {
          tTVPBaseBitmap *self;
          tjs_uint8 *dest;
          tjs_int dpitch;
          tjs_int dx;
          tjs_int dy;
          tjs_int w;
          tjs_int h;
          const tjs_int8 *src;
          tjs_int spitch;
          tjs_int sx;
          tjs_int sy;
          tTVPBBBltMethod method;
          tjs_int opa;
          bool hda;
        };
        static void TJS_USERENTRY PartialBltEntry(void *param);
        void PartialBlt(const PartialBltParam *param);

public:
	bool FillColorOnAlpha(tTVPRect rect, tjs_uint32 color, tjs_int opa)
	{
		return BlendColor(rect, color, opa, false);
	}

	bool FillColorOnAddAlpha(tTVPRect rect, tjs_uint32 color, tjs_int opa)
	{
		return BlendColor(rect, color, opa, true);
	}

	bool RemoveConstOpacity(tTVPRect rect, tjs_int level);

	bool FillMask(tTVPRect rect, tjs_int value);

	bool CopyRect(tjs_int x, tjs_int y, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tjs_int plane = (TVP_BB_COPY_MAIN|TVP_BB_COPY_MASK));

	bool Blt(tjs_int x, tjs_int y, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tTVPBBBltMethod method, tjs_int opa,
			bool hda = true);

public:
	void DoGrayScale(tTVPRect rect);




public:
};
//---------------------------------------------------------------------------




#endif
