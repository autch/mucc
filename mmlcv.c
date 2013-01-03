
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
#include <math.h>

#include "hash.h"
#include "reserved.h"
#include "mucc.h"
#include "codegen.h"

#define PART_POS0 "_part%c_start"
#define PART_POSn "_part%c_pos%d"
#define PART_LOOP "_part%c_loop"
#define DRMS_POS0 "_drums_define"
#define DRMS_POSn "_drums_%03d"
#define TITLE     "_title_str"
#define TITLE2    "_title2_str"

#define MAXMACRO 8
#define MAXNEST  8

typedef struct tagMMLPARTWK {
	short xlen;
	unsigned long total_len;
	unsigned char tatt;
	unsigned char xnote;
	unsigned char pos;
	unsigned char loop;
	unsigned char oct;
	unsigned char len;
	unsigned char vol;
	unsigned char det;
	unsigned char leg;
	unsigned char xxleg;
	unsigned char porsw;
	unsigned char pornote1;
	unsigned char pornote2;
	signed char oct1;
	signed char len1;
	signed char vol1;
	signed char det1;
	signed char trs;
} MMLPARTWK;

//                  A B C D E F G H I J
static char pmap[27] = {0,1,2,3,4,5,6,7,8,5,};

static MMLPARTWK pw[27];
static MMLPARTWK *lxppw = pw;

static char *lp;

char *macrostack[MAXMACRO];
char macrolevel = 0;
int nestdata[MAXNEST];
char nestlevel = 0;
unsigned char drumno;

static unsigned long last_f;
static unsigned long new_label;

static int tempo;
static int tempo1;

static int ttlf;

static HASHDATA *defadrs( char *str )
{
	HASHDATA *phd = SearchHash( str );

	if ( phd ) {
		if ( phd->type != TYPE_LABEL0 ) {
			error( "'%s' is alrady defined", str );
			return phd;
		}
		phd->type = TYPE_LABEL;
		phd->value = cgnowadrs();
	}
	else {
		phd = RegisterStrHash( str, TYPE_LABEL, cgnowadrs() );
	}

	return phd;
}


static void defposadrs( int pno, int srf )
{
	char tmp[256];
	MMLPARTWK *ppw = pw + pno;

	if ( !srf ) {
		snprintf( tmp, sizeof(tmp), PART_POS0, 'A'+pno );
	}
	else {
		snprintf( tmp, sizeof(tmp), PART_POSn, 'A'+pno, ++ppw->pos );
	}

	defadrs( tmp );
	//debug( "pos:%s", tmp );
}


static HASHDATA *defdrumadrs( int no )
{
	char tmp[256];

	snprintf( tmp, sizeof(tmp), DRMS_POSn, no );

	return defadrs( tmp );
}


static HASHDATA *defloopadrs( int pno )
{
	char tmp[256];

	snprintf( tmp, sizeof(tmp), PART_LOOP, 'A'+pno );

	return defadrs( tmp );
}


static void mkadrs( char *str )
{
	HASHDATA *phd = RegisterStrHash( str, TYPE_LABEL0, 0 );
	cgadrs( phd );
}


static void mkposadrs( int pno, int srf )
{
	char tmp[256];

	if ( !srf ) {
		snprintf( tmp, sizeof(tmp), PART_POS0, 'A'+pno );
	}
	else {
		snprintf( tmp, sizeof(tmp), PART_POSn, 'A'+pno, srf );
	}

	mkadrs( tmp );
}


static void mkdrumadrs( int no )
{
	char tmp[256];

	snprintf( tmp, sizeof(tmp), DRMS_POSn, no );

	mkadrs( tmp );
}


static void mkloopadrs( int pno )
{
	char tmp[256];

	snprintf( tmp, sizeof(tmp), PART_LOOP, 'A'+pno );

	mkadrs( tmp );
}


