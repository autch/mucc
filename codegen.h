
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



void cgout( int adr, int dat );
void cgout1( int a );
void cgout2( int a );
void cgoutlen( int len );
void cgadrs( HASHDATA *phd );
int cgnowadrs( void );
int cgend( FILE *fp, int type );
void cgreport( void );
