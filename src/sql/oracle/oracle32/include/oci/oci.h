/*
 * $Header: /home/cvsroot/parser3project/parser3/src/sql/oracle/oracle32/include/oci/Attic/oci.h,v 1.1 2001/08/22 14:02:18 parser Exp $
 */

/* Copyright (c) Oracle Corporation 1995, 1996, 1997, 1998, 1999. 
All Rights Reserved. */
 
/* 
   NAME 
     oci.h - V8 Oracle Call Interface public definitions

   DESCRIPTION 
     This file defines all the constants and structures required by a V8
     OCI programmer.

   RELATED DOCUMENTS 
     V8 OCI Functional Specification 
     V8 OCI Design Specification
     Oracle Call Interface Programmer's Guide Vol 1 and 2
 
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
     None

   PRIVATE FUNCTION(S) 
     None
 
   EXAMPLES 
 
   NOTES 


   MODIFIED   (MM/DD/YY)
   porangas    09/21/99 - Correct lines to 80 characters wide: 974710
   slari       08/23/99 - add OCI_DTYPE_UCB
   slari       08/20/99 - add OCI_UCBTYPE_REPLACE
   sgollapu    08/02/99 - oci sql routing
   slari       08/06/99 - rename values for OCI_SERVER_STATUS
   slari       08/02/99 - add OCI_ATTR_SERVER_STATUS
   tnbui       07/28/99 - Remove OCI_DTYPE_TIMESTAMP_ITZ                       
   amangal     07/19/99 - Merge into 8.1.6 : bug 785797
   tnbui       07/07/99 - Change ADJUSTMENT modes                              
   dsaha       07/07/99 - OCI_SAHRED_EXT
   dmwong      06/08/99 - add OCI_ATTR_APPCTX_*
   vyanaman    06/23/99 -
   vyanaman    06/21/99 - Add new OCI Datetime and Interval descriptors
   esoyleme    06/29/99 - expose MTS performance enhancements                  
   rshaikh     04/23/99 - add OCI_SQL_VERSION_*
   tnbui       05/24/99 - Remove OCIAdjStr                                     
   dsaha       05/21/99 - Add OCI_ADJUST_UNK
   mluong      05/17/99 - fix merge
   tnbui       04/05/99 - ADJUSTMENT values
   abrumm      04/16/99 - dpapi: more attributes
   dsaha       02/24/99 - Add OCI_SHOW_DML_WARNINGS
   jiyang      12/07/98 - Add OCI_NLS_DUAL_CURRENCY
   slari       12/07/98 - change OCI_NOMUTEX to OCI_NO_MUTEX
   aroy        11/30/98 - change OCI_NOCALLBACK to OCI_NO_UCB
   aroy        11/13/98 - add env modes to process modes
   slari       09/08/98 - add OCI_FNCODE_SVC2HST and _SVCRH
   aroy        09/04/98 - Add OCI_ATTR_MIGSESSION
   skray       08/14/98 - server groups for session switching
   mluong      08/11/98 - add back OCI_HTYPE_LAST.
   aroy        05/25/98 - add process handle type                              
   aroy        04/06/98 - add shared mode                                      
   slari       07/13/98 -  merge forward to 8.1.4
   slari       07/09/98 -  add OCI_BIND_RESERVED_2
   slari       07/08/98 -  add OCI_EXACT_FETCH_RESERVED_1
   dsaha       07/07/98 -  Add OCI_PARSE_ONLY
   dsaha       06/29/98 -  Add OCI_PARSE_ONLY
   slari       07/01/98 -  add OCI_BIND_RESERVED_2
   sgollapu    06/25/98 -  Fix bug 683565
   slari       06/17/98 -  remove OC_FETCH_RESERVED_2
   slari       06/11/98 -  add OCI_FETCH_RESERVED_1 and 2
   jhasenbe    05/27/98 -  Remove definitions for U-Calls (Unicode)
   jiyang      05/18/98 - remove OCI_ATTR_CARTLANG
   nbhatt      05/20/98 -  OCI_DEQ_REMOVE_NODATA
   nbhatt      05/19/98 - correct AQ opcode
   skmishra    05/06/98 - Add precision attribute to Attributes list
   aroy        04/20/98 - merge forward 8.0.5 -> 8.1.3
   schandra    05/01/98 - OCI sender id
   sgollapu    02/19/98 - enhanced array DML
   nbhatt      05/15/98 -  AQ listen call
   sgollapu    04/27/98 - more attributes
   skaluska    04/06/98 - Add OCI_PTYPE_SCHEMA, OCI_PTYPE_DATABASE
   slari       04/28/98 - add OCI_ATTR_PDPRC
   lchidamb    05/05/98 - change OCI_NAMESPACE_AQ to 1
   nbhatt      04/27/98 - AQ Notification Descriptor
   abrumm      06/24/98 - more direct path attributes
   abrumm      05/27/98 - OCI direct path interface support
   abrumm      05/08/98 - OCI direct path interface support
   lchidamb    03/02/98 - client notification additions
   kkarun      04/17/98 - Add more Interval functions
   vyanaman    04/16/98 - Add get/set TZ
   kkarun      04/14/98 - Add OCI Datetime shortnames
   vyanaman    04/13/98 - Add OCI DateTime and Interval check error codes
   kkarun      04/07/98 - Add OCI_DTYPE_DATETIME and OCI_DTYPE_INTERVAL
   esoyleme    12/15/97 - support failover callback retry
   esoyleme    04/22/98 - merge support for failover callback retry
   mluong      04/16/98 - add OCI_FNCODE_LOBLOCATORASSIGN
   rkasamse    04/17/98 - add short names for OCIPickler(Memory/Ctx) cart servi
   slari       04/10/98 - add OCI_FNCODE_SVCCTXTOLDA
   slari       04/09/98 - add OCI_FNCODE_RESET
   slari       04/07/98 - add OCI_FNCODE_LOBFILEISOPEN
   slari       04/06/98 - add OCI_FNCODE_LOBOPEN
   slari       03/20/98 - change OCI_CBTYPE_xxx to OCI_UCBTYPE_xxx
   slari       03/18/98 - add OCI_FNCODE_MAXFCN
   slari       02/12/98 - add OCI_ENV_NO_USRCB
   skabraha    04/09/98 - adding shortnames for OCIFile
   rhwu        04/03/98 - Add short names for the OCIThread package
   tanguyen    04/03/98 - add OCI_ATTR_xxxx for type inheritance
   rkasamse    04/02/98 - add OCI_ATTR_UCI_REFRESH
   nramakri    04/01/98 - Add short names for the OCIExtract package
   ewaugh      03/31/98 - Add short names for the OCIFormat package.
   jhasenbe    04/06/98 - Add definitions for U-Calls (Unicode)
                          (OCI_TEXT, OCI_UTEXT, OCI_UTEXT4)
   skmishra    03/03/98 - Add OCI_ATTR_PARSE_ERROR_OFFSET
   rwessman    03/11/98 - Added OCI_CRED_PROXY for proxy authentication
   abrumm      03/31/98 - OCI direct path interface support
   nmallava    03/03/98 - add constants for temp lob apis
   skotsovo    03/05/98 - resolve merge conflicts
   skotsovo    02/24/98 - add OCI_DTYPE_LOC
   skaluska    01/21/98 - Add OCI_ATTR_LTYPE
   rkasamse    01/06/98 - add OCI_ATTR* for obj cache enhancements
   dchatter    01/08/98 - more comments
   skabraha    12/02/97 - moved oci1.h to the front of include files.
   jiyang      12/18/97 - Add OCI_NLS_MAX_BUFSZ
   rhwu        12/02/97 - move oci1.h up
   ewaugh      12/15/97 - Add short names for the OCIFormat package.
   rkasamse    12/02/97 - Add a constant for memory cartridge services -- OCI_M
   nmallava    12/31/97 - open/close for internal lobs
   khnguyen    11/27/97 - add OCI_ATTR_LFPRECISION, OCI_ATTR_FSPRECISION
   rkasamse    11/03/97 - add types for pickler cartridge services
   mluong      11/20/97 - changed ubig_ora to ub4 per skotsovo
   ssamu       11/14/97 - add oci1.h
   jiyang      11/13/97 - Add NLS service for cartridge
   esoyleme    12/15/97 - support failover callback retry
   jwijaya     10/21/97 - change OCILobOffset/Length from ubig_ora to ub4
   cxcheng     07/28/97 - fix compile with SLSHORTNAME
   schandra    06/25/97 - AQ OCI interface
   sgollapu    07/25/97 - Add OCI_ATTR_DESC_PUBLIC
   cxcheng     06/16/97 - add OCI_ATTR_TDO
   skotsovo    06/05/97 - add fntcodes for lob buffering subsystem
   esoyleme    05/13/97 - move failover callback prototype
   skmishra    05/06/97 - stdc compiler fixes
   skmishra    04/22/97 - Provide C++ compatibility
   lchidamb    04/19/97 - add OCI_ATTR_SESSLANG
   ramkrish    04/15/97 - Add OCI_LOB_BUFFER_(NO)FREE
   sgollapu    04/18/97 - Add OCI_ATTR_TABLESPACE
   skaluska    04/17/97 - Add OCI_ATTR_SUB_NAME
   schandra    04/10/97 - Use long OCI names
   aroy        03/27/97 - add OCI_DTYPE_FILE
   sgollapu    03/26/97 - Add OCI_OTYPEs
   skmishra    04/09/97 - Added constant OCI_ROWID_LEN
   dchatter    03/21/97 - add attr OCI_ATTR_IN_V8_MODE
   lchidamb    03/21/97 - add OCI_COMMIT_ON_SUCCESS execution mode
   skmishra    03/20/97 - Added OCI_ATTR_LOBEMPTY
   sgollapu    03/19/97 - Add OCI_ATTR_OVRLD_ID
   aroy        03/17/97 - add postprocessing callback
   sgollapu    03/15/97 - Add OCI_ATTR_PARAM
   cxcheng     02/07/97 - change OCI_PTYPE codes for type method for consistenc
   cxcheng     02/05/97 - add OCI_PTYPE_TYPE_RESULT
   cxcheng     02/04/97 - rename OCI_PTYPE constants to be more consistent
   cxcheng     02/03/97 - add OCI_ATTR, OCI_PTYPE contants for describe type
   esoyleme    01/23/97 - merge neerja callback
   sgollapu    12/30/96 - Remove OCI_DTYPE_SECURITY
   asurpur     12/26/96 - CHanging OCI_NO_AUTH to OCI_AUTH
   sgollapu    12/23/96 - Add more attrs to COL, ARG, and SEQ
   sgollapu    12/12/96 - Add OCI_DESCRIBE_ONLY
   slari       12/11/96 - change prototype of OCICallbackInBind
   nbhatt      12/05/96 - "callback"
   lchidamb    11/19/96 - handle subclassing
   sgollapu    11/09/96 - OCI_PATTR_*
   dchatter    11/04/96 - add attr OCI_ATTR_CHRCNT
   mluong      11/01/96 - test
   cxcheng     10/31/96 - add #defines for OCILobLength etc
   dchatter    10/31/96 - add lob read write call back fp defs
   dchatter    10/30/96 - more changes
   rhari       10/30/96 - Include ociextp.h at the very end
   lchidamb    10/22/96 - add fdo attribute for bind/server handle
   dchatter    10/22/96 - change attr defn for prefetch parameters & lobs/file
                          calls
   slari       10/21/96 - add OCI_ENV_NO_MUTEX
   rhari       10/25/96 - Include ociextp.h
   rxgovind    10/25/96 - add OCI_LOBMAXSIZE, remove OCI_FILE_READWRITE
   sgollapu    10/24/96 - Correct OCILogon and OCILogoff
   sgollapu    10/24/96 - Correct to OCILogon and OCILogoff
   sgollapu    10/21/96 - Add ocilon and ociloff
   skaluska    10/31/96 - Add OCI_PTYPE values
   sgollapu    10/17/96 - correct OCI_ATTR_SVCCTX to OCI_ATTR_SERVER
   rwessman    10/16/96 - Added security functions and fixed olint errors.
   sthakur     10/14/96 - add more COR attributes
   cxcheng     10/14/96 - re-enable LOB functions
   sgollapu    10/10/96 - Add ocibdp and ocibdn
   slari       10/07/96 - add back OCIRowid
   aroy        10/08/96 -  add typedef ocibfill for PRO*C
   mluong      10/11/96 - replace OCI_ATTR_CHARSET* with OCI_ATTR_CHARSET_*
   cxcheng     10/10/96 - temporarily take out #define for lob functions
   sgollapu    10/02/96 - Rename OCI functions and datatypes
   skotsovo    10/01/96 - move orl lob fnts to oci
   aroy        09/10/96 - fix merge errors
   aroy        08/19/96 - NCHAR support
   jboonleu    09/05/96 - add OCI attributes for object cache
   dchatter    08/20/96 - HTYPE ranges from 1-50; DTYPE from 50-255
   slari       08/06/96 - define OCI_DTYPE_ROWID
   sthakur     08/14/96 - complex object support
   schandra    06/17/96 - Convert XA to use new OCI
   abrik       08/15/96 - OCI_ATTR_HEAPALLOC added
   aroy        07/17/96 - terminology change: ocilobd => ocilobl
   aroy        07/03/96 - add lob typedefs for Pro*C
   slari       06/28/96 - add OCI_ATTR_STMT_TYPE
   lchidamb    06/26/96 - reorg #ifndef
   schandra    05/31/96 - attribute types for internal and external client name
   asurpur     05/30/96 - Changing the value of mode
   schandra    05/18/96 - OCI_TRANS_TWOPHASE -> 0x00000001 to 0x00100000
   slari       05/30/96 - add callback function prototypes
   jbellemo    05/23/96 - remove ociisc
   schandra    04/23/96 - loosely-coupled branches
   asurpur     05/15/96 - New mode for ocicpw
   aroy        04/24/96 - making ocihandles opaque
   slari       04/18/96 - add missing defines
   schandra    03/27/96 - V8OCI - add transaction related calls
   dchatter    04/01/96 - add OCI_FILE options
   dchatter    03/21/96 - add oci2lda conversion routines
   dchatter    03/07/96 - add OCI piece definition
   slari       03/12/96 - add describe attributes
   slari       03/12/96 - add OCI_OTYPE_QUERY
   aroy        02/28/96 - Add column attributes
   slari       02/09/96 - add OCI_OBJECT
   slari       02/07/96 - add OCI_HYTPE_DSC
   aroy        01/10/96 - adding function code defines...
   dchatter    01/03/96 - define OCI_NON_BLOCKING
   dchatter    01/02/96 - Add Any descriptor
   dchatter    01/02/96 - Add Select List descriptor
   dchatter    12/29/95 - V8 OCI definitions
   dchatter    12/29/95 - Creation

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ORATYPES 
#include <oratypes.h> 
#endif
 
#ifndef OCIDFN
#include <ocidfn.h>
#endif

#ifndef OCI_ORACLE
# define OCI_ORACLE

 
/*--------------------------------------------------------------------------- 
 Short names provided for platforms which do not allow extended symbolic names 
  ---------------------------------------------------------------------------*/

