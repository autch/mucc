
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



typedef struct tagHASHDATA {
	char *str;
	unsigned long type;
	unsigned long value;
} HASHDATA;


void RegisterHash( HASHDATA *phd );

HASHDATA *SearchHash( char *str );

void ReportHash( void );


HASHDATA *RegisterStr( char *str, unsigned long type, unsigned long value );
HASHDATA *RegisterStrHash( char *str, unsigned long type, unsigned long value );

void ReportStr( void );

