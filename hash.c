
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

#include "mucc.h"
#include "hash.h"


#ifndef HASHSIZE
#define HASHSIZE 9973		// ハッシュの大きさ
							// 登録数より十分大きな(例えば3倍以上の)素数がよい
#endif //ifndef HASHSIZE


static HASHDATA *pHashMap[HASHSIZE];
static int hashcnt = 0;


//	メッセージマップ用の補助関数です。
//	ハッシュを用いて、効率よく（つもり）ハンドラ関数を検索します。

// ハッシュ関数です。
// 検索対象キー(str)から
// なるべくランダムな値が得られるように工夫します。
//
static int Hash( char *str )
{
	unsigned long data = 31;
	while ( *str ) data = (data * 311) + *str++;
	return (int)(data % HASHSIZE);
}

// ハッシュテーブル中のポインタ格納場所(HASHDATA **)を探します。
// 見つからない場合は、そこには NULL が入っています。
// 登録・検索両方で使います。
// 現状 HASHSIZE 個以上登録すると無限ループに落ちます。
//
static HASHDATA **GetHash( char *str )
{
	HASHDATA *pHash;
	int x = Hash( str );
	int dx = x;

	if ( !dx ) dx++;
	while ((pHash = pHashMap[x]) != NULL && strcmp(str,pHash->str)) {
		x = ( x + dx ) % HASHSIZE;
	}

	return pHashMap + x;
}

// ハッシュに１つのデータを登録します。
//
void RegisterHash( HASHDATA *phd )
{
	if ( hashcnt > HASHSIZE/2 ) fatal( "Hash table over" );

	//printf( "%s ", phd->str );
	*GetHash( phd->str ) = phd;
	hashcnt++;
}

// ハッシュからのデータを検索します。
//
HASHDATA *SearchHash( char *str )
{
	return *GetHash( str );
}


static char *typename[] = {
	NULL,
	"RESERVED ",
	"NOTE     ",
	"CTRLCODE ",
	"LABEL    ",
	"LABEL0   ",
	"DRMMACRO ",
	"MMLMACRO ",
};

static int repo_sub( unsigned long type )
{
	int i,  c = 0;
	printf( "----- %s (0x%02x)\n", typename[type&7], type );
	for ( i = 0; i < HASHSIZE; i++ ) {
		HASHDATA *phd = pHashMap[i];
		if ( phd && phd->type == type ) {
			c++;
			if ( type == TYPE_MMLMACRO ) {
				char *str2 = (char *)phd->value;
				printf( "\t%5d 0x%08x %s ('%s')\n", i, phd->value, phd->str, str2 );
			}
			else if ( type & TYPE_DEFINABLE ) {
				printf( "\t%5d 0x%08x %s\n", i, phd->value, phd->str );
			}
		}
	}
		printf( "\tTotal %d\n", c );
	return c;
}


void ReportHash( void )
{
	repo_sub( TYPE_RESERVED );
	repo_sub( TYPE_NOTE );
	repo_sub( TYPE_CTRLCODE );
	repo_sub( TYPE_LABEL );
	repo_sub( TYPE_LABEL0 );
	repo_sub( TYPE_DRMMACRO );
	repo_sub( TYPE_MMLMACRO );

	printf( "Total used hash entries = %d(%5.1f%%)\n", hashcnt, hashcnt * 100.0 / HASHSIZE );
}