#ifdef SLSHORTNAME
/* Translation of the long function/type names to short names for IBM only */
/* maybe lint will use this too */
#define OCISessionEnd              ocitac
#define OCIResultSetToStmt         ocirs2sh
#define OCISessionBegin            ociauth
#define OCIServerAttach            ociatch
#define OCIDescriptorAlloc         ocigdesc
#define OCIServerDetach            ocidtch
#define OCIDescriptorFree          ocifdesc
#define OCIServerVersion           ocivers
#define OCIDescribeAny             ocidsca
#define OCIBindDynamic             ocibda
#define OCIBindByName              ocibdn
#define OCIBindByPos               ocibdp
#define OCIErrorGet                ocigdr
#define OCIBindArrayOfStruct       ocibsa
#define OCIEnvInit                 ociinit
#define OCIBindObject              ocibndt
#define OCIHandleAlloc             ocighndl
#define OCIHandleFree              ocifhndl
#ifdef NEVER
#define OCIStmtBindByPos           ocibndp
#define OCIStmtBindByName          ocibndn
#endif
#define OCIAttrGet                 ocigattr
#define OCIDefineByPos             ocidfne
#define OCIAttrSet                 ocisattr
#define OCIDefineDynamic           ociddf
#define OCILdaToSvcCtx             ocild2sv
#define OCIDefineArrayOfStruct     ocidarr
#define OCIInitialize              ocipi
#define OCIDefineObject            ocidndt
#define OCIStmtExecute             ociexec
#define OCILobAppend               ocilfap
#define OCILobOpenFile             ocifopn
#define OCILobCloseFile            ocifcls
#define OCILobLocator              ocilobd
#define OCILobCopy                 ocilfcp
#define OCILobFileCreate           ocifcrt
#define OCILobFileDelete           ocifdel
#define OCILobGetLength            ocilfln
#define OCILobWrite                ocilfwr
#define OCILobRead                 ocilfrd
#define OCILobErase                ocilfer
#define OCILobTrim                 ocilftr

#define OCIStmtFetch               ocifch
#define OCIStmtGetBindInfo         ocigbp
#define OCIStmtGetPieceInfo        ocigpi
#define OCIStmtPrepare             ocireq
#define OCIStmtSetPieceInfo        ocispi
#define OCISvcCtxToLda             ocisv2ld
#define OCITransCommit             ocitxcm
#define OCITransDetach             ocitxdt
#define OCITransForget             ocitxfgt
#define OCITransPrepare            ocitxpre
#define OCITransRollback           ocitxrl
#define OCIPasswordChange          ocicpw
#define OCITransStart              ocitxst
#define OCITransMultiPrepare       ocitxmp

#define OCIBreak                   ocibreak
#define OCIParamGet                ocigparm
#define OCIParamSet                ocisparm

#define OCISecurityOpenWallet           ocizwOpenWallet
#define OCISecurityCloseWallet          ocizwCloseWallet
#define OCISecurityCreateWallet         ocizwCreateWallet
#define OCISecurityDestroyWallet        ocizwDestroyWallet
#define OCISecurityStorePersona         ocizeStorePersona
#define OCISecurityOpenPersona          ocizeOpenPersona
#define OCISecurityClosePersona         ocizeClosePersona
#define OCISecurityRemovePersona        ocizeRemovePersona
#define OCISecurityCreatePersona        ocizeCreatePersona
#define OCISecuritySetProtection        ocizeSetProtection
#define OCISecurityGetProtection        ocizeGetProtection
#define OCISecurityRemoveIdentity       ociziRemoveIdentity
#define OCISecurityCreateIdentity       ociziCreateIdentity
#define OCISecurityAbortIdentity        ociziAbortIdentity
#define OCISecurityFreeIdentity         ociziFreeIdentity
#define OCISecurityStoreTrustedIdentity ociziStoreTrustedIdentity
#define OCISecuritySign                 ocizSign
#define OCISecuritySignExpansion        ocizxSignExpansion
#define OCISecurityVerify               ocizVerify
#define OCISecurityValidate             ocizValidate
#define OCISecuritySignDetached         ocizsd_SignDetached
#define OCISecuritySignDetExpansion     ocizxsd_SignDetachedExpansion
#define OCISecurityVerifyDetached       ocizved_VerifyDetached
#define OCISecurity_PKEncrypt           ocizkec_PKEncrypt
#define OCISecurityPKEncryptExpansion   ocizxkec_PKEncryptExpansion
#define OCISecurityPKDecrypt            ocizkdc_PKDecrypt
#define OCISecurityEncrypt              ocizEncrypt
#define OCISecurityEncryptExpansion     ocizxEncryptExpansion
#define OCISecurityDecrypt              ocizDecrypt
#define OCISecurityEnvelope             ocizEnvelope
#define OCISecurityDeEnvelope           ocizDeEnvelope
#define OCISecurityKeyedHash            ocizKeyedHash
#define OCISecurityKeyedHashExpansion   ocizxKeyedHashExpansion
#define OCISecurityHash                 ocizHash
#define OCISecurityHashExpansion        ocizxHashExpansion
#define OCISecuritySeedRandom           ocizSeedRandom
#define OCISecurityRandomBytes          ocizrb_RandomBytes
#define OCISecurityRandomNumber         ocizrn_RandomNumber
#define OCISecurityInitBlock            ocizibInitBlock
#define OCISecurityReuseBlock           ocizrbReuseBlock
#define OCISecurityPurgeBlock           ocizpbPurgeBlock
#define OCISecuritySetBlock             ocizsbSetBlock
#define OCISecurityGetIdentity          ocizgi_GetIdentity

#define OCIExtractInit             ocixeini
#define OCIExtractTerm             ocixetrm
#define OCIExtractReset            ocixerst
#define OCIExtractSetNumKeys       ocixesnk
#define OCIExtractSetKey           ocixesk
#define OCIExtractFromFile         ocixeff
#define OCIExtractFromStr          ocixefs
#define OCIExtractToInt            ocixeti
#define OCIExtractToBool           ocixetb
#define OCIExtractToStr            ocixets
#define OCIExtractToOCINum         ocixeton
#define OCIExtractToList           ocixetl
#define OCIExtractFromList         ocixefl

#define OCIDateTimeGetTime         ocidt01_GetTime
#define OCIDateTimeGetDate         ocidt02_GetDate
#define OCIDateTimeGetTimeZoneOffset     ocidt03_GetTZ
#define OCIDateTimeSysTimeStamp    ocidt07_SysTS
#define OCIDateTimeAssign          ocidt08_Assign 
#define OCIDateTimeToText          ocidt09_ToText
#define OCIDateTimeFromText        ocidt10_FromText
#define OCIDateTimeCompare         ocidt11_Compare
#define OCIDateTimeCheck           ocidt12_Check
#define OCIDateTimeConvert         ocidt13_Convert
#define OCIDateTimeSubtract        ocidt14_Subtract
#define OCIDateTimeIntervalAdd     ocidt15_IntervalAdd
#define OCIDateTimeIntervalSub     ocidt16_IntervalSub

#define OCIIntervalSubtract        ociint01_Subtract  
#define OCIIntervalAdd             ociint02_Add  
#define OCIIntervalMultiply        ociint03_Multiply  
#define OCIIntervalDivide          ociint04_Divide  
#define OCIIntervalCompare         ociint05_Compare  
#define OCIIntervalFromText        ociint06_FromText  
#define OCIIntervalToText          ociint07_ToText  
#define OCIIntervalToNumber        ociint08_ToNumber  
#define OCIIntervalCheck           ociint09_Check  
#define OCIIntervalAssign          ociint10_Assign  
#define OCIIntervalGetYearMonth    ociint11_GetYearMonth
#define OCIIntervalSetYearMonth    ociint12_SetYearMonth
#define OCIIntervalGetDaySecond    ociint13_GetDaySecond
#define OCIIntervalSetDaySecond    ociint14_SetDaySecond
#define OCIIntervalFromNumber      ociint15_FromNumber

#define OCIFormatInit              ocixs01_Init
#define OCIFormatString            ocixs02_Format
#define OCIFormatTerm              ocixs03_Term
#define OCIFormatTUb1              ocixs04_TUb1
#define OCIFormatTUb2              ocixs05_TUb2
#define OCIFormatTUb4              ocixs06_TUb4
#define OCIFormatTUword            ocixs07_TUword
#define OCIFormatTUbig_ora         ocixs08_TUbig_ora
#define OCIFormatTSb1              ocixs09_TSb1
#define OCIFormatTSb2              ocixs10_TSb2
#define OCIFormatTSb4              ocixs11_TSb4
#define OCIFormatTSword            ocixs12_TSword
#define OCIFormatTSbig_ora         ocixs13_TSbig_ora
#define OCIFormatTEb1              ocixs14_TEb1
#define OCIFormatTEb2              ocixs15_TEb2
#define OCIFormatTEb4              ocixs16_TEb4
#define OCIFormatTEword            ocixs17_TEword
#define OCIFormatTChar             ocixs18_TChar
#define OCIFormatTText             ocixs19_TText
#define OCIFormatTDouble           ocixs20_TDouble
#define OCIFormatTDvoid            ocixs21_TDvoid
#define OCIFormatTEnd              ocixs22_TEnd

