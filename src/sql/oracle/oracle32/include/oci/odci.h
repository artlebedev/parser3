/*
 * $Header: /home/cvsroot/parser3project/parser3/src/sql/oracle/oracle32/include/oci/Attic/odci.h,v 1.1 2001/08/22 14:02:20 parser Exp $
 */

/* Copyright (c) Oracle Corporation 1998, 1999. All Rights Reserved. */ 
 
/* 
   NAME 
     odci.h - Oracle Data Cartridge Interface definitions

   DESCRIPTION 
     This file contains Oracle Data Cartridge Interface definitions. These
     include the ODCI Types and Constants.

   RELATED DOCUMENTS 
 
   INSPECTION STATUS 
     Inspection date: 
     Inspection status: 
     Estimated increasing cost defects per page: 
     Rule sets: 
 
   ACCEPTANCE REVIEW STATUS 
     Review date: 
     Review status: 
     Reviewers: 
 
   PUBLIC FUNCTION(S) 
     <list of external functions declared/defined - with one-line descriptions>

   PRIVATE FUNCTION(S)
     <list of static functions defined in .c file - with one-line descriptions>

   EXAMPLES

   NOTES
     - The constants defined here are replica of the constants defined 
       in ODCIConst Package defined as part of catodci.sql. If you change
       these do make the similar change in catodci.sql.

   MODIFIED   (MM/DD/YY)
   nagarwal    03/07/99 - bug# 838308 - set estimate_stats=1
   rmurthy     11/09/98 - add blocking flag
   ddas        10/31/98 - add ODCI_QUERY_SORT_ASC and ODCI_QUERY_SORT_DESC
   ddas        05/26/98 - fix ODCIPredInfo flag bits
   rmurthy     06/03/98 - add macro for RegularCall
   spsundar    05/08/98 - add constants related to ODCIIndexAlter options
   rmurthy     04/30/98 - remove include s.h
   rmurthy     04/20/98 - name fixes
   rmurthy     04/13/98 - add C mappings for odci types
   alsrivas    04/10/98 - adding defines for ODCI_INDEX1
   jsriniva    04/04/98 - Creation

*/

#ifndef OCI_ORACLE
# include <oci.h>
#endif
#ifndef ODCI_ORACLE
# define ODCI_ORACLE

/*---------------------------------------------------------------------------*/
/*                         SHORT NAMES SUPPORT SECTION                       */
/*---------------------------------------------------------------------------*/

#ifdef SLSHORTNAME

/* The following are short names that are only supported on IBM mainframes
 *   with the SLSHORTNAME defined.
 * With this all subsequent long names will actually be substituted with
 *  the short names here
 */

#define ODCIColInfo_ref             odcicir
#define ODCIColInfoList             odcicil
#define ODCIIndexInfo_ref           odciiir
#define ODCIPredInfo_ref            odcipir
#define ODCIRidList                 odcirl
#define ODCIIndexCtx_ref            odciicr
#define ODCIObject_ref              odcior
#define ODCIObjectList              odciol
#define ODCIQueryInfo_ref           odciqir
#define ODCIFuncInfo_ref            odcifir
#define ODCICost_ref                odcicr
#define ODCIArgDesc_ref             odciadr
#define ODCIArgDescList             odciadl
#define ODCIStatsOptions_ref        odcisor
#define ODCIColInfo                 odcici
#define ODCIColInfo_ind             odcicii
#define ODCIIndexInfo               odciii
#define ODCIIndexInfo_ind           odciiii
#define ODCIPredInfo                odcipi
#define ODCIPredInfo_ind            odcipii
#define ODCIIndexCtx                odciic
#define ODCIIndexCtx_ind            odciici
#define ODCIObject                  odcio
#define ODCIObject_ind              odcioi
#define ODCIQueryInfo               odciqi
#define ODCIQueryInfo_ind           odciqii
#define ODCIFuncInfo                odcifi
#define ODCIFuncInfo_infd           odcifii
#define ODCICost                    odcic
#define ODCICost_ind                odcici
#define ODCIArgDesc                 odciad
#define ODCIArgDesc_ind             odciadi
#define ODCIStatsOptions            odciso
#define ODCIStatsOptions_ind        odcisoi