static void set_label( void )
{
	int i;

	cgout1( CCD_END );

	for ( i = 0; i < 26; i++ ) {
		if ( last_f & (1<<i) ) defposadrs( i, 1 );
	}

	new_label = 1;
}



static void ana_label( void )
{
	unsigned long f = 0;
	int n = 0;

	while ( 1 ) {
		char a = *lp;
		if ( a < 'A' || a > 'Z' ) break;
		a = pmap[a - 'A'];
		lp++;
		f |= (1<<a);
		if ( !n++ ) lxppw = pw + a;
	}

	if ( n && last_f != f ) {
		last_f = f;
		set_label();
	}
	
	return;
}

static char *get_macro_name( void )
{
	static char tmp[32];
	int i = 1;

	*tmp = '_';

	while ( i < sizeof(tmp)-1 ) {
		char a = *lp;
		if ( a < 'A' || a > 'Z' ) 
		if ( a < 'a' || a > 'z' ) break;
		tmp[i++] = a;
		lp++;
	}
	tmp[i] = 0;

	return tmp;
}



static void def_macro( void )
{
	int drumf = 0;
	char *pmn;
	HASHDATA *phd;

	if ( *lp == '!' ) {
		drumf = 1;
		lp++;
	}

	pmn = get_macro_name();

	while ( *lp == '\t' || *lp == ' ' ) lp++;

	if ( !drumf ) {		// MML マクロ
		HASHDATA *phd2;
		char *p = lp;
		int i, n = strlen(p);

		for ( i = 0; i < n; i++ ) {
			if ( p[i] == ';' ) break;
		}

		while ( i ) {
			char a = p[i-1];
			if ( a != '\r' && a != '\n' && a != ' ' && a != '\t' ) break;
			i--;
		}

		p[i] = 0;

		phd2 = RegisterStr( lp, TYPE_MMLMACRO, 0 );

		while ( *lp ) lp++;

		phd = RegisterStrHash( pmn, TYPE_MMLMACRO, (unsigned long)phd2->str );
	}
	else {				// ドラム・マクロ
		if ( drumno >= 96 ) {
			error( "Macro(drums) area over '%s'", pmn );
			return;
		}

		cgout1( CCD_END );

		phd = RegisterStrHash( pmn, TYPE_DRMMACRO, drumno + 0x80 );
		defdrumadrs( drumno );

		drumno++;
	}

	if ( strcmp(phd->str, pmn) ) {
		error( "Symbol already defined '%s'", pmn );
		return;
	}
}


static int ext_macro( void )
{
	char *p = get_macro_name();
	HASHDATA *phd = SearchHash( p );

	if ( phd ) {
		if ( phd->type == TYPE_MMLMACRO ) {
			if ( macrolevel == MAXMACRO-1 ) fatal( "Macro nest over" );
			macrostack[macrolevel++] = lp;
			lp = (char *)phd->value;
			return -1;
		}
		else if ( phd->type == TYPE_LABEL ) {
			cgout1( CCD_CAL );
			cgadrs( phd );
			cgout1( 1 );
			return -1;
		}
		else if ( phd->type == TYPE_DRMMACRO ) {
			return phd->value - 0x80;
		}
	}

	error( "Macro not defined '%s'", p );
	return -1;
}


static char get1c( void )
{
	while ( 1 ) {
		char a = *lp++;
		if ( a && a != ';' ) return a;
		if ( !macrolevel ) return 0;
		lp = macrostack[--macrolevel];
	}
}


static int cvlen( int len )
{
	if ( !len ) {
		error( "Length is 0" );
		return 24;
	}
	else if ( len > 96 ) {
		error( "Length is too large %d", len );
		return 24;
	}

	return 96 / len;
}