#define OCIFileInit                ocifinit
#define OCIFileTerm                ocifterm
#define OCIFileOpen                ocifopen
#define OCIFileClose               ocifclose
#define OCIFileRead                ocifread
#define OCIFileWrite               ocifwrite
#define OCIFileSeek                ocifseek
#define OCIFileExists              ocifexists
#define OCIFileGetLength           ocifglen
#define OCIFileFlush               ocifflush


/* OCIThread short name */
#define OCIThreadProcessInit       ocitt01_ProcessInit
#define OCIThreadInit              ocitt02_Init
#define OCIThreadTerm              ocitt03_Term
#define OCIThreadIsMulti           ocitt04_IsMulti
#define OCIThreadMutexInit         ocitt05_MutexInit
#define OCIThreadMutexDestroy      ocitt06_MutexDestroy
#define OCIThreadMutexAcquire      ocitt07_MutexAcquire
#define OCIThreadMutexRelease      ocitt08_MutexRelease
#define OCIThreadKeyInit           ocitt09_KeyInit
#define OCIThreadKeyDestroy        ocitt10_KeyDestroy
#define OCIThreadKeyGet            ocitt11_KeyGet
#define OCIThreadKeySet            ocitt12_KeySet
#define OCIThreadIdInit            ocitt13_IdInit
#define OCIThreadIdDestroy         ocitt14_IdDestroy
#define OCIThreadIdSet             ocitt15_IdSet
#define OCIThreadIdSetNull         ocitt16_IdSetNull
#define OCIThreadIdGet             ocitt17_IdGet
#define OCIThreadIdSame            ocitt18_IdSame
#define OCIThreadIdNull            ocitt19_IdNull
#define OCIThreadHndInit           ocitt20_HndInit
#define OCIThreadHndDestroy        ocitt21_HndDestroy
#define OCIThreadCreate            ocitt22_Create
#define OCIThreadJoin              ocitt23_Join
#define OCIThreadClose             ocitt24_Close
#define OCIThreadHandleGet         ocitt25_HandleGet

/* Translation between the old and new datatypes */

#define OCISession                 ociusrh
#define OCIBind                    ocibndh
#define OCIDescribe                ocidsch
#define OCIDefine                  ocidfnh
#define OCIEnv                     ocienvh
#define OCIError                   ocierrh

#define OCILob                     ocilobd
#define OCILobLength               ocillen
#define OCILobMode                 ocilmo
#define OCILobOffset               ociloff

#define OCILobLocator              ocilobd
#define OCIBlobLocator		   ociblobl
#define OCIClobLocator		   ociclobl
#define OCIBFileLocator            ocibfilel

#define OCIParam                   ocipard
#define OCIResult                  ocirstd
#define OCISnapshot                ocisnad
#define OCIServer                  ocisrvh
#define OCIStmt                    ocistmh
#define OCISvcCtx                  ocisvch
#define OCITrans                   ocitxnh
#define OCICallbackInBind          ocibicfp
#define OCICallbackOutBind         ocibocfp
#define OCICallbackDefine          ocidcfp
#define OCICallbackLobRead         ocilrfp
#define OCICallbackLobWrite        ocilwfp
#define OCISecurity                ociossh
#define OCIComplexObject           ocicorh
#define OCIComplexObjectComp       ocicord
#define OCIRowid                   ociridd

#endif /* ifdef SLSHORTNAME */

/*--------------------------------------------------------------------------- 
                     PUBLIC TYPES AND CONSTANTS 
  ---------------------------------------------------------------------------*/

/*-----------------------------Handle Types----------------------------------*/
                                           /* handle types range from 1 - 49 */
#define OCI_HTYPE_FIRST          1             /* start value of handle type */
#define OCI_HTYPE_ENV            1                     /* environment handle */
#define OCI_HTYPE_ERROR          2                           /* error handle */
#define OCI_HTYPE_SVCCTX         3                         /* service handle */
#define OCI_HTYPE_STMT           4                       /* statement handle */
#define OCI_HTYPE_BIND           5                            /* bind handle */
#define OCI_HTYPE_DEFINE         6                          /* define handle */
#define OCI_HTYPE_DESCRIBE       7                        /* describe handle */
#define OCI_HTYPE_SERVER         8                          /* server handle */
#define OCI_HTYPE_SESSION        9                  /* authentication handle */
#define OCI_HTYPE_TRANS         10                     /* transaction handle */
#define OCI_HTYPE_COMPLEXOBJECT 11        /* complex object retrieval handle */
#define OCI_HTYPE_SECURITY      12                        /* security handle */
#define OCI_HTYPE_SUBSCRIPTION  13                    /* subscription handle */
#define OCI_HTYPE_DIRPATH_CTX   14                    /* direct path context */
#define OCI_HTYPE_DIRPATH_COLUMN_ARRAY 15        /* direct path column array */
#define OCI_HTYPE_DIRPATH_STREAM       16              /* direct path stream */
#define OCI_HTYPE_PROC          17                         /* process handle */
#define OCI_HTYPE_LAST          17            /* last value of a handle type */

/*---------------------------------------------------------------------------*/


/*-------------------------Descriptor Types----------------------------------*/
                                    /* descriptor values range from 50 - 255 */
#define OCI_DTYPE_FIRST 50                 /* start value of descriptor type */
#define OCI_DTYPE_LOB 50                                     /* lob  locator */
#define OCI_DTYPE_SNAP 51                             /* snapshot descriptor */
#define OCI_DTYPE_RSET 52                           /* result set descriptor */
#define OCI_DTYPE_PARAM 53  /* a parameter descriptor obtained from ocigparm */
#define OCI_DTYPE_ROWID  54                              /* rowid descriptor */
#define OCI_DTYPE_COMPLEXOBJECTCOMP  55
                                      /* complex object retrieval descriptor */
#define OCI_DTYPE_FILE 56                                /* File Lob locator */
#define	OCI_DTYPE_AQENQ_OPTIONS 57			  /* enqueue options */
#define	OCI_DTYPE_AQDEQ_OPTIONS 58			  /* dequeue options */
#define OCI_DTYPE_AQMSG_PROPERTIES 59		       /* message properties */
#define OCI_DTYPE_AQAGENT 60					 /* aq agent */
#define OCI_DTYPE_LOCATOR 61                                  /* LOB locator */
#define OCI_DTYPE_INTERVAL_YM 62                 /* Interval year month */
#define OCI_DTYPE_INTERVAL_DS 63                 /* Interval day second */
#define OCI_DTYPE_AQNFY_DESCRIPTOR  64		     /* AQ notify descriptor */
#define OCI_DTYPE_DATE 65                            /* Date */
#define OCI_DTYPE_TIME 66                            /* Time */
#define OCI_DTYPE_TIME_TZ 67                         /* Time with timezone */
#define OCI_DTYPE_TIMESTAMP 68                       /* Timestamp */
#define OCI_DTYPE_TIMESTAMP_TZ 69                /* Timestamp with timezone */
#define OCI_DTYPE_TIMESTAMP_LTZ 70             /* Timestamp with local tz */
#define OCI_DTYPE_UCB		71               /* user callback descriptor */
#define OCI_DTYPE_LAST		71        /* last value of a descriptor type */
/*---------------------------------------------------------------------------*/

/*--------------------------------LOB types ---------------------------------*/
#define OCI_TEMP_BLOB 1                /* LOB type - BLOB ------------------ */
#define OCI_TEMP_CLOB 2                /* LOB type - CLOB ------------------ */
/*---------------------------------------------------------------------------*/

/*-------------------------Object Ptr Types----------------------------------*/
#define OCI_OTYPE_NAME 1				      /* object name */
#define OCI_OTYPE_REF  2				       /* REF to TDO */
#define OCI_OTYPE_PTR  3				       /* PTR to TDO */
/*---------------------------------------------------------------------------*/

/*=============================Attribute Types===============================*/
/* 
   Note: All attributes are global.  New attibutes should be added to the end
   of the list. Before you add an attribute see if an existing one can be 
   used for your handle. 

   If you see any holes please use the holes first. 
 
*/
/*===========================================================================*/


#define OCI_ATTR_FNCODE  1                          /* the OCI function code */
#define OCI_ATTR_OBJECT   2 /* is the environment initialized in object mode */
#define OCI_ATTR_NONBLOCKING_MODE  3                    /* non blocking mode */
#define OCI_ATTR_SQLCODE  4                                  /* the SQL verb */
#define OCI_ATTR_ENV  5                            /* the environment handle */
#define OCI_ATTR_SERVER 6                               /* the server handle */
#define OCI_ATTR_SESSION 7                        /* the user session handle */
#define OCI_ATTR_TRANS   8                         /* the transaction handle */
#define OCI_ATTR_ROW_COUNT   9                  /* the rows processed so far */
#define OCI_ATTR_SQLFNCODE 10               /* the SQL verb of the statement */
#define OCI_ATTR_PREFETCH_ROWS  11    /* sets the number of rows to prefetch */
#define OCI_ATTR_NESTED_PREFETCH_ROWS 12 /* the prefetch rows of nested table*/
#define OCI_ATTR_PREFETCH_MEMORY 13         /* memory limit for rows fetched */
#define OCI_ATTR_NESTED_PREFETCH_MEMORY 14   /* memory limit for nested rows */
#define OCI_ATTR_CHAR_COUNT  15 
                    /* this specifies the bind and define size in characters */
#define OCI_ATTR_PDSCL   16                          /* packed decimal scale */
#define OCI_ATTR_FSPRECISION OCI_ATTR_PDSCL   
                                          /* fs prec for datetime data types */
#define OCI_ATTR_PDPRC   17                         /* packed decimal format */
#define OCI_ATTR_LFPRECISION OCI_ATTR_PDPRC 
                                          /* fs prec for datetime data types */
#define OCI_ATTR_PARAM_COUNT 18       /* number of column in the select list */
#define OCI_ATTR_ROWID   19                                     /* the rowid */
#define OCI_ATTR_CHARSET  20                      /* the character set value */
#define OCI_ATTR_NCHAR   21                                    /* NCHAR type */
#define OCI_ATTR_USERNAME 22                           /* username attribute */
#define OCI_ATTR_PASSWORD 23                           /* password attribute */
#define OCI_ATTR_STMT_TYPE   24                            /* statement type */
#define OCI_ATTR_INTERNAL_NAME   25             /* user friendly global name */
#define OCI_ATTR_EXTERNAL_NAME   26      /* the internal name for global txn */
#define OCI_ATTR_XID     27           /* XOPEN defined global transaction id */
#define OCI_ATTR_TRANS_LOCK 28                                            /* */
#define OCI_ATTR_TRANS_NAME 29    /* string to identify a global transaction */
#define OCI_ATTR_HEAPALLOC 30                /* memory allocated on the heap */
#define OCI_ATTR_CHARSET_ID 31                           /* Character Set ID */
#define OCI_ATTR_CHARSET_FORM 32                       /* Character Set Form */
#define OCI_ATTR_MAXDATA_SIZE 33       /* Maximumsize of data on the server  */
#define OCI_ATTR_CACHE_OPT_SIZE 34              /* object cache optimal size */
#define OCI_ATTR_CACHE_MAX_SIZE 35   /* object cache maximum size percentage */
#define OCI_ATTR_PINOPTION 36             /* object cache default pin option */
#define OCI_ATTR_ALLOC_DURATION 37
                                 /* object cache default allocation duration */
