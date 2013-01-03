
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



#include "hash.h"
#include "reserved.h"
#include "mucc.h"

#define CCD(id,str) {#str,TYPE_CTRLCODE,CCD_##id}
#define RSV(id,str) {#str,TYPE_RESERVED,RSV_##id}

static HASHDATA ReservedWord[] = {
	RSV(	DB,			db			),
	RSV(	ADRS,		adrs		),
	RSV(	ADRSD,		adrsd		),
	RSV(	ADRSM,		adrsm		),
	RSV(	INCLUDE,	include		),
	RSV(	TEMPO,		tempo		),
	RSV(	TEMPO,		_tempo		),
	RSV(	PHRASE,		phrase		),
	RSV(	GOTO,		goto		),
	RSV(	PARTN,		partn		),
	CCD(	RPT,		repeat		),
	CCD(	NXT,		next		),
	CCD(	BRK,		break		),
};

static HASHDATA CtrlCodeWord[] = {
	CCD( END, end ),
	CCD( LEX, lex ),
	CCD( RRR, rrr ),
	CCD( GAT, gat ),
	CCD( JMP, jmp ),
	CCD( CAL, cal ),
	CCD( RPT, rpt ),
	CCD( NXT, nxt ),
	CCD( TRS, trs ),
	CCD( TMP, tmp ),
	CCD( INO, ino ),
	CCD( VOL, vol ),
	CCD( ENV, env ),
	CCD( DTN, dtn ),
	CCD( NOT, not ),
	CCD( PPA, ppa ),
	CCD( PON, pon ),
	CCD( POF, pof ),
	CCD( TAT, tat ),
	CCD( VIB, vib ),
	CCD( MVL, mvl ),
	CCD( MFD, mfd ),
	CCD( PFD, pfd ),
	CCD( BND, bnd ),
	CCD( LEG, leg ),
	CCD( LOF, lof ),
	CCD( EXP, exp ),
	CCD( EXR, exr ),
};



int InitReservedWord( void )
{
	int i;
	for ( i = 0; i < sizeof(ReservedWord)/sizeof(ReservedWord[0]); i++ ) {
		RegisterHash( ReservedWord + i );
	}
	return 0;
}



int InitCtrlCodeWord( void )
{
	int i;
	for ( i = 0; i < sizeof(CtrlCodeWord)/sizeof(CtrlCodeWord[0]); i++ ) {
		RegisterHash( CtrlCodeWord + i );
	}
	return 0;
}


// 音名生成ルーチン

static void regnote( char *str, int n )
{
	//printf( "%s(%d) ", str, n );
	RegisterStrHash( str, TYPE_NOTE, n );
}

static void initnote( char note, char oct, int n )
{
	char str[4];

	// 虹越音名を生成
	str[0] = note|0x20;
	str[1] = oct;
	str[2] = '0';
	str[3] = 0;
	regnote( str, n );

	str[2] = 'b';
	regnote( str, n-1 );

	str[2] = 's';
	regnote( str, n+1 );

	// アメリカ音名を生成
	str[0] = note;
	str[2] = 0;
	regnote( str, n );

	str[1] = 'b';
	str[2] = oct;
	str[3] = 0;
	regnote( str, n-1 );

	str[1] = '#';
	regnote( str, n+1 );
}

void InitNoteName( void )
{
	static char nnam[7] = "CDEFGAB";
	static char nofs[7] = {0,2,4,5,7,9,11};
	int i;
	
	for ( i = 0; i < 10; i++ ) {
		char oct = '0'+i;
		int j;
		for ( j = 0; j < 7; j++ ) {
			initnote( nnam[j], oct, 0x80 + i*12 + nofs[j] );
		}
	}

	RegisterStrHash( "R", TYPE_CTRLCODE, CCD_RRR );
}




