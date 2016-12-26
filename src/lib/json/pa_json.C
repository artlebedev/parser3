/*
 * Copyright (C) 2009-2011 Vincent Hanquez <vincent@snarc.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 or version 3.0 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * the class, states and state transition tables has been inspired by the JSON_parser.c
 * available at http://json.org, but are quite different on the way that the
 * parser handles its parse buffer and contains significant differences that affect
 * the JSON compliance.
 */

#include "pa_json.h"

enum classes {
	C_SPACE, /* space */
	C_NL,    /* newline */
	C_WHITE, /* tab, CR */
	C_LCURB, C_RCURB, /* object opening/closing */
	C_LSQRB, C_RSQRB, /* array opening/closing */
	/* syntax symbols */
	C_COLON,
	C_COMMA,
	C_QUOTE, /* " */
	C_BACKS, /* \ */
	C_SLASH, /* / */
	C_PLUS,
	C_MINUS,
	C_DOT,
	C_ZERO, C_DIGIT, /* digits */
	C_a, C_b, C_c, C_d, C_e, C_f, C_l, C_n, C_r, C_s, C_t, C_u, /* nocaps letters */
	C_ABCDF, C_E, /* caps letters */
	C_OTHER, /* all other */
	C_STAR, /* star in C style comment */
	C_HASH, /* # for YAML comment */
	C_ERROR = 0xfe,
};

/* map from character < 128 to classes. from 128 to 256 all C_OTHER */
static uint8_t character_class[128] = {
	C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR,
	C_ERROR, C_WHITE, C_NL,    C_ERROR, C_ERROR, C_WHITE, C_ERROR, C_ERROR,
	C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR,
	C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR, C_ERROR,

	C_SPACE, C_OTHER, C_QUOTE, C_HASH,  C_OTHER, C_OTHER, C_OTHER, C_OTHER,
	C_OTHER, C_OTHER, C_STAR,  C_PLUS,  C_COMMA, C_MINUS, C_DOT,   C_SLASH,
	C_ZERO,  C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT, C_DIGIT,
	C_DIGIT, C_DIGIT, C_COLON, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER,

	C_OTHER, C_ABCDF, C_ABCDF, C_ABCDF, C_ABCDF, C_E,     C_ABCDF, C_OTHER,
	C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER,
	C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_OTHER,
	C_OTHER, C_OTHER, C_OTHER, C_LSQRB, C_BACKS, C_RSQRB, C_OTHER, C_OTHER,

	C_OTHER, C_a,     C_b,     C_c,     C_d,     C_e,     C_f,     C_OTHER,
	C_OTHER, C_OTHER, C_OTHER, C_OTHER, C_l,     C_OTHER, C_n,     C_OTHER,
	C_OTHER, C_OTHER, C_r,     C_s,     C_t,     C_u,     C_OTHER, C_OTHER,
	C_OTHER, C_OTHER, C_OTHER, C_LCURB, C_OTHER, C_RCURB, C_OTHER, C_OTHER
};

/* define all states and actions that will be taken on each transition.
 *
 * states are defined first because of the fact they are use as index in the
 * transitions table. they usually contains either a number or a prefix _ 
 * for simple state like string, object, value ...
 *
 * actions are defined starting from 0x80. state error is defined as 0xff
 */

enum states {
	STATE_GO, /* start  */
	STATE_OK, /* ok     */
	STATE_OO, /* object */
	STATE_KK, /* key    */
	STATE_CO, /* colon  */
	STATE_VV, /* value  */
	STATE_AA, /* array  */
	STATE_SS, /* string */
	STATE_E0, /* escape */
	STATE_U1, STATE_U2, STATE_U3, STATE_U4, /* unicode states */
	STATE_M0, STATE_Z0, STATE_I0, /* number states */
	STATE_R1, STATE_R2, /* real states (after-dot digits) */
	STATE_X1, STATE_X2, STATE_X3, /* exponant states */
	STATE_T1, STATE_T2, STATE_T3, /* true constant states */
	STATE_F1, STATE_F2, STATE_F3, STATE_F4, /* false constant states */
	STATE_N1, STATE_N2, STATE_N3, /* null constant states */
	STATE_C1, STATE_C2, STATE_C3, /* C-comment states */
	STATE_Y1, /* YAML-comment state */
	STATE_D1, STATE_D2, /* multi unicode states */
};