#define OCI_ATTR_PIN_DURATION 38        /* object cache default pin duration */
#define OCI_ATTR_FDO          39       /* Format Descriptor object attribute */
#define OCI_ATTR_POSTPROCESSING_CALLBACK 40
                                         /* Callback to process outbind data */
#define OCI_ATTR_POSTPROCESSING_CONTEXT 41
                                 /* Callback context to process outbind data */
#define OCI_ATTR_ROWS_RETURNED 42
               /* Number of rows returned in current iter - for Bind handles */
#define OCI_ATTR_FOCBK        43              /* Failover Callback attribute */
#define OCI_ATTR_IN_V8_MODE   44 /* is the server/service context in V8 mode */
#define OCI_ATTR_LOBEMPTY     45                              /* empty lob ? */
#define OCI_ATTR_SESSLANG     46                  /* session language handle */

#define OCI_ATTR_VISIBILITY		47		       /* visibility */
#define OCI_ATTR_RELATIVE_MSGID		48  	      /* relative message id */
#define OCI_ATTR_SEQUENCE_DEVIATION	49	       /* sequence deviation */

#define OCI_ATTR_CONSUMER_NAME		50		    /* consumer name */
#define OCI_ATTR_DEQ_MODE		51		     /* dequeue mode */
#define OCI_ATTR_NAVIGATION		52		       /* navigation */
#define OCI_ATTR_WAIT			53			     /* wait */
#define OCI_ATTR_DEQ_MSGID		54	       /* dequeue message id */

#define OCI_ATTR_PRIORITY		55			 /* priority */
#define OCI_ATTR_DELAY			56			    /* delay */
#define OCI_ATTR_EXPIRATION		57		       /* expiration */
#define OCI_ATTR_CORRELATION		58		   /* correlation id */
#define OCI_ATTR_ATTEMPTS		59		    /* # of attempts */
#define OCI_ATTR_RECIPIENT_LIST		60		   /* recipient list */
#define OCI_ATTR_EXCEPTION_QUEUE	61	     /* exception queue name */
#define OCI_ATTR_ENQ_TIME		62 /* enqueue time (only OCIAttrGet) */
#define OCI_ATTR_MSG_STATE		63/* message state (only OCIAttrGet) */
                                                   /* NOTE: 64-66 used below */
#define OCI_ATTR_AGENT_NAME		64		       /* agent name */
#define OCI_ATTR_AGENT_ADDRESS		65		    /* agent address */
#define OCI_ATTR_AGENT_PROTOCOL		66		   /* agent protocol */

#define OCI_ATTR_SENDER_ID		68			/* sender id */
#define OCI_ATTR_ORIGINAL_MSGID		69	      /* original message id */

#define OCI_ATTR_QUEUE_NAME		70		       /* queue name */
#define OCI_ATTR_NFY_MSGID              71		       /* message id */
#define OCI_ATTR_MSG_PROP               72	       /* message properties */

#define OCI_ATTR_NUM_DML_ERRORS         73       /* num of errs in array DML */
#define OCI_ATTR_DML_ROW_OFFSET         74        /* row offset in the array */

#define OCI_ATTR_DATEFORMAT             75     /* default date format string */
#define OCI_ATTR_BUF_ADDR               76                 /* buffer address */
#define OCI_ATTR_BUF_SIZE               77                    /* buffer size */
#define OCI_ATTR_DIRPATH_MODE           78  /* mode of direct path operation */
#define OCI_ATTR_DIRPATH_NOLOG          79               /* nologging option */
#define OCI_ATTR_DIRPATH_PARALLEL       80     /* parallel (temp seg) option */
#define OCI_ATTR_NUM_ROWS               81 /* number of rows in column array */
                                  /* NOTE that OCI_ATTR_NUM_COLS is a column
				   * array attribute too.
				   */
#define OCI_ATTR_COL_COUNT              82        /* columns of column array
					             processed so far.       */
#define OCI_ATTR_STREAM_OFFSET          83  /* str off of last row processed */
#define OCI_ATTR_SHARED_HEAPALLOC       84    /* Shared Heap Allocation Size */

#define OCI_ATTR_SERVER_GROUP           85              /* server group name */

#define OCI_ATTR_MIGSESSION             86   /* migratable session attribute */

#define OCI_ATTR_NOCACHE                87                 /* Temporary LOBs */

#define OCI_ATTR_MEMPOOL_SIZE           88                      /* Pool Size */
#define OCI_ATTR_MEMPOOL_INSTNAME       89                  /* Instance name */
#define OCI_ATTR_MEMPOOL_APPNAME        90               /* Application name */
#define OCI_ATTR_MEMPOOL_HOMENAME       91            /* Home Directory name */
#define OCI_ATTR_MEMPOOL_MODEL          92     /* Pool Model (proc,thrd,both)*/
#define OCI_ATTR_MODES                  93                          /* Modes */

#define OCI_ATTR_SUBSCR_NAME            94           /* name of subscription */
#define OCI_ATTR_SUBSCR_CALLBACK        95            /* associated callback */
#define OCI_ATTR_SUBSCR_CTX             96    /* associated callback context */
#define OCI_ATTR_SUBSCR_PAYLOAD         97             /* associated payload */
#define OCI_ATTR_SUBSCR_NAMESPACE       98           /* associated namespace */

#define OCI_ATTR_PROXY_CREDENTIALS      99         /* Proxy user credentials */
#define OCI_ATTR_INITIAL_CLIENT_ROLES  100       /* Initial client role list */

#define OCI_ATTR_UNK              101                   /* unknown attribute */
#define OCI_ATTR_NUM_COLS         102                   /* number of columns */
#define OCI_ATTR_LIST_COLUMNS     103        /* parameter of the column list */
#define OCI_ATTR_RDBA             104           /* DBA of the segment header */
#define OCI_ATTR_CLUSTERED        105      /* whether the table is clustered */
#define OCI_ATTR_PARTITIONED      106    /* whether the table is partitioned */
#define OCI_ATTR_INDEX_ONLY       107     /* whether the table is index only */
#define OCI_ATTR_LIST_ARGUMENTS   108      /* parameter of the argument list */
#define OCI_ATTR_LIST_SUBPROGRAMS 109    /* parameter of the subprogram list */
#define OCI_ATTR_REF_TDO          110          /* REF to the type descriptor */
#define OCI_ATTR_LINK             111              /* the database link name */
#define OCI_ATTR_MIN              112                       /* minimum value */
#define OCI_ATTR_MAX              113                       /* maximum value */
#define OCI_ATTR_INCR             114                     /* increment value */
#define OCI_ATTR_CACHE            115   /* number of sequence numbers cached */
#define OCI_ATTR_ORDER            116     /* whether the sequence is ordered */
#define OCI_ATTR_HW_MARK          117                     /* high-water mark */
#define OCI_ATTR_TYPE_SCHEMA      118                  /* type's schema name */
#define OCI_ATTR_TIMESTAMP        119             /* timestamp of the object */
#define OCI_ATTR_NUM_ATTRS        120                /* number of sttributes */
#define OCI_ATTR_NUM_PARAMS       121                /* number of parameters */
#define OCI_ATTR_OBJID            122       /* object id for a table or view */
#define OCI_ATTR_PTYPE            123           /* type of info described by */
#define OCI_ATTR_PARAM            124                /* parameter descriptor */
#define OCI_ATTR_OVERLOAD_ID      125     /* overload ID for funcs and procs */
#define OCI_ATTR_TABLESPACE       126                    /* table name space */
#define OCI_ATTR_TDO              127                       /* TDO of a type */
#define OCI_ATTR_LTYPE            128                           /* list type */
#define OCI_ATTR_PARSE_ERROR_OFFSET 129                /* Parse Error offset */
#define OCI_ATTR_IS_TEMPORARY     130          /* whether table is temporary */
#define OCI_ATTR_IS_TYPED         131              /* whether table is typed */
#define OCI_ATTR_DURATION         132         /* duration of temporary table */
#define OCI_ATTR_IS_INVOKER_RIGHTS 133                  /* is invoker rights */
#define OCI_ATTR_OBJ_NAME         134		/* top level schema obj name */
#define OCI_ATTR_OBJ_SCHEMA       135			      /* schema name */
#define OCI_ATTR_OBJ_ID           136          /* top level schema object id */

#define OCI_ATTR_DIRPATH_SORTED_INDEX    137 /* index that data is sorted on */

            /* direct path index maint method (see oci8dp.h) */
#define OCI_ATTR_DIRPATH_INDEX_MAINT_METHOD 138

    /* parallel load: db file, initial and next extent sizes */

#define OCI_ATTR_DIRPATH_FILE               139      /* DB file to load into */
#define OCI_ATTR_DIRPATH_STORAGE_INITIAL    140       /* initial extent size */
#define OCI_ATTR_DIRPATH_STORAGE_NEXT       141          /* next extent size */


#define OCI_ATTR_TRANS_TIMEOUT              142       /* transaction timeout */
#define OCI_ATTR_SERVER_STATUS		    143 /* state of the server handle */
#define OCI_ATTR_STATEMENT                  144 /* statement txt in stmt hdl */


/* ----- Temporary attribute value for UCS2 character set ID -------- */ 
#define OCI_UCS2ID            1000                        /* UCS2 charset ID */

/*============================== End OCI Attribute Types ====================*/

/*---------------- Server Handle Attribute Values ---------------------------*/

/* OCI_ATTR_SERVER_STATUS */
#define OCI_SERVER_NOT_CONNECTED	0x0 
#define OCI_SERVER_NORMAL      		0x1 

/*---------------------------------------------------------------------------*/

/*------------------------- Supported Namespaces  ---------------------------*/
#define OCI_SUBSCR_NAMESPACE_ANONYMOUS   0            /* Anonymous Namespace */
#define OCI_SUBSCR_NAMESPACE_AQ          1                /* Advanced Queues */
#define OCI_SUBSCR_NAMESPACE_MAX         2          /* Max Name Space Number */


/*-------------------------Credential Types----------------------------------*/
#define OCI_CRED_RDBMS    1                    /* database username/password */
#define OCI_CRED_EXT      2               /* externally provided credentials */
#define OCI_CRED_PROXY    3                          /* proxy authentication */
/*---------------------------------------------------------------------------*/

/*------------------------Error Return Values--------------------------------*/
#define OCI_SUCCESS 0                      /* maps to SQL_SUCCESS of SAG CLI */
#define OCI_SUCCESS_WITH_INFO 1             /* maps to SQL_SUCCESS_WITH_INFO */
#define OCI_RESERVED_FOR_INT_USE 200            /* reserved for internal use */ 
#define OCI_NO_DATA 100                               /* maps to SQL_NO_DATA */
#define OCI_ERROR -1                                    /* maps to SQL_ERROR */
#define OCI_INVALID_HANDLE -2                  /* maps to SQL_INVALID_HANDLE */
#define OCI_NEED_DATA 99                            /* maps to SQL_NEED_DATA */
#define OCI_STILL_EXECUTING -3123                   /* OCI would block error */
#define OCI_CONTINUE -24200    /* Continue with the body of the OCI function */
/*---------------------------------------------------------------------------*/

/*------------------DateTime and Interval check Error codes------------------*/

