
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



// ハッシュのタイプ
#define TYPE_DEFINABLE  0x10
#define TYPE_DIRECTCODE 0x20

#define TYPE_RESERVED 0x01 // 予約語
#define TYPE_NOTE     0x22 // 音名
#define TYPE_CTRLCODE 0x23 // コントロールコード

#define TYPE_LABEL    0x14 // ラベル
#define TYPE_LABEL0   0x15 // ラベル(前方参照用)
#define TYPE_DRMMACRO 0x36 // ドラムマクロ
#define TYPE_MMLMACRO 0x17 // ＭＭＬマクロ

// トークンのタイプ
#define TT_MML    16
#define TT_LABEL  8
#define TT_NUMBER 4
#define TT_STRING 2
#define TT_OTHER  1


void error( char *msg, ... );
void warning( char *msg, ... );
void fatal( char *msg, ... );
void debug( char *msg, ... );

char *gettoken( int *pff );
char *mucc_getline( void );