#endif                                                        /* SLSHORTNAME */

/*---------------------------------------------------------------------------
                     PUBLIC TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/

/* Constants for Return Status */
#define ODCI_SUCCESS             0
#define ODCI_ERROR               1
#define ODCI_WARNING             2

/* Constants for ODCIPredInfo.Flags */
#define ODCI_PRED_EXACT_MATCH    0x0001
#define ODCI_PRED_PREFIX_MATCH   0x0002
#define ODCI_PRED_INCLUDE_START  0x0004
#define ODCI_PRED_INCLUDE_STOP   0x0008
#define ODCI_PRED_OBJECT_FUNC    0x0010
#define ODCI_PRED_OBJECT_PKG     0x0020
#define ODCI_PRED_OBJECT_TYPE    0x0040

/* Constants for QueryInfo.Flags */
#define ODCI_QUERY_FIRST_ROWS    0x01
#define ODCI_QUERY_ALL_ROWS      0x02
#define ODCI_QUERY_SORT_ASC      0x04
#define ODCI_QUERY_SORT_DESC     0x08
#define ODCI_QUERY_BLOCKING      0x10

/* Constants for ScnFlg(Func /w Index Context) */
#define ODCI_CLEANUP_CALL        1
#define ODCI_REGULAR_CALL        2

/* Constants for ODCIFuncInfo.Flags */
#define ODCI_OBJECT_FUNC         0x01
#define ODCI_OBJECT_PKG          0x02
#define ODCI_OBJECT_TYPE         0x04

/* Constants for ODCIArgDesc.ArgType */
#define ODCI_ARG_OTHER           1
#define ODCI_ARG_COL             2                                 /* column */
#define ODCI_ARG_LIT             3                                /* literal */
#define ODCI_ARG_ATTR            4                       /* object attribute */
#define ODCI_ARG_NULL            5

/* Constants for ODCIStatsOptions.Options */
#define ODCI_PERCENT_OPTION      1
#define ODCI_ROW_OPTION          2

/* Constants for ODCIStatsOptions.Flags */
#define ODCI_ESTIMATE_STATS     0x01
#define ODCI_COMPUTE_STATS      0x02
#define ODCI_VALIDATE           0x04

/* Constants for ODCIIndexAlter parameter alter_option */
#define ODCI_ALTIDX_NONE         0
#define ODCI_ALTIDX_RENAME       1
#define ODCI_ALTIDX_REBUILD      2
#define ODCI_ALTIDX_REBUILD_ONL  3

/*---------------------------------------------------------------------------
                     ODCI TYPES
  ---------------------------------------------------------------------------*/
/*
 * These are C mappings for the OTS types defined in catodci.sql
 */

typedef OCIRef ODCIColInfo_ref;
typedef OCIArray ODCIColInfoList;
typedef OCIRef ODCIIndexInfo_ref;
typedef OCIRef ODCIPredInfo_ref;
typedef OCIArray ODCIRidList;
typedef OCIRef ODCIIndexCtx_ref;
typedef OCIRef ODCIObject_ref;
typedef OCIArray ODCIObjectList;
typedef OCIRef ODCIQueryInfo_ref;
typedef OCIRef ODCIFuncInfo_ref;
typedef OCIRef ODCICost_ref;
typedef OCIRef ODCIArgDesc_ref;
typedef OCIArray ODCIArgDescList;
typedef OCIRef ODCIStatsOptions_ref;
 
struct ODCIColInfo
{
   OCIString* TableSchema;
   OCIString* TableName;
   OCIString* ColName;
   OCIString* ColTypName;
   OCIString* ColTypSchema;
};
typedef struct ODCIColInfo ODCIColInfo;
 
struct ODCIColInfo_ind
{
   OCIInd atomic;
   OCIInd TableSchema;
   OCIInd TableName;
   OCIInd ColName;
   OCIInd ColTypName;
   OCIInd ColTypSchema;
};
typedef struct ODCIColInfo_ind ODCIColInfo_ind;
 
struct ODCIIndexInfo
{
   OCIString*       IndexSchema;
   OCIString*       IndexName;
   ODCIColInfoList* IndexCols;
};
typedef struct ODCIIndexInfo ODCIIndexInfo;
 