/* DateTime Error Codes used by OCIDateTimeCheck() */
#define   OCI_DT_INVALID_DAY         0x1                          /* Bad day */
#define   OCI_DT_DAY_BELOW_VALID     0x2      /* Bad DAy Low/high bit (1=low)*/
#define   OCI_DT_INVALID_MONTH       0x4                       /*  Bad MOnth */
#define   OCI_DT_MONTH_BELOW_VALID   0x8   /* Bad MOnth Low/high bit (1=low) */
#define   OCI_DT_INVALID_YEAR        0x10                        /* Bad YeaR */
#define   OCI_DT_YEAR_BELOW_VALID    0x20  /*  Bad YeaR Low/high bit (1=low) */
#define   OCI_DT_INVALID_HOUR        0x40                       /*  Bad HouR */
#define   OCI_DT_HOUR_BELOW_VALID    0x80   /* Bad HouR Low/high bit (1=low) */
#define   OCI_DT_INVALID_MINUTE      0x100                     /* Bad MiNute */
#define   OCI_DT_MINUTE_BELOW_VALID  0x200 /*Bad MiNute Low/high bit (1=low) */
#define   OCI_DT_INVALID_SECOND      0x400                    /*  Bad SeCond */
#define   OCI_DT_SECOND_BELOW_VALID  0x800  /*bad second Low/high bit (1=low)*/
#define   OCI_DT_DAY_MISSING_FROM_1582 0x1000     
                                 /*  Day is one of those "missing" from 1582 */
#define   OCI_DT_YEAR_ZERO           0x2000       /* Year may not equal zero */
#define   OCI_DT_INVALID_TIMEZONE    0x4000                 /*  Bad Timezone */
#define   OCI_DT_INVALID_FORMAT      0x8000         /* Bad date format input */


/* Interval Error Codes used by OCIInterCheck() */
#define   OCI_INTER_INVALID_DAY         0x1                       /* Bad day */
#define   OCI_INTER_DAY_BELOW_VALID     0x2  /* Bad DAy Low/high bit (1=low) */
#define   OCI_INTER_INVALID_MONTH       0x4                     /* Bad MOnth */
#define   OCI_INTER_MONTH_BELOW_VALID   0x8 /*Bad MOnth Low/high bit (1=low) */
#define   OCI_INTER_INVALID_YEAR        0x10                     /* Bad YeaR */
#define   OCI_INTER_YEAR_BELOW_VALID    0x20 /*Bad YeaR Low/high bit (1=low) */
#define   OCI_INTER_INVALID_HOUR        0x40                     /* Bad HouR */
#define   OCI_INTER_HOUR_BELOW_VALID    0x80 /*Bad HouR Low/high bit (1=low) */
#define   OCI_INTER_INVALID_MINUTE      0x100                  /* Bad MiNute */
#define   OCI_INTER_MINUTE_BELOW_VALID  0x200 
                                            /*Bad MiNute Low/high bit(1=low) */
#define   OCI_INTER_INVALID_SECOND      0x400                  /* Bad SeCond */
#define   OCI_INTER_SECOND_BELOW_VALID  0x800   
                                            /*bad second Low/high bit(1=low) */
#define   OCI_INTER_INVALID_FRACSEC     0x1000      /* Bad Fractional second */
#define   OCI_INTER_FRACSEC_BELOW_VALID 0x2000  
                                           /* Bad fractional second Low/High */


/*------------------------Parsing Syntax Types-------------------------------*/
#define OCI_V7_SYNTAX 2       /* V815 language - for backwards compatibility */
#define OCI_V8_SYNTAX 3       /* V815 language - for backwards compatibility */
#define OCI_NTV_SYNTAX 1    /* Use what so ever is the native lang of server */
                     /* these values must match the values defined in kpul.h */
/*---------------------------------------------------------------------------*/

/*------------------------Scrollable Cursor Options--------------------------*/
#define OCI_FETCH_NEXT 0x02                                      /* next row */
#define OCI_FETCH_FIRST 0x04                  /* first row of the result set */
#define OCI_FETCH_LAST 0x08                /* the last row of the result set */
#define OCI_FETCH_PRIOR 0x10         /* the previous row relative to current */
#define OCI_FETCH_ABSOLUTE 0x20                /* absolute offset from first */
#define OCI_FETCH_RELATIVE 0x40                /* offset relative to current */
#define OCI_FETCH_RESERVED_1 0x80               /* reserved for internal use */

/*---------------------------------------------------------------------------*/

/*------------------------Bind and Define Options----------------------------*/
#define OCI_SB2_IND_PTR   0x01                                     /* unused */
#define OCI_DATA_AT_EXEC  0x02                       /* data at execute time */
#define OCI_DYNAMIC_FETCH 0x02                          /* fetch dynamically */
#define OCI_PIECEWISE     0x04                    /* piecewise DMLs or fetch */
#define OCI_DEFINE_RESERVED_1 0x08              /* reserved for internal use */
#define OCI_BIND_RESERVED_2   0x10              /* reserved for internal use */
#define OCI_DEFINE_RESERVED_2 0x20              /* reserved for internal use */
/*---------------------------------------------------------------------------*/

/*-----------------------------  Various Modes ------------------------------*/
#define OCI_DEFAULT  0x00 /* the default value for parameters and attributes */

/*-------------OCIInitialize Modes / OCICreateEnvironment Modes -------------*/
#define OCI_THREADED  0x01     /* the application is in threaded environment */
#define OCI_OBJECT    0x02       /* the application is in object environment */
#define OCI_EVENTS    0x04          /* the application is enabled for events */
#define OCI_RESERVED1 0x08                      /* Reserved for internal use */
#define OCI_SHARED    0x10              /* the application is in shared mode */
#define OCI_RESERVED2 0x20                      /* Reserved for internal use */
/* The following *TWO* are only valid for OCICreateEnvironment call */
#define OCI_NO_UCB    0x40            /* No user callback called during init */
#define OCI_NO_MUTEX  0x80            /* the environment handle will not be  */
                                         /*  protected by a mutex internally */
#define OCI_SHARED_EXT 0x100                        /* Used for shared forms */
#define OCI_CACHE     0x200                                /* used by iCache */

/*---------------------------------------------------------------------------*/

/*----------------------------- OCIEnvInit Modes ----------------------------*/
/* NOTE: NO NEW MODES SHOULD BE ADDED HERE BECAUSE THE RECOMMENDED METHOD 
 * IS TO USE THE NEW OCICreateEnvironment MODES.
 */
#define OCI_ENV_NO_UCB 0x01         /* A user callback will not be called in
                                       OCIEnvInit() */
#define OCI_ENV_NO_MUTEX 0x08 /* the environment handle will not be protected
				 by a mutex internally */
/*---------------------------------------------------------------------------*/

/*------------------------ Parse Modes --------------------------------------*/
#define OCI_NO_SHARING        0x01      /* turn off statement handle sharing */
                  /* This flag is only valid when process is in sharing mode */
/*---------------------------------------------------------------------------*/

/*----------------------- Execution Modes -----------------------------------*/
#define OCI_BATCH_MODE        0x01  /* batch the oci statement for execution */
#define OCI_EXACT_FETCH       0x02         /* fetch the exact rows specified */
#define OCI_KEEP_FETCH_STATE  0x04                                 /* unused */
#define OCI_SCROLLABLE_CURSOR 0x08                      /* cursor scrollable */
#define OCI_DESCRIBE_ONLY     0x10            /* only describe the statement */
#define OCI_COMMIT_ON_SUCCESS 0x20        /* commit, if successful execution */
#define OCI_NON_BLOCKING      0x40                           /* non-blocking */
#define OCI_BATCH_ERRORS      0x80             /* batch errors in array dmls */
#define OCI_PARSE_ONLY       0x100               /* only parse the statement */
#define OCI_EXACT_FETCH_RESERVED_1 0x200        /* reserved for internal use */
#define OCI_SHOW_DML_WARNINGS 0x400
            /* return OCI_SUCCESS_WITH_INFO for del/upd with no where clause */

/*---------------------------------------------------------------------------*/

/*------------------------Authentication Modes-------------------------------*/
#define OCI_MIGRATE     0x0001                    /* migratable auth context */
#define OCI_SYSDBA      0x0002                   /* for SYSDBA authorization */
#define OCI_SYSOPER     0x0004                  /* for SYSOPER authorization */
#define OCI_PRELIM_AUTH 0x0008              /* for preliminary authorization */
/*---------------------------------------------------------------------------*/
/*-----------------------------End Various Modes ----------------------------*/

/*------------------------Piece Information----------------------------------*/
#define OCI_PARAM_IN 0x01                                    /* in parameter */
#define OCI_PARAM_OUT 0x02                                  /* out parameter */
/*---------------------------------------------------------------------------*/

/*------------------------ Transaction Start Flags --------------------------*/
/* NOTE: OCI_TRANS_JOIN and OCI_TRANS_NOMIGRATE not supported in 8.0.X       */
#define OCI_TRANS_NEW          0x00000001 /* starts a new transaction branch */
#define OCI_TRANS_JOIN         0x00000002    /* join an existing transaction */
#define OCI_TRANS_RESUME       0x00000004         /* resume this transaction */
#define OCI_TRANS_STARTMASK    0x000000ff

#define OCI_TRANS_READONLY     0x00000100   /* starts a readonly transaction */
#define OCI_TRANS_READWRITE    0x00000200 /* starts a read-write transaction */
#define OCI_TRANS_SERIALIZABLE 0x00000400
                                        /* starts a serializable transaction */
#define OCI_TRANS_ISOLMASK     0x0000ff00

#define OCI_TRANS_LOOSE        0x00010000        /* a loosely coupled branch */
#define OCI_TRANS_TIGHT        0x00020000        /* a tightly coupled branch */
#define OCI_TRANS_TYPEMASK     0x000f0000

#define OCI_TRANS_NOMIGRATE    0x00100000      /* non migratable transaction */

/*---------------------------------------------------------------------------*/

/*------------------------ Transaction End Flags ----------------------------*/
#define OCI_TRANS_TWOPHASE      0x01000000           /* use two phase commit */
/*---------------------------------------------------------------------------*/

/*------------------------- AQ Constants ------------------------------------
 * NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
 * The following constants must match the PL/SQL dbms_aq constants
 * NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
 */
/* ------------------------- Visibility flags -------------------------------*/
#define OCI_ENQ_IMMEDIATE	1   /* enqueue is an independent transaction */
#define OCI_ENQ_ON_COMMIT	2  /* enqueue is part of current transaction */

/* ----------------------- Dequeue mode flags -------------------------------*/
#define OCI_DEQ_BROWSE		1   /* read message without acquiring a lock */
#define OCI_DEQ_LOCKED		2   /* read and obtain write lock on message */
#define OCI_DEQ_REMOVE		3          /* read the message and delete it */
#define OCI_DEQ_REMOVE_NODATA   4    /* delete message w'o returning payload */

/* ----------------- Dequeue navigation flags -------------------------------*/
#define OCI_DEQ_FIRST_MSG	 1     /* get first message at head of queue */
#define OCI_DEQ_NEXT_MSG	 3         /* next message that is available */
#define OCI_DEQ_NEXT_TRANSACTION 2    /* get first message of next txn group */

/* --------------------- Message states -------------------------------------*/
#define OCI_MSG_WAITING		1 /* the message delay has not yet completed */
#define OCI_MSG_READY		0    /* the message is ready to be processed */
#define OCI_MSG_PROCESSED	2          /* the message has been processed */
#define OCI_MSG_EXPIRED		3    /* message has moved to exception queue */

/* --------------------- Sequence deviation ---------------------------------*/
#define OCI_ENQ_BEFORE		2  /* enqueue message before another message */
#define OCI_ENQ_TOP		3     /* enqueue message before all messages */

/* ------------------------- Visibility flags -------------------------------*/
#define OCI_DEQ_IMMEDIATE	1   /* dequeue is an independent transaction */
#define OCI_DEQ_ON_COMMIT	2  /* dequeue is part of current transaction */

/* ------------------------ Wait --------------------------------------------*/
#define OCI_DEQ_WAIT_FOREVER 	-1   /* wait forever if no message available */
#define OCI_DEQ_NO_WAIT	 	0  /* do not wait if no message is available */

/* ------------------------ Delay -------------------------------------------*/
#define OCI_MSG_NO_DELAY	0        /* message is available immediately */

/* ------------------------- Expiration -------------------------------------*/
#define OCI_MSG_NO_EXPIRATION -1                /* message will never expire */

