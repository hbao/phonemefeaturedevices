/**
 * Copyright (c) 2004-2008 Blue Whale Systems Ltd. All Rights Reserved. 
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER 
 *  
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 
 * 2 only, as published by the Free Software Foundation.  
 *  
 * This software is provided "as is," and the copyright holder makes no representations or warranties, express or
 * implied, including but not limited to warranties of merchantability or fitness for any particular purpose or that the
 * use of this software or documentation will not infringe any third party patents, copyrights, trademarks or other
 * rights.
 * 
 * The copyright holder will not be liable for any direct, indirect special or consequential damages arising out of any
 * use of this software or documentation.
 * 
 * See the GNU  General Public License version 2 for more details 
 * (a copy is included at /legal/license.txt).  
 *  
 * You should have received a copy of the GNU General Public License 
 * version 2 along with this work; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 
 * 02110-1301 USA  
 *  
 * Please contact Blue Whale Systems Ltd, Suite 123, The LightBox
 * 111 Power Road, Chiswick, London, W4 5PY, United Kingdom or visit 
 * www.bluewhalesystems.com if you need additional 
 * information or have any questions.  
 */



#ifndef BLUEWHALEPLATFORMAPPLICATION_H
#define BLUEWHALEPLATFORMAPPLICATION_H

#include <OSVersion.h>

#if __S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__
#include <aknapp.h>
#include <aknviewappui.h>
#define __BWM_APPLICATION__ CAknApplication
#define __BWM_DOCUMENT__ CEikDocument
#define __BWM_APPUI__ CAknViewAppUi
#define __BWM_VIEW__ CAknView
#elif __UIQ_VERSION_NUMBER__ >= __UIQ_V3_FP0_VERSION_NUMBER__
#include <qikapplication.h>
#include <qikdocument.h>
#include <qikappui.h>
#define __BWM_APPLICATION__ CQikApplication
#define __BWM_DOCUMENT__ CQikDocument
#define __BWM_APPUI__ CQikAppUi
#define __BWM_VIEW__ CQikViewBase
#endif

#include "Application.h"

#if (__S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__) && (__S60_VERSION__ < __S60_V3_FP0_VERSION_NUMBER__)
#ifndef __WINS__
#include <ECompXL.h>
#endif
#endif

class CBlueWhalePlatformApplication : public __BWM_APPLICATION__
{
private:
    CApaDocument* CreateDocumentL();

    TUid AppDllUid() const;
#if (__S60_VERSION__ >= __S60_V2_FP1_VERSION_NUMBER__) && (__S60_VERSION__ < __S60_V3_FP0_VERSION_NUMBER__)
#ifndef __WINS__
	TECompXL    iECompXL;        // support for global C++ objects 
#endif
#endif
};

#endif /* BLUEWHALEPLATFORMAPPLICATION_H */