static int getlen( int ans )
{
	char a = get1c();
	int xf = 0;

	if ( a == '%' ) { xf = 1; a = *lp++; }

	if ( a < '0' || a > '9' ) {
		lp--;
	}
	else {
		ans = a - '0';

		while ( 1 ) {
			a = *lp;
			if ( a < '0' || a > '9' ) break;
			ans = ans * 10 + a-'0';
			lp++;
		}

		if ( !xf ) ans = cvlen( ans );
	}

	if ( *lp == '.' ) {		// 付点
		int x = ans;
		do {
			ans += (x>>=1);
		} while ( *++lp == '.' );
	}

	return ans;
}



static int getval( void )
{
	int ans = 0, sgn = 1;
	char a = get1c();

	if ( a == '+' ) a = *lp++;
	else if ( a == '-' ) { sgn = -1; a = *lp++; }

	if ( a < '0' || a > '9' ) {
		lp--;
		return ans;
	}

	ans = a - '0';

	while ( 1 ) {
		a = *lp;
		if ( a < '0' || a > '9' ) break;
		ans = ans * 10 + a-'0';
		lp++;
	}

	ans *= sgn;

	return ans;
}


static void ctrlgen( int ch, int code, int n )
{
	cgout1( code );

	while ( 1 ) {
		char a;
		cgout1( getval() );
		if ( !--n ) return;
		a = *lp;
		if ( a != ',' ) {
			error( "Too few parameters '%c'", ch );
		}
		lp++;
	}
}


static void notegen( MMLPARTWK *ppw, int code )
{
	int len = getlen(ppw->len + ppw->len1);

	new_label = 0;

	if ( *lp == '&' ) {
		lp++;
		if ( ppw->xxleg < 2 ) ppw->xxleg = 2;
	}

	if ( ppw->porsw == 2 ) {
		int x = (int)(log(fabs(ppw->pornote2 - ppw->pornote1) / len) / log(2.0));
		x <<= 4;
		x += 128;
		//printf( "porta %d -> %d\n", ppw->pornote2, ppw->pornote1 );
		//printf( "porta spd %d\n", x );

		cgout1( CCD_PPA );
		cgout1( ppw->pornote2 - ppw->pornote1 );
		cgout1( x );

		ppw->porsw = 0;

		cgout1( CCD_PON );
	}

	if ( ppw->xxleg ) ppw->xxleg--;

	if ( ppw->xxleg ) {
		if ( !(ppw->leg & 1) ) {
			cgout1( CCD_LEG );
			ppw->leg |= 1;
		}
	}
	else {
		if ( ppw->leg & 1 ) {
			cgout1( CCD_LOF );
			ppw->leg &= ~1;
		}
	}

	if ( !ppw->porsw ) {
		if ( ppw->xlen != len ) {
			ppw->xlen = len;
			cgoutlen( len );
		}
			ppw->total_len += len;

		cgout1( (code >= 0 && code < 96) ? 128+code : CCD_RRR );
	}
	else {
		ppw->pornote2 = ppw->pornote1;
		ppw->pornote1 = code;
	}
}


static void porgen( MMLPARTWK *ppw )
{
	ppw->porsw = 2;
	notegen( ppw, ppw->pornote1 );
	cgout1( CCD_POF );
}




static void _repeat( void )
{
	cgout1(CCD_RPT);
	if ( nestlevel == MAXNEST-1 ) fatal( "REPAET nest over" );
	nestdata[nestlevel++] = cgnowadrs();
	cgout1(0);
}

static void _next( int n )
{
	if ( !nestlevel ) fatal( "REPEAT nest failed" );
	cgout( nestdata[--nestlevel], n );
	cgout1(CCD_NXT);
}