/* -------------------------- END AQ Constants ----------------------------- */

/* --------------------END DateTime and Interval Constants ------------------*/

/*-----------------------Object Types----------------------------------------*/
/*-----------Object Types **** Not to be Used **** --------------------------*/
/* Deprecated */
#define OCI_OTYPE_UNK           0
#define OCI_OTYPE_TABLE         1
#define OCI_OTYPE_VIEW          2
#define OCI_OTYPE_SYN           3
#define OCI_OTYPE_PROC          4
#define OCI_OTYPE_FUNC          5
#define OCI_OTYPE_PKG           6
#define OCI_OTYPE_STMT          7
/*---------------------------------------------------------------------------*/

/*=======================Describe Handle Parameter Attributes ===============*/
/* 
   These attributes are orthogonal to the other set of attributes defined 
   above.  These attrubutes are tobe used only for the desscribe handle 
*/
/*===========================================================================*/
/* Attributes common to Columns and Stored Procs */
#define OCI_ATTR_DATA_SIZE      1                /* maximum size of the data */
#define OCI_ATTR_DATA_TYPE      2     /* the SQL type of the column/argument */
#define OCI_ATTR_DISP_SIZE      3                        /* the display size */
#define OCI_ATTR_NAME           4         /* the name of the column/argument */
#define OCI_ATTR_PRECISION      5                /* precision if number type */
#define OCI_ATTR_SCALE          6                    /* scale if number type */
#define OCI_ATTR_IS_NULL        7                            /* is it null ? */
#define OCI_ATTR_TYPE_NAME      8
  /* name of the named data type or a package name for package private types */
#define OCI_ATTR_SCHEMA_NAME    9             /* the schema name */
#define OCI_ATTR_SUB_NAME       10      /* type name if package private type */
#define OCI_ATTR_POSITION       11
                    /* relative position of col/arg in the list of cols/args */
/* complex object retrieval parameter attributes */
#define OCI_ATTR_COMPLEXOBJECTCOMP_TYPE		50 
#define OCI_ATTR_COMPLEXOBJECTCOMP_TYPE_LEVEL	51
#define OCI_ATTR_COMPLEXOBJECT_LEVEL		52
#define OCI_ATTR_COMPLEXOBJECT_COLL_OUTOFLINE   53

/* Only Columns */
#define OCI_ATTR_DISP_NAME      100                      /* the display name */

/*Only Stored Procs */
#define OCI_ATTR_OVERLOAD       210           /* is this position overloaded */
#define OCI_ATTR_LEVEL          211            /* level for structured types */
#define OCI_ATTR_HAS_DEFAULT    212                   /* has a default value */
#define OCI_ATTR_IOMODE         213                         /* in, out inout */
#define OCI_ATTR_RADIX          214                       /* returns a radix */
#define OCI_ATTR_NUM_ARGS       215             /* total number of arguments */

/* only named type attributes */
#define OCI_ATTR_TYPECODE                  216       /* object or collection */
#define OCI_ATTR_COLLECTION_TYPECODE       217     /* varray or nested table */
#define OCI_ATTR_VERSION                   218      /* user assigned version */
#define OCI_ATTR_IS_INCOMPLETE_TYPE        219 /* is this an incomplete type */
#define OCI_ATTR_IS_SYSTEM_TYPE            220              /* a system type */
#define OCI_ATTR_IS_PREDEFINED_TYPE        221          /* a predefined type */
#define OCI_ATTR_IS_TRANSIENT_TYPE         222           /* a transient type */
#define OCI_ATTR_IS_SYSTEM_GENERATED_TYPE  223      /* system generated type */
#define OCI_ATTR_HAS_NESTED_TABLE          224 /* contains nested table attr */
#define OCI_ATTR_HAS_LOB                   225        /* has a lob attribute */
#define OCI_ATTR_HAS_FILE                  226       /* has a file attribute */
#define OCI_ATTR_COLLECTION_ELEMENT        227 /* has a collection attribute */
#define OCI_ATTR_NUM_TYPE_ATTRS            228  /* number of attribute types */
#define OCI_ATTR_LIST_TYPE_ATTRS           229    /* list of type attributes */
#define OCI_ATTR_NUM_TYPE_METHODS          230     /* number of type methods */
#define OCI_ATTR_LIST_TYPE_METHODS         231       /* list of type methods */
#define OCI_ATTR_MAP_METHOD                232         /* map method of type */
#define OCI_ATTR_ORDER_METHOD              233       /* order method of type */

/* only collection element */
#define OCI_ATTR_NUM_ELEMS                 234         /* number of elements */

/* only type methods */
#define OCI_ATTR_ENCAPSULATION             235        /* encapsulation level */
#define OCI_ATTR_IS_SELFISH                236             /* method selfish */
#define OCI_ATTR_IS_VIRTUAL                237                    /* virtual */
#define OCI_ATTR_IS_INLINE                 238                     /* inline */
#define OCI_ATTR_IS_CONSTANT               239                   /* constant */
#define OCI_ATTR_HAS_RESULT                240                 /* has result */
#define OCI_ATTR_IS_CONSTRUCTOR            241                /* constructor */
#define OCI_ATTR_IS_DESTRUCTOR             242                 /* destructor */
#define OCI_ATTR_IS_OPERATOR               243                   /* operator */
#define OCI_ATTR_IS_MAP                    244               /* a map method */
#define OCI_ATTR_IS_ORDER                  245               /* order method */
#define OCI_ATTR_IS_RNDS                   246  /* read no data state method */
#define OCI_ATTR_IS_RNPS                   247      /* read no process state */
#define OCI_ATTR_IS_WNDS                   248 /* write no data state method */
#define OCI_ATTR_IS_WNPS                   249     /* write no process state */

#define OCI_ATTR_DESC_PUBLIC               250              /* public object */

/* Object Cache Enhancements : attributes for User Constructed Instances     */
#define OCI_ATTR_CACHE_CLIENT_CONTEXT      251
#define OCI_ATTR_UCI_CONSTRUCT		   252
#define OCI_ATTR_UCI_DESTRUCT 		   253
#define OCI_ATTR_UCI_COPY 		   254
#define OCI_ATTR_UCI_PICKLE 		   255
#define OCI_ATTR_UCI_UNPICKLE 		   256
#define OCI_ATTR_UCI_REFRESH 		   257

/* for type inheritance */
#define OCI_ATTR_IS_SUBTYPE                258
#define OCI_ATTR_SUPERTYPE_SCHEMA_NAME     259
#define OCI_ATTR_SUPERTYPE_NAME            260

/* for schemas */
#define OCI_ATTR_LIST_OBJECTS              261	/* list of objects in schema */

/* for database */
#define OCI_ATTR_NCHARSET_ID               262		      /* char set id */
#define OCI_ATTR_LIST_SCHEMAS              263		  /* list of schemas */
#define OCI_ATTR_MAX_PROC_LEN              264	     /* max procedure length */
#define OCI_ATTR_MAX_COLUMN_LEN            265	   /* max column name length */
#define OCI_ATTR_CURSOR_COMMIT_BEHAVIOR    266	   /* cursor commit behavior */
#define OCI_ATTR_MAX_CATALOG_NAMELEN       267	       /* catalog namelength */
#define OCI_ATTR_CATALOG_LOCATION          268		 /* catalog location */
#define OCI_ATTR_SAVEPOINT_SUPPORT         269		/* savepoint support */
#define OCI_ATTR_NOWAIT_SUPPORT            270		   /* nowait support */
#define OCI_ATTR_AUTOCOMMIT_DDL            271		   /* autocommit DDL */
#define OCI_ATTR_LOCKING_MODE              272		     /* locking mode */

/* for externally initialized context */
#define OCI_ATTR_APPCTX_SIZE               273 /* count of context to be init*/
#define OCI_ATTR_APPCTX_LIST               274 /* count of context to be init*/
#define OCI_ATTR_APPCTX_NAME               275 /* name  of context to be init*/
#define OCI_ATTR_APPCTX_ATTR               276 /* attr  of context to be init*/
#define OCI_ATTR_APPCTX_VALUE              277 /* value of context to be init*/

/*---------------------------End Describe Handle Attributes -----------------*/

/*---------------- Describe Handle Parameter Attribute Values ---------------*/

/* OCI_ATTR_CURSOR_COMMIT_BEHAVIOR */
#define OCI_CURSOR_OPEN   0 
#define OCI_CURSOR_CLOSED 1

/* OCI_ATTR_CATALOG_LOCATION */
#define OCI_CL_START 0
#define OCI_CL_END   1

/* OCI_ATTR_SAVEPOINT_SUPPORT */
#define OCI_SP_SUPPORTED   0
#define OCI_SP_UNSUPPORTED 1

/* OCI_ATTR_NOWAIT_SUPPORT */
#define OCI_NW_SUPPORTED   0
#define OCI_NW_UNSUPPORTED 1

/* OCI_ATTR_AUTOCOMMIT_DDL */
#define OCI_AC_DDL    0
#define OCI_NO_AC_DDL 1

/* OCI_ATTR_LOCKING_MODE */
#define OCI_LOCK_IMMEDIATE 0
#define OCI_LOCK_DELAYED   1

/*---------------------------------------------------------------------------*/

/*---------------------------OCIPasswordChange-------------------------------*/
#define OCI_AUTH         0x08        /* Change the password but do not login */


/*------------------------Other Constants------------------------------------*/
#define OCI_MAX_FNS   100                     /* max number of OCI Functions */
#define OCI_SQLSTATE_SIZE 5  
#define OCI_ERROR_MAXMSG_SIZE   1024         /* max size of an error message */
#define OCI_LOBMAXSIZE MINUB4MAXVAL                 /* maximum lob data size */
#define OCI_ROWID_LEN             23 
/*---------------------------------------------------------------------------*/

/*------------------------ Fail Over Events ---------------------------------*/
#define OCI_FO_END          0x00000001
#define OCI_FO_ABORT        0x00000002   
#define OCI_FO_REAUTH       0x00000004
#define OCI_FO_BEGIN        0x00000008 
#define OCI_FO_ERROR        0x00000010
/*---------------------------------------------------------------------------*/

/*------------------------ Fail Over Callback Return Codes ------------------*/
#define OCI_FO_RETRY        25410
/*---------------------------------------------------------------------------*/

/*------------------------- Fail Over Types ---------------------------------*/
#define OCI_FO_NONE           0x00000001
#define OCI_FO_SESSION        0x00000002
#define OCI_FO_SELECT         0x00000004
#define OCI_FO_TXNAL          0x00000008
/*---------------------------------------------------------------------------*/

/*-----------------------Function Codes--------------------------------------*/
#define OCI_FNCODE_INITIALIZE     1                         /* OCIInitialize */
#define OCI_FNCODE_HANDLEALLOC  2                          /* OCIHandleAlloc */
#define OCI_FNCODE_HANDLEFREE  3                            /* OCIHandleFree */
#define OCI_FNCODE_DESCRIPTORALLOC  4                  /* OCIDescriptorAlloc */
#define OCI_FNCODE_DESCRIPTORFREE  5                    /* OCIDescriptorFree */
#define OCI_FNCODE_ENVINIT   6                                 /* OCIEnvInit */
#define OCI_FNCODE_SERVERATTACH   7                       /* OCIServerAttach */
#define OCI_FNCODE_SERVERDETACH   8                       /* OCIServerDetach */
/* unused         9 */ 
#define OCI_FNCODE_SESSIONBEGIN  10                       /* OCISessionBegin */
#define OCI_FNCODE_SESSIONEND   11                          /* OCISessionEnd */
#define OCI_FNCODE_PASSWORDCHANGE   12                  /* OCIPasswordChange */
#define OCI_FNCODE_STMTPREPARE   13                        /* OCIStmtPrepare */
                                                      /* unused       14- 16 */
#define OCI_FNCODE_BINDDYNAMIC   17                        /* OCIBindDynamic */
#define OCI_FNCODE_BINDOBJECT  18                           /* OCIBindObject */
                                                                /* 19 unused */
