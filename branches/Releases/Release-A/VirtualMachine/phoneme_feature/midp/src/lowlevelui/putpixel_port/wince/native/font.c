/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <ddraw.h>

#include <kni.h>
#include <midp_logging.h>
#include <pcsl_memory.h>

#include <gxapi_constants.h>

#include "gxj_intern_graphics.h"
#include "gxj_intern_putpixel.h"
#include "gxj_intern_font_bitmap.h"

#include "gx_font.h"
#include "gxj_putpixel.h"
#include "font_internal.h"

#define MAX_FONTS   24
#define MAX_CHARS  256

/*
STYLE_PLAIN        0
STYLE_BOLD         1
STYLE_ITALIC       2
STYLE_UNDERLINED   4

SIZE_SMALL         8
SIZE_MEDIUM        0
SIZE_LARGE        16

FACE_SYSTEM        0
FACE_MONOSPACE    32
FACE_PROPORTIONAL 64
*/

// #3789: WinCE: support different font sizes and styles
//
// This is a complete reimplementation of the WinCE font subsystem. It
// no longer uses the compiled-in single style and size monospace bitmap 
// font, but different platform fonts and styles. The drawing methods 
// print text on an internal buffer and blit the pixels to their final
// destination.

/*
MS VC++ 9.0 _MSC_VER = 1500
MS VC++ 8.0 _MSC_VER = 1400
MS VC++ 7.1 _MSC_VER = 1310
MS VC++ 7.0 _MSC_VER = 1300
MS VC++ 6.0 _MSC_VER = 1200
MS VC++ 5.0 _MSC_VER = 1100

MS eVC++ 4.0 _MSC_VER = 1200-1202
*/

#if (_MSC_VER < 1300)
#define CHECK_UTF8 0
#define MulDiv(a,b,c) ((a)*(b)/(c))
#else
#define CHECK_UTF8 1
#endif

/* convert color to 16bit color */
#define RGB24TORGB16(x) (((( x ) & 0x00F80000) >> 8) + \
                         ((( x ) & 0x0000FC00) >> 5) + \
             ((( x ) & 0x000000F8) >> 3) )

extern int gfFontInit = 0;

HWND hwnd = NULL;
int init_fonts = 0;

HWND winceapp_get_window_handle();
HFONT wince_fonts[MAX_FONTS];
int wince_font_ascent[MAX_FONTS], wince_font_descent[MAX_FONTS], wince_font_leading[MAX_FONTS];
int wince_font_width[MAX_FONTS][MAX_CHARS];

