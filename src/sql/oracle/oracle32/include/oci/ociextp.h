/*
 * $Header: /home/cvsroot/parser3project/parser3/src/sql/oracle/oracle32/include/oci/Attic/ociextp.h,v 1.1 2001/08/22 14:02:19 parser Exp $
 */

/* Copyright (c) Oracle Corporation 1996, 1997, 1999. All Rights Reserved. */ 
 
/* 
   NAME 
     ociextp.h - Interface Definitions for PL/SQL External Procedures

   DESCRIPTION 
     This header file contains C language callable interface from 
     PL/SQL External Procedures.

   PUBLIC FUNCTION(S) 
     OCIExtProcAllocCallMemory  - Allocate Call memory
     OCIExtProcRaiseExcp        - Raise Exception
     OCIExtProcRaiseExcpWithMsg - Raise Exception with message
     OCIExtProcGetEnv           - Get OCI Environment

   PRIVATE FUNCTION(S)
     <list of static functions defined in .c file - with one-line descriptions>

   EXAMPLES

   NOTES
     <other useful comments, qualifications, etc.>

   MODIFIED   (MM/DD/YY)
   whe         09/01/99 - 976457:check __cplusplus for C++ code
   asethi      04/15/99 - Created (by moving ociextp.h from /vobs/plsql/public)
   rhari       03/25/97 - Use ifndef
   rhari       12/18/96 - Include oratypes.h
   rhari       12/11/96 - #416977, Flip values of return codes
   rhari       12/02/96 - Define Return Code Macros
   rhari       11/18/96 - Error number is int
   rhari       10/30/96 - Fix OCIExtProcRaiseExcpWithMsg
   rhari       10/30/96 - Get rid of warnings
   rhari       10/04/96 - Fix OCIExtProcRaiseExcpWithMsg
   rhari       09/23/96 - Creation

*/

 
#ifndef OCIEXTP_ORACLE
# define OCIEXTP_ORACLE

# ifndef ORATYPES 
#  include <oratypes.h> 
# endif

# ifndef OCI_ORACLE
#  include <oci.h>
# endif

/*---------------------------------------------------------------------------
                     PUBLIC TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/


/* ----------------------------- Return Codes ----------------------------- */
/* Success and Error return codes for certain external procedure interface 
 * functions. If a particular interface function returns OCIEXTPROC_SUCCESS
 * or OCIEXTPROC_ERROR, then applications must use these macros to check
 * for return values.
 *
 *     OCIEXTPROC_SUCCESS  -- External Procedure Success Return Code 
 *     OCIEXTPROC_ERROR    -- External Procedure Failure Return Code 
 */
#define OCIEXTPROC_SUCCESS 0
#define OCIEXTPROC_ERROR   1


/* --------------------------- With-Context Type --------------------------- */
/*
 * The C callable interface to PL/SQL External Procedures require the 
 * With-Context parameter to be passed. The type of this structure is 
 * OCIExtProcContext is is opaque to the user.
 *
 * The user can declare the With-Context parameter in the application as
 *
 *    OCIExtProcContext *with_context;
 */
typedef struct OCIExtProcContext OCIExtProcContext;



/* ----------------------- OCIExtProcAllocCallMemory ----------------------- */
/* OCIExtProcAllocCallMemory
 *    Allocate N bytes of memory for the duration of the External Procedure.
 *
 *    Memory thus allocated will be freed by PL/SQL upon return from the
 *    External Procedure. You must not use any kind of 'free' function on 
 *    memory allocated by OCIExtProcAllocCallMemory.
 *    Use this function to allocate memory for function returns.
 * 
 * PARAMETERS
 * Input : 
 *    with_context - The with_context pointer that is passed to the C 
 *                   External Procedure.
 *                  Type of with_context : OCIExtProcContext *
 *    amount       - The number of bytes to allocate.
 *                   Type of amount : size_t
 *
 * Output :
 *    Nothing
 *
 * Return :
 *    An untyped (opaque) Pointer to the allocated memory. 
 *
 * Errors :
 *    A 0 return value should be treated as an error
 *
 * EXAMPLE 
 *  text *ptr = (text *)OCIExtProcAllocCallMemory(wctx, 1024)
 *
 */
#define OCIExtProcAllocCallMemory(with_context, amount) \
ociepacm(with_context, (size_t)amount)




