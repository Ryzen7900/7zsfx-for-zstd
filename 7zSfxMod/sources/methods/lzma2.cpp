/*---------------------------------------------------------------------------*/
/* File:        lzma2.cpp                                                    */
/* Created:     Thu, 03 May 2007 10:26:00 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Last update: Mon, 22 Mar 2010 11:26:07 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Revision:    1055                                                         */
/*---------------------------------------------------------------------------*/
/* Revision:    1055                                                         */
/* Updated:     Mon, 22 Mar 2010 11:26:07 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: New file stamp.                                              */
/*---------------------------------------------------------------------------*/
#include "stdafx.h"

#ifdef COMPRESS_LZMA2
	#include "7zip\Compress\LZMA2Decoder.cpp"
	#include "7zip\Compress\LZMA2Register.cpp"
	#include "7zip\Compress\ZstdRegister.cpp"
	#include "7zip\Compress\ZstdDecoder.cpp"
#include "7zip\Common\CWrappers.cpp"

	extern "C" {
		#ifndef COMPRESS_LZMA
			#include "../C/LzmaDec.c"
		#endif
		//#include "../C/Lzma2Dec.c"
		#include "../C\zstd\zstd_decompress.c"
		#include "../C\zstd\xxhash.c"
		#include "../C\zstd\zstd_ddict.c"
		#include "../C\zstd\zstd_decompress_block.c"
		#include "../C\zstd\huf_decompress.c"
		#include "../C\zstd\zstd_common.c"
       
		//#include "../C\zstd\error_private.c"
		//#include "../C\zstd\fse_decompress.c"
		//#include "../C\zstd\entropy_common.c"
	}
#endif