HFONT wince_create_font(HDC hdc, int face, int style, int size) {
    LOGFONT lf;
    int height;

    height = (size == SIZE_SMALL ? 8 : (size == SIZE_LARGE ? 15 : 11));

    // Clear out the lf structure to use when creating the font.
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfQuality = DRAFT_QUALITY;
    lf.lfHeight = -MulDiv(height, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    lf.lfWeight = ((style & STYLE_BOLD) ? FW_BOLD : FW_NORMAL);
    lf.lfItalic = ((style & STYLE_ITALIC) ? TRUE : FALSE);
    lf.lfUnderline = ((style & STYLE_UNDERLINED) ? TRUE : FALSE);

    return CreateFontIndirect(&lf);
}

int wince_init_fonts() {
    int i, j;
    int face, style, size;
    HDC hdcMain;
    HFONT hFontOld, hFontNew;
    TEXTMETRIC metric;
    SIZE pointsize;
    jchar character;

    gfFontInit = 1;
    if (init_fonts)
        return 0;

    init_fonts = -1;

    memset(wince_fonts, 0, sizeof(wince_fonts));
    memset(wince_font_ascent, 0, sizeof(wince_font_ascent));
    memset(wince_font_descent, 0, sizeof(wince_font_descent));
    memset(wince_font_leading, 0, sizeof(wince_font_leading));

    hwnd = (HWND)winceapp_get_window_handle();
    hdcMain = GetDC(hwnd);
    if (hdcMain != NULL) {
        for (i=0; i<MAX_FONTS; i++) {
            face = 0;
            style = i & (STYLE_PLAIN|STYLE_BOLD|STYLE_ITALIC|STYLE_UNDERLINED);
            size = i & (SIZE_SMALL|SIZE_MEDIUM|SIZE_LARGE);
            // hFontNew = wince_create_font(face, style, size);
            // hFontOld = SelectObject(hdcMain, hFontNew);
            wince_fonts[i] = wince_create_font(hdcMain, face, style, size);
            hFontOld = SelectObject(hdcMain, wince_fonts[i]);
            if (!GetTextMetrics(hdcMain, &metric)) {
                printf("ERROR! 0x%X calling GetTextMetrics()\n", GetLastError());
                return 1;
            }

            wince_font_ascent[i]  = metric.tmAscent;
            wince_font_descent[i] = metric.tmDescent;
            wince_font_leading[i] = metric.tmExternalLeading;

            for (j=0; j<MAX_CHARS; j++) {
                character = j;
                GetTextExtentPoint32W(hdcMain, &character, 1, &pointsize);
                wince_font_width[i][j] = pointsize.cx;
            }

            // SelectObject(hdcMain, hFontOld);
            // DeleteObject(hFontNew);
        }
        ReleaseDC(hwnd, hdcMain);
    } else {
        printf("ERROR: wince_init_fonts(): hdcMain == NULL!\n");
    }
    init_fonts = 1;
    return 0;
}

int wince_delete_fonts() {
    int i;
    for (i=0; i<MAX_FONTS; i++) {
        if (wince_fonts[i]) {
            DeleteObject(wince_fonts[i]);
            wince_fonts[i] = NULL;
        }
    }
    return 0;
}

HFONT wince_get_font(int face, int style, int size) {
    return wince_fonts[style|size];
}

#if CHECK_UTF8
int convert_utf8_to_utf16(const jchar *oldCharArray, int n, jchar *newCharArray) {
    jbyte byte1, byte2, byte3, byte4;
    int i, offs;

    offs = 0;
    for (i=0; i<n; i++) {
        if ((oldCharArray[i] & 0xff00) == 0xff00) {
            byte1 = oldCharArray[i] & 0x00ff;
            if ((byte1 & 0x80) == 0) {
                newCharArray[offs++] = (jchar)byte1;
            } else if ((byte1 & 0xe0) == 0xc0) {
                byte2 = oldCharArray[++i] & 0x00ff;
                newCharArray[offs++] = (jchar)(((byte1 & 0x1f) << 6) | (byte2 & 0x3f));
            } else if ((byte1 & 0xf0) == 0xe0){
                byte2 = oldCharArray[++i] & 0x00ff;
                byte3 = oldCharArray[++i] & 0x00ff;
                newCharArray[offs++] = (jchar)(((byte1 & 0x0f) << 12) | ((byte2 & 0x3f) << 6) | (byte3 & 0x3f));
            } else if ((byte1 & 0xf8) == 0xf0) {
                // More than 16 bits! Unsupported by jchar!
                byte2 = oldCharArray[++i] & 0x00ff;
                byte3 = oldCharArray[++i] & 0x00ff;
                byte4 = oldCharArray[++i] & 0x00ff;
                newCharArray[offs++] = (jchar)0;
            }
        } else {
            newCharArray[offs++] = oldCharArray[i];
        }
    }
    for (i=offs; i<=n; i++) {
        newCharArray[i] = (jchar)0;
    }

    return offs;
}
#endif

void gx_port_draw_chars(jint pixel, const jshort *clip,
                   gxj_screen_buffer *dest,
                   int dotted,
                   int face, int style, int size,
                   int x, int y, int anchor,
                   const jchar *charArray, int n) {
    HDC hdcMain, hdc;
    HFONT hFontOld, hFontNew;
    HBITMAP hbmpText;
    BITMAPINFO bi;
    HGDIOBJ hobj;
    SIZE pointsize;
    int i, j, px, py, deltax;
    UINT *ptPixels;
    jchar *utf16str;

    if (n <= 0) {
        /* nothing to do */
        return;
    }

    if (dest == NULL) {
        printf("ERROR: gx_port_draw_chars(): dest == NULL!\n");
        return;
    }

    utf16str = charArray;
#if CHECK_UTF8
    utf16str = (jchar *)pcsl_mem_malloc((n+4)*sizeof(jchar));
    n = convert_utf8_to_utf16(charArray, n, utf16str);
#endif

    size = (size == SIZE_SMALL ? SIZE_SMALL : (size == SIZE_LARGE ? SIZE_LARGE : SIZE_MEDIUM));

    hdcMain = GetDC(hwnd);
    if (hdcMain != NULL) {
        hdc = CreateCompatibleDC(hdcMain);
        // hFontNew = wince_create_font(face, style, size);
        hFontNew = wince_get_font(face, style, size);
        hFontOld = (HFONT)SelectObject(hdc, hFontNew);

        GetTextExtentPoint32W(hdc, utf16str, n, &pointsize);
        if (style & STYLE_ITALIC) {
            pointsize.cx += (size == SIZE_SMALL ? 3 : (size == SIZE_LARGE ? 6 : 4));
        }

        if ((anchor & RIGHT) != 0) {
            x -= pointsize.cx;
        } else if ((anchor & HCENTER) != 0) {
            x -= (pointsize.cx / 2);
        }

        if ((anchor & BOTTOM) != 0) {
            y -= pointsize.cy;
        } else if ((anchor & VCENTER) != 0) {
            y -= pointsize.cy/2;
        }

        // create a BITMAPINFO with minimal initilisation for the CreateDIBSection
        ZeroMemory(&bi,sizeof(BITMAPINFO));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = pointsize.cx;
        bi.bmiHeader.biHeight = pointsize.cy;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;

        hbmpText = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS, (void **)&ptPixels, NULL, 0);
        if (hbmpText) {
            hobj = SelectObject(hdc, hbmpText);
            PatBlt(hdc, 0, 0, pointsize.cx, pointsize.cy, WHITENESS);
            SetTextColor(hdc, 0x00);
            ExtTextOut(hdc, 0, 0, ETO_CLIPPED|ETO_OPAQUE, NULL, utf16str, n, NULL);

            for (i=pointsize.cy-1; i>=0; i--) {
                py = y+pointsize.cy-1-i;

                if ((py < clip[1]) || (py > clip[3]))
                    continue;

                deltax = i*pointsize.cx;
                for (j=0; j<pointsize.cx; j++) {
                    px = x+j;

                    if ((px < clip[0]) || (px > clip[2]))
                        continue;

                    if (!ptPixels[deltax + j]) {
                        dest->pixelData[py*(dest->width) + px] = pixel;
                    }
                }
            }
            SelectObject(hdc, hobj);
            DeleteObject(hbmpText);
        }

        // SelectObject(hdc, hFontOld);
        // DeleteObject(hFontNew);

        DeleteDC(hdc);
        ReleaseDC(hwnd, hdcMain);
    }
#if CHECK_UTF8
    pcsl_mem_free(utf16str);
#endif
}