#define OCI_FNCODE_BINDARRAYOFSTRUCT   20            /* OCIBindArrayOfStruct */
#define OCI_FNCODE_STMTEXECUTE  21                         /* OCIStmtExecute */
                                                             /* unused 22-24 */
#define OCI_FNCODE_DEFINEOBJECT  25                       /* OCIDefineObject */
#define OCI_FNCODE_DEFINEDYNAMIC   26                    /* OCIDefineDynamic */
#define OCI_FNCODE_DEFINEARRAYOFSTRUCT  27         /* OCIDefineArrayOfStruct */
#define OCI_FNCODE_STMTFETCH   28                            /* OCIStmtFetch */
#define OCI_FNCODE_STMTGETBIND   29                    /* OCIStmtGetBindInfo */
                                                            /* 30, 31 unused */
#define OCI_FNCODE_DESCRIBEANY  32                         /* OCIDescribeAny */
#define OCI_FNCODE_TRANSSTART  33                           /* OCITransStart */
#define OCI_FNCODE_TRANSDETACH  34                         /* OCITransDetach */
#define OCI_FNCODE_TRANSCOMMIT  35                         /* OCITransCommit */
                                                                /* 36 unused */
#define OCI_FNCODE_ERRORGET   37                              /* OCIErrorGet */
#define OCI_FNCODE_LOBOPENFILE  38                         /* OCILobFileOpen */
#define OCI_FNCODE_LOBCLOSEFILE  39                       /* OCILobFileClose */
                                             /* 40 was LOBCREATEFILE, unused */
                                         /* 41 was OCILobFileDelete, unused  */
#define OCI_FNCODE_LOBCOPY  42                                 /* OCILobCopy */
#define OCI_FNCODE_LOBAPPEND  43                             /* OCILobAppend */
#define OCI_FNCODE_LOBERASE  44                               /* OCILobErase */
#define OCI_FNCODE_LOBLENGTH  45                          /* OCILobGetLength */
#define OCI_FNCODE_LOBTRIM  46                                 /* OCILobTrim */
#define OCI_FNCODE_LOBREAD  47                                 /* OCILobRead */
#define OCI_FNCODE_LOBWRITE  48                               /* OCILobWrite */
                                                                /* 49 unused */
#define OCI_FNCODE_SVCCTXBREAK 50                                /* OCIBreak */
#define OCI_FNCODE_SERVERVERSION  51                     /* OCIServerVersion */
/* unused 52, 53 */
#define OCI_FNCODE_ATTRGET 54                                  /* OCIAttrGet */
#define OCI_FNCODE_ATTRSET 55                                  /* OCIAttrSet */
#define OCI_FNCODE_PARAMSET 56                                /* OCIParamSet */
#define OCI_FNCODE_PARAMGET 57                                /* OCIParamGet */
#define OCI_FNCODE_STMTGETPIECEINFO   58              /* OCIStmtGetPieceInfo */
#define OCI_FNCODE_LDATOSVCCTX 59                          /* OCILdaToSvcCtx */
                                                                /* 60 unused */
#define OCI_FNCODE_STMTSETPIECEINFO   61              /* OCIStmtSetPieceInfo */
#define OCI_FNCODE_TRANSFORGET 62                          /* OCITransForget */
#define OCI_FNCODE_TRANSPREPARE 63                        /* OCITransPrepare */
#define OCI_FNCODE_TRANSROLLBACK  64                     /* OCITransRollback */
#define OCI_FNCODE_DEFINEBYPOS 65                          /* OCIDefineByPos */
#define OCI_FNCODE_BINDBYPOS 66                              /* OCIBindByPos */
#define OCI_FNCODE_BINDBYNAME 67                            /* OCIBindByName */
#define OCI_FNCODE_LOBASSIGN  68                             /* OCILobAssign */
#define OCI_FNCODE_LOBISEQUAL  69                           /* OCILobIsEqual */
#define OCI_FNCODE_LOBISINIT  70                      /* OCILobLocatorIsInit */
/* 71 was lob locator size in beta2 */
#define OCI_FNCODE_LOBENABLEBUFFERING  71           /* OCILobEnableBuffering */
#define OCI_FNCODE_LOBCHARSETID  72                       /* OCILobCharSetID */
#define OCI_FNCODE_LOBCHARSETFORM  73                   /* OCILobCharSetForm */
#define OCI_FNCODE_LOBFILESETNAME  74                   /* OCILobFileSetName */
#define OCI_FNCODE_LOBFILEGETNAME  75                   /* OCILobFileGetName */
#define OCI_FNCODE_LOGON 76                                      /* OCILogon */
#define OCI_FNCODE_LOGOFF 77                                    /* OCILogoff */
#define OCI_FNCODE_LOBDISABLEBUFFERING 78          /* OCILobDisableBuffering */
#define OCI_FNCODE_LOBFLUSHBUFFER 79                    /* OCILobFlushBuffer */
#define OCI_FNCODE_LOBLOADFROMFILE 80                  /* OCILobLoadFromFile */

#define OCI_FNCODE_LOBOPEN  81                                 /* OCILobOpen */
#define OCI_FNCODE_LOBCLOSE  82                               /* OCILobClose */
#define OCI_FNCODE_LOBISOPEN  83                             /* OCILobIsOpen */
#define OCI_FNCODE_LOBFILEISOPEN  84                     /* OCILobFileIsOpen */
#define OCI_FNCODE_LOBFILEEXISTS  85                     /* OCILobFileExists */
#define OCI_FNCODE_LOBFILECLOSEALL  86                 /* OCILobFileCloseAll */
#define OCI_FNCODE_LOBCREATETEMP  87                /* OCILobCreateTemporary */
#define OCI_FNCODE_LOBFREETEMP  88                    /* OCILobFreeTemporary */
#define OCI_FNCODE_LOBISTEMP  89                        /* OCILobIsTemporary */

#define OCI_FNCODE_AQENQ  90                                     /* OCIAQEnq */
#define OCI_FNCODE_AQDEQ  91                                     /* OCIAQDeq */
#define OCI_FNCODE_RESET  92                                     /* OCIReset */
#define OCI_FNCODE_SVCCTXTOLDA  93                         /* OCISvcCtxToLda */
#define OCI_FNCODE_LOBLOCATORASSIGN 94                /* OCILobLocatorAssign */

#define OCI_FNCODE_UBINDBYNAME 95

#define OCI_FNCODE_AQLISTEN  96				      /* OCIAQListen */

#define OCI_FNCODE_SVC2HST 97                                    /* reserved */
#define OCI_FNCODE_SVCRH   98                                    /* reserved */
                           /* 97 and 98 are reserved for Oracle internal use */

#define OCI_FNCODE_TRANSMULTIPREPARE   99            /* OCITransMultiPrepare */
#define OCI_FNCODE_MAXFCN 99                    /* maximum OCI function code */


/*---------------------------------------------------------------------------*/

/*-----------------------Handle Definitions----------------------------------*/
typedef struct OCIEnv           OCIEnv;            /* OCI environment handle */
typedef struct OCIError         OCIError;                /* OCI error handle */
typedef struct OCISvcCtx        OCISvcCtx;             /* OCI service handle */
typedef struct OCIStmt          OCIStmt;             /* OCI statement handle */
typedef struct OCIBind          OCIBind;                  /* OCI bind handle */
typedef struct OCIDefine        OCIDefine;              /* OCI Define handle */
typedef struct OCIDescribe      OCIDescribe;          /* OCI Describe handle */
typedef struct OCIServer        OCIServer;              /* OCI Server handle */
typedef struct OCISession       OCISession;     /* OCI Authentication handle */
typedef struct OCIComplexObject OCIComplexObject;          /* OCI COR handle */
typedef struct OCITrans         OCITrans;          /* OCI Transaction handle */
typedef struct OCISecurity      OCISecurity;          /* OCI Security handle */
typedef struct OCISubscription  OCISubscription;      /* subscription handle */

/*-----------------------Descriptor Definitions------------------------------*/
typedef struct OCISnapshot      OCISnapshot;      /* OCI snapshot descriptor */
typedef struct OCIResult        OCIResult;      /* OCI Result Set Descriptor */
typedef struct OCILobLocator    OCILobLocator; /* OCI Lob Locator descriptor */
typedef struct OCIParam         OCIParam;        /* OCI PARameter descriptor */
typedef struct OCIComplexObjectComp OCIComplexObjectComp;
                                                       /* OCI COR descriptor */
typedef struct OCIRowid OCIRowid;                    /* OCI ROWID descriptor */

typedef struct OCIDateTime OCIDateTime;           /* OCI DateTime descriptor */
typedef struct OCIInterval OCIInterval;           /* OCI Interval descriptor */

typedef struct OCIUcb 		OCIUcb;      /* OCI User Callback descriptor */

/*-------------------------- AQ Descriptors ---------------------------------*/
typedef struct OCIAQEnqOptions    OCIAQEnqOptions; /* AQ Enqueue Options hdl */
typedef struct OCIAQDeqOptions    OCIAQDeqOptions; /* AQ Dequeue Options hdl */
typedef struct OCIAQMsgProperties OCIAQMsgProperties;  /* AQ Mesg Properties */
typedef struct OCIAQAgent	  OCIAQAgent;	      /* AQ Agent descriptor */
typedef struct OCIAQNfyDescriptor OCIAQNfyDescriptor;	/* AQ Nfy descriptor */
/*---------------------------------------------------------------------------*/
 
/* Lob typedefs for Pro*C */
typedef struct OCILobLocator OCIClobLocator;    /* OCI Character LOB Locator */
typedef struct OCILobLocator OCIBlobLocator;       /* OCI Binary LOB Locator */
typedef struct OCILobLocator OCIBFileLocator; /* OCI Binary LOB File Locator */
/*---------------------------------------------------------------------------*/

/* Undefined value for tz in interval types*/
#define OCI_INTHR_UNK 24

  /* These defined adjustment values */
#define OCI_ADJUST_UNK            10
#define OCI_ORACLE_DATE           0
#define OCI_ANSI_DATE             1

/*------------------------ Lob-specific Definitions -------------------------*/

/*
 * ociloff - OCI Lob OFFset
 *
 * The offset in the lob data.  The offset is specified in terms of bytes for
 * BLOBs and BFILes.  Character offsets are used for CLOBs, NCLOBs.
 * The maximum size of internal lob data is 4 gigabytes.  FILE LOB 
 * size is limited by the operating system.
 */
typedef ub4 OCILobOffset;

/*
 * ocillen - OCI Lob LENgth (of lob data)
 *
 * Specifies the length of lob data in bytes for BLOBs and BFILes and in 
 * characters for CLOBs, NCLOBs.  The maximum length of internal lob
 * data is 4 gigabytes.  The length of FILE LOBs is limited only by the
 * operating system.
 */
typedef ub4 OCILobLength;
/*
 * ocilmo - OCI Lob open MOdes
 *
 * The mode specifies the planned operations that will be performed on the
 * FILE lob data.  The FILE lob can be opened in read-only mode only.
 * 
 * In the future, we may include read/write, append and truncate modes.  Append
 * is equivalent to read/write mode except that the FILE is positioned for
 * writing to the end.  Truncate is equivalent to read/write mode except that
 * the FILE LOB data is first truncated to a length of 0 before use.
 */
enum OCILobMode
{
  OCI_LOBMODE_READONLY = 1,                                     /* read-only */
  OCI_LOBMODE_READWRITE = 2             /* read_write for internal lobs only */
};
typedef enum OCILobMode OCILobMode;

/*---------------------------------------------------------------------------*/


/*----------------------------Piece Definitions------------------------------*/

/* if ocidef.h is being included in the app, ocidef.h should precede oci.h */

/* 
 * since clients may  use oci.h, ocidef.h and ocidfn.h the following defines
 * need to be guarded, usually internal clients
 */