/* the following are actions that need to be taken */
enum actions {
	STATE_KS = 0x80, /* key separator */
	STATE_SP, /* comma separator */
	STATE_AB, /* array begin */
	STATE_AE, /* array ending */
	STATE_OB, /* object begin */
	STATE_OE, /* object end */
	STATE_CB, /* C-comment begin */
	STATE_YB, /* YAML-comment begin */
	STATE_CE, /* YAML/C comment end */
	STATE_FA, /* false */
	STATE_TR, /* true */
	STATE_NU, /* null */
	STATE_DE, /* double detected by exponent */
	STATE_DF, /* double detected by . */
	STATE_SE, /* string end */
	STATE_MX, /* integer detected by minus */
	STATE_ZX, /* integer detected by zero */
	STATE_IX, /* integer detected by 1-9 */
	STATE_UC, /* Unicode character read */
};

/* some compilers have the following names defined */
#undef _C2
#undef ST_
#undef PT_

/* error state */
#define STATE___ 	0xff

#define NR_STATES 	(STATE_D2 + 1)
#define NR_CLASSES	(C_HASH + 1)

#define IS_STATE_ACTION(s) ((s) & 0x80)
#define ST_(x) STATE##x
#define PT_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,a1,b1,c1,d1,e1,f1,g1,h1)				\
	{ ST_(a),ST_(b),ST_(c),ST_(d),ST_(e),ST_(f),ST_(g),ST_(h),ST_(i),ST_(j),ST_(k),ST_(l),ST_(m),ST_(n),		\
	  ST_(o),ST_(p),ST_(q),ST_(r),ST_(s),ST_(t),ST_(u),ST_(v),ST_(w),ST_(x),ST_(y),ST_(z),ST_(a1),ST_(b1),		\
	  ST_(c1),ST_(d1),ST_(e1),ST_(f1),ST_(g1),ST_(h1) }