void gx_port_get_fontinfo(int face, int style, int size, int *ascent, int *descent, int *leading) {
    size = (size == SIZE_SMALL ? SIZE_SMALL : (size == SIZE_LARGE ? SIZE_LARGE : SIZE_MEDIUM));
    *ascent = wince_font_ascent[style|size];
    *descent = wince_font_descent[style|size];
    *leading = wince_font_leading[style|size];
}

int gx_port_get_charswidth(int face, int style, int size, const jchar *charArray, int n) {
    HDC hdcMain;
    HFONT hFontOld, hFontNew;
    SIZE pointsize;
    int retval;
    int i;
    jchar *utf16str;

    utf16str = charArray;
#if CHECK_UTF8
    utf16str = (jchar *)pcsl_mem_malloc((n+4)*sizeof(jchar));
    n = convert_utf8_to_utf16(charArray, n, utf16str);
#endif

    size = (size == SIZE_SMALL ? SIZE_SMALL : (size == SIZE_LARGE ? SIZE_LARGE : SIZE_MEDIUM));

    retval = 0;
    for (i=0; i<n; i++) {
        if (utf16str[i] < MAX_CHARS) {
            retval += wince_font_width[style|size][utf16str[i]];
        } else {
            retval = 0;
            hdcMain = GetDC(hwnd);
            if (hdcMain != NULL) {
                // hFontNew = wince_create_font(face, style, size);
                hFontNew = wince_get_font(face, style, size);
                hFontOld = SelectObject(hdcMain, hFontNew);
                GetTextExtentPoint32W(hdcMain, utf16str, n, &pointsize);
                retval = pointsize.cx;

                // SelectObject(hdcMain, hFontOld);
                // DeleteObject(hFontNew);
                ReleaseDC(hwnd, hdcMain);
            }
            break;
        }
    }
#if CHECK_UTF8
    pcsl_mem_free(utf16str);
#endif
    return retval;
}

#ifdef __cplusplus
} // extern "C"
#endif
