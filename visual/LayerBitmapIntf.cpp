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