struct ODCIIndexInfo_ind
{
   OCIInd atomic;
   OCIInd IndexSchema;
   OCIInd IndexName;
   OCIInd IndexCols;
};
typedef struct ODCIIndexInfo_ind ODCIIndexInfo_ind;
 
struct ODCIPredInfo
{
   OCIString* ObjectSchema;
   OCIString* ObjectName;
   OCIString* MethodName;
   OCINumber  Flags;
};
typedef struct ODCIPredInfo ODCIPredInfo;
 
struct ODCIPredInfo_ind
{
   OCIInd atomic;
   OCIInd ObjectSchema;
   OCIInd ObjectName;
   OCIInd MethodName;
   OCIInd Flags;
};
typedef struct ODCIPredInfo_ind ODCIPredInfo_ind;
 
struct ODCIIndexCtx
{
   struct ODCIIndexInfo IndexInfo;
   OCIString*           Rid;
};
typedef struct ODCIIndexCtx ODCIIndexCtx;
 
struct ODCIIndexCtx_ind
{
   OCIInd                   atomic;
   struct ODCIIndexInfo_ind IndexInfo;
   OCIInd                   Rid;
};
typedef struct ODCIIndexCtx_ind ODCIIndexCtx_ind;
 
struct ODCIObject
{
   OCIString* ObjectSchema;
   OCIString* ObjectName;
};
typedef struct ODCIObject ODCIObject;
 
struct ODCIObject_ind
{
   OCIInd atomic;
   OCIInd ObjectSchema;
   OCIInd ObjectName;
};
typedef struct ODCIObject_ind ODCIObject_ind;
 
struct ODCIQueryInfo
{
   OCINumber       Flags;
   ODCIObjectList* AncOps;
};
typedef struct ODCIQueryInfo ODCIQueryInfo;

 
struct ODCIQueryInfo_ind
{
   OCIInd atomic;
   OCIInd Flags;
   OCIInd AncOps;
};
typedef struct ODCIQueryInfo_ind ODCIQueryInfo_ind;
 
struct ODCIFuncInfo
{
   OCIString* ObjectSchema;
   OCIString* ObjectName;
   OCIString* MethodName;
   OCINumber Flags;
};
typedef struct ODCIFuncInfo ODCIFuncInfo;
 
struct ODCIFuncInfo_ind
{
   OCIInd atomic;
   OCIInd ObjectSchema;
   OCIInd ObjectName;
   OCIInd MethodName;
   OCIInd Flags;
};
typedef struct ODCIFuncInfo_ind ODCIFuncInfo_ind;
 
struct ODCICost
{
   OCINumber CPUcost;
   OCINumber IOcost;
   OCINumber NetworkCost;
};
typedef struct ODCICost ODCICost;
 
struct ODCICost_ind
{
   OCIInd atomic;
   OCIInd CPUcost;
   OCIInd IOcost;
   OCIInd NetworkCost;
};
typedef struct ODCICost_ind ODCICost_ind;
 
struct ODCIArgDesc
{
   OCINumber  ArgType;
   OCIString* TableName;
   OCIString* TableSchema;
   OCIString* ColName;
};
typedef struct ODCIArgDesc ODCIArgDesc;
 
struct ODCIArgDesc_ind
{
   OCIInd atomic;
   OCIInd ArgType;
   OCIInd TableName;
   OCIInd TableSchema;
   OCIInd ColName;
};
typedef struct ODCIArgDesc_ind ODCIArgDesc_ind;
 
struct ODCIStatsOptions
{
   OCINumber Sample;
   OCINumber Options;
   OCINumber Flags;
};
typedef struct ODCIStatsOptions ODCIStatsOptions;
 
struct ODCIStatsOptions_ind
{
   OCIInd atomic;
   OCIInd Sample;
   OCIInd Options;
   OCIInd Flags;
};
typedef struct ODCIStatsOptions_ind ODCIStatsOptions_ind;
 

/*---------------------------------------------------------------------------
                     PRIVATE TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                           PUBLIC FUNCTIONS
  ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                          PRIVATE FUNCTIONS
  ---------------------------------------------------------------------------*/


#endif                                              /* ODCI_ORACLE */
