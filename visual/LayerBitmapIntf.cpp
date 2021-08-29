//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Base Layer Bitmap implementation
//---------------------------------------------------------------------------
#include <vector>

#include "tjsCommHead.h"

#include "DebugIntf.h"
#include "LayerBitmapIntf.h"
#include "MsgIntf.h"
#include "DebugIntf.h"
#include "tvpgl.h"
#include "argb.h"
#include "tjsUtils.h"
#include "ThreadIntf.h"

//#define TVP_FORCE_BILINEAR


//---------------------------------------------------------------------------
// To forcing bilinear interpolation, define TVP_FORCE_BILINEAR.

#ifdef TVP_FORCE_BILINEAR
	#define TVP_BILINEAR_FORCE_COND true
#else
	#define TVP_BILINEAR_FORCE_COND false
#endif
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// intact ( does not affect ) gamma adjustment data
tTVPGLGammaAdjustData TVPIntactGammaAdjustData =
{ 1.0, 0, 255, 1.0, 0, 255, 1.0, 0, 255 };
//---------------------------------------------------------------------------
static float sBmFactor[] =
{
  59, // bmCopy,
  59, // bmCopyOnAlpha,
  52, // bmAlpha,
  52, // bmAlphaOnAlpha,
  61, // bmAdd,
  59, // bmSub,
  45, // bmMul,
  10, // bmDodge,
  58, // bmDarken,
  56, // bmLighten,
  42, // bmScreen,
  52, // bmAddAlpha,
  52, // bmAddAlphaOnAddAlpha,
  52, // bmAddAlphaOnAlpha,
  52, // bmAlphaOnAddAlpha,
  52, // bmCopyOnAddAlpha,
  32, // bmPsNormal,
  30, // bmPsAdditive,
  29, // bmPsSubtractive,
  27, // bmPsMultiplicative,
  27, // bmPsScreen,
  15, // bmPsOverlay,
  15, // bmPsHardLight,
  10, // bmPsSoftLight,
  10, // bmPsColorDodge,
  10, // bmPsColorDodge5,
  10, // bmPsColorBurn,
  29, // bmPsLighten,
  29, // bmPsDarken,
  29, // bmPsDifference,
  26, // bmPsDifference5,
  66, // bmPsExclusion
};

//---------------------------------------------------------------------------
static tjs_int GetAdaptiveThreadNum(tjs_int pixelNum, float factor)
{
  if (pixelNum >= factor * 500)
    return TVPGetThreadNum();
  else
    return 1;
}
//---------------------------------------------------------------------------
#define RET_VOID
#define BOUND_CHECK(x) \
{ \
	tjs_int i; \
	if(rect.left < 0) rect.left = 0; \
	if(rect.top < 0) rect.top = 0; \
	if(rect.right > (i=GetWidth())) rect.right = i; \
	if(rect.bottom > (i=GetHeight())) rect.bottom = i; \
	if(rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0) \
		return x; \
}