#ifndef OCI_FLAGS
#define OCI_FLAGS
#define OCI_ONE_PIECE 0                                         /* one piece */
#define OCI_FIRST_PIECE 1                                 /* the first piece */
#define OCI_NEXT_PIECE 2                          /* the next of many pieces */
#define OCI_LAST_PIECE 3                                   /* the last piece */
#endif
/*---------------------------------------------------------------------------*/

/*--------------------------- FILE open modes -------------------------------*/
#define OCI_FILE_READONLY 1             /* readonly mode open for FILE types */
/*---------------------------------------------------------------------------*/
/*--------------------------- LOB open modes --------------------------------*/
#define OCI_LOB_READONLY 1              /* readonly mode open for ILOB types */
#define OCI_LOB_READWRITE 2                /* read write mode open for ILOBs */

/*----------------------- LOB Buffering Flush Flags -------------------------*/
#define OCI_LOB_BUFFER_FREE   1 
#define OCI_LOB_BUFFER_NOFREE 2
/*---------------------------------------------------------------------------*/

/*--------------------------- OCI Statement Types ---------------------------*/

#define  OCI_STMT_SELECT  1                              /* select statement */
#define  OCI_STMT_UPDATE  2                              /* update statement */
#define  OCI_STMT_DELETE  3                              /* delete statement */
#define  OCI_STMT_INSERT  4                              /* Insert Statement */
#define  OCI_STMT_CREATE  5                              /* create statement */
#define  OCI_STMT_DROP    6                                /* drop statement */
#define  OCI_STMT_ALTER   7                               /* alter statement */
#define  OCI_STMT_BEGIN   8                   /* begin ... (pl/sql statement)*/
#define  OCI_STMT_DECLARE 9                /* declare .. (pl/sql statement ) */
/*---------------------------------------------------------------------------*/

/*--------------------------- OCI Parameter Types ---------------------------*/
#define OCI_PTYPE_UNK           0                               /* unknown   */
#define OCI_PTYPE_TABLE		1                               /* table     */
#define OCI_PTYPE_VIEW		2                               /* view      */
#define OCI_PTYPE_PROC		3                               /* procedure */
#define OCI_PTYPE_FUNC		4                               /* function  */
#define OCI_PTYPE_PKG		5                               /* package   */
#define OCI_PTYPE_TYPE          6                       /* user-defined type */
#define OCI_PTYPE_SYN		7                               /* synonym   */
#define OCI_PTYPE_SEQ		8                               /* sequence  */
#define OCI_PTYPE_COL		9                               /* column    */
#define OCI_PTYPE_ARG		10                              /* argument  */
#define OCI_PTYPE_LIST		11                              /* list      */
#define OCI_PTYPE_TYPE_ATTR     12          /* user-defined type's attribute */
#define OCI_PTYPE_TYPE_COLL     13              /* collection type's element */
#define OCI_PTYPE_TYPE_METHOD   14             /* user-defined type's method */
#define OCI_PTYPE_TYPE_ARG      15    /* user-defined type method's argument */
#define OCI_PTYPE_TYPE_RESULT   16      /* user-defined type method's result */
#define OCI_PTYPE_SCHEMA	17                                 /* schema */
#define OCI_PTYPE_DATABASE      18                               /* database */
/*---------------------------------------------------------------------------*/

/*----------------------------- OCI List Types ------------------------------*/
#define OCI_LTYPE_UNK           0                               /* unknown   */
#define OCI_LTYPE_COLUMN        1                             /* column list */
#define OCI_LTYPE_ARG_PROC      2                 /* procedure argument list */
#define OCI_LTYPE_ARG_FUNC      3                  /* function argument list */
#define OCI_LTYPE_SUBPRG        4                         /* subprogram list */
#define OCI_LTYPE_TYPE_ATTR     5                          /* type attribute */
#define OCI_LTYPE_TYPE_METHOD   6                             /* type method */
#define OCI_LTYPE_TYPE_ARG_PROC 7    /* type method w/o result argument list */
#define OCI_LTYPE_TYPE_ARG_FUNC 8      /* type method w/result argument list */
#define OCI_LTYPE_SCH_OBJ       9                      /* schema object list */
#define OCI_LTYPE_DB_SCH        10                   /* database schema list */

/*---------------------------------------------------------------------------*/

/*-------------------------- Memory Cartridge Services ---------------------*/
#define OCI_MEMORY_CLEARED  1

/*-------------------------- Pickler Cartridge Services ---------------------*/
typedef struct OCIPicklerTdsCtx OCIPicklerTdsCtx;
typedef struct OCIPicklerTds OCIPicklerTds;
typedef struct OCIPicklerImage OCIPicklerImage;
typedef struct OCIPicklerFdo OCIPicklerFdo;
typedef ub4 OCIPicklerTdsElement;

/*---------------------------------------------------------------------------*/

/*--------------------------- User Callback Constants -----------------------*/
#define OCI_UCBTYPE_ENTRY	1                          /* entry callback */
#define OCI_UCBTYPE_EXIT	2                           /* exit callback */
#define OCI_UCBTYPE_REPLACE	3                    /* replacement callback */

/*---------------------------------------------------------------------------*/

/*--------------------- NLS service type and constance ----------------------*/
#define OCI_NLS_DAYNAME1      1                    /* Native name for Monday */
#define OCI_NLS_DAYNAME2      2                   /* Native name for Tuesday */
#define OCI_NLS_DAYNAME3      3                 /* Native name for Wednesday */
#define OCI_NLS_DAYNAME4      4                  /* Native name for Thursday */
#define OCI_NLS_DAYNAME5      5                    /* Native name for Friday */
#define OCI_NLS_DAYNAME6      6              /* Native name for for Saturday */
#define OCI_NLS_DAYNAME7      7                /* Native name for for Sunday */
#define OCI_NLS_ABDAYNAME1    8        /* Native abbreviated name for Monday */
#define OCI_NLS_ABDAYNAME2    9       /* Native abbreviated name for Tuesday */
#define OCI_NLS_ABDAYNAME3    10    /* Native abbreviated name for Wednesday */
#define OCI_NLS_ABDAYNAME4    11     /* Native abbreviated name for Thursday */
#define OCI_NLS_ABDAYNAME5    12       /* Native abbreviated name for Friday */
#define OCI_NLS_ABDAYNAME6    13 /* Native abbreviated name for for Saturday */
#define OCI_NLS_ABDAYNAME7    14   /* Native abbreviated name for for Sunday */
#define OCI_NLS_MONTHNAME1    15                  /* Native name for January */
#define OCI_NLS_MONTHNAME2    16                 /* Native name for February */
#define OCI_NLS_MONTHNAME3    17                    /* Native name for March */
#define OCI_NLS_MONTHNAME4    18                    /* Native name for April */
#define OCI_NLS_MONTHNAME5    19                      /* Native name for May */
#define OCI_NLS_MONTHNAME6    20                     /* Native name for June */
#define OCI_NLS_MONTHNAME7    21                     /* Native name for July */
#define OCI_NLS_MONTHNAME8    22                   /* Native name for August */
#define OCI_NLS_MONTHNAME9    23                /* Native name for September */
#define OCI_NLS_MONTHNAME10   24                  /* Native name for October */
#define OCI_NLS_MONTHNAME11   25                 /* Native name for November */
#define OCI_NLS_MONTHNAME12   26                 /* Native name for December */
#define OCI_NLS_ABMONTHNAME1  27      /* Native abbreviated name for January */
#define OCI_NLS_ABMONTHNAME2  28     /* Native abbreviated name for February */
#define OCI_NLS_ABMONTHNAME3  29        /* Native abbreviated name for March */
#define OCI_NLS_ABMONTHNAME4  30        /* Native abbreviated name for April */
#define OCI_NLS_ABMONTHNAME5  31          /* Native abbreviated name for May */
#define OCI_NLS_ABMONTHNAME6  32         /* Native abbreviated name for June */
#define OCI_NLS_ABMONTHNAME7  33         /* Native abbreviated name for July */
#define OCI_NLS_ABMONTHNAME8  34       /* Native abbreviated name for August */
#define OCI_NLS_ABMONTHNAME9  35    /* Native abbreviated name for September */
#define OCI_NLS_ABMONTHNAME10 36      /* Native abbreviated name for October */
#define OCI_NLS_ABMONTHNAME11 37     /* Native abbreviated name for November */
#define OCI_NLS_ABMONTHNAME12 38     /* Native abbreviated name for December */
#define OCI_NLS_YES           39   /* Native string for affirmative response */
#define OCI_NLS_NO            40                 /* Native negative response */
#define OCI_NLS_AM            41           /* Native equivalent string of AM */
#define OCI_NLS_PM            42           /* Native equivalent string of PM */
#define OCI_NLS_AD            43           /* Native equivalent string of AD */
#define OCI_NLS_BC            44           /* Native equivalent string of BC */
#define OCI_NLS_DECIMAL       45                        /* decimal character */
#define OCI_NLS_GROUP         46                          /* group separator */
#define OCI_NLS_DEBIT         47                   /* Native symbol of debit */
#define OCI_NLS_CREDIT        48                  /* Native sumbol of credit */
#define OCI_NLS_DATEFORMAT    49                       /* Oracle date format */
#define OCI_NLS_INT_CURRENCY  50            /* International currency symbol */
#define OCI_NLS_LOC_CURRENCY  51                   /* Locale currency symbol */
#define OCI_NLS_LANGUAGE      52                            /* Language name */
#define OCI_NLS_ABLANGUAGE    53           /* Abbreviation for language name */
#define OCI_NLS_TERRITORY     54                           /* Territory name */
#define OCI_NLS_CHARACTER_SET 55                       /* Character set name */
#define OCI_NLS_LINGUISTIC_NAME    56                     /* Linguistic name */
#define OCI_NLS_CALENDAR      57                            /* Calendar name */
#define OCI_NLS_DUAL_CURRENCY 78                     /* Dual currency symbol */

#define OCI_NLS_MAXBUFSZ   100 /* Max buffer size may need for OCINlsGetInfo */

#define OCI_NLS_BINARY            0x1           /* for the binary comparison */
#define OCI_NLS_LINGUISTIC        0x2           /* for linguistic comparison */
#define OCI_NLS_CASE_INSENSITIVE  0x10    /* for case-insensitive comparison */

#define OCI_NLS_UPPERCASE         0x20               /* convert to uppercase */
#define OCI_NLS_LOWERCASE         0x40               /* convert to lowercase */

typedef struct OCIMsg  OCIMsg;
typedef ub4            OCIWchar;


/*--------------------------------------------------------------------------- 
                     PRIVATE TYPES AND CONSTANTS 
  ---------------------------------------------------------------------------*/
 
/* None */

/*--------------------------------------------------------------------------- 
                           PUBLIC FUNCTIONS 
  ---------------------------------------------------------------------------*/

/* see ociap.h or ocikp.h */
 
/*--------------------------------------------------------------------------- 
                          PRIVATE FUNCTIONS 
  ---------------------------------------------------------------------------*/

/* None */

 
#endif                                              /* OCI_ORACLE */


/* more includes */

#ifndef OCI1_ORACLE
#include <oci1.h>
#endif

#ifndef ORO_ORACLE
#include <oro.h>
#endif

#ifndef ORI_ORACLE
#include <ori.h>
#endif

#ifndef ORL_ORACLE
#include <orl.h>
#endif

#ifndef ORT_ORACLE
#include <ort.h>
#endif

#ifndef OCIEXTP_ORACLE
#include <ociextp.h>
#endif

#if defined(__STDC__) || defined(__cplusplus)
#include <ociapr.h>
#include <ociap.h>
#else
#include <ocikpr.h>
#include <ocikp.h>
#endif

#ifndef OCI8DP_ORACLE
#include <oci8dp.h>         /* interface definitions for the direct path api */
#endif

#ifndef OCIEXTP_ORACLE
#include <ociextp.h>
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