static ana_body( MMLPARTWK *ppw )
{
	while ( 1 ) {
		char a = get1c();
		if ( !a ) break;
		switch ( a ) {
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			{
				static char tbl[] = {9,11,0,2,4,5,7};
				int note = tbl[a-'a'] + (ppw->oct + ppw->oct1)*12;
				while ( 1 ) {
					if ( *lp == '+' ) note++;
					else if ( *lp == '-' ) note--;
					else break;
					lp++;
				}
				
				if ( !(ppw->tatt & 1) ) ppw->xnote = note;
				notegen(ppw, ppw->xnote);
				break;
			}
			case 'r':
				notegen(ppw, -1);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '.':
				lp--;
				notegen(ppw, ppw->xnote);
				break;
			case 'x':
				notegen(ppw, ppw->xnote);
				break;
			case 'l':
				if ( *lp =='+' && *lp == '+' ) {
					ppw->len1 = cvlen( getval() );
				}
				else {
					ppw->len = cvlen( getval() );
				}
				break;
			case 'o':
				if ( *lp =='+' && *lp == '+' ) {
					ppw->oct1 = getval();
				}
				else {
					ppw->oct = getval();
				}
				break;
			case 't':
			case 'T':
				if ( *lp =='+' && *lp == '+' ) {
					tempo1 = getval();
				}
				else {
					tempo = getval();
				}
				cgout1(CCD_TMP);
				cgout1(tempo + tempo1);
				new_label = 0;
				break;
			case 'v' :
				if ( *lp =='+' && *lp == '+' ) {
					cgout1(CCD_EXR);
					cgout1(getval());
				}
				else {
					cgout1(CCD_EXP);
					cgout1(getval());
				}
				new_label = 0;
				break;
			case 'V':
				if ( *lp =='+' && *lp == '+' ) {
					ppw->vol1 = getval();
				}
				else {
					ppw->vol = getval();
				}
				cgout1(CCD_VOL);
				cgout1(ppw->vol + ppw->vol1);
				new_label = 0;
				break;
			case 'D':
				if ( *lp =='D' ) {
					lp++;
					ppw->det1 = getval();
				}
				else {
					ppw->det = getval();
				}
				cgout1(CCD_DTN);
				cgout1(ppw->det + ppw->det1);
				new_label = 0;
				break;
			case '[':
				ppw->xlen = 0;
				_repeat();
				new_label = 0;
				break;
			case ']':
				_next( getval() );	// 回数
				ppw->xlen = 0;
				new_label = 0;
				break;
			case ':':
				cgout1( CCD_BRK );
				new_label = 0;
				break;
			case '<':
				ppw->oct--;
				break;
			case '>':
				ppw->oct++;
				break;
			case '_':
			{
				if ( *lp == '_' ) {
					lp++;
					ppw->trs += getval();
				}
				else {
					ppw->trs = getval();
				}
				cgout1( CCD_TRS );
				cgout1( ppw->trs );
				new_label = 0;
				break;
			}
			case '!':
			{
				int note = ext_macro();
				if ( note >= 0 ) {
					notegen(ppw, note);
					ppw->xnote = note;
				}
				new_label = 0;
				break;
			}
			case 'i':
				def_macro();
				break;
			case 'L':
			{
				int i;
				int f = last_f;
				if ( !new_label ) set_label();
				for ( i = 0; i < 26; i++ ) {
					MMLPARTWK *ppw2 = pw + i;
					if ( f & (1<<i) ) ppw2->loop = ppw2->pos;
				}
				break;
			}
			case '=' :
				cgout1( CCD_TAT );
				cgout1( ppw->tatt = getval() );
				new_label = 0;
				break;

			case '@' :
				ctrlgen(a, CCD_INO, 1);
				new_label = 0;
				break;

			case 'E' :
				ctrlgen(a, CCD_ENV, 4);
				new_label = 0;
				break;

			case 'Y' :
				ctrlgen(a, CCD_VIB, 4);
				new_label = 0;
				break;

			case 'Z' :
				ctrlgen(a, CCD_PPA, 2);
				new_label = 0;
				break;

			case '?' :
				cgout1( CCD_NOT );
				new_label = 0;
				break;

			case '{' :
				ppw->porsw = 1;
				break;

			case '}' :
				porgen( ppw );
				break;

			case 'q':
			case 'Q':
			{
				int n = getval();
				if ( a == 'q' ) {
					n = ( (24 - n) * 99 ) / 24;
				}
				else {
					n = ( n * 99 ) / 8;
				}
				if ( n < 1 ) n = 1;
				else if ( n > 99 ) n = 99;
				cgout1( CCD_GAT );
				cgout1( n );
				new_label = 0;
				break;
			}
			case '(':
			case ')':
			{
				int n = 1;
				if ( *lp >= '0' && *lp <= '9' ) n = getval();
				if ( a == '(' ) n = -n;
				cgout1( CCD_EXR );
				cgout1( n );
				new_label = 0;
				break;
			}
			// 無視されるもの
			case '/':
				break;
			case 'C':
				getval();
				break;

			default:
				fatal( "Unkown MML character '%c'", a );
		}
	}
}