//---------------------------------------------------------------------------
// tTVPBaseBitmap
//---------------------------------------------------------------------------
tTVPBaseBitmap::tTVPBaseBitmap(tjs_uint w, tjs_uint h, tjs_uint bpp, bool unpadding) :
		tTVPNativeBaseBitmap(w, h, bpp, unpadding)
{
}
//---------------------------------------------------------------------------
tTVPBaseBitmap::~tTVPBaseBitmap()
{
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::SetSizeWithFill(tjs_uint w, tjs_uint h, tjs_uint32 fillvalue)
{
	// resize, and fill the expanded region with specified value.

	tjs_uint orgw = GetWidth();
	tjs_uint orgh = GetHeight();

	SetSize(w, h);

	if(w > orgw && h > orgh)
	{
		// both width and height were expanded
		tTVPRect rect;
		rect.left = orgw;
		rect.top = 0;
		rect.right = w;
		rect.bottom = h;
		Fill(rect, fillvalue);

		rect.left = 0;
		rect.top = orgh;
		rect.right = orgw;
		rect.bottom = h;
		Fill(rect, fillvalue);
	}
	else if(w > orgw)
	{
		// width was expanded
		tTVPRect rect;
		rect.left = orgw;
		rect.top = 0;
		rect.right = w;
		rect.bottom = h;
		Fill(rect, fillvalue);
	}
	else if(h > orgh)
	{
		// height was expanded
		tTVPRect rect;
		rect.left = 0;
		rect.top = orgh;
		rect.right = w;
		rect.bottom = h;
		Fill(rect, fillvalue);
	}
}
//---------------------------------------------------------------------------
tjs_uint32 tTVPBaseBitmap::GetPoint(tjs_int x, tjs_int y) const
{
	// get specified point's color or color index
	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	if(Is32BPP())
		return  *( (const tjs_uint32*)GetScanLine(y) + x); // 32bpp
	else
		return  *( (const tjs_uint8*)GetScanLine(y) + x); // 8bpp
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::SetPoint(tjs_int x, tjs_int y, tjs_uint32 value)
{
	// set specified point's color(and opacity) or color index
	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	if(Is32BPP())
		*( (tjs_uint32*)GetScanLineForWrite(y) + x) = value; // 32bpp
	else
		*( (tjs_uint8*)GetScanLine(y) + x) = value; // 8bpp

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::SetPointMain(tjs_int x, tjs_int y, tjs_uint32 color)
{
	// set specified point's color (mask is not touched)
	// for 32bpp
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	tjs_uint32 *addr = (tjs_uint32*)GetScanLineForWrite(y) + x;
	*addr &= 0xff000000;
	*addr += color & 0xffffff;

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::SetPointMask(tjs_int x, tjs_int y, tjs_int mask)
{
	// set specified point's mask (color is not touched)
	// for 32bpp
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	tjs_uint32 *addr = (tjs_uint32*)GetScanLineForWrite(y) + x;
	*addr &= 0x00ffffff;
	*addr += (mask & 0xff) << 24;

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::Fill(tTVPRect rect, tjs_uint32 value)
{
	// fill target rectangle represented as "rect", with color ( and opacity )
	// passed by "value".
	// value must be : 0xAARRGGBB (for 32bpp) or 0xII ( for 8bpp )
	BOUND_CHECK(false);

	if(!IsIndependent())
	{
		if(rect.left == 0 && rect.top == 0 &&
			rect.right == (tjs_int)GetWidth() && rect.bottom == (tjs_int)GetHeight())
		{
			// cover overall
			IndependNoCopy(); // indepent with no-copy
		}
	}

        tjs_int pitch = GetPitchBytes();
        tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        tjs_int h = rect.bottom - rect.top;
        tjs_int w = rect.right - rect.left;
        bool is32bpp = Is32BPP();

        tjs_int taskNum = GetAdaptiveThreadNum(w * h, 150);
        TVPBeginThreadTask(taskNum);
        PartialFillParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1 = h * (i + 1) / taskNum;
          PartialFillParam *param = params + i;
          param->self = this;
          param->dest = dest;
          param->pitch = pitch;
          param->x = rect.left;
          param->y = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->value = value;
          param->is32bpp = is32bpp;
          TVPExecThreadTask(&PartialFillEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}

void TJS_USERENTRY tTVPBaseBitmap::PartialFillEntry(void *v)
{
  const PartialFillParam *param = (const PartialFillParam *)v;
  param->self->PartialFill(param);
}

void tTVPBaseBitmap::PartialFill(const PartialFillParam *param)
{


	if(param->is32bpp)
	{
		// 32bpp
		tjs_int pitch = param->pitch;
		tjs_uint8 *sc = param->dest + param->y * pitch;
                tjs_int x = param->x;
		tjs_int height = param->h;
		tjs_int width = param->w;
                tjs_uint32 value = param->value;

                // don't use no cache version. (for test reason)
#if 0
		if(height * width >= 64*1024/4)
		{
                        while (height--) 
			{
				tjs_uint32 * p = (tjs_uint32*)sc + x;
				TVPFillARGB_NC(p, width, value);
				sc += pitch;
			}
		}
		else
#endif
		{
                        while (height--)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + x;
				TVPFillARGB(p, width, value);
				sc += pitch;
			}
		}
	}
	else
	{
		// 8bpp
		tjs_int pitch = param->pitch;
		tjs_uint8 *sc = param->dest + param->y * pitch;
                tjs_int x = param->x;
                tjs_int height = param->h;
                tjs_int width = param->w;
                tjs_uint32 value = param->value;

		while (height--)
		{
			tjs_uint8 * p = (tjs_uint8*)sc + x;
			memset(p, value, width);
			sc += pitch;
		}
	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::FillColor(tTVPRect rect, tjs_uint32 color, tjs_int opa)
{
	// fill rectangle with specified color.
	// this ignores destination alpha (destination alpha will not change)
	// opa is fill opacity if opa is positive value.
	// negative value of opa is not allowed.
	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(opa == 0) return false; // no action

	if(opa < 0) opa = 0;
	if(opa > 255) opa = 255;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        tjs_int h = rect.bottom - rect.top;
        tjs_int w = rect.right - rect.left;

        tjs_int taskNum = GetAdaptiveThreadNum(w * h, opa == 255 ? 115.f : 55.f);
        TVPBeginThreadTask(taskNum);
        PartialFillColorParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1 = h * (i + 1) / taskNum;
          PartialFillColorParam *param = params + i;
          param->self = this;
          param->dest = dest;
          param->pitch = pitch;
          param->x = rect.left;
          param->y = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->color = color;
          param->opa = opa;
          TVPExecThreadTask(&PartialFillColorEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}

void TJS_USERENTRY tTVPBaseBitmap::PartialFillColorEntry(void *v)
{
  const PartialFillColorParam *param = (const PartialFillColorParam *)v;
  param->self->PartialFillColor(param);
}

void tTVPBaseBitmap::PartialFillColor(const PartialFillColorParam *param)
{
  tjs_uint8 *sc = param->dest + param->y * param->pitch;
  tjs_int opa = param->opa;
  tjs_uint32 color = param->color;
  tjs_int left = param->x;
  tjs_int width = param->w;
  tjs_int height = param->h;
  tjs_int pitch = param->pitch;
        
	if(opa == 255)
	{
		// complete opaque fill
		while(height--)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + left;
			TVPFillColor(p, width, color);
			sc += pitch;
		}
	}
	else
	{
		// alpha fill
                while(height--)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + left;
			TVPConstColorAlphaBlend(p, width, color, opa);
			sc += pitch;
		}
	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::BlendColor(tTVPRect rect, tjs_uint32 color, tjs_int opa,
	bool additive)
{
	// fill rectangle with specified color.
	// this considers destination alpha (additive or simple)

	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(opa == 0) return false; // no action
	if(opa < 0) opa = 0;
	if(opa > 255) opa = 255;

        if(opa == 255 && !IsIndependent())
          {
            if(rect.left == 0 && rect.top == 0 &&
               rect.right == (tjs_int)GetWidth() && rect.bottom == (tjs_int)GetHeight())
              {
                // cover overall
                IndependNoCopy(); // indepent with no-copy
              }
          }

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        tjs_int h = rect.bottom - rect.top;
        tjs_int w = rect.right - rect.left;

        tjs_int factor;
        if (opa == 255)
          factor = 148;
        else if (! additive)
          factor = 25;
        else
          factor = 147;
        tjs_int taskNum = GetAdaptiveThreadNum(w * h, static_cast<float>(factor) );
        TVPBeginThreadTask(taskNum);
        PartialBlendColorParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1 = h * (i + 1) / taskNum;
          PartialBlendColorParam *param = params + i;
          param->self = this;
          param->dest = dest;
          param->pitch = pitch;
          param->x = rect.left;
          param->y = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->color = color;
          param->opa = opa;
          param->additive = additive;
          TVPExecThreadTask(&PartialBlendColorEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}

void TJS_USERENTRY tTVPBaseBitmap::PartialBlendColorEntry(void *v)
{
  const PartialBlendColorParam *param = (const PartialBlendColorParam *)v;
  param->self->PartialBlendColor(param);
}

void tTVPBaseBitmap::PartialBlendColor(const PartialBlendColorParam *param)
{
  tjs_uint32 color = param->color;
  tjs_int opa = param->opa;
  bool additive = param->additive;

	if(opa == 255)
	{
		// complete opaque fill
		color |= 0xff000000;

		tjs_int pitch = param->pitch;
		tjs_uint8 *sc = param->dest + pitch * param->y;
                tjs_int left = param->x;
		tjs_int width = param->w;
                tjs_int height = param->h;

                while (height--)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + left;
			TVPFillARGB(p, width, color);
			sc += pitch;
		}
	}
	else
	{
		// alpha fill
		tjs_int pitch = param->pitch;
		tjs_uint8 *sc = param->dest + pitch * param->y;
                tjs_int left = param->x;
		tjs_int width = param->w;
                tjs_int height = param->h;

		if(!additive)
		{
                        while(height--)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + left;
				TVPConstColorAlphaBlend_d(p, width, color, opa);
				sc += pitch;
			}
		}
		else
		{
                        while(height--)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + left;
				TVPConstColorAlphaBlend_a(p, width, color, opa);
				sc += pitch;
			}
		}
	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::RemoveConstOpacity(tTVPRect rect, tjs_int level)
{
	// remove constant opacity from bitmap. ( similar to PhotoShop's eraser tool )
	// level is a strength of removing ( 0 thru 255 )
	// this cannot work with additive alpha mode.

	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(level == 0) return false; // no action
	if(level < 0) level = 0;
	if(level > 255) level = 255;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        tjs_int h = rect.bottom - rect.top;
        tjs_int w = rect.right - rect.left;

        tjs_int taskNum = GetAdaptiveThreadNum(w * h, level == 255 ? 83.f : 50.f);
        TVPBeginThreadTask(taskNum);
        PartialRemoveConstOpacityParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1 = h * (i + 1) / taskNum;
          PartialRemoveConstOpacityParam *param = params + i;
          param->self = this;
          param->dest = dest;
          param->pitch = pitch;
          param->x = rect.left;
          param->y = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->level = level;
          TVPExecThreadTask(&PartialRemoveConstOpacityEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}

void TJS_USERENTRY tTVPBaseBitmap::PartialRemoveConstOpacityEntry(void *v)
{
  const PartialRemoveConstOpacityParam *param = (const PartialRemoveConstOpacityParam *)v;
  param->self->PartialRemoveConstOpacity(param);
}

void tTVPBaseBitmap::PartialRemoveConstOpacity(const PartialRemoveConstOpacityParam *param)
{
  tjs_int pitch = param->pitch;
  tjs_uint8 *sc = param->dest + pitch * param->y;
  tjs_int left = param->x;
  tjs_int width = param->w;
  tjs_int height = param->h;
  tjs_int level = param->level;

	if(level == 255)
	{
		// completely remove opacity
                while(height--)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + left;
			TVPFillMask(p, width, 0);
			sc += pitch;
		}
	}
	else
	{
                while(height--)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + left;
			TVPRemoveConstOpacity(p, width, level);
			sc += pitch;
		}

	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::FillMask(tTVPRect rect, tjs_int value)
{
	// fill mask with specified value.
	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        tjs_int h = rect.bottom - rect.top;
        tjs_int w = rect.right - rect.left;

        tjs_int taskNum = GetAdaptiveThreadNum(w * h, 84);
        TVPBeginThreadTask(taskNum);
        PartialFillMaskParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1 = h * (i + 1) / taskNum;
          PartialFillMaskParam *param = params + i;
          param->self = this;
          param->dest = dest;
          param->pitch = pitch;
          param->x = rect.left;
          param->y = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->value = value;
          TVPExecThreadTask(&PartialFillMaskEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}

void TJS_USERENTRY tTVPBaseBitmap::PartialFillMaskEntry(void *v)
{
  const PartialFillMaskParam *param = (const PartialFillMaskParam *)v;
  param->self->PartialFillMask(param);
}

void tTVPBaseBitmap::PartialFillMask(const PartialFillMaskParam *param)
{
  tjs_int pitch = param->pitch;
  tjs_uint8 *sc = param->dest + pitch * param->y;
  tjs_int left = param->x;
  tjs_int width = param->w;
  tjs_int height = param->h;
  tjs_int value = param->value;

        while(height--)
	{
		tjs_uint32 * p = (tjs_uint32*)sc + left;
		TVPFillMask(p, width, value);
		sc += pitch;
	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::CopyRect(tjs_int x, tjs_int y, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tjs_int plane)
{
	// copy bitmap rectangle.
	// TVP_BB_COPY_MAIN in "plane" : main image is copied
	// TVP_BB_COPY_MASK in "plane" : mask image is copied
	// "plane" is ignored if the bitmap is 8bpp
	// the source rectangle is ( "refrect" ) and the destination upper-left corner
	// is (x, y).
	if(!Is32BPP()) plane = (TVP_BB_COPY_MASK|TVP_BB_COPY_MAIN);
	if(x == 0 && y == 0 && refrect.left == 0 && refrect.top == 0 &&
		refrect.right == (tjs_int)ref->GetWidth() &&
		refrect.bottom == (tjs_int)ref->GetHeight() &&
		(tjs_int)GetWidth() == refrect.right &&
		(tjs_int)GetHeight() == refrect.bottom &&
		plane == (TVP_BB_COPY_MASK|TVP_BB_COPY_MAIN) &&
		(bool)!Is32BPP() == (bool)!ref->Is32BPP())
	{
		// entire area of both bitmaps
		AssignBitmap(*ref);
		return true;
	}

	// bound check
	tjs_int bmpw, bmph;

	bmpw = ref->GetWidth();
	bmph = ref->GetHeight();

	if(refrect.left < 0)
		x -= refrect.left, refrect.left = 0;
	if(refrect.right > bmpw)
		refrect.right = bmpw;

	if(refrect.left >= refrect.right) return false;

	if(refrect.top < 0)
		y -= refrect.top, refrect.top = 0;
	if(refrect.bottom > bmph)
		refrect.bottom = bmph;

	if(refrect.top >= refrect.bottom) return false;

	bmpw = GetWidth();
	bmph = GetHeight();

	tTVPRect rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + refrect.get_width();
	rect.bottom = rect.top + refrect.get_height();

	if(rect.left < 0)
	{
		refrect.left += -rect.left;
		rect.left = 0;
	}

	if(rect.right > bmpw)
	{
		refrect.right -= (rect.right - bmpw);
		rect.right = bmpw;
	}

	if(refrect.left >= refrect.right) return false; // not drawable

	if(rect.top < 0)
	{
		refrect.top += -rect.top;
		rect.top = 0;
	}

	if(rect.bottom > bmph)
	{
		refrect.bottom -= (rect.bottom - bmph);
		rect.bottom = bmph;
	}

	if(refrect.top >= refrect.bottom) return false; // not drawable


        // transfer
        tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        const tjs_uint8 *src = (const tjs_uint8*)ref->GetScanLine(0);
        tjs_int dpitch = GetPitchBytes();
        tjs_int spitch = ref->GetPitchBytes();
	tjs_int w = refrect.get_width();
	tjs_int h = refrect.get_height();
	tjs_int pixelsize = (Is32BPP()?sizeof(tjs_uint32):sizeof(tjs_uint8));
        bool backwardCopy = (ref == this && rect.top > refrect.top);

        tjs_int taskNum = (ref == this) ? 1 : GetAdaptiveThreadNum(w * h, 66);
        TVPBeginThreadTask(taskNum);
        PartialCopyRectParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1=  h * (i + 1) / taskNum;
          PartialCopyRectParam *param = params + i;
          param->self = this;
          param->pixelsize = pixelsize;
          param->dest = dest;
          param->dpitch = dpitch;
          param->dx = rect.left;
          param->dy = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->src = reinterpret_cast<const tjs_int8*>(src);
          param->spitch = spitch;
          param->sx = refrect.left;
          param->sy = refrect.top + y0;
          param->plane = plane;
          param->backwardCopy = backwardCopy;
          TVPExecThreadTask(&PartialCopyRectEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}

void TJS_USERENTRY tTVPBaseBitmap::PartialCopyRectEntry(void *v)
{
  const PartialCopyRectParam *param = (const PartialCopyRectParam *)v;
  param->self->PartialCopyRect(param);
}

void tTVPBaseBitmap::PartialCopyRect(const PartialCopyRectParam *param)
{
	// transfer
	tjs_int pixelsize = (Is32BPP()?sizeof(tjs_uint32):sizeof(tjs_uint8));
	tjs_int dpitch = param->dpitch;
	tjs_int spitch = param->spitch;
	tjs_int w = param->w;
	tjs_int wbytes = param->w * pixelsize;
	tjs_int h = param->h;
        tjs_int plane = param->plane;
        bool backwardCopy = param->backwardCopy;

	if(backwardCopy)
	{
		// backward copy
#if 0
		tjs_uint8 * dest = (tjs_uint8*)GetScanLineForWrite(rect.bottom-1) +
			rect.left*pixelsize;
		const tjs_uint8 * src = (const tjs_uint8*)ref->GetScanLine(refrect.bottom-1) +
			refrect.left*pixelsize;
#endif
		tjs_uint8 * dest = param->dest + dpitch * (param->dy + param->h - 1) + param->dx * pixelsize;
		const tjs_uint8 * src = reinterpret_cast<const tjs_uint8*>(param->src + spitch * (param->sy + param->h - 1) + param->sx * pixelsize);

		switch(plane)
		{
		case TVP_BB_COPY_MAIN:
			while(h--)
			{
				TVPCopyColor((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest -= dpitch;
				src -= spitch;
			}
			break;
		case TVP_BB_COPY_MASK:
			while(h--)
			{
				TVPCopyMask((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest -= dpitch;
				src -= spitch;
			}
			break;
		case TVP_BB_COPY_MAIN|TVP_BB_COPY_MASK:
			while(h--)
			{
				memmove(dest, src, wbytes);
				dest -= dpitch;
				src -= spitch;
			}
			break;
		}
	}
	else
	{
		// forward copy
#if 0
		tjs_uint8 * dest = (tjs_uint8*)GetScanLineForWrite(rect.top) +
			rect.left*pixelsize;
		const tjs_uint8 * src = (const tjs_uint8*)ref->GetScanLine(refrect.top) +
			refrect.left*pixelsize;
#endif
		tjs_uint8 * dest = param->dest + dpitch * (param->dy) + param->dx * pixelsize;
		const tjs_uint8 * src = reinterpret_cast<const tjs_uint8*>(param->src + spitch * (param->sy) + param->sx * pixelsize);

		switch(plane)
		{
		case TVP_BB_COPY_MAIN:
			while(h--)
			{
				TVPCopyColor((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest += dpitch;
				src += spitch;
			}
			break;
		case TVP_BB_COPY_MASK:
			while(h--)
			{
				TVPCopyMask((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest += dpitch;
				src += spitch;
			}
			break;
		case TVP_BB_COPY_MAIN|TVP_BB_COPY_MASK:
			while(h--)
			{
				memmove(dest, src, wbytes);
				dest += dpitch;
				src += spitch;
			}
			break;
		}
	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::Blt(tjs_int x, tjs_int y, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tTVPBBBltMethod method, tjs_int opa, bool hda)
{
	// blt src bitmap with various methods.

	// hda option ( hold destination alpha ) holds distination alpha,
	// but will select more complex function ( and takes more time ) for it ( if
	// can do )

	// this function does not matter whether source and destination bitmap is
	// overlapped.

	if(opa == 255 && method == bmCopy && !hda)
	{
		return CopyRect(x, y, ref, refrect);
	}

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(opa == 0) return false; // opacity==0 has no action

	// bound check
	tjs_int bmpw, bmph;

	bmpw = ref->GetWidth();
	bmph = ref->GetHeight();

	if(refrect.left < 0)
		x -= refrect.left, refrect.left = 0;
	if(refrect.right > bmpw)
		refrect.right = bmpw;

	if(refrect.left >= refrect.right) return false;

	if(refrect.top < 0)
		y -= refrect.top, refrect.top = 0;
	if(refrect.bottom > bmph)
		refrect.bottom = bmph;

	if(refrect.top >= refrect.bottom) return false;

	bmpw = GetWidth();
	bmph = GetHeight();


	tTVPRect rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + refrect.get_width();
	rect.bottom = rect.top + refrect.get_height();

	if(rect.left < 0)
	{
		refrect.left += -rect.left;
		rect.left = 0;
	}

	if(rect.right > bmpw)
	{
		refrect.right -= (rect.right - bmpw);
		rect.right = bmpw;
	}

	if(refrect.left >= refrect.right) return false; // not drawable

	if(rect.top < 0)
	{
		refrect.top += -rect.top;
		rect.top = 0;
	}

	if(rect.bottom > bmph)
	{
		refrect.bottom -= (rect.bottom - bmph);
		rect.bottom = bmph;
	}

	if(refrect.top >= refrect.bottom) return false; // not drawable

        tjs_uint8 *dest = (tjs_uint8*)GetScanLineForWrite(0);
        const tjs_uint8 *src = (const tjs_uint8*)ref->GetScanLine(0);
        tjs_int dpitch = GetPitchBytes();
        tjs_int spitch = ref->GetPitchBytes();
	tjs_int w = refrect.get_width();
	tjs_int h = refrect.get_height();

        tjs_int taskNum = GetAdaptiveThreadNum(w * h, sBmFactor[method]);
        TVPBeginThreadTask(taskNum);
        PartialBltParam params[TVPMaxThreadNum];
        for (tjs_int i = 0; i < taskNum; i++) {
          tjs_int y0, y1;
          y0 = h * i / taskNum;
          y1=  h * (i + 1) / taskNum;
          PartialBltParam *param = params + i;
          param->self = this;
          param->dest = dest;
          param->dpitch = dpitch;
          param->dx = rect.left;
          param->dy = rect.top + y0;
          param->w = w;
          param->h = y1 - y0;
          param->src = reinterpret_cast<const tjs_int8*>(src);
          param->spitch = spitch;
          param->sx = refrect.left;
          param->sy = refrect.top + y0;
          param->method = method;
          param->opa = opa;
          param->hda = hda;
          TVPExecThreadTask(&PartialBltEntry, TVP_THREAD_PARAM(param));
        }
        TVPEndThreadTask();

        return true;
}


void TJS_USERENTRY tTVPBaseBitmap::PartialBltEntry(void *v)
{
  const PartialBltParam *param = (const PartialBltParam *)v;
  param->self->PartialBlt(param);
}

void tTVPBaseBitmap::PartialBlt(const PartialBltParam *param)
{
  tjs_uint8 *dest;
  const tjs_uint8 *src;
  tjs_int dpitch, dx, dy, w, h, spitch, sx, sy;
  tTVPBBBltMethod method;
  tjs_int opa;
  bool hda;

  dest = param->dest;
  dpitch = param->dpitch;
  dx = param->dx;
  dy = param->dy;
  w = param->w;
  h = param->h;
  src = reinterpret_cast<const tjs_uint8*>(param->src);
  spitch = param->spitch;
  sx = param->sx;
  sy = param->sy;
  method = param->method;
  opa = param->opa;
  hda = param->hda;

  dest += dy * dpitch + dx * sizeof(tjs_uint32);
  src  += sy * spitch + sx * sizeof(tjs_uint32);

#define TVP_BLEND_4(basename) /* blend for 4 types (normal, opacity, HDA, HDA opacity) */ \
	if(opa == 255)                                                            \
	{                                                                         \
		if(!hda)                                                              \
		{                                                                     \
			while(h--)                                                        \
				basename((tjs_uint32*)dest, (tjs_uint32*)src, w),             \
				dest+=dpitch, src+=spitch;                                    \
                                                                              \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			while(h--)                                                        \
				basename##_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),       \
				dest+=dpitch, src+=spitch;                                    \
		}                                                                     \
	}                                                                         \
	else                                                                      \
	{                                                                         \
		if(!hda)                                                              \
		{                                                                     \
			while(h--)                                                        \
				basename##_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),    \
				dest+=dpitch, src+=spitch;                                    \
		}                                                                     \
		else                                                                  \
		{                                                                     \
			while(h--)                                                        \
				basename##_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),\
				dest+=dpitch, src+=spitch;                                    \
		}                                                                     \
	}


	switch(method)
	{
	case bmCopy:
		// constant ratio alpha blendng
		if(opa == 255 && hda)
		{
			while(h--)
				TVPCopyColor((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
		}
		else if(!hda)
		{
			while(h--)
				TVPConstAlphaBlend((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
		}
		else
		{
			while(h--)
				TVPConstAlphaBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
		}
		break;

	case bmCopyOnAlpha:
		// constant ratio alpha blending (assuming source is opaque)
		// with consideration of destination alpha
		if(opa == 255)
			while(h--)
				TVPCopyOpaqueImage((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		else
			while(h--)
				TVPConstAlphaBlend_d((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		break;


	case bmAlpha:
		// alpha blending, ignoring destination alpha
		TVP_BLEND_4(TVPAlphaBlend);
		break;

	case bmAlphaOnAlpha:
		// alpha blending, with consideration of destination alpha
		if(opa == 255)
			while(h--)
				TVPAlphaBlend_d((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		else
			while(h--)
				TVPAlphaBlend_do((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		break;

	case bmAdd:
		// additive blending ( this does not consider distination alpha )
		TVP_BLEND_4(TVPAddBlend);
		break;

	case bmSub:
		// subtractive blending ( this does not consider distination alpha )
		TVP_BLEND_4(TVPSubBlend);
		break;

	case bmMul:
		// multiplicative blending ( this does not consider distination alpha )
		TVP_BLEND_4(TVPMulBlend);
		break;


	case bmDodge:
		// color dodge mode ( this does not consider distination alpha )
		TVP_BLEND_4(TVPColorDodgeBlend);
		break;


	case bmDarken:
		// darken mode ( this does not consider distination alpha )
		TVP_BLEND_4(TVPDarkenBlend);
		break;


	case bmLighten:
		// lighten mode ( this does not consider distination alpha )
		TVP_BLEND_4(TVPLightenBlend);
		break;


	case bmScreen:
		// screen multiplicative mode ( this does not consider distination alpha )
		TVP_BLEND_4(TVPScreenBlend);
		break;


	case bmAddAlpha:
		// Additive Alpha
		TVP_BLEND_4(TVPAdditiveAlphaBlend);
		break;


	case bmAddAlphaOnAddAlpha:
		// Additive Alpha on Additive Alpha
		if(opa == 255)
		{
			while(h--)
				TVPAdditiveAlphaBlend_a((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		}
		else
		{
			while(h--)
				TVPAdditiveAlphaBlend_ao((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		}
		break;


	case bmAddAlphaOnAlpha:
		// additive alpha on simple alpha
		// Not yet implemented
		break;

	case bmAlphaOnAddAlpha:
		// simple alpha on additive alpha
		if(opa == 255)
		{
			while(h--)
				TVPAlphaBlend_a((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		}
		else
		{
			while(h--)
				TVPAlphaBlend_ao((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		}
		break;

	case bmCopyOnAddAlpha:
		// constant ratio alpha blending (assuming source is opaque)
		// with consideration of destination additive alpha
		if(opa == 255)
			while(h--)
				TVPCopyOpaqueImage((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		else
			while(h--)
				TVPConstAlphaBlend_a((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		break;


	case bmPsNormal:
		// Photoshop compatible normal blend
		// (may take the same effect as bmAlpha)
		TVP_BLEND_4(TVPPsAlphaBlend);
		break;

	case bmPsAdditive:
		// Photoshop compatible additive blend
		TVP_BLEND_4(TVPPsAddBlend);
		break;

	case bmPsSubtractive:
		// Photoshop compatible subtractive blend
		TVP_BLEND_4(TVPPsSubBlend);
		break;

	case bmPsMultiplicative:
		// Photoshop compatible multiplicative blend
		TVP_BLEND_4(TVPPsMulBlend);
		break;

	case bmPsScreen:
		// Photoshop compatible screen blend
		TVP_BLEND_4(TVPPsScreenBlend);
		break;

	case bmPsOverlay:
		// Photoshop compatible overlay blend
		TVP_BLEND_4(TVPPsOverlayBlend);
		break;

	case bmPsHardLight:
		// Photoshop compatible hard light blend
		TVP_BLEND_4(TVPPsHardLightBlend);
		break;

	case bmPsSoftLight:
		// Photoshop compatible soft light blend
		TVP_BLEND_4(TVPPsSoftLightBlend);
		break;

	case bmPsColorDodge:
		// Photoshop compatible color dodge blend
		TVP_BLEND_4(TVPPsColorDodgeBlend);
		break;

	case bmPsColorDodge5:
		// Photoshop 5.x compatible color dodge blend
		TVP_BLEND_4(TVPPsColorDodge5Blend);
		break;

	case bmPsColorBurn:
		// Photoshop compatible color burn blend
		TVP_BLEND_4(TVPPsColorBurnBlend);
		break;

	case bmPsLighten:
		// Photoshop compatible compare (lighten) blend
		TVP_BLEND_4(TVPPsLightenBlend);
		break;

	case bmPsDarken:
		// Photoshop compatible compare (darken) blend
		TVP_BLEND_4(TVPPsDarkenBlend);
		break;

	case bmPsDifference:
		// Photoshop compatible difference blend
		TVP_BLEND_4(TVPPsDiffBlend);
		break;

	case bmPsDifference5:
		// Photoshop 5.x compatible difference blend
		TVP_BLEND_4(TVPPsDiff5Blend);
		break;

	case bmPsExclusion:
		// Photoshop compatible exclusion blend
		TVP_BLEND_4(TVPPsExclusionBlend);
		break;


	default:
				 ;
	}
}

//---------------------------------------------------------------------------
// some blur operation template functions to select algorithm by base integer type
template <typename tARGB, typename base_int_type>
void TVPAddSubVertSum(base_int_type *dest, const tjs_uint32 *addline,
	const tjs_uint32 *subline, tjs_int len)
{
}
template <>
void TVPAddSubVertSum<tTVPARGB<tjs_uint16>, tjs_uint16 >(tjs_uint16 *dest,
	const tjs_uint32 *addline, const tjs_uint32 *subline, tjs_int len)
{
	TVPAddSubVertSum16(dest, addline, subline, len);
}
template <>
void TVPAddSubVertSum<tTVPARGB_AA<tjs_uint16>, tjs_uint16 >(tjs_uint16 *dest,
	const tjs_uint32 *addline, const tjs_uint32 *subline, tjs_int len)
{
	TVPAddSubVertSum16_d(dest, addline, subline, len);
}
template <>
void TVPAddSubVertSum<tTVPARGB<tjs_uint32>, tjs_uint32 >(tjs_uint32 *dest,
	const tjs_uint32 *addline, const tjs_uint32 *subline, tjs_int len)
{
	TVPAddSubVertSum32(dest, addline, subline, len);
}
template <>
void TVPAddSubVertSum<tTVPARGB_AA<tjs_uint32>, tjs_uint32 >(tjs_uint32 *dest,
	const tjs_uint32 *addline, const tjs_uint32 *subline, tjs_int len)
{
	TVPAddSubVertSum32_d(dest, addline, subline, len);
}

template <typename tARGB, typename base_int_type>
void TVPDoBoxBlurAvg(tjs_uint32 *dest, base_int_type *sum,
	const base_int_type * add, const base_int_type * sub,
		tjs_int n, tjs_int len)
{
}
template <>
void TVPDoBoxBlurAvg<tTVPARGB<tjs_uint16>, tjs_uint16 >(tjs_uint32 *dest, tjs_uint16 *sum,
	const tjs_uint16 * add, const tjs_uint16 * sub, tjs_int n, tjs_int len)
{
	TVPDoBoxBlurAvg16(dest, sum, add, sub, n, len);
}
template <>
void TVPDoBoxBlurAvg<tTVPARGB_AA<tjs_uint16>, tjs_uint16  >(tjs_uint32 *dest, tjs_uint16 *sum,
	const tjs_uint16 * add, const tjs_uint16 * sub, tjs_int n, tjs_int len)
{
	TVPDoBoxBlurAvg16_d(dest, sum, add, sub, n, len);
}
template <>
void TVPDoBoxBlurAvg<tTVPARGB<tjs_uint32>, tjs_uint32  >(tjs_uint32 *dest, tjs_uint32 *sum,
	const tjs_uint32 * add, const tjs_uint32 * sub, tjs_int n, tjs_int len)
{
	TVPDoBoxBlurAvg32(dest, sum, add, sub, n, len);
}
template <>
void TVPDoBoxBlurAvg<tTVPARGB_AA<tjs_uint32>, tjs_uint32  >(tjs_uint32 *dest, tjs_uint32 *sum,
	const tjs_uint32 * add, const tjs_uint32 * sub, tjs_int n, tjs_int len)
{
	TVPDoBoxBlurAvg32_d(dest, sum, add, sub, n, len);
}


//---------------------------------------------------------------------------


template <typename tARGB>
void tTVPBaseBitmap::DoBoxBlurLoop(const tTVPRect &rect, const tTVPRect & area)
{
	// Box-Blur template function used by tTVPBaseBitmap::DoBoxBlur family.
	// Based on contributed blur code by yun, say thanks to him.

	typedef typename tARGB::base_int_type base_type;

	tjs_int width = GetWidth();
	tjs_int height = GetHeight();

	tjs_int dest_buf_size = area.top <= 0 ? (1-area.top) : 0;

	tjs_int vert_sum_left_limit = rect.left + area.left;
	if(vert_sum_left_limit < 0) vert_sum_left_limit = 0;
	tjs_int vert_sum_right_limit = (rect.right-1) + area.right;
	if(vert_sum_right_limit >= width) vert_sum_right_limit = width - 1;


	tARGB * vert_sum = NULL; // vertical sum of the pixel
	tjs_uint32 * * dest_buf = NULL; // destination pixel temporary buffer

	tjs_int vert_sum_count;

	try
	{
		// allocate buffers
		vert_sum = (tARGB*)TJSAlignedAlloc(sizeof(tARGB) *
			(vert_sum_right_limit - vert_sum_left_limit + 1 + 1), 4); // use 128bit aligned allocation

		if(dest_buf_size)
		{
			dest_buf = new tjs_uint32 * [dest_buf_size];
			for(tjs_int i = 0; i < dest_buf_size; i++)
				dest_buf[i] = new tjs_uint32[rect.right - rect.left];
		}

		// initialize vert_sum
		{
			for(tjs_int i = vert_sum_right_limit - vert_sum_left_limit + 1 -1; i>=0; i--)
				vert_sum[i].Zero();

			tjs_int v_init_start = rect.top + area.top;
			if(v_init_start < 0) v_init_start = 0;
			tjs_int v_init_end = rect.top + area.bottom;
			if(v_init_end >= height) v_init_end = height - 1;
			vert_sum_count = v_init_end - v_init_start + 1;
			for(tjs_int y = v_init_start; y <= v_init_end; y++)
			{
				const tjs_uint32 * add_line;
				add_line = (const tjs_uint32*)GetScanLine(y);
				tARGB * vs = vert_sum;
				for(int x = vert_sum_left_limit; x <= vert_sum_right_limit; x++)
					*(vs++) += add_line[x];
			}
		}

		// prepare variables to be used in following loop
		tjs_int h_init_start = rect.left + area.left; // this always be the same value as vert_sum_left_limit
		if(h_init_start < 0) h_init_start = 0;
		tjs_int h_init_end = rect.left + area.right;
		if(h_init_end >= width) h_init_end = width - 1;

		tjs_int left_frac_len =
			rect.left + area.left < 0 ? -(rect.left + area.left) : 0;
		tjs_int right_frac_len =
			rect.right + area.right >= width ? rect.right + area.right - width + 1: 0;
		tjs_int center_len = rect.right - rect.left - left_frac_len - right_frac_len;

		if(center_len < 0)
		{
			left_frac_len = rect.right - rect.left;
			right_frac_len = 0;
			center_len = 0;
		}
		tjs_int left_frac_lim = rect.left + left_frac_len;
		tjs_int center_lim = rect.left + left_frac_len + center_len;

		// for each line
		tjs_int dest_buf_free = dest_buf_size;
		tjs_int dest_buf_wp = 0;

		for(tjs_int y = rect.top; y < rect.bottom; y++)
		{
			// rotate dest_buf
			if(dest_buf_free == 0)
			{
				// dest_buf is full;
				// write last dest_buf back to the bitmap
				memcpy(
					rect.left + ((tjs_uint32*)GetScanLineForWrite(y - dest_buf_size)),
					dest_buf[dest_buf_wp],
					(rect.right - rect.left) * sizeof(tjs_uint32));
			}
			else
			{
				dest_buf_free --;
			}

			// build initial sum
			tARGB sum;
			sum.Zero();
			tjs_int horz_sum_count = h_init_end - h_init_start + 1;

			for(tjs_int x = h_init_start; x <= h_init_end; x++)
				sum += vert_sum[x - vert_sum_left_limit];

			// process a line
			tjs_uint32 *dp = dest_buf[dest_buf_wp];
			tjs_int x = rect.left;

			//- do left fraction part
			for(; x < left_frac_lim; x++)
			{
				tARGB tmp = sum;
				tmp.average(horz_sum_count * vert_sum_count);

				*(dp++) = tmp;

				// update sum
				if(x + area.left >= 0)
				{
					sum -= vert_sum[x + area.left - vert_sum_left_limit];
					horz_sum_count --;
				}
				if(x + area.right + 1 < width)
				{
					sum += vert_sum[x + area.right + 1 - vert_sum_left_limit];
					horz_sum_count ++;
				}
			}

			//- do center part
			if(center_len > 0)
			{
				// uses function in tvpgl
				TVPDoBoxBlurAvg<tARGB>(dp, (base_type*)&sum,
					(const base_type *)(vert_sum + x + area.right + 1 - vert_sum_left_limit),
					(const base_type *)(vert_sum + x + area.left - vert_sum_left_limit),
					horz_sum_count * vert_sum_count,
					center_len);
				dp += center_len;
			}
			x = center_lim;

			//- do right fraction part
			for(; x < rect.right; x++)
			{
				tARGB tmp = sum;
				tmp.average(horz_sum_count * vert_sum_count);

				*(dp++) = tmp;

				// update sum
				if(x + area.left >= 0)
				{
					sum -= vert_sum[x + area.left - vert_sum_left_limit];
					horz_sum_count --;
				}
				if(x + area.right + 1 < width)
				{
					sum += vert_sum[x + area.right + 1 - vert_sum_left_limit];
					horz_sum_count ++;
				}
			}

			// update vert_sum
			if(y != rect.bottom - 1)
			{
				const tjs_uint32 * sub_line;
				const tjs_uint32 * add_line;
				sub_line =
					y + area.top < 0 ?
						(const tjs_uint32 *)NULL :
						(const tjs_uint32 *)GetScanLine(y + area.top);
				add_line =
					y + area.bottom + 1 >= height ?
						(const tjs_uint32 *)NULL :
						(const tjs_uint32 *)GetScanLine(y + area.bottom + 1);

				if(sub_line && add_line)
				{
					// both sub_line and add_line are available
					// uses function in tvpgl
					TVPAddSubVertSum<tARGB>((base_type*)vert_sum,
						add_line + vert_sum_left_limit,
						sub_line + vert_sum_left_limit,
						vert_sum_right_limit - vert_sum_left_limit + 1);

				}
				else if(sub_line)
				{
					// only sub_line is available
					tARGB * vs = vert_sum;
					for(int x = vert_sum_left_limit; x <= vert_sum_right_limit; x++)
						*vs -= sub_line[x], vs ++;
					vert_sum_count --;
				}
				else if(add_line)
				{
					// only add_line is available
					tARGB * vs = vert_sum;
					for(int x = vert_sum_left_limit; x <= vert_sum_right_limit; x++)
						*vs += add_line[x], vs ++;
					vert_sum_count ++;
				}
			}

			// step dest_buf_wp
			dest_buf_wp++;
			if(dest_buf_wp >= dest_buf_size) dest_buf_wp = 0;
		}

		// write remaining dest_buf back to the bitmap
		while(dest_buf_free < dest_buf_size)
		{
			memcpy(
				rect.left +
				(tjs_uint32*)(GetScanLineForWrite(rect.bottom - (dest_buf_size - dest_buf_free))),
				dest_buf[dest_buf_wp], (rect.right - rect.left) * sizeof(tjs_uint32));

			dest_buf_wp++;
			if(dest_buf_wp >= dest_buf_size) dest_buf_wp = 0;
			dest_buf_free++;
		}
	}
	catch(...)
	{
		// exception caught
		if(vert_sum) TJSAlignedDealloc(vert_sum);
		if(dest_buf_size)
		{
			if(dest_buf)
			{
				for(tjs_int i = 0 ; i < dest_buf_size; i++)
					if(dest_buf[i]) delete [] dest_buf[i];
				delete [] dest_buf;
			}
		}
		throw;
	}

	// free buffeers
	TJSAlignedDealloc(vert_sum);
	if(dest_buf_size)
	{
		for(tjs_int i = 0 ; i < dest_buf_size; i++) delete [] dest_buf[i];
		delete [] dest_buf;
	}
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::InternalDoBoxBlur(tTVPRect rect, tTVPRect area, bool hasalpha)
{
	BOUND_CHECK(false);

	if(area.right < area.left)
		std::swap(area.right, area.left);
	if(area.bottom < area.top)
		std::swap(area.bottom, area.top);

	if(area.left == 0 && area.right == 0 &&
		area.top == 0 && area.bottom == 0) return false; // no conversion occurs

	if(area.left > 0 || area.right < 0 || area.top > 0 || area.bottom < 0)
		TVPThrowExceptionMessage(TVPBoxBlurAreaMustContainCenterPixel);


	tjs_uint64 area_size = (tjs_uint64)
		(area.right - area.left + 1) * (area.bottom - area.top + 1);
	if(area_size < 256)
	{
		if(!hasalpha)
			DoBoxBlurLoop<tTVPARGB<tjs_uint16> >(rect, area);
		else
			DoBoxBlurLoop<tTVPARGB_AA<tjs_uint16> >(rect, area);
	}
	else if(area_size < (1L<<24))
	{
		if(!hasalpha)
			DoBoxBlurLoop<tTVPARGB<tjs_uint32> >(rect, area);
		else
			DoBoxBlurLoop<tTVPARGB_AA<tjs_uint32> >(rect, area);
	}
	else
		TVPThrowExceptionMessage(TVPBoxBlurAreaMustBeSmallerThan16Million);

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::DoBoxBlur(const tTVPRect & rect, const tTVPRect & area)
{
	// Blur the bitmap with box-blur algorithm.
	// 'rect' is a rectangle to blur.
	// 'area' is an area which destination pixel refers.
	// right and bottom of 'area' *does contain* pixels in the boundary.
	// eg. area:(-1,-1,1,1)  : Blur is to be performed using average of 3x3
	//                          pixels around the destination pixel.
	//     area:(-10,0,10,0) : Blur is to be performed using average of 21x1
	//                          pixels around the destination pixel. This results
	//                          horizontal blur.

	return InternalDoBoxBlur(rect, area, false);
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::DoBoxBlurForAlpha(const tTVPRect & rect, const tTVPRect &area)
{
	return InternalDoBoxBlur(rect, area, true);
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::UDFlip(const tTVPRect &rect)
{
	// up-down flip for given rectangle

	if(rect.left < 0 || rect.top < 0 || rect.right > (tjs_int)GetWidth() ||
		rect.bottom > (tjs_int)GetHeight())
				TVPThrowExceptionMessage(TVPSrcRectOutOfBitmap);

	tjs_int h = (rect.bottom - rect.top) /2;
	tjs_int w = rect.right - rect.left;
	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * l1 = (tjs_uint8*)GetScanLineForWrite(rect.top);
	tjs_uint8 * l2 = (tjs_uint8*)GetScanLineForWrite(rect.bottom - 1);


	if(Is32BPP())
	{
		// 32bpp
		l1 += rect.left * sizeof(tjs_uint32);
		l2 += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPSwapLine32((tjs_uint32*)l1, (tjs_uint32*)l2, w);
			l1 += pitch;
			l2 -= pitch;
		}
	}
	else
	{
		// 8bpp
		l1 += rect.left;
		l2 += rect.left;
		while(h--)
		{
			TVPSwapLine8(l1, l2, w);
			l1 += pitch;
			l2 -= pitch;
		}
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::LRFlip(const tTVPRect &rect)
{
	// left-right flip
	if(rect.left < 0 || rect.top < 0 || rect.right > (tjs_int)GetWidth() ||
		rect.bottom > (tjs_int)GetHeight())
				TVPThrowExceptionMessage(TVPSrcRectOutOfBitmap);

	tjs_int h = rect.bottom - rect.top;
	tjs_int w = rect.right - rect.left;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);

	if(Is32BPP())
	{
		// 32bpp
		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPReverse32((tjs_uint32*)line, w);
			line += pitch;
		}
	}
	else
	{
		// 8bpp
		line += rect.left;
		while(h--)
		{
			TVPReverse8(line, w);
			line += pitch;
		}
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::DoGrayScale(tTVPRect rect)
{
	if(!Is32BPP()) return;  // 8bpp is always grayscaled bitmap

	BOUND_CHECK(RET_VOID);

	tjs_int h = rect.bottom - rect.top;
	tjs_int w = rect.right - rect.left;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);


	line += rect.left * sizeof(tjs_uint32);
	while(h--)
	{
		TVPDoGrayScale((tjs_uint32*)line, w);
		line += pitch;
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::AdjustGamma(tTVPRect rect, const tTVPGLGammaAdjustData & data)
{
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	BOUND_CHECK(RET_VOID);

	if(!memcmp(&data, &TVPIntactGammaAdjustData, sizeof(tTVPGLGammaAdjustData)))
		return;

	tTVPGLGammaAdjustTempData temp;
	TVPInitGammaAdjustTempData(&temp, &data);

	try
	{
		tjs_int h = rect.bottom - rect.top;
		tjs_int w = rect.right - rect.left;

		tjs_int pitch = GetPitchBytes();
		tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);


		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPAdjustGamma((tjs_uint32*)line, w, &temp);
			line += pitch;
		}

	}
	catch(...)
	{
		TVPUninitGammaAdjustTempData(&temp);
		throw;
	}

	TVPUninitGammaAdjustTempData(&temp);
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::AdjustGammaForAdditiveAlpha(tTVPRect rect, const tTVPGLGammaAdjustData & data)
{
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	BOUND_CHECK(RET_VOID);

	if(!memcmp(&data, &TVPIntactGammaAdjustData, sizeof(tTVPGLGammaAdjustData)))
		return;

	tTVPGLGammaAdjustTempData temp;
	TVPInitGammaAdjustTempData(&temp, &data);

	try
	{
		tjs_int h = rect.bottom - rect.top;
		tjs_int w = rect.right - rect.left;

		tjs_int pitch = GetPitchBytes();
		tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);


		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPAdjustGamma_a((tjs_uint32*)line, w, &temp);
			line += pitch;
		}

	}
	catch(...)
	{
		TVPUninitGammaAdjustTempData(&temp);
		throw;
	}

	TVPUninitGammaAdjustTempData(&temp);
}
//---------------------------------------------------------------------------



