
/////////////////////////////////////////////////////////////////////////////
//
//             /
//      -  P  /  E  C  E  -
//           /                 mobile equipment
//
//              System Programs
//
//
// PIECE TOOLS : mucc : Ver 1.00
//
// Copyright (C)2001 AUQAPLUS Co., Ltd. / OeRSTED, Inc. all rights reserved.
//
// Coded by MIO.H (OeRSTED)
//
// Comments:
//
// ミュージック・データ・コンバーター
//
//  v1.00 2001.11.09 MIO.H
//



enum RESERVE {
	RSV_DB,			// db
	RSV_ADRS,		// adrs
	RSV_ADRSD,		// adrsd
	RSV_ADRSM,		// adrsm
	RSV_TEMPO,		// tempo
	RSV_INCLUDE,	// include
	RSV_PHRASE,		// phrase
	RSV_REPEAT,		// repeat
	RSV_NEXT,		// next
	RSV_BREAK,		// break
	RSV_GOTO,		// goto
	RSV_PARTN,		// partn
//	RSV_,			// 
};

enum CTRLCODE {
	CCD_END	=	0,
	CCD_LEX	=	127,
	CCD_RRR	=	0xE0+0,
	CCD_GAT	=	0xE0+1,
	CCD_JMP	=	0xE0+2,
	CCD_CAL	=	0xE0+3,
	CCD_RPT	=	0xE0+4,
	CCD_NXT	=	0xE0+5,
	CCD_TRS	=	0xE0+6,
	CCD_TMP	=	0xE0+7,
	CCD_INO	=	0xE0+8,
	CCD_VOL	=	0xE0+9,
	CCD_ENV	=	0xE0+10,
	CCD_DTN	=	0xE0+11,
	CCD_NOT	=	0xE0+12,
	CCD_PPA	=	0xE0+13,
	CCD_PON	=	0xE0+14,
	CCD_POF	=	0xE0+15,
	CCD_TAT	=	0xE0+16,
	CCD_VIB	=	0xE0+17,
	CCD_MVL	=	0xE0+18,
	CCD_MFD	=	0xE0+19,
	CCD_PFD	=	0xE0+20,
	CCD_BND	=	0xE0+21,
	CCD_BRK	=	0xF6,
	CCD_NOP	=	0xF7,
	CCD_LEG	=	0xF8,
	CCD_LOF	=	0xF9,
	CCD_EXP	=	0xFa,
	CCD_EXR	=	0xFb,
//	CCD_,			//
};

