
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
#include <ctype.h>

#include "hash.h"
#include "reserved.h"
#include "mucc.h"
#include "codegen.h"


void parse_mml_line( int len, char *ptr );


static void RegLabel( char *p )
{
	int adrs = cgnowadrs();
	HASHDATA *phd = RegisterStrHash( p, TYPE_LABEL, adrs );

	if ( phd ) {
		if ( phd->type == TYPE_LABEL0 ) {
			phd->type = TYPE_LABEL;
			phd->value = adrs;
		}
		else if ( phd->type != TYPE_LABEL ) {
			error( "'%s' is already defined.", p );
		}
	}
}

int getnum( char *p )
{
	return atoi( p );
}

int getval( void )
{
	int f;
	char *p = gettoken( &f );
	return getnum( p );
}

static void RegLabel2( char *p )
{
	HASHDATA *phd = RegisterStrHash( p, TYPE_LABEL0, -1 );
	cgadrs( phd );
}


static void adrsd( void )
{
	static int drums = 0;
	int f;
	char *p = gettoken( &f );
	RegLabel2( p );
	RegisterStrHash( p+1, TYPE_DRMMACRO, 0x80 + drums++ );
}

static void adrsm( void )
{
	getval();
	// 捨てます。
	cgout2( 0 );
}

static void adrs( void )
{
	int f;
	char *p = gettoken( &f );
	if ( f & TT_NUMBER ) cgout2( getnum(p) );
	else RegLabel2( p );
}

static void include( void )
{
	int f;
	char *p = gettoken( &f );
	// 捨てます。
}

static void _goto( void )
{
	cgout1( CCD_JMP );
	adrs();
}

static void tempo( void )
{
	cgout1( CCD_TMP );
	cgout1( getval() );
}

static void partn( void )
{
	cgout1( getval() );
}

static void phrase( void )
{
	cgout1( CCD_CAL );
	adrs();
	cgout1( getval() );
}

void parse_msr( void )
{
	while ( 1 ) {
		int f;
		char *p = gettoken( &f );
		//char a;
		if ( !p ) break;
		//a = *p;
		if ( f & TT_LABEL ) {
			int n = strlen(p);
			if ( p[n-1] == ':' ) p[n-1] = 0;
			RegLabel( p );
		}
		else if ( f & TT_NUMBER ) {
			cgout1( getnum( p ) );
		}
		else if ( f & TT_STRING ) {
			int i, n = strlen( p );
			if ( n > 2 && p[0] == p[n-1] ) n--;
			for ( i = 1; i < n; i++ ) cgout1( p[i] );
		}
		else if ( f & TT_OTHER ) {
			HASHDATA *phd = SearchHash( p );
			if ( phd ) {
				if ( phd->type == TYPE_RESERVED )
					switch ( phd->value ) {
						case RSV_ADRS:
							adrs();
							break;
						case RSV_ADRSM:
							adrsm();
							break;
						case RSV_ADRSD:
							adrsd();
							break;
						case RSV_INCLUDE:
							include();
							break;
						case RSV_GOTO:
							_goto();
							break;
						case RSV_TEMPO:
							tempo();
							break;
						case RSV_PARTN:
							partn();
							break;
						case RSV_PHRASE:
							phrase();
							break;
				}
				else if ( phd->type & TYPE_DIRECTCODE ) {
					cgout1( phd->value );
				}
			}
			else {
				error( "Unkown commands '%s'.", p );
			}
		}
		else if ( f & TT_MML ) {
			parse_mml_line( -1, p );
		}
	}
}


void setup_msr( void )
{
}

