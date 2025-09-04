
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
#include <stdarg.h>
// #include <unistd.h>

#include "mucc.h"
#include "hash.h"
#include "codegen.h"


// reserved.c
int InitReservedWord( void );
int InitCtrlCodeWord( void );
void InitNoteName( void );

// msrcv.c
void setup_msr( void );
void parse_msr( void );

// mmlcv.c
void setup_mml( int partn );
void parse_mml( void );
void end_mml( void );



char *srcname;			// 入力ソースファイル
FILE *srcin;
char *outname = "aaa";	// 出力ファイル

char outtype = 0;
char intype = 0;
char verbose = 0;

int lineno = 0;
int error_cnt = 0;
int warning_cnt = 0;

static char lnbuff[1024];


// エラー表示系
//
void error( char *msg, ... )
{
	va_list ap;

	fprintf( stderr, "%s(%d) : Error: ", srcname, lineno );

	va_start( ap, msg );
	vfprintf( stderr, msg, ap );
	va_end( ap );

	fputc( '\n', stderr );

	error_cnt++;
}

void warning( char *msg, ... )
{
	va_list ap;

	fprintf( stderr, "%s(%d) : Warning: ", srcname, lineno );

	va_start( ap, msg );
	vfprintf( stderr, msg, ap );
	va_end( ap );

	fputc( '\n', stderr );

	warning_cnt++;
}

void fatal( char *msg, ... )
{
	va_list ap;

	fprintf( stderr, "%s(%d) : Fatal: ", srcname, lineno );

	va_start( ap, msg );
	vfprintf( stderr, msg, ap );
	va_end( ap );

	fputc( '\n', stderr );

	exit( -1 );		// Fatal Exit
}

void debug( char *msg, ... )
{
	va_list ap;

	fprintf( stderr, "%s(%d) : Debug: ", srcname, lineno );

	va_start( ap, msg );
	vfprintf( stderr, msg, ap );
	va_end( ap );

	fputc( '\n', stderr );
}


#if 0
void cut0d0a( char *p )
{
	int n = strlen( p );
	while ( n ) {
		if ( p[n-1] != '\n' && p[n-1] != '\r' ) break;
		p[--n] = 0;
	}
}
#endif


// トークンの切り出し
//
char *gettoken( int *pff )
{
	static char *cp = NULL;
	char *p = cp, *tp = NULL;

	while ( 1 ) {
		if ( !p ) {
			p = lnbuff;
			cp = tp = NULL;
			if ( !fgets( lnbuff, sizeof(lnbuff), srcin ) ) return NULL;
			lineno++;
			//cut0d0a( p );
			if ( *p == '%' ) {
				*pff = TT_MML;
				return p+1;
			}
		}
		while ( 1 ) {
			char a = *p;
			if ( a == 0 ) {
				p = NULL;
				break;
			}
			else if ( a == ' ' || a == '\t' || a == '\n' || a == '\r' || a == ',' ) {
				*p++ = 0;
				break;
			}
			else if ( !tp ) {
				if ( a == ';' ) {
					p = NULL;
					break;
				}
				else if ( a == '\'' || a == '\"' ) {
					char ss = a;
					tp = p;
					while ( 1 ) {
						a = *++p;
						if ( a == ss || a == 0 ) break;
					}
				}
				else {
					tp = p;
				}
			}
			p++;
		}
		if ( tp ) {
			int f = 0;
			char a = *tp;
			cp = p;
			if ( tp == lnbuff ) f = TT_LABEL;
			else if ( isdigit(a) || a=='-' || a=='+' || a=='$' ) f = TT_NUMBER;
			else if ( a=='\'' || a=='\"' ) f = TT_STRING;
			else f = TT_OTHER;
			*pff = f;
			return tp;
		}
	}
}

// 1行の切り出し
//
char *mucc_getline( void )
{
	char *p = fgets( lnbuff, sizeof(lnbuff), srcin );
	if ( p ) {
		lineno++;
		//cut0d0a( p );
	}
	return p;
}


void conv( char *inf, char *outf )
{
	FILE *fp = fopen( inf, "rt" );

	if ( !fp ) fatal( "File open error %s", inf );

	srcin = fp;
	srcname = inf;

	if ( !intype ) {
		setup_mml( MAXPART );
		parse_mml();
		end_mml();
	}
	else {
		InitReservedWord();
		InitCtrlCodeWord();
		InitNoteName();

		parse_msr();
	}

	fclose( fp );

	if ( !error_cnt ) {
		fp = fopen( outf, "wb" );

		if ( !fp ) fatal( "File open error %s\n", outf );

		cgend( fp, outtype );
		fclose( fp );

		cgreport();
	}
	else {
		unlink( outf );
	}

	if ( verbose ) {
		ReportHash();
		ReportStr();
	}
}



int ckext( char *fname, char *ext, ... )
{
	char *p = ext;
	int ret = 0;
	int n1 = strlen( fname );
	va_list marker;

	va_start( marker, ext );     /* 可変個の引数の初期化 */
	while ( !ret && p ) {
		int n2 = strlen( p );
		ret = !strcmp( fname+n1-n2, p );
		//printf( "%d %s\n", ret, p );
		p = va_arg( marker, char *);
	}
	va_end( marker );            /* 可変個の引数のリセット */

	return ret;
}


void params( char *p )
{
	if ( *p == '-' ) {
		switch( p[1] ) {
			case '1':
				intype = 1;
				break;
			case 'v':
				verbose = 1;
				break;
		}
	}
	else if ( !srcname ) {
		srcname = p;
		if ( !intype ) intype = ckext( p, ".s", ".seq", NULL );
	}
	else {
		outname = p;
		outtype = ckext( p, ".c", ".cpp", ".dat", ".inc", NULL );
	}
}


#ifndef __EMSCRIPTEN__
int main( int argc, char *argv[] )
{
	int i;

	for ( i = 1; i < argc; i++ ) params( argv[i] );

	if ( srcname ) {
		conv( srcname, outname );
	}
	else {
		fprintf( stderr, "PIECE music code converter ver1.00 by MIO.H (16part ext. by Autch)\n" );
		fprintf( stderr, "usage:mucc [-options] <infile>[<outfile>]\n" );
		error_cnt = 1;
	}

	return error_cnt;
}

#endif // !__EMSCRIPTEN__
