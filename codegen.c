
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



// コード・ジェネレータ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mucc.h"
#include "hash.h"
#include "codegen.h"
#include "reserved.h"


#define MAXFWAD 2000	// 前方参照ラベルの最大個数

static unsigned char cgbuff[65536];
static int cgp = 0;

typedef struct tagFWAD {
	HASHDATA *phd;
	unsigned short adr;
	unsigned short lno;
} FWAD;

static FWAD fwad[MAXFWAD];
static int fap = 0;


void cgout( int adr, int dat )
{
	if ( adr >= cgp ) fatal( "Output buffer over" );
	cgbuff[adr] = dat;
}

void cgout1( int a )
{
	if ( cgp >= sizeof(cgbuff) ) fatal( "Output buffer over" );
	cgbuff[cgp++] = a;
}

void cgout2( int a )
{
	cgout1( a );
	cgout1( a >> 8 );
}

void cgoutlen( int len )
{
	if ( len < 127 ) cgout1( len );
	else if ( len < 256 ) { cgout1( CCD_LEX ); cgout1( len ); }
	else if ( len < 127+256 ) { cgout1( CCD_LEX ); cgout1( len-256 ); }
	else error( "length over" );
}

void cgadrs( HASHDATA *phd )
{
	//printf( "%d 0x%04x\n", phd->type, phd->value );
	if ( phd->type == TYPE_LABEL0 ) {
		if ( fap < MAXFWAD ) {
			FWAD *pf = fwad + fap++;
			pf->adr = cgp;
			pf->phd = phd;
		}
		else fatal( "Too many forward label" );
	}
	cgout2( phd->value );
}

int cgnowadrs( void )
{
	return cgp;
}

int cgfwad( void )
{
	int i;
	// 前方参照ラベルの解決
	for ( i = 0; i < fap; i++ ) {
		FWAD *pf = fwad + i;
		HASHDATA *phd = pf->phd;
		if ( phd->type == TYPE_LABEL ) {
			cgbuff[pf->adr] = (unsigned char)phd->value;
			cgbuff[pf->adr+1] = (unsigned char)(phd->value>>8);
		}
		else if ( phd->type == TYPE_LABEL0 ) {
			if ( phd->value ) error( "Unknown label '%s'", phd->str );
		}
		else {
			fatal( "Unknown label '%s'", phd->str );
		}
	}
	return 0;
}


static int cgbin( FILE *fp )
{
	return fwrite( cgbuff, 1, cgp, fp ) == (unsigned )cgp;
}

static int cgsrc( FILE *fp )
{
	int i;
	for ( i = 0; i < cgp; i++ ) {
		fprintf( fp, "0x%02x,", cgbuff[i] );
		if ( (i&7)==7 ) fprintf( fp, "\r\n" );
	}
	return 0;
}


int cgend( FILE *fp, int type )
{
	cgfwad();
	return type ? cgsrc( fp ) : cgbin( fp );
}

void cgreport( void )
{
	fprintf( stderr, "Total %d(0x%0x) bytes\n", cgp, cgp );
}