// 仮のいいかげんな処理…
void gencomment( void )
{
	int tf = 0;

	if ( !strncmp( lp, "Title2", 6 ) ) { tf = 2; lp+=6; }
	else if ( !strncmp( lp, "Title", 5 ) ) { tf = 1; lp+=5; }

	if ( !tf ) return;

	while ( *lp == ' ' || *lp == '\t' ) lp++;

	if ( !(ttlf & tf) ) {
		ttlf |= tf;
		if ( tf == 1 ) { defadrs( TITLE ); }
		else { cgout1(0); defadrs( TITLE2 ); }
	}

	while ( *lp && *lp != '\n' && *lp != '\r' ) {
		if ( tf == 2 && *lp == ';' ) {
			cgout1( '\n' );
		}
		else cgout1( *lp );
		lp++;
	}
}




void setup_mml( int partn )
{
	int i;

	memset(pw, 0, sizeof pw);

	cgout1( 0 );

	cgout1( partn );
	for ( i = 0; i < partn; i++ ) {
		mkposadrs( i, 0 );
	}

	mkadrs( DRMS_POS0 );
	mkadrs( TITLE );
	mkadrs( TITLE2 );

	//defadrs( TITLE );
	//defadrs( TITLE2 );
}


static int aaa;

void parse_mml( void )
{
	while ( 1 ) {
		char *p = mucc_getline();
		if ( !p ) break;
		if ( *p == ';' ) continue;
		if ( *p == '#' ) { lp = p+1; gencomment(); continue; }
		if ( *p == '@' ) { aaa = 4; continue; }
		if ( aaa ) { aaa--; continue; }

		lp = p;
		if ( *lp == '!' ) { lp++; def_macro(); }
		else ana_label();
		ana_body( lxppw );
	}

}



void end_mml( void )
{
	int pno;

	cgout1( CCD_END );

	for ( pno = 0; pno < 6; pno ++ ) {
		MMLPARTWK *ppw = pw + pno;
		if ( ppw->pos ) {
			int i;
			defposadrs( pno, 0 );
			for ( i = 0; i < ppw->pos; i++ ) {
				if ( i+1 == ppw->loop ) defloopadrs( pno );
				cgout1( CCD_CAL );
				mkposadrs( pno, i+1 );
				cgout1( 1 );
			}
			if ( ppw->loop ) {
				cgout1( CCD_JMP );
				mkloopadrs( pno );
			}
			else cgout1( CCD_END );
		}
		fprintf(stderr, "%c: %ld ticks\n", 'A' + pno, ppw->total_len);
	}

	if ( drumno ) {
		int i;
		defadrs( DRMS_POS0 );
		for ( i = 0; i < drumno; i++ ) {
			mkdrumadrs( i );
		}
	}

}



void parse_mml_line( int len, char *ptr )
{
	static MMLPARTWK pw0;
	
	//pw0.xlen = len;
	lp = ptr;

	ana_body( &pw0 );
}


