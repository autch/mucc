
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "mucc.h"

#define MAXSTRS 5000
#define MAXCBUF (80*1024)

static HASHDATA strhd[MAXSTRS];
static char cbuff[MAXCBUF];

static int hdc = 0;
static int cbc = 0;

HASHDATA *RegisterStr( char *str, unsigned long type, unsigned long value )
{
	HASHDATA *phd = NULL;
	int n = strlen(str);

	if ( n == 0 ) return 0;

	if ( cbc + n + 1 >= MAXCBUF ) fatal( "String area over(1)" );

	if ( hdc + 1 >= MAXSTRS ) fatal( "String area over(2)" );

	phd = strhd + hdc++;
	phd->str = cbuff + cbc;
	phd->type = type;
	phd->value = value;

	memcpy( phd->str, str, n+1 );

	cbc += n+1;

	return phd;
}


HASHDATA *RegisterStrHash( char *str, unsigned long type, unsigned long value )
{
	HASHDATA *phd = SearchHash( str );
	if ( !phd ) {
		phd = RegisterStr( str, type, value );
		if ( phd ) RegisterHash( phd );
	}

	return phd;
}


void ReportStr( void )
{
	printf( "Total used string entries = %d(%5.1f%%)\n", hdc, hdc * 100.0 / MAXSTRS );
	printf( "Total used string area = %d(%5.1f%%)\n", cbc, cbc * 100.0 / MAXCBUF );
}