/* map from the (previous state+new character class) to the next parser transition */
static const uint8_t state_transition_table[NR_STATES][NR_CLASSES] = {
/*             white                                                                            ABCDF  other    */
/*          sp  nl  |   {   }   [   ]   :   ,   "   \   /   +   -   .   0   19  a   b   c   d   e   f   l   n   r   s   t   u   |   E   |   *   # */
/*GO*/ PT_(_GO,_GO,_GO,_OB,___,_AB,___,___,___,___,___,_CB,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_YB),
/*OK*/ PT_(_OK,_OK,_OK,___,_OE,___,_AE,___,_SP,___,___,_CB,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_YB),
/*OO*/ PT_(_OO,_OO,_OO,___,_OE,___,___,___,___,_SS,___,_CB,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_YB),
/*KK*/ PT_(_KK,_KK,_KK,___,___,___,___,___,___,_SS,___,_CB,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_YB),
/*CO*/ PT_(_CO,_CO,_CO,___,___,___,___,_KS,___,___,___,_CB,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_YB),
/*VV*/ PT_(_VV,_VV,_VV,_OB,___,_AB,___,___,___,_SS,___,_CB,___,_MX,___,_ZX,_IX,___,___,___,___,___,_F1,___,_N1,___,___,_T1,___,___,___,___,___,_YB),
/*AA*/ PT_(_AA,_AA,_AA,_OB,___,_AB,_AE,___,___,_SS,___,_CB,___,_MX,___,_ZX,_IX,___,___,___,___,___,_F1,___,_N1,___,___,_T1,___,___,___,___,___,_YB),
/**************************************************************************************************************************************************/
/*SS*/ PT_(_SS,___,___,_SS,_SS,_SS,_SS,_SS,_SS,_SE,_E0,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS,_SS),
/*E0*/ PT_(___,___,___,___,___,___,___,___,___,_SS,_SS,_SS,___,___,___,___,___,___,_SS,___,___,___,_SS,___,_SS,_SS,___,_SS,_U1,___,___,___,___,___),
/*U1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_U2,_U2,_U2,_U2,_U2,_U2,_U2,_U2,___,___,___,___,___,___,_U2,_U2,___,___,___),
/*U2*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_U3,_U3,_U3,_U3,_U3,_U3,_U3,_U3,___,___,___,___,___,___,_U3,_U3,___,___,___),
/*U3*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_U4,_U4,_U4,_U4,_U4,_U4,_U4,_U4,___,___,___,___,___,___,_U4,_U4,___,___,___),
/*U4*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_UC,_UC,_UC,_UC,_UC,_UC,_UC,_UC,___,___,___,___,___,___,_UC,_UC,___,___,___),
/**************************************************************************************************************************************************/
/*M0*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_Z0,_I0,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/*Z0*/ PT_(_OK,_OK,_OK,___,_OE,___,_AE,___,_SP,___,___,_CB,___,___,_DF,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_YB),
/*I0*/ PT_(_OK,_OK,_OK,___,_OE,___,_AE,___,_SP,___,___,_CB,___,___,_DF,_I0,_I0,___,___,___,___,_DE,___,___,___,___,___,___,___,___,_DE,___,___,_YB),
/*R1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_R2,_R2,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/*R2*/ PT_(_OK,_OK,_OK,___,_OE,___,_AE,___,_SP,___,___,_CB,___,___,___,_R2,_R2,___,___,___,___,_X1,___,___,___,___,___,___,___,___,_X1,___,___,_YB),
/*X1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,_X2,_X2,___,_X3,_X3,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/*X2*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_X3,_X3,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/*X3*/ PT_(_OK,_OK,_OK,___,_OE,___,_AE,___,_SP,___,___,___,___,___,___,_X3,_X3,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/**************************************************************************************************************************************************/
/*T1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_T2,___,___,___,___,___,___,___,___),
/*T2*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_T3,___,___,___,___,___),
/*T3*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_TR,___,___,___,___,___,___,___,___,___,___,___,___),
/*F1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_F2,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/*F2*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_F3,___,___,___,___,___,___,___,___,___,___),
/*F3*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_F4,___,___,___,___,___,___,___),
/*F4*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_FA,___,___,___,___,___,___,___,___,___,___,___,___),
/*N1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_N2,___,___,___,___,___),
/*N2*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_N3,___,___,___,___,___,___,___,___,___,___),
/*N3*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_NU,___,___,___,___,___,___,___,___,___,___),
/**************************************************************************************************************************************************/
/*C1*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_C2,___),
/*C2*/ PT_(_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C3,_C2),
/*C3*/ PT_(_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_CE,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C2,_C3,_C2),
/*Y1*/ PT_(_Y1,_CE,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1,_Y1),
/*D1*/ PT_(___,___,___,___,___,___,___,___,___,___,_D2,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___),
/*D2*/ PT_(___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,___,_U1,___,___,___,___,___),
};
/* map from (previous state+new character class) to the buffer policy. ignore=0/append=1/escape=2 */
static const uint8_t buffer_policy_table[NR_STATES][NR_CLASSES] = {
/*          white                                                                            ABCDF  other     */
/*      sp nl  |  {  }  [  ]  :  ,  "  \  /  +  -  .  0  19 a  b  c  d  e  f  l  n  r  s  t  u  |  E  |  *  # */
/*GO*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*OK*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*OO*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*KK*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*CO*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*VV*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*AA*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/**************************************************************************************************************/
/*SS*/ { 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
/*E0*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 0, 0, 0 },
/*U1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
/*U2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
/*U3*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
/*U4*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
/**************************************************************************************************************/
/*M0*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*Z0*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*I0*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
/*R1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*R2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
/*X1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*X2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*X3*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/**************************************************************************************************************/
/*T1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*T2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*T3*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*F1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*F2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*F3*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*F4*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*N1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*N2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*N3*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/**************************************************************************************************************/
/*C1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*C2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*C3*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*Y1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*D1*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
/*D2*/ { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

#define MODE_ARRAY 0
#define MODE_OBJECT 1

#define parser_malloc(parser, s) parser->config.user_malloc(s)
#define parser_realloc(parser, p, s) parser->config.user_realloc(p, s)
#define parser_free(parser, p) parser->config.user_free(p)

static int state_grow(json_parser *parser)
{
	uint32_t newsize = parser->stack_size * 2;
	void *ptr;

	if (parser->config.max_nesting != 0)
		return JSON_ERROR_NESTING_LIMIT;

	ptr = parser_realloc(parser, parser->stack, newsize * sizeof(uint8_t));
	if (!ptr)
		return JSON_ERROR_NO_MEMORY;
	parser->stack = (uint8_t *)ptr;
	parser->stack_size = newsize;
	return 0;
}

static int state_push(json_parser *parser, uint8_t mode)
{
	if (parser->stack_offset >= parser->stack_size) {
		int ret = state_grow(parser);
		if (ret)
			return ret;
	}
	parser->stack[parser->stack_offset++] = mode;
	return 0;
}

static int state_pop(json_parser *parser, uint8_t mode)
{
	if (parser->stack_offset == 0)
		return JSON_ERROR_POP_EMPTY;
	parser->stack_offset--;
	if (parser->stack[parser->stack_offset] != mode)
		return JSON_ERROR_POP_UNEXPECTED_MODE;
	return 0;
}

static int buffer_grow(json_parser *parser)
{
	uint32_t newsize;
	void *ptr;
	uint32_t max = parser->config.max_data;

	if (max > 0 && parser->buffer_size == max)
		return JSON_ERROR_DATA_LIMIT;
	newsize = parser->buffer_size * 2;
	if (max > 0 && newsize > max)
		newsize = max;

	ptr = parser_realloc(parser, parser->buffer, newsize * sizeof(char));
	if (!ptr)
		return JSON_ERROR_NO_MEMORY;
	parser->buffer = (char *)ptr;
	parser->buffer_size = newsize;
	return 0;
}

static int buffer_push(json_parser *parser, unsigned char c)
{
	int ret;

	if (parser->buffer_offset + 1 >= parser->buffer_size) {
		ret = buffer_grow(parser);
		if (ret)
			return ret;
	}
	parser->buffer[parser->buffer_offset++] = c;
	return 0;
}

static int do_callback_withbuf(json_parser *parser, int type)
{
	if (!parser->callback)
		return 0;
	parser->buffer[parser->buffer_offset] = '\0';
	return (*parser->callback)(parser->userdata, type, parser->buffer, parser->buffer_offset);
}

static int do_callback(json_parser *parser, int type)
{
	if (!parser->callback)
		return 0;
	return (*parser->callback)(parser->userdata, type, NULL, 0);
}

static int do_buffer(json_parser *parser)
{
	int ret = 0;

	switch (parser->type) {
	case JSON_KEY: case JSON_STRING:
	case JSON_FLOAT: case JSON_INT:
	case JSON_NULL: case JSON_TRUE: case JSON_FALSE:
		ret = do_callback_withbuf(parser, parser->type);
		if (ret)
			return ret;
		break;
	default:
		break;
	}
	parser->buffer_offset = 0;
	return ret;
}

static const uint8_t hextable[] = {
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,255,255,255,255,255,255,
	255, 10, 11, 12, 13, 14, 15,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255, 10, 11, 12, 13, 14, 15,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};

#define hex(c) (hextable[(uint8_t) c])

/* high surrogate range from d800 to dbff */
/* low surrogate range dc00 to dfff */
#define IS_HIGH_SURROGATE(uc) (((uc) & 0xfc00) == 0xd800)
#define IS_LOW_SURROGATE(uc)  (((uc) & 0xfc00) == 0xdc00)

/* transform an unicode [0-9A-Fa-f]{4} sequence into a proper value */
static int decode_unicode_char(json_parser *parser)
{
	uint32_t uval;
	char *b = parser->buffer;
	uint32_t offset = parser->buffer_offset;

	uval = (hex(b[offset - 4]) << 12) | (hex(b[offset - 3]) << 8)
	     | (hex(b[offset - 2]) << 4) | hex(b[offset - 1]);

	parser->buffer_offset -= 4;

	/* fast case */
	if (!parser->unicode_multi && uval < 0x80) {
		b[parser->buffer_offset++] = (char) uval;
		return 0;
	}

	if (parser->unicode_multi) {
		if (!IS_LOW_SURROGATE(uval))
			return JSON_ERROR_UNICODE_MISSING_LOW_SURROGATE;

		uval = 0x10000 + ((parser->unicode_multi & 0x3ff) << 10) + (uval & 0x3ff);
		b[parser->buffer_offset++] = (char) ((uval >> 18) | 0xf0);
		b[parser->buffer_offset++] = (char) (((uval >> 12) & 0x3f) | 0x80);
		b[parser->buffer_offset++] = (char) (((uval >> 6) & 0x3f) | 0x80);
		b[parser->buffer_offset++] = (char) ((uval & 0x3f) | 0x80);
		parser->unicode_multi = 0;
		return 0;
	}

	if (IS_LOW_SURROGATE(uval))
		return JSON_ERROR_UNICODE_UNEXPECTED_LOW_SURROGATE;
	if (IS_HIGH_SURROGATE(uval)) {
		parser->unicode_multi = (uint16_t)uval;
		return 0;
	}

	if (uval < 0x800) {
		b[parser->buffer_offset++] = (char) ((uval >> 6) | 0xc0);
		b[parser->buffer_offset++] = (char) ((uval & 0x3f) | 0x80);
	} else {
		b[parser->buffer_offset++] = (char) ((uval >> 12) | 0xe0);
		b[parser->buffer_offset++] = (char) (((uval >> 6) & 0x3f) | 0x80);
		b[parser->buffer_offset++] = (char) (((uval >> 0) & 0x3f) | 0x80);
	}
	return 0;
}

static int buffer_push_escape(json_parser *parser, unsigned char next)
{
	char c = '\0';

	switch (next) {
	case 'b': c = '\b'; break;
	case 'f': c = '\f'; break;
	case 'n': c = '\n'; break;
	case 'r': c = '\r'; break;
	case 't': c = '\t'; break;
	case '"': c = '"'; break;
	case '/': c = '/'; break;
	case '\\': c = '\\'; break;
	}
	/* push the escaped character */
	return buffer_push(parser, c);
}

#define CHK(f) { ret = f; if (ret) return ret; }

static int act_uc(json_parser *parser)
{
	int ret;
	CHK(decode_unicode_char(parser));
	parser->state = (uint8_t)((parser->unicode_multi) ? STATE_D1 : STATE_SS);
	return 0;
}

static int act_yb(json_parser *parser)
{
	if (!parser->config.allow_yaml_comments)
		return JSON_ERROR_COMMENT_NOT_ALLOWED;
	parser->save_state = parser->state;
	return 0;
}

static int act_cb(json_parser *parser)
{
	if (!parser->config.allow_c_comments)
		return JSON_ERROR_COMMENT_NOT_ALLOWED;
	parser->save_state = parser->state;
	return 0;
}

static int act_ce(json_parser *parser)
{
	parser->state = (parser->save_state > STATE_AA) ? (uint8_t)(STATE_OK) : (uint8_t)(parser->save_state);
	return 0;
}

static int act_ob(json_parser *parser)
{
	int ret;
	CHK(do_callback(parser, JSON_OBJECT_BEGIN));
	CHK(state_push(parser, MODE_OBJECT));
	parser->expecting_key = 1;
	return 0;
}

static int act_oe(json_parser *parser)
{
	int ret;
	CHK(state_pop(parser, MODE_OBJECT));
	CHK(do_callback(parser, JSON_OBJECT_END));
	parser->expecting_key = 0;
	return 0;
}

static int act_ab(json_parser *parser)
{
	int ret;
	CHK(do_callback(parser, JSON_ARRAY_BEGIN));
	CHK(state_push(parser, MODE_ARRAY));
	return 0;
}
static int act_ae(json_parser *parser)
{
	int ret;
	CHK(state_pop(parser, MODE_ARRAY));
	CHK(do_callback(parser, JSON_ARRAY_END));
	return 0;
}

static int act_se(json_parser *parser)
{
	int ret;
	CHK(do_callback_withbuf(parser, (parser->expecting_key) ? JSON_KEY : JSON_STRING));
	parser->buffer_offset = 0;
	parser->state = (uint8_t)((parser->expecting_key) ? STATE_CO : STATE_OK);
	parser->expecting_key = 0;
	return 0;
}

static int act_sp(json_parser *parser)
{
	if (parser->stack_offset == 0)
		return JSON_ERROR_COMMA_OUT_OF_STRUCTURE;
	if (parser->stack[parser->stack_offset - 1] == MODE_OBJECT) {
		parser->expecting_key = 1;
		parser->state = STATE_KK;
	} else
		parser->state = STATE_VV;
	return 0;
}

struct action_descr
{
	int (*call)(json_parser *parser);
	json_type type;
	uint8_t state; /* 0 if we let the callback set the value it want */
	uint8_t dobuffer;
};

static struct action_descr actions_map[] = {
	{ NULL,   JSON_NONE,  STATE_VV, 0 }, /* KS */
	{ act_sp, JSON_NONE,  0,        1 }, /* SP */
	{ act_ab, JSON_NONE,  STATE_AA, 0 }, /* AB */
	{ act_ae, JSON_NONE,  STATE_OK, 1 }, /* AE */
	{ act_ob, JSON_NONE,  STATE_OO, 0 }, /* OB */
	{ act_oe, JSON_NONE,  STATE_OK, 1 }, /* OE */
	{ act_cb, JSON_NONE,  STATE_C1, 1 }, /* CB */
	{ act_yb, JSON_NONE,  STATE_Y1, 1 }, /* YB */
	{ act_ce, JSON_NONE,  0,        0 }, /* CE */
	{ NULL,   JSON_FALSE, STATE_OK, 0 }, /* FA */
	{ NULL,   JSON_TRUE,  STATE_OK, 0 }, /* TR */
	{ NULL,   JSON_NULL,  STATE_OK, 0 }, /* NU */
	{ NULL,   JSON_FLOAT, STATE_X1, 0 }, /* DE */
	{ NULL,   JSON_FLOAT, STATE_R1, 0 }, /* DF */
	{ act_se, JSON_NONE,  0,        0 }, /* SE */
	{ NULL,   JSON_INT,   STATE_M0, 0 }, /* MX */
	{ NULL,   JSON_INT,   STATE_Z0, 0 }, /* ZX */
	{ NULL,   JSON_INT,   STATE_I0, 0 }, /* IX */
	{ act_uc, JSON_NONE,  0,        0 }, /* UC */
};

static int do_action(json_parser *parser, uint8_t next_state)
{
	struct action_descr *descr = &actions_map[next_state & ~0x80];

	if (descr->call) {
		int ret;
		if (descr->dobuffer)
			CHK(do_buffer(parser));
		CHK((descr->call)(parser));
	}
	if (descr->state)
		parser->state = descr->state;
	parser->type = descr->type;
	return 0;
}

/** json_parser_init initialize a parser structure taking a config,
 * a config and its userdata.
 * return JSON_ERROR_NO_MEMORY if memory allocation failed or SUCCESS.
 */
int json_parser_init(json_parser *parser, json_config *config,
                     json_parser_callback callback, void *userdata)
{
	memset(parser, 0, sizeof(*parser));

	if (config)
		memcpy(&parser->config, config, sizeof(json_config));
	parser->callback = callback;
	parser->userdata = userdata;

	/* initialise parsing stack and state */
	parser->stack_offset = 0;
	parser->state = STATE_GO;

	/* initialize the parse stack */
	parser->stack_size = (parser->config.max_nesting > 0)
		? parser->config.max_nesting
		: LIBJSON_DEFAULT_STACK_SIZE;

	parser->stack = (uint8_t *)parser_malloc(parser, parser->stack_size * sizeof(parser->stack[0]));
	if (!parser->stack)
		return JSON_ERROR_NO_MEMORY;

	/* initialize the parse buffer */
	parser->buffer_size = (parser->config.buffer_initial_size > 0)
		? parser->config.buffer_initial_size
		: LIBJSON_DEFAULT_BUFFER_SIZE;

	if (parser->config.max_data > 0 && parser->buffer_size > parser->config.max_data)
		parser->buffer_size = parser->config.max_data;

	parser->buffer = (char *)parser_malloc(parser, parser->buffer_size * sizeof(char));
	if (!parser->buffer) {
		parser_free(parser, parser->stack);
		return JSON_ERROR_NO_MEMORY;
	}
	return 0;
}

/** json_parser_free freed memory structure allocated by the parser */
int json_parser_free(json_parser *parser)
{
	if (!parser)
		return 0;
	parser_free(parser, parser->stack);
	parser_free(parser, parser->buffer);
	parser->stack = NULL;
	parser->buffer = NULL;
	return 0;
}

/** json_parser_is_done return 0 is the parser isn't in a finish state. !0 if it is */
int json_parser_is_done(json_parser *parser)
{
	/* need to compare the state to !GO to not accept empty document */
	return parser->stack_offset == 0 && parser->state != STATE_GO;
}

/** json_parser_string append a string s with a specific length to the parser
 * return 0 if everything went ok, a JSON_ERROR_* otherwise.
 * the user can supplied a valid processed pointer that will
 * be fill with the number of processed characters before returning */
int json_parser_string(json_parser *parser, const char *s,
                       uint32_t length, uint32_t *processed)
{
	int ret;
	uint8_t next_class, next_state;
	uint32_t buffer_policy;
	uint32_t i;

	ret = 0;
	for (i = 0; i < length; i++) {
		unsigned char ch = s[i];

		ret = 0;
		next_class = (ch >= 128) ? (uint8_t)(C_OTHER) : (uint8_t)(character_class[ch]);
		if (next_class == C_ERROR) {
			ret = JSON_ERROR_BAD_CHAR;
			break;
		}

		next_state = state_transition_table[parser->state][next_class];
		buffer_policy = buffer_policy_table[parser->state][next_class];
		if (next_state == STATE___) {
			ret = JSON_ERROR_UNEXPECTED_CHAR;
			break;
		}

		/* add char to buffer */
		if (buffer_policy) {
			ret = (buffer_policy == 2)
				? buffer_push_escape(parser, ch)
				: buffer_push(parser, ch);
			if (ret)
				break;
		}

		/* move to the next level */
		if (IS_STATE_ACTION(next_state))
			ret = do_action(parser, next_state);
		else
			parser->state = next_state;
		if (ret)
			break;
	}
	if (processed)
		*processed = i;
	return ret;
}

/** json_parser_char append one single char to the parser
 * return 0 if everything went ok, a JSON_ERROR_* otherwise */
int json_parser_char(json_parser *parser, unsigned char ch)
{
	return json_parser_string(parser, (char *) &ch, 1, NULL);
}