/* -------------------------- OCIExtProcRaiseExcp -------------------------- */
/* OCIExtProcRaiseExcp
 *    Raise an Exception to PL/SQL.
 *
 *  Calling this function signalls an exception back to PL/SQL. After a
 *  successful return from this function, the External Procedure must start 
 *  its exit handling and return back to PL/SQL. Once an exception is 
 *  signalled to PL/SQL, INOUT and OUT arguments, if any, are not processed 
 *  at all.
 * 
 * PARAMETERS
 * Input :
 *   with_context - The with_context pointer that is passed to the C
 *                  External Procedure. 
 *                  Type of with_context : OCIExtProcContext *
 *   errnum       - Oracle Error number to signal to PL/SQL. errnum
 *                  must be a positive number and in the range 1 to 32767
 *                  Type of errnum : int
 * Output :
 *   Nothing
 *
 * Return :
 *   OCIEXTPROC_SUCCESS - If the call was successful.
 *   OCIEXTPROC_ERROR   - If the call failed.
 *
 */
#define OCIExtProcRaiseExcp(with_context, errnum) \
ocieperr(with_context, (int)errnum)





/* ---------------------- OCIExtProcRaiseExcpWithMsg ---------------------- */
/* OCIExtProcRaiseExcpWithMsg
 *    Raise an exception to PL/SQL. In addition, substitute the
 *    following error message string within the standard Oracle error 
 *    message string. See note for OCIExtProcRaiseExcp
 * 
 * PARAMETERS
 * Input :
 *   with_context  - The with_context pointer that is passed to the C
 *                   External Procedure.
 *                   Type of with_context : OCIExtProcContext *
 *   errnum        - Oracle Error number to signal to PL/SQL. errnum
 *                   must be a positive number and in the range 1 to 32767
 *                   Type of errnum : int
 *   errmsg        - The error message associated with the errnum.
 *                   Type of errmsg : char *
 *   len           - The length of the error message. 0 if errmsg is
 *                   null terminated string.
 *                   Type of len : size_t
 * Output :
 *   Nothing
 *
 * Return :
 *  OCIEXTPROC_SUCCESS - If the call was successful.
 *  OCIEXTPROC_ERROR   - If the call failed.
 *
 */
#define OCIExtProcRaiseExcpWithMsg(with_context, errnum, errmsg, msglen) \
ociepmsg(with_context, (int)errnum, errmsg, (size_t)msglen)



/* --------------------------- OCIExtProcGetEnv --------------------------- */
/* OCIExtProcGetEnv
 *    Get OCI Environment
 * 
 * PARAMETERS
 * Input :
 *    with_context - The with_context pointer that is passed to the C
 *                   External Procedure.
 *
 * Output :
 *    envh - The OCI Environment handle.
 *    svch - The OCI Service handle.
 *    errh - The OCI Error handle.
 *
 * Return :
 *  OCI_SUCCESS - Successful completion of the function.
 *  OCI_ERROR   - Error.
 *
 */
#define OCIExtProcGetEnv(with_context, envh, svch, errh) \
ociepgoe(with_context, envh, svch, errh)




/*---------------------------------------------------------------------------
                     PRIVATE TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                           PUBLIC FUNCTIONS
  ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                          PRIVATE FUNCTIONS
  ---------------------------------------------------------------------------*/



#if !defined(__STDC__) && !defined(__cplusplus)        /* K&R C - not ANSI C */
dvoid *ociepacm(/*_ OCIExtProcContext *with_context, size_t amount _*/);
#else                                                              /* ANSI C */
dvoid *ociepacm(OCIExtProcContext *with_context, size_t amount);
#endif



#if !defined(__STDC__) && !defined(__cplusplus)        /* K&R C - not ANSI C */
size_t ocieperr(/*_ OCIExtProcContext *with_context, int error_number _*/);
#else                                                              /* ANSI C */
size_t ocieperr(OCIExtProcContext *with_context, int error_number);
#endif



#if !defined(__STDC__) && !defined(__cplusplus)        /* K&R C - not ANSI C */
size_t ociepmsg(/*_ OCIExtProcContext *with_context, int error_number,
                 text *error_message, size_t len _*/);
#else                                                              /* ANSI C */
size_t ociepmsg(OCIExtProcContext *with_context, int error_number,
                text *error_message, size_t len );
#endif



#if !defined(__STDC__) && !defined(__cplusplus)        /* K&R C - not ANSI C */
sword ociepgoe(/*_ OCIExtProcContext *with_context, OCIEnv **envh, 
               OCISvcCtx **svch, OCIError **errh _*/);
#else                                                              /* ANSI C */
sword ociepgoe(OCIExtProcContext *with_context, OCIEnv **envh, 
               OCISvcCtx **svch, OCIError **errh);
#endif


#endif                                                     /* OCIEXTP_ORACLE */
