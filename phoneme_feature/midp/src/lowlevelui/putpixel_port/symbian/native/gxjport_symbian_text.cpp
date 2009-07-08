/*
*/
#include <kni.h>
#include <midp_logging.h>

extern "C" {
#include "gxj_intern_graphics.h"
#include "gxj_intern_putpixel.h"
#include "gxj_intern_font_bitmap.h"
}
#include <gxutl_graphics.h>
#include <os_symbian.hpp>

//#define __DEBUGAPI__

#ifdef __DEBUGAPI__
#include <pcsl_print.h>
#include <stdio.h>
#define DEBUGMSG(_AA) {}
#define DEBUGMSG1(_AA,_BB)  \
{ \
char buffer[256]; \
sprintf(buffer,_AA,_BB); \
pcsl_print(buffer); \
}
#define DEBUGMSG3(_AA,_BB,_CC,_DD)  \
{ \
char buffer[256]; \
sprintf(buffer,_AA,_BB,_CC,_DD); \
pcsl_print(buffer); \
}
#else
#define DEBUGMSG(_AA) 
#define DEBUGMSG1(_AA,_BB)
#define DEBUGMSG3(_AA,_BB,_CC,_DD)
#endif


extern "C"
{
void
gx_draw_chars(jint pixel, const jshort *clip, 
	      const java_imagedata *dst, int dotted, 
	      int face, int style, int size,
	      int x, int y, int anchor, const jchar *charArray, int n);
void
gx_get_fontinfo(int face, int style, int size, 
		int *ascent, int *descent, int *leading);

int
gx_get_charswidth(int face, int style, int size, 
		  const jchar *charArray, int n);
		  
}

/**
 * @file
 *
 * platform dependent character drawing
 */
 
void
gx_draw_chars(jint pixel, const jshort *clip, 
	      const java_imagedata *dst, int dotted, 
	      int face, int style, int size,
	      int x, int y, int anchor, const jchar *charArray, int n)
{
	gxj_screen_buffer screen_buffer;
    gxj_screen_buffer *dest = gxj_get_image_screen_buffer_impl(dst, &screen_buffer, NULL);
    dest = (gxj_screen_buffer *)getScreenBuffer(dest);

	DEBUGMSG1("gx_draw_chars 0x%08x\n",dest);
	static_cast<MApplication*>(Dll::Tls())->DrawChars(pixel,clip,dest,dotted,face,style,size,x,y,anchor,charArray,n);
}

/**
 * Obtains the ascent, descent and leading info for the font indicated.
 *
 * @param face The face of the font (Defined in <B>Font.java</B>)
 * @param style The style of the font (Defined in <B>Font.java</B>)
 * @param size The size of the font (Defined in <B>Font.java</B>)
 * @param ascent The font's ascent should be returned here.
 * @param descent The font's descent should be returned here.
 * @param leading The font's leading should be returned here.
 */
void
gx_get_fontinfo(int face, int style, int size, 
		int *ascent, int *descent, int *leading) 
{
	DEBUGMSG3("gx_get_fontinfo face %d style %d size %d\n",face,style,size);
	static_cast<MApplication*>(Dll::Tls())->FontInfo(face,style,size,ascent,descent,leading);
}

/**
 * Gets the advance width for the first n characters in charArray if
 * they were to be drawn in the font indicated by the parameters.
 *
 * <p>
 * <b>Reference:</b>
 * Related Java declaration:
 * <pre>
 *     charWidth(C)I
 * </pre>
 *
 * @param face The font face to be used (Defined in <B>Font.java</B>)
 * @param style The font style to be used (Defined in
 * <B>Font.java</B>)
 * @param size The font size to be used. (Defined in <B>Font.java</B>)
 * @param charArray The string to be measured
 * @param n The number of character to be measured
 * @return The total advance width in pixels (a non-negative value)
 */
int
gx_get_charswidth(int face, int style, int size, 
		  const jchar *charArray, int n) 
{
	return static_cast<MApplication*>(Dll::Tls())->CharsWidth(face,style,size,charArray,n);
}
