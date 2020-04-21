/*////////////////////////////////////////////////////////////////////////////

This file is part of the G2Ext SDK headers.

//////////////////////////////////////////////////////////////////////////////

The G2Ext SDK headers

Copyright � 2009, 2010 by Paindevs and Patrick Vogel

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

//////////////////////////////////////////////////////////////////////////////

						=== DO NOT CHANGE THIS FILE ===

/////////////////////////////////////////////////////////////////////////////*/

#ifndef __G2EXT_VERSION_H__
#define __G2EXT_VERSION_H__

/** This constant defines the current major version.
 * The major version is only increased upon compatibility breaking api changes.
 *  (g2ext and mod major version have to match exactly)
 */
#define G2EXT_MAJOR_VERSION 1

/** This constant defines the current minor version.
 * The minor version is increased upon compatible api changes.
 *  (g2ext supports older mods; mods don't support older g2ext minor versions)
 */
#define G2EXT_MINOR_VERSION 1

/** This constant defines the current svn revision.
 * The value is being generated automatically.
 */
#define G2EXT_SUBVERSION_REVISION 32

/** Autogenerated version data */

#define _Q(x) #x
#define Q(x) _Q(x)
#define _W(x) L ## #x
#define W(x) _W(x)

#define G2EXT_VER_FILE	 		G2EXT_MAJOR_VERSION, G2EXT_MINOR_VERSION, 0, 32
#define G2EXT_VER_PRODUCT 		G2EXT_MAJOR_VERSION, G2EXT_MINOR_VERSION, 0, 32

#define G2EXT_VER_FILE_S	 	Q(G2EXT_MAJOR_VERSION ## . ## G2EXT_MINOR_VERSION ## . ## 0 ## . ## 32)
#define G2EXT_VER_PRODUCT_S 	Q(G2EXT_MAJOR_VERSION ## . ## G2EXT_MINOR_VERSION ## . ## 0 ## . ## 32)

#define G2EXT_VER_FILE_WS	 	W(G2EXT_MAJOR_VERSION ## . ## G2EXT_MINOR_VERSION ## . ## 0 ## . ## 32)
#define G2EXT_VER_PRODUCT_WS 	W(G2EXT_MAJOR_VERSION ## . ## G2EXT_MINOR_VERSION ## . ## 0 ## . ## 32)

/** Autogenerated values */
#define SVN_LAST 		"13.May.2010 :: 17:55:31"
#define SVN_LAST_DATE 	"13.May.2010"
#define SVN_LAST_TIME	"17:55:31"
#define SVN_NOW  		"13.May.2010 :: 17:56:42"
#define SVN_NOW_DATE  	"13.May.2010"
#define SVN_NOW_TIME  	"17:56:42"
#define SVN_REV  		"32"

#define G2EXT_VERSIONA	"G2Ext 'rising phoenix' :: version: " G2EXT_VER_PRODUCT_S " :: built: 13.May.2010 :: 17:56:42"
#define G2EXT_VERSIONW	L"G2Ext 'rising phoenix' :: version: " G2EXT_VER_PRODUCT_WS L" :: built: 13.May.2010 :: 17:56:42"

#endif  //__G2EXT_VERSION_H__