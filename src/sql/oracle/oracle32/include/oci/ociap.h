/*
 * $Header: /home/cvsroot/parser3project/parser3/src/sql/oracle/oracle32/include/oci/Attic/ociap.h,v 1.1 2001/08/22 14:02:18 parser Exp $
 */

/* Copyright (c) Oracle Corporation 1996, 1997, 1998, 1999. 
All Rights Reserved. */ 
 
/* NOTE:  See 'header_template.doc' in the 'doc' dve under the 'forms' 
      directory for the header file template that includes instructions. 
*/
 
/* 
   NAME 
     ociap.h - Oracle Call Interface - Ansi Prototypes

   DESCRIPTION 
     <short description of component this file declares/defines> 

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
     <other useful comments, qualifications, etc.>

   MODIFIED   (MM/DD/YY)
   porangas    09/22/99 - fix lines that exceed 80 chars:bug#974710
   slari       09/01/99 - remove OCIEnvCallback
   slari       08/23/99 - add OCIUcb in user callback functions
   dsaha       07/07/99 - Add OCIFEnvCreate for forms
   vyanaman    06/21/99 - Change OCI DateTime/Interval APIs.
   esoyleme    07/01/99 - expose MTS performance enhancements                  
   whe         06/14/99 - bug727872:add CONST to match definitions
   kkarun      02/23/99 - Fix OCIDateTime APIs
   jiyang      12/07/98 - Add comments for OCI_NLS_DUAL_CURRENCY
   aroy        12/01/98 - add OCIEnvCreate
   slari       11/23/98 - use ORASTDARG
   slari       11/21/98 - replace ellipsis by arglist in OCIUserCallback
   thchang     10/20/98 - correct comment on OCILobCreateTemporary
   slari       09/08/98 - allow envh to receive error info also in CallbackReg/
   kkarun      09/02/98 - Change const to CONST
   aroy        08/04/98 - add OCITerminate calls                               
   nramakri    06/25/98 - remove CONST from some OCIPickler APIs
   jiyang      06/22/98 - Fix a lint error
   nmallava    06/08/98 - ociistemporary -> envhp
   jhasenbe    05/27/98 - Remove definitions for U-Calls (Unicode)
   nmallava    05/18/98 - add comments
   sgollapu    05/19/98 - Change text to OraText
   aroy        04/20/98 - merge forward 8.0.5 -> 8.1.3
   nbhatt      05/14/98 -  aq listen call
   lchidamb    03/02/98 - Client Notification prototypes
   vyanaman    04/19/98 - System Timestamp
   kkarun      04/17/98 - Add more Interval functions
   vyanaman    04/17/98 - Fix min (proc error)
   vyanaman    04/16/98 - Add get/set TZ
   kkarun      04/13/98 - Add Datetime prototypes
   rkasamse    04/13/98 - change OCIEnv* to dvoid* for context/memory cart serv
   rkasamse    04/15/98 - chage pickler cart interface
   slari       03/20/98 - change proto of OCIUserCallback
   slari       02/17/98 - add OCIUserCallback
   jiyang      04/02/98 - Accept both env and user handles for NLS
   rkasamse    03/20/98 - remove prototypes for OCIMemoryDuration* functions.
   tsaulys     03/20/98 - use environment or session handle
   nmallava    04/09/98 - OCILobLocatorAssign
   nmallava    04/07/98 - lobgetchunksize and writeappend apis
   jhasenbe    04/06/98 - Add new interfaces for Unicode support
   nmallava    03/17/98 - add interfaces
   nmallava    03/16/98 - add open/close apis
   nmallava    03/10/98 - add temporary lobs apis
   sgollapu    07/10/97 - Add OCIReset
   sgollapu    02/09/98 - OCI non-blocking
   nramakri    01/16/98 - remove #ifdef NEVER clause for OCIExtract
   rmurthy     01/08/98 - OCIContextGenerateKey: change ub1 to ub4
   ewaugh      12/18/97 - Turn type wrappers into functions.
   skabraha    12/02/97 - adding OCIFile functions
   rhwu        12/02/97 - add OCI Thread
   nramakri    12/15/97 - move to core4
   nramakri    12/11/97 - modify OCIExtract prototype
   ewaugh      12/10/97 - add OCIFormat prototypes
   nmallava    12/17/97 - Add ilob open and close apis
   rkasamse    12/03/97 - Change some of the function names for pickler cartrid
   nramakri    11/12/97 - add OCIExtract prototypes
   rkasamse    11/21/97 - add prototypes for memory cartridge services and cont
   rkasamse    11/03/97 - Add pickler cartridge interfaces.
   jiyang      11/11/97 - Add NLS service for cartridge
   tanguyen    08/19/97 -
   cxcheng     07/30/97 - replace OCISvcCtx with OCISvcCtx
   schandra    06/25/97 - AQ OCI interface
   bnainani    07/21/97 - add prototypes for Oracle XA extensions
   esoyleme    05/13/97 - move failover callback prototype
   skmishra    05/06/97 - stdc compiler fixes
   skmishra    04/24/97 - C++ Compatibility changes
   skotsovo    04/21/97 - make lob parameter names consistent
   rwhitman    04/16/97 - Fix LOB prototypes - Olint OCI 8.0.3
   ramkrish    04/15/97 - Add free flag to OCILobFlushBuffer
   dchatter    04/10/97 - add nzt.h inclusion
   cxcheng     04/09/97 - change objnamp from CONST text* to dvoid*
   cxcheng     04/08/97 - fix prototype of OCIDescribeAny()
   skotsovo    03/31/97 - remove OCILobLocatorSize
   skotsovo    03/27/97 - add OCILobLoadFromFile
   bcchang     02/18/97 - Fix syntax error
   dchatter    01/13/97 - fix comments on LOB calls
   aroy        01/10/97 - remove ocilobfilecreate delete
   sgollapu    12/27/96 - Correct OCILogon prototype
   dchatter    01/04/97 - comments to describe the functions
   sgollapu    11/25/96 - Change OCILobFileIsExistent
   schandra    11/18/96 - Remove xa.h include
   sgollapu    11/09/96 - Change prototype of OCIDescribeAny
   dchatter    10/31/96 - delete CONST from lob write cb fn
   dchatter    10/30/96 - more changes
   dchatter    10/26/96 - lob/file long name corrections
   slari       10/16/96 - delete unused calls
   rwessman    10/29/96 - Fixed OCISecurityGetIdentity prototype
   bcchang     10/25/96 - Fix syntax error
   sgollapu    10/22/96 - Add OCILogon and OCILogoff
   rwessman    10/16/96 - Added cryptographic and digital signature functions
   sgollapu    10/10/96 - Add ocibdp and ocibdn
   rxgovind    10/07/96 - add oci file calls
   skotsovo    10/01/96 - move orl lob fnts to oci
   skotsovo    09/20/96 - in OCILobGetLength(), remove the 'isnull' parameter.
   aroy        08/29/96 - change prototype for Nchar Lob support
   dchatter    08/21/96 - OCIResultSetToStmt prototype change
   sthakur     08/14/96 - add OCIParamSet
   schandra    07/26/96 - TX OCI return values - sb4->sword
   aroy        07/17/96 - terminology change: OCILobLocator => OCILobLocator
   dchatter    07/01/96 - create ANSI prototypes
   dchatter    07/01/96 - Creation

*/

 
#ifndef OCIAP_ORACLE
# define OCIAP_ORACLE

# ifndef ORATYPES 
#  include <oratypes.h> 
# endif 
 
#ifndef ORASTDARG
#include <stdarg.h>
#define ORASTDARG
#endif

#ifndef OCIDFN
#include <ocidfn.h>
#endif

#ifndef NZT_ORACLE
#include <nzt.h>
#endif /* NZT_ORACLE */
 
#ifndef OCI_ORACLE
#include <oci.h>
#endif

#ifndef ORT_ORACLE
#include <ort.h>
#endif



/*---------------------------------------------------------------------------
                     PUBLIC TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                     PRIVATE TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                           PUBLIC FUNCTIONS
  ---------------------------------------------------------------------------*/

/*****************************************************************************
                              DESCRIPTION
******************************************************************************
Note: the descriptions of the functions are alphabetically arranged. Please 
maintain the arrangement when adding a new function description. The actual 
prototypes are below this comment section and donot follow any alphabetical 
ordering. 


--------------------------------OCIAttrGet------------------------------------

OCIAttrGet()
Name
OCI Attribute Get
Purpose
This call is used to get a particular attribute of a handle. 
Syntax
sword OCIAttrGet ( CONST dvoid    *trgthndlp,
                 ub4            trghndltyp,
                 dvoid          *attributep,
                 ub4            *sizep,
                 ub4            attrtype,
                 OCIError       *errhp );
Comments
This call is used to get a particular attribute of a handle.
See Appendix�B,  "Handle Attributes",  for a list of handle types and their 
readable attributes.
Parameters
trgthndlp (IN) - is the pointer to a handle type. 
trghndltyp (IN) - is the handle type. 
attributep (OUT) - is a pointer to the storage for an attribute value. The 
attribute value is filled in. 
sizep (OUT) - is the size of the attribute value. 
This can be passed in as NULL for most parameters as the size is well known. 
For text* parameters, a pointer to a ub4 must be passed in to get the length 
of the string. 
attrtype (IN) - is the type of attribute.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
Related Functions
OCIAttrSet()

--------------------------------OCIAttrSet------------------------------------


OCIAttrSet()
Name
OCI Attribute Set
Purpose
This call is used to set a particular attribute of a handle or a descriptor. 
Syntax
sword OCIAttrSet ( dvoid       *trgthndlp,
                 ub4         trghndltyp,
                 dvoid       *attributep,
                 ub4         size,
                 ub4         attrtype,
                 OCIError    *errhp );
Comments
This call is used to set a particular attribute of a handle or a descriptor. 
See Appendix B for a list of handle types and their writeable attributes.
Parameters
trghndlp (IN/OUT) - the pointer to a handle type whose attribute gets 
modified. 
trghndltyp (IN/OUT) - is the handle type. 
attributep (IN) - a pointer to an attribute value. 
The attribute value is copied into the target handle. If the attribute value 
is a pointer, then only the pointer is copied, not the contents of the pointer.
size (IN) - is the size of an attribute value. This can be passed in as 0 for 
most attributes as the size is already known by the OCI library. For text*
attributes, a ub4 must be passed in set to the length of the string. 
attrtype (IN) - the type of attribute being set.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
Related Functions
OCIAttrGet()



--------------------------------OCIBindArrayOfStruct--------------------------



OCIBindArrayOfStruct()
Name
OCI Bind for Array of Structures
Purpose
This call sets up the skip parameters for a static array bind.
Syntax
sword OCIBindArrayOfStruct ( OCIBind     *bindp,
                           OCIError    *errhp,
                           ub4         pvskip, 
                           ub4         indskip, 
                           ub4         alskip, 
                           ub4         rcskip );
Comments
This call sets up the skip parameters necessary for a static array bind.
This call follows a call to OCIBindByName() or OCIBindByPos(). The bind 
handle returned by that initial bind call is used as a parameter for the 
OCIBindArrayOfStruct() call.
For information about skip parameters, see the section "Arrays of Structures" 
on page 4-16.
Parameters
bindp (IN) - the handle to a bind structure. 
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
pvskip (IN) - skip parameter for the next data value. 
indskip (IN) - skip parameter for the next indicator value or structure. 
alskip (IN) - skip parameter for the next actual length value. 
rcskip (IN) - skip parameter for the next column-level return code value. 
Related Functions
OCIAttrGet()


--------------------------------OCIBindByName---------------------------------

OCIBindByName()
Name
OCI Bind by Name
Purpose
Creates an association between a program variable and a placeholder in a SQL 
statement or PL/SQL block.
Syntax
sword OCIBindByName (
              OCIStmt       *stmtp, 
              OCIBind       **bindp,
              OCIError      *errhp,
              CONST OraText    *placeholder,
              sb4           placeh_len,
              dvoid         *valuep,
              sb4           value_sz,
              ub2           dty,
              dvoid         *indp,
              ub2           *alenp,
              ub2           *rcodep,
              ub4           maxarr_len,
              ub4           *curelep, 
              ub4           mode ); 
Description
This call is used to perform a basic bind operation. The bind creates an 
association between the address of a program variable and a placeholder in a 
SQL statement or PL/SQL block. The bind call also specifies the type of data 
which is being bound, and may also indicate the method by which data will be 
provided at runtime.
This function also implicitly allocates the bind handle indicated by the bindp 
parameter.
Data in an OCI application can be bound to placeholders statically or 
dynamically. Binding is static when all the IN bind data and the OUT bind 
buffers are well-defined just before the execute. Binding is dynamic when the 
IN bind data and the OUT bind buffers are provided by the application on 
demand at execute time to the client library. Dynamic binding is indicated by 
setting the mode parameter of this call to OCI_DATA_AT_EXEC.
Related Functions: For more information about dynamic binding, see 
the section "Runtime Data Allocation and Piecewise Operations" on 
page 5-16.
Both OCIBindByName() and OCIBindByPos() take as a parameter a bind handle, 
which is implicitly allocated by the bind call A separate bind handle is 
allocated for each placeholder the application is binding.
Additional bind calls may be required to specify particular attributes 
necessary when binding certain data types or handling input data in certain 
ways:
If arrays of structures are being utilized, OCIBindArrayOfStruct() must 
be called to set up the necessary skip parameters.
If data is being provided dynamically at runtime, and the application 
will be using user-defined callback functions, OCIBindDynamic() must 
be called to register the callbacks.
If a named data type is being bound, OCIBindObject() must be called to 
specify additional necessary information.
Parameters
stmth (IN/OUT) - the statement handle to the SQL or PL/SQL statement 
being processed.
bindp (IN/OUT) - a pointer to a pointer to a bind handle which is implicitly 
allocated by this call.  The bind handle  maintains all the bind information for 
this particular input value. The handle is feed implicitly when the statement 
handle is deallocated.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
placeholder (IN) - the placeholder attributes are specified by name if ocibindn() 
is being called.
placeh_len (IN) - the length of the placeholder name specified in placeholder.
valuep (IN/OUT) - a pointer to a data value or an array of data values of the 
type specified in the dty parameter. An array of data values can be specified 
for mapping into a PL/SQL table or for providing data for SQL multiple-row 
operations. When an array of bind values is provided, this is called an array 
bind in OCI terms. Additional attributes of the array bind (not bind to a 
column of ARRAY type) are set up in OCIBindArrayOfStruct() call. 
For a REF, named data type  bind, the valuep parameter is used only for IN 
bind data. The pointers to OUT buffers are set in the pgvpp parameter 
initialized by OCIBindObject(). For named data type and REF binds, the bind 
values are unpickled into the Object Cache. The OCI object navigational calls 
can then be used to navigate the objects and the refs in the Object Cache.
If the OCI_DATA_AT_EXEC mode is specified in the mode parameter, valuep 
is ignored for all data types. OCIBindArrayOfStruct() cannot be used and 
OCIBindDynamic() must be invoked to provide callback functions if desired. 
value_sz (IN) - the size of a data value. In the case of an array bind, this is the 
maximum size of any element possible with the actual sizes being specified in 
the alenp parameter. 
If the OCI_DATA_AT_EXEC mode is specified, valuesz defines the maximum 
size of the data that can be ever provided at runtime for data types other than 
named data types or REFs. 
dty (IN) - the data type of the value(s) being bound. Named data types 
(SQLT_NTY) and REFs (SQLT_REF) are valid only if the application has been 
initialized in object mode. For named data types, or REFs, additional calls 
must be made with the bind handle to set up the datatype-specific attributes.
indp (IN/OUT) - pointer to an indicator variable or array. For scalar data 
types, this is a pointer to sb2 or an array of sb2s. For named data types, 
this pointer is ignored and the actual pointer to the indicator structure or 
an array of indicator structures is initialized by OCIBindObject(). 
Ignored for dynamic binds.
See the section "Indicator Variables" on page 2-43 for more information about 
indicator variables.
alenp (IN/OUT) - pointer to array of actual lengths of array elements. Each 
element in alenp is the length of the data in the corresponding element in the 
bind value array before and after the execute. This parameter is ignored for 
dynamic binds.
rcodep (OUT) - pointer to array of column level return codes. This parameter 
is ignored for dynamic binds.
maxarr_len (IN) - the maximum possible number of elements of type dty in a 
PL/SQL binds. This parameter is not required for non-PL/SQL binds. If 
maxarr_len is non-zero, then either OCIBindDynamic() or 
OCIBindArrayOfStruct() can be invoked to set up additional bind attributes. 
curelep(IN/OUT) - a pointer to the actual number of elements. This parameter 
is only required for PL/SQL binds.
mode (IN) - the valid modes for this parameter are:
OCI_DEFAULT. This is default mode.
OCI_DATA_AT_EXEC. When this mode is selected, the value_sz 
parameter defines the maximum size of the data that can be ever 
provided at runtime. The application must be ready to provide the OCI 
library runtime IN data buffers at any time and any number of times. 
Runtime data is provided in one of the two ways:
callbacks using a user-defined function which must be registered 
with a subsequent call to OCIBindDynamic(). 
a polling mechanism using calls supplied by the OCI. This mode 
is assumed if no callbacks are defined.
For more information about using the OCI_DATA_AT_EXEC mode, see 
the section "Runtime Data Allocation and Piecewise Operations" on 
page 5-16.
When the allocated buffers are not required any more, they should be 
freed by the client. 
Related Functions
OCIBindDynamic(), OCIBindObject(), OCIBindArrayOfStruct(), OCIAttrGet()



-------------------------------OCIBindByPos-----------------------------------


OCIBindByPos()
Name
OCI Bind by Position
Purpose
Creates an association between a program variable and a placeholder in a SQL 
statement or PL/SQL block.
Syntax
sword OCIBindByPos ( 
              OCIStmt      *stmtp, 
              OCIBind      **bindp,
              OCIError     *errhp,
              ub4          position,
              dvoid        *valuep,
              sb4          value_sz,
              ub2          dty,
              dvoid        *indp,
              ub2          *alenp,
              ub2          *rcodep,
              ub4          maxarr_len,
              ub4          *curelep, 
              ub4          mode);

Description
This call is used to perform a basic bind operation. The bind creates an 
association between the address of a program variable and a placeholder in a 
SQL statement or PL/SQL block. The bind call also specifies the type of data 
which is being bound, and may also indicate the method by which data will be 
provided at runtime.
This function also implicitly allocates the bind handle indicated by the bindp 
parameter.
Data in an OCI application can be bound to placeholders statically or 
dynamically. Binding is static when all the IN bind data and the OUT bind 
buffers are well-defined just before the execute. Binding is dynamic when the 
IN bind data and the OUT bind buffers are provided by the application on 
demand at execute time to the client library. Dynamic binding is indicated by 
setting the mode parameter of this call to OCI_DATA_AT_EXEC.
Related Functions: For more information about dynamic binding, see 
the section "Runtime Data Allocation and Piecewise Operations" on 
page 5-16
Both OCIBindByName() and OCIBindByPos() take as a parameter a bind handle, 
which is implicitly allocated by the bind call A separate bind handle is 
allocated for each placeholder the application is binding.
Additional bind calls may be required to specify particular attributes 
necessary when binding certain data types or handling input data in certain 
ways:
If arrays of structures are being utilized, OCIBindArrayOfStruct() must 
be called to set up the necessary skip parameters.
If data is being provided dynamically at runtime, and the application 
will be using user-defined callback functions, OCIBindDynamic() must 
be called to register the callbacks.
If a named data type is being bound, OCIBindObject() must be called to 
specify additional necessary information.
Parameters
stmth (IN/OUT) - the statement handle to the SQL or PL/SQL statement 
being processed.
bindp (IN/OUT) - a pointer to a pointer to a bind handle which is implicitly 
allocated by this call.  The bind handle  maintains all the bind information for 
this particular input value. The handle is feed implicitly when the statement 
handle is deallocated.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
position (IN) - the placeholder attributes are specified by position if ocibindp() 
is being called.
valuep (IN/OUT) - a pointer to a data value or an array of data values of the 
type specified in the dty parameter. An array of data values can be specified 
for mapping into a PL/SQL table or for providing data for SQL multiple-row 
operations. When an array of bind values is provided, this is called an array 
bind in OCI terms. Additional attributes of the array bind (not bind to a 
column of ARRAY type) are set up in OCIBindArrayOfStruct() call. 
For a REF, named data type  bind, the valuep parameter is used only for IN 
bind data. The pointers to OUT buffers are set in the pgvpp parameter 
initialized by OCIBindObject(). For named data type and REF binds, the bind 
values are unpickled into the Object Cache. The OCI object navigational calls 
can then be used to navigate the objects and the refs in the Object Cache.
If the OCI_DATA_AT_EXEC mode is specified in the mode parameter, valuep 
is ignored for all data types. OCIBindArrayOfStruct() cannot be used and 
OCIBindDynamic() must be invoked to provide callback functions if desired. 
value_sz (IN) - the size of a data value. In the case of an array bind, this is the 
maximum size of any element possible with the actual sizes being specified in 
the alenp parameter. 
If the OCI_DATA_AT_EXEC mode is specified, valuesz defines the maximum 
size of the data that can be ever provided at runtime for data types other than 
named data types or REFs. 
dty (IN) - the data type of the value(s) being bound. Named data types 
(SQLT_NTY) and REFs (SQLT_REF) are valid only if the application has been 
initialized in object mode. For named data types, or REFs, additional calls 
must be made with the bind handle to set up the datatype-specific attributes.
indp (IN/OUT) - pointer to an indicator variable or array. For scalar data 
types, this is a pointer to sb2 or an array of sb2s. For named data types, 
this pointer is ignored and the actual pointer to the indicator structure or 
an array of indicator structures is initialized by OCIBindObject(). Ignored 
for dynamic binds.
See the section "Indicator Variables" on page 2-43 for more information about 
indicator variables.
alenp (IN/OUT) - pointer to array of actual lengths of array elements. Each 
element in alenp is the length of the data in the corresponding element in the 
bind value array before and after the execute. This parameter is ignored for 
dynamic binds.
rcodep (OUT) - pointer to array of column level return codes. This parameter 
is ignored for dynamic binds.
maxarr_len (IN) - the maximum possible number of elements of type dty in a 
PL/SQL binds. This parameter is not required for non-PL/SQL binds. If 
maxarr_len is non-zero, then either OCIBindDynamic() or 
OCIBindArrayOfStruct() can be invoked to set up additional bind attributes. 
curelep(IN/OUT) - a pointer to the actual number of elements. This parameter 
is only required for PL/SQL binds.
mode (IN) - the valid modes for this parameter are:
OCI_DEFAULT. This is default mode.
OCI_DATA_AT_EXEC. When this mode is selected, the value_sz 
parameter defines the maximum size of the data that can be ever 
provided at runtime. The application must be ready to provide the OCI 
library runtime IN data buffers at any time and any number of times. 
Runtime data is provided in one of the two ways:
callbacks using a user-defined function which must be registered 
with a subsequent call to OCIBindDynamic() . 
a polling mechanism using calls supplied by the OCI. This mode 
is assumed if no callbacks are defined.
For more information about using the OCI_DATA_AT_EXEC mode, see 
the section "Runtime Data Allocation and Piecewise Operations" on 
page 5-16.
When the allocated buffers are not required any more, they should be 
freed by the client. 
Related Functions
OCIBindDynamic(), OCIBindObject(), OCIBindArrayOfStruct(), OCIAttrGet()



-------------------------------OCIBindDynamic---------------------------------

OCIBindDynamic()
Name
OCI Bind Dynamic Attributes
Purpose
This call is used to register user callbacks for dynamic data allocation. 
Syntax
sword OCIBindDynamic( OCIBind     *bindp,
                    OCIError    *errhp,
                    dvoid       *ictxp, 
                    OCICallbackInBind         (icbfp)(
                                dvoid            *ictxp,
                                OCIBind          *bindp,
                                ub4              iter, 
                                ub4              index, 
                                dvoid            **bufpp,
                                ub4              *alenp,
                                ub1              *piecep, 
                                dvoid            **indp ),
                    dvoid       *octxp,
                    OCICallbackOutBind         (ocbfp)(
                                dvoid            *octxp,
                                OCIBind          *bindp,
                                ub4              iter, 
                                ub4              index, 
                                dvoid            **bufp, 
                                ub4              **alenpp,
                                ub1              *piecep,
                                dvoid            **indpp, 
                                ub2              **rcodepp)   );
Comments
This call is used to register user-defined callback functions for providing 
data for an UPDATE or INSERT if OCI_DATA_AT_EXEC mode was specified in a 
previous call to OCIBindByName() or OCIBindByPos(). 
The callback function pointers must return OCI_CONTINUE if it the call is 
successful. Any return code other than OCI_CONTINUE signals that the client 
wishes to abort processing immediately.
For more information about the OCI_DATA_AT_EXEC mode, see the section 
"Runtime Data Allocation and Piecewise Operations" on page 5-16.
Parameters
bindp (IN/OUT) - a bind handle returned by a call to OCIBindByName() or 
OCIBindByPos(). 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
ictxp (IN) - the context pointer required by the call back function icbfp. 
icbfp (IN) - the callback function which returns a pointer to the IN bind 
value or piece at run time. The callback takes in the following parameters. 
ictxp (IN/OUT) - the context pointer for this callback function. 
bindp (IN) - the bind handle passed in to uniquely identify this bind 
variable. 
iter (IN) - 1-based execute iteration value. 
index (IN) - index of the current array, for an array bind. 1 based not 
greater than curele parameter of the bind call. 
index (IN) - index of the current array, for an array bind. This parameter 
is 1-based, and may not be greater than curele parameter of the bind call. 
bufpp (OUT) - the pointer to the buffer. 
piecep (OUT) - which piece of the bind value. This can be one of the 
following values - OCI_ONE_PIECE, OCI_FIRST_PIECE, 
OCI_NEXT_PIECE and OCI_LAST_PIECE.
indp (OUT) - contains the indicator value. This is apointer to either an 
sb2 value or a pointer to an indicator structure for binding named data 
types. 
indszp (OUT) - contains the indicator value size. A pointer containing 
the size of either an sb2 or an indicator structure pointer. 
octxp (IN) - the context pointer required by the callback function ocbfp. 
ocbfp (IN) - the callback function which returns a pointer to the OUT bind 
value or piece at run time. The callback takes in the following parameters. 
octxp (IN/OUT) - the context pointer for this call back function. 
bindp (IN) - the bind handle passed in to uniquely identify this bind 
variable. 
iter (IN) - 1-based execute iteration value. 
index (IN) - index of the current array, for an array bind. This parameter 
is 1-based, and must not be greater than curele parameter of the bind call. 
bufpp (OUT) - a pointer to a buffer to write the bind value/piece. 
buflp (OUT) - returns the buffer size. 
alenpp (OUT) - a pointer to a storage for OCI to fill in the size of the bind 
value/piece after it has been read. 
piecep (IN/OUT) - which piece of the bind value. It will be set by the 
library to be one of the following values - OCI_ONE_PIECE or 
OCI_NEXT_PIECE. The callback function can leave it unchanged or set 
it to OCI_FIRST_PIECE or OCI_LAST_PIECE. By default - 
OCI_ONE_PIECE. 
indpp (OUT) - returns a pointer to contain the indicator value which 
either an sb2 value or a pointer to an indicator structure for named data 
types. 
indszpp (OUT) - returns a pointer to return the size of the indicator 
value which is either size of an sb2 or size of an indicator structure. 
rcodepp (OUT) - returns a pointer to contains the return code. 
Related Functions
OCIAttrGet()


---------------------------------OCIBindObject--------------------------------


OCIBindObject()
Name
OCI Bind Object
Purpose
This function sets up additional attributes which are required for a named 
data type (object)  bind.
Syntax
sword OCIBindObject ( OCIBind          *bindp,
                    OCIError         *errhp, 
                    CONST OCIType    *type,
                    dvoid            **pgvpp, 
                    ub4              *pvszsp, 
                    dvoid            **indpp, 
                    ub4              *indszp, );
Comments
This function sets up additional attributes which binding a named data type 
or a REF. An error will be returned if this function is called when the OCI 
environment has been initialized in non-object mode. 
This call takes as a paramter a type descriptor object (TDO) of datatype 
OCIType for the named data type being defined.  The TDO can be retrieved 
with a call to OCITypeByName().
If the OCI_DATA_AT_EXEC mode was specified in ocibindn() or ocibindp(), the 
pointers to the IN buffers are obtained either using the callback icbfp 
registered in the OCIBindDynamic() call or by the OCIStmtSetPieceInfo() call. 
The buffers are dynamically allocated for the OUT data and the pointers to 
these buffers are returned either by calling ocbfp() registered by the 
OCIBindDynamic() or by setting the pointer to the buffer in the buffer passed 
in by OCIStmtSetPieceInfo() called when OCIStmtExecute() returned 
OCI_NEED_DATA. The memory of these client library- allocated buffers must be 
freed when not in use anymore by using the OCIObjectFreee() call.
Parameters
bindp ( IN/OUT) - the bind handle returned by the call to OCIBindByName() 
or OCIBindByPos(). 
errhp ( IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
type ( IN) - points to the TDO which describes the type of the program 
variable being bound. Retrieved by calling OCITypeByName().
pgvpp ( IN/OUT) - points to a pointer to the program variable buffer. For an 
array, pgvpp points to an array of pointers. When the bind variable is also an 
OUT variable, the OUT Named Data Type value or REF is allocated 
(unpickled) in the Object Cache, and a pointer to the value or REF is returned,
At the end of execute, when all OUT values have been received, pgvpp points 
to an array of pointer(s) to these newly allocated named data types in the 
object cache. 
pgvpp is ignored if the OCI_DATA_AT_EXEC mode is set. Then the Named 
Data Type buffers are requested at runtime. For static array binds, skip 
factors may be specified using the OCIBindArrayOfStruct() call. The skip 
factors are used to compute the address of the next pointer to the value, the 
indicator structure and their sizes.
pvszsp ( IN/OUT) - points to the size of the program variable. The size of the 
named data type is not required on input. For an array, pvszsp is an array of 
ub4s. On return, for OUT bind variables, this points to size(s) of the Named 
Data Types and REFs received. pvszsp is ignored if the OCI_DATA_AT_EXEC 
mode is set. Then the size of the buffer is taken at runtime.
indpp ( IN/OUT) - points to a pointer to the program variable buffer 
containing the parallel indicator structure. For an array, points to an array of 
pointers. When the bind variable is also an OUT bind variable, memory is 
allocated in the object cache, to store the unpickled OUT indicator values. At 
the end of the execute when all OUT values have been received, indpp points 
to the pointer(s) to these newly allocated indicator structure(s). 
indpp is ignored if the OCI_DATA_AT_EXEC mode is set. Then the indicator 
is requested at runtime.
indszp ( IN/OUT) - points to the size of the IN indicator structure program 
variable. For an array, it is an array of sb2s. On return for OUT bind variables, 
this points to size(s) of the received OUT indicator structures.
indszp is ignored if the OCI_DATA_AT_EXEC mode is set. Then the indicator 
size is requested at runtime.
Related Functions
OCIAttrGet()



----------------------------------OCIBreak------------------------------------


OCIBreak()
Name
OCI Break
Purpose
This call performs an immediate (asynchronous) abort of any currently 
executing OCI function that is associated with a server .
Syntax
sword OCIBreak ( dvoid      *hndlp,
                 OCIError   *errhp);
Comments
This call performs an immediate (asynchronous) abort of any currently 
executing OCI function that is associated with a server. It is normally used 
to stop a long-running OCI call being processed on the server.
This call can take either the service context handle or the server context 
handle as a parameter to identify the function to be aborted.
Parameters
hndlp (IN) - the service context handle or the server context handle.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
Related Functions

------------------------------OCIDateTimeAssign --------------------------
sword OCIDateTimeAssign(dvoid *hndl, OCIError *err, CONST OCIDateTime *from, 
		        OCIDateTime *to);
NAME: OCIDateTimeAssign - OCIDateTime Assignment
PARAMETERS:
hndl (IN) - Session/Env handle.
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
from (IN) - datetime to be assigned
to (OUT) - lhs of assignment
DESCRIPTION:
        Performs date assignment. The type of the output will be same as that
	of input

------------------------------OCIDateTimeCheck----------------------------
sword OCIDateTimeCheck(dvoid *hndl, OCIError *err, CONST OCIDateTime *date, 
		 ub4 *valid );
NAME: OCIDateTimeCheck - OCIDateTime CHecK if the given date is valid
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
date (IN) - date to be checked
valid (OUT) -  returns zero for a valid date, otherwise 
  		the ORed combination of all error bits specified below:
   Macro name                   Bit number      Error
   ----------                   ----------      -----
   OCI_DATE_INVALID_DAY         0x1             Bad day
   OCI_DATE_DAY_BELOW_VALID     0x2             Bad DAy Low/high bit (1=low)
   OCI_DATE_INVALID_MONTH       0x4             Bad MOnth
   OCI_DATE_MONTH_BELOW_VALID   0x8             Bad MOnth Low/high bit (1=low)
   OCI_DATE_INVALID_YEAR        0x10            Bad YeaR
   OCI_DATE_YEAR_BELOW_VALID    0x20            Bad YeaR Low/high bit (1=low)
   OCI_DATE_INVALID_HOUR        0x40            Bad HouR
   OCI_DATE_HOUR_BELOW_VALID    0x80            Bad HouR Low/high bit (1=low)
   OCI_DATE_INVALID_MINUTE      0x100           Bad MiNute
   OCI_DATE_MINUTE_BELOW_VALID	0x200           Bad MiNute Low/high bit (1=low)
   OCI_DATE_INVALID_SECOND      0x400           Bad SeCond
   OCI_DATE_SECOND_BELOW_VALID  0x800           bad second Low/high bit (1=low)
   OCI_DATE_DAY_MISSING_FROM_1582 0x1000        Day is one of those "missing"
                                                from 1582
   OCI_DATE_YEAR_ZERO           0x2000          Year may not equal zero
   OCI_DATE_INVALID_TIMEZONE    0x4000          Bad Timezone
   OCI_DATE_INVALID_FORMAT      0x8000          Bad date format input

   So, for example, if the date passed in was 2/0/1990 25:61:10 in
   (month/day/year hours:minutes:seconds format), the error returned
   would be OCI_DATE_INVALID_DAY | OCI_DATE_DAY_BELOW_VALID |
   OCI_DATE_INVALID_HOUR | OCI_DATE_INVALID_MINUTE

DESCRIPTION:
  	Check if the given date is valid.
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
        OCI_ERROR if
  	  'date' and 'valid' pointers are NULL pointers

------------------------------- OCIDateTimeCompare----------------------------
sword OCIDateTimeCompare(dvoid *hndl, OCIError *err, CONST OCIDateTime *date1, 
                     CONST OCIDateTime *date2,  sword *result );
NAME: OCIDateTimeCompare - OCIDateTime CoMPare dates
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
date1, date2 (IN) - dates to be compared
result (OUT) - comparison result, 0 if equal, -1 if date1 < date2, 
  		1 if date1 > date2
DESCRIPTION:
The function OCIDateCompare compares two dates. It returns -1 if 
date1 is smaller than date2, 0 if they are equal, and 1 if date1 is 
greater than date2.
RETURNS:
       OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
        OCI_ERROR if
  	  invalid date
	  input dates are not mutually comparable

------------------------------OCIDateTimeConvert----------------------
sword OCIDateTimeConvert(dvoid *hndl, OCIError *err, OCIDateTime *indate, 
				OCIDateTime *outdate);
NAME: OCIDateTimeConvert - Conversion between different DATETIME types
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
indate (IN) - pointer to input date
outdate (OUT) - pointer to output datetime 
DESCRIPTION: Converts one datetime type to another. The result type is
       the type of the 'outdate' descriptor.
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
	OCI_ERROR if
	    conversion not possible.
   
---------------------------- OCIDateTimeFromText-----------------------
sword OCIDateTimeFromText(dvoid *hndl, OCIError *err, CONST OraText *date_str, 
             size_t d_str_length, CONST OraText *fmt, ub1 fmt_length,
             CONST OraText *lang_name, size_t lang_length, OCIDateTime *date );
NAME: OCIDateTimeFromText - OCIDateTime convert String FROM Date
PARAMETERS:
hndl (IN) - Session/Env handle. If Session Handle is passed, the 
		    conversion takes place in session NLS_LANGUAGE and
		    session NLS_CALENDAR, otherwise the default is used.
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
date_str (IN) - input string to be converted to Oracle date
d_str_length (IN) - size of the input string, if the length is -1
  		then 'date_str' is treated as a null terminated  string
fmt (IN) - conversion format; if 'fmt' is a null pointer, then
                the string is expected to be in the default format for
		the datetime type.
fmt_length (IN) - length of the 'fmt' parameter
lang_name (IN) - language in which the names and abbreviations of
  	        days and months are specified, if null i.e. (OraText *)0,
  		the default language of session is used, 
lang_length (IN) - length of the 'lang_name' parameter
date (OUT) - given string converted to date
DESCRIPTION:
  	Converts the given string to Oracle datetime type set in the 
        OCIDateTime descriptor according to the specified format. Refer to 
        "TO_DATE" conversion function described in "Oracle SQL Language 
        Reference Manual" for a description of format.
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
        OCI_ERROR if
  	  invalid format
  	  unknown language
  	  invalid input string

--------------------------- OCIDateTimeGetDate-------------------------
sword OCIDateTimeGetDate(dvoid *hndl, OCIError *err,  CONST OCIDateTime *date, 
	                   sb2 *year, ub1 *month, ub1 *day );
NAME: OCIDateTimeGetDate - OCIDateTime Get Date (year, month, day)  
				portion of DATETIME. 
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
datetime (IN) - Pointer to OCIDateTime 
year      (OUT) - year value
month     (OUT) - month value
day       (OUT) - day value

--------------------------- OCIDateTimeGetTime ------------------------
sword OCIDateTimeGetTime(dvoid *hndl, OCIError *err, OCIDateTime *datetime,
		 ub1 *hour, ub1 *minute, ub1 *sec, ub4 *fsec);
NAME: OCIDateTimeGetTime - OCIDateTime Get Time (hour, min, second, 
			fractional second)  of DATETIME. 
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
datetime (IN) - Pointer to OCIDateTime 
hour      (OUT) - hour value
minute       (OUT) - minute value
sec       (OUT) - second value
fsec      (OUT) - Fractional Second value

--------------------------- OCIDateTimeGetTimeZoneOffset ----------------------
sword OCIDateTimeGetTimeZoneOffset(dvoid *hndl,OCIError *err,CONST 
              OCIDateTime *datetime,sb1 *hour,sb1  *minute);

NAME: OCIDateTimeGetTimeZoneOffset - OCIDateTime Get TimeZone (hour, minute)  
                         portion of DATETIME. 
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
datetime (IN) - Pointer to OCIDateTime 
hour      (OUT) - TimeZone Hour value
minute     (OUT) - TimeZone Minute value

--------------------------- OCIDateTimeSysTimeStamp---------------------
sword OCIDateTimeSysTimeStamp(dvoid *hndl, OCIError *err, 
              OCIDateTime *sys_date );
 
NAME: OCIDateTimeSysTimeStamp - Returns system date/time as a TimeStamp with 
                      timezone
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
sys_date (OUT) - Pointer to output timestamp
 
DESCRIPTION: 
        Gets the system current date and time as a timestamp with timezone
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.


------------------------------OCIDateTimeIntervalAdd----------------------
sword OCIDateTimeIntervalAdd(dvoid *hndl, OCIError *err, OCIDateTime *datetime,
	 OCIInterval *inter, OCIDateTime *outdatetime);
NAME: OCIDateTimeIntervalAdd - Adds an interval to datetime
PARAMETERS:
hndl (IN) - Session/Env handle.
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
datetime (IN) - pointer to input datetime
inter    (IN) - pointer to interval 
outdatetime (IN) - pointer to output datetime. The output datetime 
				will be of same type as input datetime
DESCRIPTION: 
	Adds an interval to a datetime to produce a resulting datetime
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
	OCI_ERROR if:
		resulting date is before Jan 1, -4713
		resulting date is after Dec 31, 9999

------------------------------OCIDateTimeIntervalSub----------------------
sword OCIDateTimeIntervalSub(dvoid *hndl, OCIError *err, OCIDateTime *datetime,
              OCIInterval *inter, OCIDateTime *outdatetime);
NAME: OCIDateTimeIntervalSub - Subtracts an interval from a datetime
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
datetime (IN) - pointer to input datetime
inter    (IN) - pointer to interval 
outdatetime (IN) - pointer to output datetime. The output datetime 
				will be of same type as input datetime
DESCRIPTION: 
 	Subtracts an interval from a datetime and stores the result in a
     	datetime
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
	OCI_ERROR if:
		resulting date is before Jan 1, -4713
		resulting date is after Dec 31, 9999

--------------------------- OCIDateTimeConstruct-------------------------
sword OCIDateTimeConstruct(dvoid  *hndl,OCIError *err,OCIDateTime *datetime,
               sb2 year,ub1 month,ub1 day,ub1 hour,ub1 min,ub1 sec,ub4 fsec,
               OraText  *timezone,size_t timezone_length);

NAME: OCIDateTimeConstruct - Construct an OCIDateTime. Only the relevant
       fields for the OCIDateTime descriptor types are used.
PARAMETERS:
        hndl (IN) - Session/Env handle. 
        err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
        datetime (IN) - Pointer to OCIDateTime 
        year      (IN) - year value
        month     (IN) - month value
        day       (IN) - day value        
        hour      (IN) - hour value
        min       (IN) - minute value
        sec       (IN) - second value
        fsec      (IN) - Fractional Second value
        timezone  (IN) - Timezone string
        timezone_length(IN) - Length of timezone string

DESCRIPTION:
       Constructs a DateTime descriptor. The type of the datetime is the
       type of the OCIDateTime descriptor. Only the relevant fields based
       on the type are used. For Types with timezone, the date and time
       fields are assumed to be in the local time of the specified timezone.
       If timezone is not specified, then session default timezone is
       assumed.
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_ERROR if datetime is not valid.

------------------------------OCIDateTimeSubtract-----------------------
sword OCIDateTimeSubtract(dvoid *hndl, OCIError *err, OCIDateTime *indate1, 
		OCIDateTime *indate2, OCIInterval *inter);
NAME: OCIDateTimeSubtract - subtracts two datetimes to return an interval
PARAMETERS:
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
indate1(IN) - pointer to subtrahend
indate2(IN) - pointer to minuend
inter  (OUT) - pointer to output interval
DESCRIPTION: 
	Takes two datetimes as input and stores their difference in an 
        interval. The type of the interval is the type of the 'inter'
        descriptor.
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
	OCI_ERROR if
	   datetimes are not comparable.

--------------------------- OCIDateTimeToText--------------------------
sword OCIDateTimeToText(dvoid *hndl, OCIError *err, CONST OCIDateTime *date, 
                        CONST OraText *fmt, ub1 fmt_length, ub1 fsprec, 
                        CONST OraText *lang_name, size_t lang_length, 
                        size_t *buf_size, OraText *buf );
NAME: OCIDateTimeToText - OCIDateTime convert date TO String 
PARAMETERS:
hndl (IN) - Session/Env handle. If Session Handle is passed, the 
		    conversion takes place in session NLS_LANGUAGE and
		    session NLS_CALENDAR, otherwise the default is used.
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
date (IN) - Oracle datetime to be converted
fmt (IN) - conversion format, if null string pointer (OraText*)0, then
                the date is converted to a character string in the
                default format for that type.
fmt_length (IN) - length of the 'fmt' parameter
fsprec (IN) - specifies the fractional second precision in which the
               fractional seconds is returned.
lang_name (IN) - specifies the language in which the names and 
  	        abbreviations of months and days are returned;
  	        default language of session is used if 'lang_name' 
  		is null i.e. (OraText *)0
lang_length (IN) - length of the 'nls_params' parameter
buf_size (IN/OUT) - size of the buffer; size of the resulting string
                is returned via this parameter
buf (OUT) - buffer into which the converted string is placed
DESCRIPTION:
        Converts the given date to a string according to the specified format.
        Refer to "TO_DATE" conversion function described in
        "Oracle SQL Language Reference Manual" for a description of format
        and NLS arguments. The converted null-terminated date string is
        stored in the buffer 'buf'.
RETURNS:
        OCI_SUCCESS if the function completes successfully.
        OCI_INVALID_HANDLE if 'err' is NULL.
        OCI_ERROR if
  	  buffer too small
  	  invalid format
  	  unknown language
          overflow error


------------------------------OCIDefineArrayOfStruct--------------------------


OCIDefineArrayOfStruct()
Name
OCI Define for Array of Structures
Purpose
This call specifies additional attributes necessary for a static array define.
Syntax
sword OCIDefineArrayOfStruct ( OCIDefine   *defnp,
                             OCIError    *errhp,
                             ub4         pvskip, 
                             ub4         indskip, 
                             ub4         rlskip,
                             ub4         rcskip );
Comments
This call specifies additional attributes necessary for an array define, used in 
an array of structures (multi-row, multi-column) fetch.
For more information about skip parameters, see the section "Skip Parameters" 
on page 4-17.
Parameters
defnp (IN) - the handle to the define structure which was returned by a call 
to OCIDefineByPos().
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
pvskip (IN) - skip parameter for the next data value.
indskip (IN) - skip parameter for the next indicator location. 
rlskip (IN) - skip parameter for the next return length value.
rcskip (IN) - skip parameter for the next return code.
Related Functions
OCIAttrGet()





OCIDefineByPos()
Name
OCI Define By Position
Purpose
Associates an item in a select-list with the type and output data buffer. 
Syntax
sb4 OCIDefineByPos ( 
              OCIStmt     *stmtp, 
              OCIDefine   **defnp,
              OCIError    *errhp,
              ub4         position,
              dvoid       *valuep,
              sb4         value_sz,
              ub2         dty,
              dvoid       *indp,
              ub2         *rlenp,
              ub2         *rcodep,
              ub4         mode );
Comments
This call defines an output buffer which will receive data retreived from 
Oracle. The define is a local step which is necessary when a SELECT statement 
returns data to your OCI application.
This call also implicitly allocates the define handle for the select-list item.
Defining attributes of a column for a fetch is done in one or more calls. The 
first call is to OCIDefineByPos(), which defines the minimal attributes 
required to specify the fetch. 
This call takes as a parameter a define handle, which must have been 
previously allocated with a call to OCIHandleAlloc().
Following the call to OCIDefineByPos() additional define calls may be 
necessary for certain data types or fetch modes:
A call to OCIDefineArrayOfStruct() is necessary to set up skip parameters 
for an array fetch of multiple columns.
A call to OCIDefineObject() is necessary to set up the appropriate 
attributes of a named data type fetch. In this case the data buffer pointer 
in ocidefn() is ignored.
Both OCIDefineArrayOfStruct() and OCIDefineObject() must be called 
after ocidefn() in order to fetch multiple rows with a column of named 
data types.
For a LOB define, the buffer pointer must be a lob locator of type 
OCILobLocator , allocated by the OCIDescAlloc() call. LOB locators, and not 
LOB values, are always returned for a LOB column. LOB values can then be 
fetched using OCI LOB calls on the fetched locator.
For NCHAR (fixed and varying length), the buffer pointer must point to an 
array of bytes sufficient for holding the required NCHAR characters. 
Nested table columns are defined and fetched like any other named data type. 
If the mode parameter is this call is set to OCI_DYNAMIC_FETCH, the client 
application can fetch data dynamically at runtime.
Runtime data can be provided in one of two ways:
callbacks using a user-defined function which must be registered with a 
subsequent call to OCIDefineDynamic(). When the client library needs a 
buffer to return the fetched data, the callback will be invoked and the 
runtime buffers provided will return a piece or the whole data. 
a polling mechanism using calls supplied by the OCI. This mode is 
assumed if no callbacks are defined. In this case, the fetch call returns the 
OCI_NEED_DATA error code, and a piecewise polling method is used 
to provide the data.
Related Functions: For more information about using the 
OCI_DYNAMIC_FETCH mode, see the section "Runtime Data 
Allocation and Piecewise Operations" on page 5-16 of Volume 1..
For more information about the define step, see the section "Defining" 
on page 2-30.
Parameters
stmtp (IN) - a handle to the requested SQL query operation.
defnp (IN/OUT) - a pointer to a pointer to a define handle which is implicitly 
allocated by this call.  This handle is used to  store the define information 
for this column.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
position (IN) - the position of this value in the select list. Positions are 
1-based and are numbered from left to right. For example, in the SELECT 
statement
SELECT empno, ssn, mgrno FROM employees;
empno is at position 1, ssn is at position 2, and mgrno is at position 3.
valuep (IN/OUT) - a pointer to a buffer or an array of buffers of the type 
specified in the dty parameter. A number of buffers can be specified when 
results for more than one row are desired in a single fetch call.
value_sz (IN) - the size of each valuep buffer in bytes. If the data is stored 
internally in VARCHAR2 format, the number of characters desired, if different 
from the buffer size in bytes, may be additionally specified by the using 
OCIAttrSet(). 
In an NLS conversion environment, a truncation error will be generated if the 
number of bytes specified is insufficient to handle the number of characters 
desired.
dty (IN) - the data type. Named data type (SQLT_NTY) and REF (SQLT_REF) 
are valid only if the environment has been intialized with in object mode. 
indp - pointer to an indicator variable or array. For scalar data types, 
pointer to sb2 or an array of sb2s. Ignored for named data types. For named 
data types, a pointer to a named data type indicator structure or an array of 
named data type indicator structures is associated by a subsequent 
OCIDefineObject() call. 
See the section "Indicator Variables" on page 2-43 for more information about 
indicator variables.
rlenp (IN/OUT) - pointer to array of length of data fetched. Each element in 
rlenp is the length of the data in the corresponding element in the row after 
the fetch. 
rcodep (OUT) - pointer to array of column-level return codes
mode (IN) - the valid modes are:
OCI_DEFAULT. This is the default mode.
OCI_DYNAMIC_FETCH. For applications requiring dynamically 
allocated data at the time of fetch, this mode must be used. The user may 
additionally call OCIDefineDynamic() to set up a callback function that 
will be invoked to receive the dynamically allocated buffers and to set 
up the memory allocate/free callbacks and the context for the callbacks. 
valuep and value_sz are ignored in this mode. 
Related Functions
OCIDefineArrayOfStruct(), OCIDefineDynamic(), OCIDefineObject()




OCIDefineDynamic()
Name
OCI Define Dynamic Fetch Attributes
Purpose
This call is used to set the additional attributes required if the 
OCI_DYNAMIC_FETCH mode was selected in OCIDefineByPos(). 
Syntax
sword OCIDefineDynamic( OCIDefine   *defnp,
                      OCIError    *errhp,
                      dvoid       *octxp, 
                      OCICallbackDefine (ocbfp)(
                                  dvoid             *octxp,
                                  OCIDefine         *defnp,
                                  ub4               iter, 
                                  dvoid             **bufpp,
                                  ub4               **alenpp,
                                  ub1               *piecep,
                                  dvoid             **indpp,
                                  ub2               **rcodep)  );
Comments
This call is used to set the additional attributes required if the 
OCI_DYNAMIC_FETCH mode has been selected in a call to 
OCIDefineByPos(). 
When the OCI_DYNAMIC_FETCH mode is selected, buffers will be 
dynamically allocated for REF, and named data type, values to receive the 
data. The pointers to these buffers will be returned. 
If OCI_DYNAMIC_FETCH mode was selected, and the call to 
OCIDefineDynamic() is skipped, then the application can fetch data piecewise 
using OCI calls.
For more information about OCI_DYNAMIC_FETCH mode, see the section 
"Runtime Data Allocation and Piecewise Operations" on page 5-16.
Parameters
defnp (IN/OUT) - the handle to a define structure returned by a call to 
OCIDefineByPos().
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
octxp (IN) - points to a context for the callback function. 
ocbfp (IN) - points to a callback function. This is invoked at runtime to get 
a pointer to the buffer into which the fetched data or a piece of it will be 
retreived. The callback also specifies the indicator, the return code and the 
lengths of the data piece and indicator. The callback has the following 
parameters:
octxp (IN) - a context pointer passed as an argument to all the callback 
functions.
defnp (IN) - the define handle.
iter (IN) - which row of this current fetch.
bufpp (OUT) - returns a pointer to a buffer to store the column value, ie. 
*bufp points to some appropriate storage for the column value.
alenpp (OUT) - returns a pointer to the length of the buffer. *alenpp 
contains the size of the buffer after return from callback. Gets set to 
actual data size after fetch.
piecep (IN/OUT) - returns a piece value, as follows:
The IN value can be OCI_ONE_PIECE, OCI_FIRST_PIECE or 
OCI_NEXT_PIECE.
The OUT value can be OCI_ONE_PIECE if the IN value was 
OCI_ONE_PIECE.
The OUT value can be OCI_ONE_PIECE or OCI_FIRST_PIECE if 
the IN value was OCI_FIRST_PIECE.
The OUT value can only be OCI_NEXT_PIECE or 
OCI_LAST_PIECE if the IN value was OCI_NEXT_PIECE. 
indpp (IN) - indicator variable pointer
rcodep (IN) - return code variable pointer
Related Functions
OCIAttrGet()
OCIDefineObject()




OCIDefineObject()
Name
OCI Define Named Data Type attributes
Purpose
Sets up additional attributes necessary for a Named Data Type define.
Syntax
sword OCIDefineObject ( OCIDefine       *defnp,
                      OCIError        *errhp,
                      CONST OCIType   *type,
                      dvoid           **pgvpp, 
                      ub4             *pvszsp, 
                      dvoid           **indpp, 
                      ub4             *indszp );
Comments
This call sets up additional attributes necessary for a Named Data Type define.An error will be returned if this function is called when the OCI environment 
has been initialized in non-Object mode.
This call takes as a paramter a type descriptor object (TDO) of datatype 
OCIType for the named data type being defined.  The TDO can be retrieved 
with a call to OCITypeByName().
See the description of OCIInitialize() on page 13 - 43 for more information 
about initializing the OCI process environment.
Parameters
defnp (IN/OUT) - a define handle previously allocated in a call to 
OCIDefineByPos(). 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
type (IN, optional) - points to the Type Descriptor Object (TDO) which 
describes the type of the program variable. Only used for program variables 
of type SQLT_NTY. This parameter is optional, and may be passed as NULL 
if it is not being used.
pgvpp (IN/OUT) - points to a pointer to a program variable buffer. For an 
array, pgvpp points to an array of pointers. Memory for the fetched named data 
type instance(s) is dynamically allocated in the object cache. At the end of the 
fetch when all the values have been received, pgvpp points to the pointer(s) to
these newly allocated named data type instance(s). The application must call 
OCIObjectMarkDel() to deallocate the named data type instance(s) when they 
are no longer needed. 
pvszsp (IN/OUT) - points to the size of the program variable. For an array, it 
is an array of ub4s. On return points to the size(s) of unpickled fetched 
values.
indpp (IN/OUT) - points to a pointer to the program variable buffer 
containing the parallel indicator structure. For an array, points to an array 
of pointers. Memory is allocated to store the indicator structures in the 
object cache. At the end of the fetch when all values have been received, 
indpp points to the pointer(s) to these newly allocated indicator structure(s).
indszp (IN/OUT) - points to the size(s) of the indicator structure program 
variable. For an array, it is an array of ub4s. On return points to the size(s)
of the unpickled fetched indicator values.
Related Functions
OCIAttrGet()



OCIDescAlloc()
Name
OCI Get DESCriptor or lob locator
Purpose
Allocates storage to hold certain data types. The descriptors can be used as 
bind or define variables.
Syntax
sword OCIDescAlloc ( CONST dvoid   *parenth,
                   dvoid         **descpp, 
                   ub4           type,
                   size_t        xtramem_sz,
                   dvoid         **usrmempp);
Comments
Returns a pointer to an allocated and initialized structure, corresponding to 
the type specified in type. A non-NULL descriptor or LOB locator is returned 
on success. No diagnostics are available on error.
This call returns OCI_SUCCESS if successful, or OCI_INVALID_HANDLE if 
an out-of-memory error occurs. 
Parameters
parenth (IN) - an environment handle. 
descpp (OUT) - returns a descriptor or LOB locator of desired type. 
type (IN) - specifies the type of descriptor or LOB locator to be allocated. 
The specific types are:
OCI_DTYPE_SNAP - specifies generation of snapshot descriptor of C 
type - OCISnapshot
OCI_DTYPE_LOB - specifies generation of a LOB data type locator of C 
type - OCILobLocator
OCI_DTYPE_RSET - specifies generation of a descriptor of C type 
OCIResult that references a result set (a number of rows as a result of a 
query). This descriptor is bound to a bind variable of data type 
SQLT_RSET (result set). The descriptor has to be converted into a 
statement handle using a function - OCIResultSetToStmt() - which can 
then be passed to OCIDefineByPos() and OCIStmtFetch() to retrieve the 
rows of the result set.
OCI_DTYPE_ROWID - specifies generation of a ROWID descriptor of C 
type OCIRowid.
OCI_DTYPE_COMPLEXOBJECTCOMP - specifies generation of a 
complex object retrieval descriptor of C type 
OCIComplexObjectComp.
xtramemsz (IN) - specifies an amount of user memory to be allocated for use 
by the application. 
usrmempp (OUT) - returns a pointer to the user memory of size xtramemsz 
allocated by the call for the user. 
Related Functions
OCIDescFree()




OCIDescFree()
Name
OCI Free DESCriptor
Purpose
Deallocates a previously allocated descriptor.
Syntax
sword OCIDescFree ( dvoid    *descp,
                  ub4      type);
Comments
This call frees up storage associated with the descriptor, corresponding to the
type specified in type. Returns OCI_SUCCESS or OCI_INVALID_HANDLE. 
All descriptors must be explicitly deallocated. OCI will not deallocate a 
descriptor if the environment handle is deallocated.
Parameters
descp (IN) - an allocated descriptor. 
type (IN) - specifies the type of storage to be freed. The specific types are: 
OCI_DTYPE_SNAP - snapshot descriptor
OCI_DTYPE_LOB - a LOB data type descriptor
OCI_DTYPE_RSET - a descriptor that references a result set (a number 
of rows as a result of a query).
OCI_DTYPE_ROWID - a ROWID descriptor
OCI_DTYPE_COMPLEXOBJECTCOMP - a complex object retrieval 
descriptor
Related Functions
OCIDescAlloc()



OCIDescribeAny()
Name
OCI DeSCribe Any
Purpose
Describes existing schema objects.
Syntax
sword OCIDescribeAny ( OCISvcCtx     *svchp,
                     OCIError      *errhp,
                     dvoid         *objptr,
                     ub4           objnm_len,
		     ub1           objptr_typ,
                     ub1 	   info_level,
		     ub1           objtype,
                     OCIDesc       *dschp );
Comments
This is a generic describe call that describes existing schema objects: tables,
views, synonyms, procedures, functions, packages, sequences, and types. As a 
result of this call, the describe handle is populated with the object-specific 
attributes which can be obtained through an OCIAttrGet() call.
An OCIParamGet() on the describe handle returns a parameter descriptor for a 
specified position. Parameter positions begin with 1. Calling OCIAttrGet() on 
the parameter descriptor returns the specific attributes of a stored procedure 
or function parameter or a table column descriptor as the case may be. 
These subsequent calls do not need an extra round trip to the server because 
the entire schema object description cached on the client side by 
OCIDescribeAny(). Calling OCIAttrGet() on the describe handle can also return 
the total number of positions.
See the section "Describing" on page 2-33 for more information about describe 
operations.
Parameters
TO BE UPDATED
svchp (IN/OUT) - a service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
objptr (IN) - the name of the object (a null-terminated string) to be 
described. Only procedure or function names are valid when connected to an 
Oracle7 Server.
objptr_len (IN) - the length of the string. Must be non-zero.
objptr_typ (IN) - Must be OCI_OTYPE_NAME, OCI_OTYPE_REF, or OCI_OTYPE_PTR.
info_level (IN) - reserved for future extensions. Pass OCI_DEFAULT.
objtype (IN/OUT) - object type.
dschp (IN/OUT) - a describe handle that is populated with describe 
information about the object after the call.
Related Functions
OCIAttrGet()



OCIEnvCreate()
Name
OCI ENVironment CREATE
Purpose
This function creates and initializes an environment for the rest of
the OCI functions to work under.  This call is a replacement for both
the OCIInitialize and OCIEnvInit calls.
Syntax
sword OCIEnvCreate  ( OCIEnv        **envhpp, 
                      ub4           mode, 
                      CONST dvoid   *ctxp, 
                      CONST dvoid   *(*malocfp) 
                                    (dvoid *ctxp, 
                                        size_t size), 
                      CONST dvoid   *(*ralocfp) 
                                    (dvoid *ctxp, 
                                       dvoid *memptr, 
                                       size_t newsize), 
                      CONST void    (*mfreefp) 
                                    ( dvoid *ctxp, 
                                       dvoid *memptr))
                      size_t    xtramemsz,
                      dvoid     **usrmempp );
 
Comments
This call creates an environment for all the OCI calls using the modes
specified by the user. This call can be used instead of the two calls
OCIInitialize and OCIEnvInit. This function returns an environment handle
which is then used by the remaining OCI functions. There can be multiple
environments in OCI each with its own environment modes.  This function 
also performs any process level initialization if required by any mode.
For example if the user wants to initialize an environment as OCI_THREADED,
then all libraries that are used by OCI are also initialized in the
threaded mode. 

This call should be invoked before anny other OCI call and should be used
instead of the OCIInitialize and OCIEnvInit calls. This is the recommended
call, although OCIInitialize and OCIEnvInit calls will still be supported
for backward compatibility. 
 
envpp (OUT) - a pointer to a handle to the environment. 
mode (IN) - specifies initialization of the mode. The valid modes are:
OCI_DEFAULT - default mode.
OCI_THREADED - threaded environment. In this mode, internal data 
structures are protected from concurrent accesses by multiple threads. 
OCI_OBJECT - will use navigational object interface. 
ctxp (IN) - user defined context for the memory call back routines. 
malocfp (IN) - user-defined memory allocation function. If mode is 
OCI_THREADED, this memory allocation routine must be thread safe.
ctxp - context pointer for the user-defined memory allocation function.
size - size of memory to be allocated by the user-defined memory 
allocation function
ralocfp (IN) - user-defined memory re-allocation function. If mode is 
OCI_THREADED, this memory allocation routine must be thread safe.
ctxp - context pointer for the user-defined memory reallocation 
function.
memp - pointer to memory block
newsize - new size of memory to be allocated
mfreefp (IN) - user-defined memory free function. If mode is 
OCI_THREADED, this memory free routine must be thread safe.
ctxp - context pointer for the user-defined memory free function.
memptr - pointer to memory to be freed
xtramemsz (IN) - specifies the amount of user memory to be allocated. 
usrmempp (OUT) - returns a pointer to the user memory of size xtramemsz 
allocated by the call for the user.

Example

Related Functions
OCIInitialize, OCIEnvInit

OCIEnvInit()
Name
OCI INITialize environment
Purpose
This call initializes the OCI environment handle.
Syntax
sword OCIEnvInit ( OCIEnv    **envp,
                 ub4       mode,
                 size_t    xtramemsz,
                 dvoid     **usrmempp );
Comments
Initializes the OCI environment handle. No changes are done on an initialized 
handle. If OCI_ERROR or OCI_SUCCESS_WITH_INFO is returned, the 
environment handle can be used to obtain ORACLE specific errors and 
diagnostics.
This call is processed locally, without a server round-trip.
Parameters
envpp (OUT) - a pointer to a handle to the environment. 
mode (IN) - specifies initialization of an environment mode. The only valid 
mode is OCI_DEFAULT for default mode
xtramemsz (IN) - specifies the amount of user memory to be allocated. 
usrmempp (OUT) - returns a pointer to the user memory of size xtramemsz 
allocated by the call for the user.
Example
See the description of OCISessionBegin() on page 13-84 for an example showing 
the use of OCIEnvInit(). 
Related Functions




OCIErrorGet()
Name
OCI Get Diagnostic Record
Purpose
Returns an error message in the buffer provided and an ORACLE error.
Syntax
sword OCIErrorGet ( dvoid      *hndlp, 
                  ub4        recordno,
                  OraText       *sqlstate,
                  ub4        *errcodep, 
                  OraText       *bufp,
                  ub4        bufsiz,
                  ub4        type );
Comments
Returns an error message in the buffer provided and an ORACLE error. 
Currently does not support SQL state. This call can be called a multiple 
number of times if there are more than one diagnostic record for an error.
The error handle is originally allocated with a call to OCIHandleAlloc().
Parameters
hndlp (IN) - the error handle, in most cases, or the environment handle (for 
errors on OCIEnvInit(), OCIHandleAlloc()).
recordno (IN) - indicates the status record from which the application seeks 
info. Starts from 1. 
sqlstate (OUT) - Not supported in Version 8.0.
errcodep (OUT) - an ORACLE Error is returned.
bufp (OUT) - the error message text is returned.
bufsiz (IN) - the size of the buffer provide to get the error message.
type (IN) - the type of the handle.
Related Functions
OCIHandleAlloc()

OCIExtractInit
Name
OCI Extract Initialize 
Purpose
This function initializes the parameter manager. 
Syntax
sword OCIExtractInit(dvoid *hndl, OCIError *err);
Comments
It must be called before calling any other parameter manager routine. The NLS 
information is stored inside the parameter manager context and used in 
subsequent calls to OCIExtract routines.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
Related Functions
OCIExtractTerm()

OCIExtractTerm
Name
OCI Extract Terminate
Purpose
This function releases all dynamically allocated storage and may perform 
other internal bookkeeping functions.
Syntax
sword OCIExtractTerm(dvoid *hndl, OCIError *err);
Comments
It must be called when the parameter manager is no longer being used.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
Related Functions
OCIExtractInit()

OCIExtractReset
Name
OCI Extract Reset
Purpose
The memory currently used for parameter storage, key definition storage, and 
parameter value lists is freed and the structure is reinitialized.
Syntax
sword OCIExtractReset(dvoid *hndl, OCIError *err);
Comments
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
Related Functions

OCIExtractSetNumKeys
Name
OCI Extract Set Number of Keys
Purpose
Informs the parameter manager of the number of keys that will be registered.
Syntax
sword OCIExtractSetNumKeys(dvoid *hndl, OCIError *err, uword numkeys);
Comments
This routine must be called prior to the first call of OCIExtractSetKey().  
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
numkeys (IN) - The number of keys that will be registered with 
               OCIExtractSetKey().
Related Functions
OCIExtractSetKey()

OCIExtractSetKey
Name
OCI Extract Set Key definition
Purpose
Registers information about a key with the parameter manager.
Syntax
sword OCIExtractSetKey(dvoid *hndl, OCIError *err, CONST OraText *name, 
                       ub1 type, ub4 flag, CONST dvoid *defval, 
                       CONST sb4 *intrange, CONST OraText *CONST *strlist);
Comments
This routine must be called after calling OCIExtractSetKey() and before 
calling OCIExtractFromFile() or OCIExtractFromStr().  
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
name (IN) - The name of the key.
type (IN) - The type of the key (OCI_EXTRACT_TYPE_INTEGER, 
            OCI_EXTRACT_TYPE_OCINUM, OCI_EXTRACT_TYPE_STRING, or 
            OCI_EXTRACT_TYPE_BOOLEAN).
flag (IN) - Set to OCI_EXTRACT_MULTIPLE if the key can take multiple values 
            or 0 otherwise.
defval (IN) - Set to the default value for the key.  May be NULL if there is 
               no default.  A string default must be a (text*) type, an 
               integer default must be an (sb4*) type, and a boolean default 
               must be a (ub1*) type.
intrange (IN) - Starting and ending values for the allowable range of integer 
                values.  May be NULL if the key is not an integer type or if 
                all integer values are acceptable.
strlist (IN) - List of all acceptable text strings for the key.  May be NULL 
               if the key is not a string type or if all text values are 
               acceptable.
Related Functions
OCIExtractSetNumKeys()

OCIExtractFromFile
Name
OCI Extract parameters From File
Purpose
The keys and their values in the given file are processed. 
Syntax
sword OCIExtractFromFile(dvoid *hndl, OCIError *err, ub4 flag, 
OraText *filename);
Comments
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
flag (IN) - Zero or has one or more of the following bits set: 
           OCI_EXTRACT_CASE_SENSITIVE, OCI_EXTRACT_UNIQUE_ABBREVS, or 
           OCI_EXTRACT_APPEND_VALUES. 
filename (IN) - Null-terminated filename string.
Related Functions

OCIExtractFromStr
Name
OCI Extract parameters From String
Purpose
The keys and their values in the given string are processed. 
Syntax
sword OCIExtractFromStr(dvoid *hndl, OCIError *err, ub4 flag, OraText *input);
Comments
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
flag (IN) - Zero or has one or more of the following bits set: 
           OCI_EXTRACT_CASE_SENSITIVE, OCI_EXTRACT_UNIQUE_ABBREVS, or 
           OCI_EXTRACT_APPEND_VALUES. 
input (IN) - Null-terminated input string.
Related Functions

OCIExtractToInt
Name
OCI Extract To Integer
Purpose
Gets the integer value for the specified key.
Syntax
sword OCIExtractToInt(dvoid *hndl, OCIError *err, OraText *keyname, uword valno, 
                      sb4 *retval);
Comments
The valno'th value (starting with 0) is returned.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, OCI_NO_DATA, or OCI_ERROR. 
OCI_NO_DATA means that there is no valno'th value for this key.
Parameters
hndl (IN) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
keyname (IN) - Key name.
valno (IN) - Which value to get for this key.
retval (OUT) - The actual integer value.
Related Functions

OCIExtractToBool
Name
OCI Extract To Boolean
Purpose
Gets the boolean value for the specified key. 
Syntax
sword OCIExtractToBool(dvoid *hndl, OCIError *err, OraText *keyname, 
                       uword valno, ub1 *retval);
Comments
The valno'th value (starting with 0) is returned.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, OCI_NO_DATA, or OCI_ERROR. 
OCI_NO_DATA means that there is no valno'th value for this key.
Parameters
hndl (IN) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
keyname (IN) - Key name.
valno (IN) - Which value to get for this key.
retval (OUT) - The actual boolean value.
Related Functions

OCIExtractToStr
Name
OCI Extract To String
Purpose
Gets the string value for the specified key.
Syntax
sword OCIExtractToStr(dvoid *hndl, OCIError *err, OraText *keyname, uword valno, 
                      OraText *retval, uword buflen);
Comments
The valno'th value (starting with 0) is returned.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, OCI_NO_DATA, or OCI_ERROR. 
OCI_NO_DATA means that there is no valno'th value for this key.
Parameters
hndl (IN) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
keyname (IN) - Key name.
valno (IN) - Which value to get for this key.
retval (OUT) - The actual null-terminated string value.
buflen (IN) - The length of the buffer for retval.
Related Functions

Note: The following OCIExtract functions are unavailable in this release

OCIExtractToOCINum
Name
OCI Extract To OCI Number
Purpose
Gets the OCINumber value for the specified key.
Syntax
sword OCIExtractToOCINum(dvoid *hndl, OCIError *err, OraText *keyname, 
                         uword valno, OCINumber *retval);
Comments
The valno'th value (starting with 0) is returned.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, OCI_NO_DATA, or OCI_ERROR. 
OCI_NO_DATA means that there is no valno'th value for this key.
Parameters
hndl (IN) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
keyname (IN) - Key name.
valno (IN) - Which value to get for this key.
retval (OUT) - The actual OCINumber value.
Related Functions

OCIExtractToList
Name
OCI Extract To parameter List
Purpose
Generates a list of parameters from the parameter structures that are stored 
in memory. 
Syntax
sword OCIExtractToList(dvoid *hndl, OCIError *err, uword *numkeys);
Comments
Must be called before OCIExtractValues() is called.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
numkeys (OUT) - Number of distinct keys stored in memory.
Related Functions
OCIExtractFromList()

OCIExtractFromList
Name
OCI Extract From parameter List
Purpose
Generates a list of values for the a parameter in the parameter list.
Syntax
sword OCIExtractFromList(dvoid *hndl, OCIError *err, uword index, OraText *name, 
                         ub1 *type, uword *numvals, dvoid ***values);
Comments
Parameters are specified by an index. OCIExtractToList() must be called prior 
to calling this routine to generate the parameter list from the parameter 
structures that are stored in memory. 
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN) - The OCI environment or session handle.
err (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
               err and this function returns OCI_ERROR. Diagnostic information 
               can be obtained by calling OCIErrorGet().
name (OUT) - Name of the key for the current parameter.
type (OUT) - Type of the current parameter (OCI_EXTRACT_TYPE_STRING, 
             OCI_EXTRACT_TYPE_INTEGER, OCI_EXTRACT_TYPE_OCINUM, or 
             OCI_EXTRACT_TYPE_BOOLEAN)
numvals (OUT) - Number of values for this parameter.
values (OUT) - The values for this parameter.
Related Functions
OCIExtractToList()


************************  OCIFileClose() ***********************************
 
Name
 OCIFileClose - Oracle Call Interface FILE i/o CLOSE

Purpose
 Close a previously opened file.

Syntax
 sword OCIFileClose ( dvoid             *hndl, 
             	      OCIError          *err,
                      OCIFileObject     *filep )

Comments
 This function will close a previously opened file. If the function succeeds
 then OCI_SUCCESS will be returned, else OCI_ERROR. 
 
Parameters
 hndl  (IN) - the OCI environment or session handle.
 err (OUT) - the OCI error handle
 filep (IN) - the OCIFile file object

Related Functions
 OCIFileOpen.  



********************* OCIFileExists() **************************************

Name
 OCIFileExists - Oracle Call Interface FILE i/o EXIST

Purpose
 Check to see if the file exists.

Syntax
 sword OCIFileExists ( dvoid           *hndl, 
            	      OCIError         *err, 
                      OraText          *filename,
                      OraText          *path,
                      ub1              *flag )

Comments
 This function will set the flag to TRUE if the file exists else it will
 be set to FALSE.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR. 

Parameters
 hndl(IN) - OCI environment or session handle
 err(OUT) - OCI error handle
 filename(IN) - filename
 path(IN) - path of the file
 flag(OUT) - whether the file exists or not

Related Functions.
 None.
     

 **************************** OCIFileFlush() ******************************


Name
 OCIFileFlush - Oracle Call Interface File i/o FLUSH

Purpose
 Flush the buffers associated with the file to the disk.

Syntax
 sword OCIFileFlush ( dvoid             *hndl, 
             	      OCIError          *err,
                      OCIFileObject     *filep )

Comments
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.

Parameters 
 hndl (IN) - the OCI environment or session handle.
 err (OUT) - the OCI error handle
 filep (IN) - the OCIFile file object

Related Functions
 OCIFileOpen, OCIFileWrite



 *************************** OCIFileGetLength() ****************************

Name
 OCIFileGetLength - Oracle Call Interface FILE i/o GET file LENGTH

Purpose
 Get the length of a file.

Syntax
 OCIFileGetLength(dvoid           *hndl, 
	          OCIError        *err,
                  OraText         *filename,
                  OraText         *path,
                  ubig_ora        *lenp )

Comments
 The length of the file will be returned in lenp.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.
 
Parameters
 hndl (IN) - the OCI environment or session handle.
 err (OUT) - the OCI error handle.  If  there is an error, it is recorded 
 in err and this function returns OCI_ERROR.  Diagnostic information can be 
 obtained by calling OCIErrorGet().
 filename (IN) - file name.
 path (IN) - path of the file.
 lenp (OUT) - On output, it is the length of the file in bytes.
 is the number of bytes in the file.

Related Functions
 None.



******************************** OCIFileInit() *****************************
   
Name
 OCIFileInit - Oracle Call Interface FILE i/o INITialize

Purpose
 Initialize the OCI File I/O package and create the OCIFile context.

Syntax
 sword OCIFileInit ( dvoid *hndl, 
                     OCIError *err)

Comments
 This function should be called before any of the OCIFile functions are
 used.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.
 
Parameters
 hndl(IN) - OCI environment or session handle.
 err(OUT) - OCI error structure.

Related Functions
 OCIFileTerm
     


********************************* OCIFileOpen() *****************************

Name
 OCIFileOpen - Oracle Call Interface File i/o OPEN

Purpose
     Open a file.

Syntax
 sword OCIFileOpen ( dvoid               *hndl, 
	             OCIError            *err,
                     OCIFileObject      **filep,
                     OraText             *filename,
                     OraText             *path,
                     ub4                  mode,
                     ub4                  create, 
                     ub4                  type )

Comments
 OCIFileOpen returns a handle to the open file in filep if the file is
 successfully opened. 
 If one wants to use the standard file objects (stdin, stdout & stderr)
 then OCIFileOpen whould be called with the type filed containing the 
 appropriate type (see the parameter type). If any of the standard files 
 are specified then filename, path, mode and create are ignored.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.

Parameters
 hndl (OUT) - the OCI environment or session handle.
 err (OUT) - the OCI error handle.  If  there is an error, it is recorded 
 in err and this function returns OCI_ERROR.  Diagnostic information can be 
 obtained by calling OCIErrorGet().
 filep (OUT) - the file object to be returned.
 filename (IN) - file name (NULL terminated string).
 path (IN) - path of the file (NULL terminated string).
 mode - mode in which to open the file (valid modes are OCI_FILE_READONLY,
 OCI_FILE_WRITEONLY, OCI_FILE_READ_WRITE).
 create - should the file be created if it does not exist. Valid values
 are: 
     OCI_FILE_TRUNCATE - create a file regardless of whether or not it exists. 
                        If the file already exists overwrite it.
     OCI_FILE_EXIST - open it if it exists, else fail.
     OCI_FILE_EXCL - fail if the file exists, else create.
     OCI_FILE_CREATE - open the file if it exists, and create it if it doesn't.
     OCI_FILE_APPEND - set the file pointer to the end of the file prior to 
                      writing(this flag can be OR'ed with OCI_FILE_EXIST or
                      OCI_FILE_CREATE).
type - file type. Valid values are OCI_FILE_TEXT, OCI_FILE_BIN, 
       OCI_FILE_STDIN, OCI_FILE_STDOUT and OCI_FILE_STDERR.
       If any of the standard files are specified then filename, path, mode
       and create are ignored.

Related Functions.
 OCIFileClose



************************** OCIFileRead() ************************************
   
Name
 OCIFileRead - Oracle Call Interface FILE i/o READ

Purpose
 Read from a file into a buffer.

Syntax
 sword OCIFileRead ( dvoid            *hndl, 
          	     OCIError         *err,
                     OCIFileObject    *filep,
                     dvoid            *bufp,
                     ub4               bufl,
                     ub4              *bytesread )

Comments
 Upto bufl bytes from the file will be read into bufp. The user should
 allocate memory for the buffer.
 The number of bytes read would be in bytesread.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.

Parameters
 hndl (IN) - the OCI environment or session handle.
 err (OUT) - the OCI error handle.  If  there is an error, it is recorded 
 in err and this function returns OCI_ERROR.  Diagnostic information can be 
 obtained by calling OCIErrorGet().
 filep (IN/OUT) - a File Object that uniquely references the file.
 bufp (IN) - the pointer to a buffer into which the data will be read. The 
 length of the allocated memory is assumed to be bufl. 
 bufl - the length of the buffer in bytes. 
 bytesread (OUT) - the number of bytes read.

Related Functions
 OCIFileOpen, OCIFileSeek, OCIFileWrite



****************************** OCIFileSeek() ******************************

Name
 OCIFileSeek - Oracle Call Interface FILE i/o SEEK

Purpose
 Perfom a seek to a byte position.

Syntax
 sword OCIFileSeek ( dvoid           *hndl, 
        	     OCIError        *err,  
                     OCIFileObject   *filep,
                     uword            origin,
                     ubig_ora         offset,
                     sb1              dir)

Comments
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.

Parameters
 hndl (IN) - the OCI environment or session handle.
 err (OUT) - the OCI error handle.  If  there is an error, it is recorded 
 in err and this function returns OCI_ERROR.  Diagnostic information can be 
 obtained by calling OCIErrorGet().
 filep (IN/OUT) - a file handle that uniquely references the file.
 origin - The starting point we want to seek from. NOTE: The starting 
 point may be OCI_FILE_SEEK_BEGINNING (beginning), OCI_FILE_SEEK_CURRENT 
 (current position), or OCI_FILE_SEEK_END (end of file). 
 offset - The number of bytes from the origin we want to start reading from. 
 dir - The direction we want to go from the origin. NOTE: The direction 
 can be either OCI_FILE_FORWARD or OCI_FILE_BACKWARD. 
 
Related Function
 OCIFileOpen, OCIFileRead, OCIFileWrite



*************************** OCIFileTerm() **********************************

Name
 OCIFileTerm - Oracle Call Interface FILE i/o TERMinate

Purpose
 Terminate the OCI File I/O package and destroy the OCI File context.

Syntax
 sword OCIFileTerm ( dvoid *hndl, 
                     OCIError *err )

Comments
 After this function has been called no OCIFile function should be used.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.
 
Parameters
 hndl(IN) - OCI environment or session handle.
 err(OUT) - OCI error structure. 
  
Related Functions 
 OCIFileInit   
 

********************************* OCIFileWrite() **************************** 

Name 
 OCIFileWrite - Oracle Call Interface FILE i/o WRITE

Purpose
  Write data from buffer into a file.

Syntax
 sword OCIFileWrite ( dvoid            *hndl, 
            	      OCIError         *err,  
                      OCIFileObject    *filep,
                      dvoid            *bufp, 
                      ub4               buflen
                      ub4              *byteswritten )

Comments
 The number of bytes written will be in *byteswritten.
 The function will return OCI_ERROR if any error is encountered, else
 it will return OCI_ERROR.

Parameters
 hndl (IN) - the OCI environment or session handle.
 err (OUT) - the OCI error handle.  If  there is an error, it is recorded 
 in err and this function returns OCI_ERROR.  Diagnostic information can be 
 obtained by calling OCIErrorGet().
 filep (IN/OUT) - a file handle that uniquely references the file.
 bufp (IN) - the pointer to a buffer from which the data will be written. 
 The length of the allocated memory is assumed to be the value passed
 in bufl. 
 bufl - the length of the buffer in bytes.
 byteswritten (OUT) - the number of bytes written.
 
Related Functions
 OCIFileOpen, OCIFileSeek, OCIFileRead 





OCIHandleAlloc() 
Name
OCI Get HaNDLe
Purpose
This call returns a pointer to an allocated and initialized handle.
Syntax
sword OCIHandleAlloc ( CONST dvoid   *parenth,
                     dvoid         **hndlpp, 
                     ub4           type, 
                     size_t        xtramem_sz,
                     dvoid         **usrmempp);
Comments
Returns a pointer to an allocated and initialized structure, corresponding to 
the type specified in type. A non-NULL handle is returned on success. Bind 
handle and define handles are allocated with respect to a statement handle. All
other handles are allocated with respect to an environment handle which is 
passed in as a parent handle.
No diagnostics are available on error. This call returns OCI_SUCCESS if 
successful, or OCI_INVALID_HANDLE if an out-of-memory error occurs.
Handles must be allocated using OCIHandleAlloc() before they can be passed 
into an OCI call.
Parameters
parenth (IN) - an environment or a statement handle. 
hndlpp (OUT) - returns a handle to a handle type. 
type (IN) - specifies the type of handle to be allocated. The specific types 
are: 
OCI_HTYPE_ERROR - specifies generation of an error report handle of 
C type OCIError
OCI_HTYPE_SVCCTX - specifies generation of a service context handle 
of C type OCISvcCtx
OCI_HTYPE_STMT - specifies generation of a statement (application 
request) handle of C type OCIStmt
OCI_HTYPE_BIND - specifies generation of a bind information handle 
of C type OCIBind
OCI_HTYPE_DEFINE - specifies generation of a column definition 
handle of C type OCIDefine
OCI_HTYPE_DESCRIBE  - specifies generation of a select list 
description handle of C type OCIDesc
OCI_HTYPE_SERVER - specifies generation of a server context handle 
of C type OCIServer
OCI_HTYPE_SESSION - specifies generation of an authentication 
context handle of C type OCISession
OCI_HTYPE_TRANS - specifies generation of a transaction context
handle of C type OCITrans
OCI_HTYPE_COMPLEXOBJECT - specifies generation of a complex 
object retrieval handle of C type OCIComplexObject
OCI_HTYPE_SECURITY - specifies generation of a security handle of C 
type OCISecurity
xtramem_sz (IN) - specifies an amount of user memory to be allocated.
usrmempp (OUT) - returns a pointer to the user memory of size xtramemsz 
allocated by the call for the user. 
Related Functions
OCIHandleFree()



OCIHandleFree()
Name
OCI Free HaNDLe
Purpose
This call explicitly deallocates a handle.
Syntax
sword OCIHandleFree ( dvoid     *hndlp,
                    ub4       type);
Comments
This call frees up storage associated with a handle, corresponding to the type 
specified in the type parameter.
This call returns either OCI_SUCCESS or OCI_INVALID_HANDLE.
All handles must be explicitly deallocated. OCI will not deallocate a child 
handle if the parent is deallocated.
Parameters
hndlp (IN) - an opaque pointer to some storage.
type (IN) - specifies the type of storage to be allocated. The specific types are:
OCI_HTYPE_ENV - an environment handle
OCI_HTYPE_ERROR - an error report handle
OCI_HTYPE_SVCCTX - a service context handle
OCI_HTYPE_STMT - a statement (application request) handle
OCI_HTYPE_BIND - a bind information handle
OCI_HTYPE_DEFINE - a column definition handle
OCI_HTYPE_DESCRIBE  - a select list description handle
OCI_HTYPE_SERVER - a server handle
OCI_HTYPE_SESSION - a user authentication handle
OCI_HTYPE_TRANS - a transaction handle
OCI_HTYPE_COMPLEXOBJECT - a complex object retrieval handle
OCI_HTYPE_SECURITY - a security handle
Related Functions
OCIHandleAlloc()




OCIInitialize()
Name
OCI Process Initialize
Purpose
Initializes the OCI process environment.
Syntax
sword OCIInitialize ( ub4           mode,
                    CONST dvoid   *ctxp, 
                    CONST dvoid   *(*malocfp) 
                                  ( dvoid *ctxp,
                                    size_t size ),
                    CONST dvoid   *(*ralocfp)
                                  ( dvoid *ctxp,
                                    dvoid *memp,
                                    size_t newsize ),
                    CONST void    (*mfreefp)
                                  ( dvoid *ctxp,
                                    dvoid *memptr ));
Comments
This call initializes the OCI process environment.
OCIInitialize() must be invoked before any other OCI call. 
Parameters
mode (IN) - specifies initialization of the mode. The valid modes are:
OCI_DEFAULT - default mode.
OCI_THREADED - threaded environment. In this mode, internal data 
structures are protected from concurrent accesses by multiple threads. 
OCI_OBJECT - will use navigational object interface. 
ctxp (IN) - user defined context for the memory call back routines. 
malocfp (IN) - user-defined memory allocation function. If mode is 
OCI_THREADED, this memory allocation routine must be thread safe.
ctxp - context pointer for the user-defined memory allocation function.
size - size of memory to be allocated by the user-defined memory 
allocation function
ralocfp (IN) - user-defined memory re-allocation function. If mode is 
OCI_THREADED, this memory allocation routine must be thread safe.
ctxp - context pointer for the user-defined memory reallocation 
function.
memp - pointer to memory block
newsize - new size of memory to be allocated
mfreefp (IN) - user-defined memory free function. If mode is 
OCI_THREADED, this memory free routine must be thread safe.
ctxp - context pointer for the user-defined memory free function.
memptr - pointer to memory to be freed
Example
See the description of OCIStmtPrepare() on page 13-96 for an example showing 
the use of OCIInitialize().
Related Functions

--------------------------------OCITerminate------------------------------------

OCITerminate()
Name
OCI process Terminate
Purpose
Do cleanup before process termination
Syntax
sword OCITerminate (ub4 mode);

Comments
This call performs  OCI related clean up before the OCI process terminates.
If the process is running in shared mode then the OCI process is disconnected
from the shared memory subsystem.

OCITerminate() should be the last OCI call in any process.

Parameters
mode (IN) - specifies different termination modes.

OCI_DEFAULT - default mode.

Example

Related Functions
OCIInitialize()

---------------------- OCIIntervalAssign --------------------------------- 
sword OCIIntervalAssign(dvoid *hndl, OCIError *err, 
                    CONST OCIInterval *inpinter, OCIInterval *outinter );

  DESCRIPTION
    Copies one interval to another to create a replica
  PARAMETERS
     hndl (IN) - Session/Env handle.
    err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
    (IN)  inpinter - Input Interval 
    (OUT) outinter - Output Interval 
  RETURNS
     OCI_INVALID_HANDLE if 'err' is NULL.
     OCI_SUCCESS otherwise

 ---------------------- OCIIntervalCheck ------------------------------------ 
sword OCIIntervalCheck(dvoid *hndl, OCIError *err, CONST OCIInterval *interval,
			 ub4 *valid );

  DESCRIPTION
    Checks the validity of an interval
  PARAMETERS
     hndl (IN) - Session/Env handle.
    err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
    (IN)  interval - Interval to be checked 
    (OUT) valid     - Zero if the interval is valid, else returns an Ored
	combination of the following codes.

   Macro name                   Bit number      Error
   ----------                   ----------      -----
   OCI_INTER_INVALID_DAY         0x1           Bad day
   OCI_INTER_DAY_BELOW_VALID     0x2           Bad DAy Low/high bit (1=low)
   OCI_INTER_INVALID_MONTH       0x4           Bad MOnth
   OCI_INTER_MONTH_BELOW_VALID   0x8           Bad MOnth Low/high bit (1=low)
   OCI_INTER_INVALID_YEAR        0x10          Bad YeaR
   OCI_INTER_YEAR_BELOW_VALID    0x20          Bad YeaR Low/high bit (1=low)
   OCI_INTER_INVALID_HOUR        0x40          Bad HouR
   OCI_INTER_HOUR_BELOW_VALID    0x80          Bad HouR Low/high bit (1=low)
   OCI_INTER_INVALID_MINUTE      0x100         Bad MiNute
   OCI_INTER_MINUTE_BELOW_VALID	 0x200         Bad MiNute Low/high bit(1=low)
   OCI_INTER_INVALID_SECOND      0x400         Bad SeCond
   OCI_INTER_SECOND_BELOW_VALID  0x800         bad second Low/high bit(1=low)
   OCI_INTER_INVALID_FRACSEC     0x1000        Bad Fractional second
   OCI_INTER_FRACSEC_BELOW_VALID 0x2000        Bad fractional second Low/High

	
  RETURNS
    OCI_SUCCESS if interval is okay
    OCI_INVALID_HANDLE if 'err' is NULL.

 ---------------------- OCIIntervalCompare ----------------------------------- 
sword OCIIntervalCompare(dvoid *hndl, OCIError *err, OCIInterval *inter1, 
			OCIInterval *inter2, sword *result );

  DESCRIPTION
	Compares two intervals, returns 0 if equal, -1 if inter1 < inter2, 
      	1 if inter1 > inter2
  PARAMETERS
     hndl (IN) - Session/Env handle. 
     err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
     inter1  (IN)   - Interval to be compared 
     inter2  (IN)   - Interval to be compared 
     result  (OUT)  - 	comparison result, 0 if equal, -1 if inter1 < inter2, 
                	1 if inter1 > inter2

  RETURNS
     OCI_SUCCESS on success
     OCI_INVALID_HANDLE if 'err' is NULL.
     OCI_ERROR if 
	the two input datetimes are not mutually comparable.

---------------------- OCIIntervalDivide ------------------------------------ 
sword OCIIntervalDivide(dvoid *hndl, OCIError *err, OCIInterval *dividend, 
		OCINumber *divisor, OCIInterval *result );
 
  DESCRIPTION
     Divides an interval by an Oracle Number to produce an interval
  PARAMETERS
	hndl (IN) - Session/Env handle. 
     err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
     dividend  (IN)   - Interval to be divided 
     divisor   (IN)   - Oracle Number dividing `dividend' 
     result    (OUT)  - resulting interval (dividend / divisor) 
  RETURNS
     OCI_SUCCESS on success
     OCI_INVALID_HANDLE if 'err' is NULL.

 ---------------------- OCIIntervalFromNumber -------------------- 
sword OCIIntervalFromNumber(dvoid *hndl, OCIError *err, 
               OCIInterval *inter, OCINumber *number);
  DESCRIPTION
    Converts an interval to an Oracle Number
  PARAMETERS
     hndl (IN) - Session/Env handle. 
    err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
    (OUT)  interval - Interval to be converted 
    (IN) number - Oracle number result  (in years for YEARMONTH interval
                     and in days for DAYSECOND)
  RETURNS
    OCI_SUCCESS on success 
    OCI_INVALID_HANDLE if 'err' is NULL.
    OCI_ERROR on error.
  NOTES
    Fractional portions of the date (for instance, minutes and seconds if
    the unit chosen is hours) will be included in the Oracle number produced.
    Excess precision will be truncated.
 
 ---------------------- OCIIntervalFromText --------------------------------- 
sword OCIIntervalFromText( dvoid *hndl, OCIError *err, CONST OraText *inpstr, 
		size_t str_len, OCIInterval *result );

  DESCRIPTION
    Given an interval string produce the interval represented by the string.
    The type of the interval is the type of the 'result' descriptor.
  PARAMETERS

     hndl (IN) - Session/Env handle. 
     err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
    (IN)  inpstr - Input string 
    (IN)  str_len - Length of input string 
    (OUT) result - Resultant interval 
  RETURNS
    OCI_SUCCESS on success
    OCI_INVALID_HANDLE if 'err' is NULL.
    OCI_ERROR if
     	there are too many fields in the literal string
	the year is out of range (-4713 to 9999)
  	if the month is out of range (1 to 12)
 	if the day of month is out of range (1 to 28...31)
    	if hour is not in range (0 to 23)
     	if hour is not in range (0 to 11)
  	if minute is not in range (0 to 59)
    	if seconds in minute not in range (0 to 59)
   	if seconds in day not in range (0 to 86399)
  	if the interval is invalid


 ---------------------- OCIIntervalGetDaySecond -------------------- 

  DESCRIPTION
     Gets values of day second interval
  PARAMETERS
	hndl (IN) - Session/Env handle.
        err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().     
        day     (OUT) - number of days
        hour    (OUT) - number of hours
        min     (OUT) - number of mins
        sec     (OUT) - number of secs
        fsec    (OUT) - number of fractional seconds
	result     (IN)  - resulting interval 
  RETURNS
	OCI_SUCCESS on success
        OCI_INVALID_HANDLE if 'err' is NULL.


 ---------------------- OCIIntervalGetYearMonth -------------------- 

  DESCRIPTION
     Gets year month from an interval
  PARAMETERS
	hndl (IN) - Session/Env handle.
        err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().     
	year    (OUT)   - year value
	month   (OUT)   - month value
	result     (IN)  - resulting interval 
  RETURNS
	OCI_SUCCESS on success
        OCI_INVALID_HANDLE if 'err' is NULL.



-------------------------- OCIIntervalAdd ------------------------------
sword OCIIntervalAdd(dvoid *hndl, OCIError *err, OCIInterval *addend1, 
			OCIInterval *addend2, OCIInterval *result );
NAME OCIIntervalAdd - Adds two intervals 
PARAMETERS
hndl (IN) - Session/Env handle. 
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
addend1  (IN)   - Interval to be added 
addend2  (IN)   - Interval to be added 
result   (OUT)  - resulting interval (addend1 + addend2) 
DESCRIPTION
     Adds two intervals to produce a resulting interval
RETURNS
     OCI_SUCCESS on success
     OCI_ERROR if:
	the two input intervals are not mutually comparable.
	the resulting year would go above SB4MAXVAL
	the resulting year would go below SB4MINVAL
     OCI_INVALID_HANDLE if 'err' is NULL.
NOTES
     The two input intervals must be mutually comparable

 ---------------------- OCIIntervalSubtract ------------------------------- 
sword OCIIntervalSubtract(dvoid *hndl, OCIError *err, OCIInterval *minuend, 
			    OCIInterval *subtrahend, OCIInterval *result );
NAME - OCIIntervalSubtract - subtracts two intervals
PARAMETERS
hndl (IN) - Session/Env handle.
err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().     
minuend    (IN)   - interval to be subtracted from 
subtrahend (IN)   - interval subtracted from minuend 
result     (OUT)  - resulting interval (minuend - subtrahend) 
DESCRIPTION
     Subtracts two intervals and stores the result in an interval
RETURNS
	OCI_SUCCESS on success
        OCI_INVALID_HANDLE if 'err' is NULL.
	OCI_ERROR if:
	   the two input intervals are not mutually comparable.
	   the resulting leading field would go below SB4MINVAL	
	   the resulting leading field would go above SB4MAXVAL

---------------------- OCIIntervalMultiply --------------------------------- 
sword OCIIntervalMultiply(dvoid *hndl, OCIError *err, CONST OCIInterval *inter,
			OCINumber *nfactor, OCIInterval *result );

  DESCRIPTION
     Multiplies an interval by an Oracle Number to produce an interval
  PARAMETERS
	hndl (IN) - Session/Env handle. 
     err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
     inter  (IN)   - Interval to be multiplied 
     nfactor  (IN)   - Oracle Number to be multiplied 
     result   (OUT)  - resulting interval (ifactor * nfactor) 
  RETURNS
     OCI_SUCCESS on success
     OCI_INVALID_HANDLE if 'err' is NULL.
     OCI_ERROR if:
	the resulting year would go above SB4MAXVAL
	the resulting year would go below SB4MINVAL


 ---------------------- OCIIntervalSetDaySecond -------------------- 

  DESCRIPTION
     Sets day second interval
  PARAMETERS
	hndl (IN) - Session/Env handle.
        err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().     
        day     (IN) - number of days
        hour    (IN) - number of hours
        min     (IN) - number of mins
        sec     (IN) - number of secs
        fsec    (IN) - number of fractional seconds
	result     (OUT)  - resulting interval 
  RETURNS
	OCI_SUCCESS on success
        OCI_INVALID_HANDLE if 'err' is NULL.


 ---------------------- OCIIntervalSetYearMonth -------------------- 

  DESCRIPTION
     Sets year month interval
  PARAMETERS
	hndl (IN) - Session/Env handle.
        err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().     
	year    (IN)   - year value
	month   (IN)   - month value
	result     (OUT)  - resulting interval 
  RETURNS
	OCI_SUCCESS on success
        OCI_INVALID_HANDLE if 'err' is NULL.


----------------------- OCIIntervalToNumber ---------------------------------
sword OCIIntervalToNumber(dvoid *hndl, OCIError *err, CONST OCIInterval *inter,
			OCINumber *number);

  DESCRIPTION
    Converts an interval to an Oracle Number
  PARAMETERS
     hndl (IN) - Session/Env handle. 
    err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
    (IN)  inter - Interval to be converted 
    (OUT) number - Oracle number result  (in years for YEARMONTH interval
                     and in days for DAYSECOND)
  RETURNS
    OCI_INVALID_HANDLE if 'err' is NULL.
    OCI_SUCCESS on success 
  NOTES
    Fractional portions of the date (for instance, minutes and seconds if
    the unit chosen is hours) will be included in the Oracle number produced.
    Excess precision will be truncated.
 
------------------------------- OCIIntervalToText -------------------------
sword OCIIntervalToText( dvoid *hndl, OCIError *err, CONST OCIInterval *inter,
			ub1 lfprec, ub1 fsprec, OraText *buffer, 
                        size_t buflen, size_t *resultlen );

  DESCRIPTION
    Given an interval, produces a string representing the interval.
  PARAMETERS
     hndl (IN) - Session/Env handle. 
    err (IN/OUT) - error handle. If there is an error, it is
                recorded in 'err' and this function returns OCI_ERROR.
                The error recorded in 'err' can be retrieved by calling
                OCIErrorGet().
    (IN)  inter - Interval to be converted 
    (IN)  lfprec  - Leading field precision. Number of digits used to
                represent the leading field.
    (IN)  fsprec  - Fractional second precision of the interval. Number of
                digits used to represent the fractional seconds.
    (OUT) buffer - buffer to hold result 
    (IN)  buflen - length of above buffer 
    (OUT) resultlen - length of result placed into buffer 
 
  RETURNS
    OCI_SUCCESS on success
    OCI_INVALID_HANDLE if 'err' is NULL.
    OCI_ERROR 
    	if the buffer is not large enough to hold the result
  NOTES
    The interval literal will be output as `year' or `[year-]month' for
    YEAR-MONTH intervals and as `seconds' or `minutes[:seconds]' or
    `hours[:minutes[:seconds]]' or `days[ hours[:minutes[:seconds]]]' for
    DAY-TIME intervals (where optional fields are surrounded by brackets).


OCILdaToSvcCtx()
Name
OCI toggle version 7 Lda_Def to SerVice context handle
Purpose
Converts a V7 Lda_Def to a V8 service context handle.
Syntax
sword OCILdaToSvcCtx ( OCISvcCtx  **svchpp,
                     OCIError   *errhp,
                     Lda_Def    *ldap );
Comments
Converts a V7 Lda_Def to a V8 service context handle. The action of this call 
can be reversed by passing the resulting service context handle to the 
OCISvcCtxToLda() function.
Parameters
svchpp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
ldap (IN/OUT) - the V7 logon data area returned by OCISvcCtxToLda() from 
this service context.
Related Functions
OCISvcCtxToLda()




OCILobAppend()

Name
OCI Lob APpend

Purpose
Appends a LOB value at the end of another LOB. 

Syntax
sword OCILobAppend ( OCISvcCtx        *svchp,
                   OCIError         *errhp,
                   OCILobLocator    *dst_locp,
                   OCILobLocator    *src_locp );
Comments
Appends a LOB value at the end of LOB. The data is 
copied from the source to the destination at the end of the destination. The 
source and the destination must already exist. The destination LOB is 
extended to accommodate the newly written data.

It is an error to extend the destination LOB beyond the maximum length 
allowed or to try to copy from a NULL LOB. 

Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
dst_locp (IN/OUT) - a locator uniquely referencing the destination LOB. 
src_locp (IN/OUT) - a locator uniquely referencing the source LOB. 

Related Functions
OCILobTrim()
OCIErrorGet()
OCILobWrite()
OCILobCopy()



OCILobAssign()

Name
OCI Lob ASsiGn

Purpose
Assigns one LOB/FILE locator to another.

Syntax
sword OCILobAssign ( OCIEnv                *envhp, 
                     OCIError              *errhp, 
                     CONST OCILobLocator   *src_locp, 
                     OCILobLocator         **dst_locpp );

Comments
Assign source locator to destination locator.  After the assignment, both 
locators refer to the same LOB data.  For internal LOBs, the source locator's 
LOB data gets copied to the destination locator's LOB data only when the 
destination locator gets stored in the table.  Therefore, issuing a flush of 
the object containing the destination locator will copy the LOB data. For 
FILEs only the locator that refers to the OS file is copied to the table. The
OS file is not copied.
Note: The only difference between this and OCILobLocatorAssign is that this
takes an environment handle whereas OCILobLocatorAssign takes an OCI service
handle

Parameters
envhp (IN/OUT) - OCI environment handle initialized in object mode.
errhp (IN/OUT) - The OCI error handle. If there is an error, it is recorded 
in errhp and this function returns OCI_ERROR. Diagnostic information can be 
obtained by calling OCIErrorGet().
src_locp (IN) - LOB locator to copy from.
dst_locpp (IN/OUT) - LOB locator to copy to.  The caller must allocate space 
for the OCILobLocator by calling OCIDescriptorAlloc().

See also
OCIErrorGet()
OCILobIsEqual()
OCILobLocatorIsInit()
OCILobLocatorAssign()


OCILobCharSetForm()

Name
OCI Lob Get Character Set Form

Purpose
Gets the LOB locator's character set fpr,, if any.

Syntax
sword OCILobCharSetForm ( OCIEnv                    *envhp, 
                          OCIError                  *errhp, 
                          CONST OCILobLocator       *locp, 
                          ub1                       *csfrm );

Comments
Returns the character set form of the input LOB locator in the csfrm output 
parameter. 

Parameters
envhp (IN/OUT) - OCI environment handle initialized in object mode.
errhp (IN/OUT) - error handle. The OCI error handle. If there is an error, it 
is recorded in err and this function returns OCI_ERROR. Diagnostic 
information can be obtained by calling OCIErrorGet().
locp (IN) - LOB locator for which to get the character set form.
csfrm(OUT) - character set form of the input LOB locator.  If the input 
locator is for a BLOB or a BFILE, csfrm is set to 0 since there is no concept 
of a character set for binary LOBs/FILEs.  The caller must allocate space for 
the csfrm (ub1) and not write into the space.
See also
OCIErrorGet(), OCILobCharSetId(), OCILobLocatorIsInit



OCILobCharSetId()

Name
OCI Lob get Character Set IDentifier

Purpose
Gets the LOB locator's character set ID, if any.

Syntax
sword OCILobCharSetId ( OCIEnv                    *envhp, 
                        OCIError                  *errhp, 
                        CONST OCILobLocator       *locp, 
                        ub2                       *csid );

Comments
Returns the character set ID of the input LOB locator in the cid output 
parameter. 

Parameters
envhp (IN/OUT) - OCI environment handle initialized in object mode.
errhp (IN/OUT) - error handle. The OCI error handle. If there is an error, it 
is recorded in err and this function returns OCI_ERROR. Diagnostic 
information can be obtained by calling OCIErrorGet().
locp (IN) - LOB locator for which to get the character set ID.
csid (OUT) - character set ID of the input LOB locator.  If the input locator 
is for a BLOB or a BFILE, csid is set to 0 since there is no concept of a 
character set for binary LOBs/FILEs.  The caller must allocate space for the 
character set id of type ub2 and not write into the space.

See also
OCIErrorGet(), OCILobCharSetForm(), OCILobLocatorIsInit()



OCILobCopy()

Name
OCI Lob Copy

Purpose
Copies a portion of a LOB value into another LOB value.

Syntax
sword OCILobCopy ( OCISvcCtx        *svchp,
                   OCIError         *errhp,
                   OCILobLocator    *dst_locp,
                   OCILobLocator    *src_locp,
                   ub4              amount,
                   ub4              dst_offset,
                   ub4              src_offset );

Comments
Copies a portion of a LOB value into another LOB as specified. The data 
is copied from the source to the destination. The source (src_locp) and the 
destination (dlopb) LOBs must already exist.
If the data already exists at the destination's start position, it is 
overwritten with the source data. If the destination's start position is 
beyond the end of the current data, a hole is created from the end of the data
to the beginning of the newly written data from the source. The destination 
LOB is extended to accommodate the newly written data if it extends 
beyond the current length of the destination LOB. 
It is an error to extend the destination LOB beyond the maximum length 
allowed or to try to copy from a NULL LOB.
Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
dst_locp (IN/OUT) - a locator uniquely referencing the destination LOB. 
src_locp (IN/OUT) - a locator uniquely referencing the source LOB. 
amount (IN) - the number of character or bytes, as appropriate, to be copied.
dst_offset (IN) - this is the absolute offset for the destination LOB. 
For character LOBs it is the number of characters from the beginning of the 
LOB at which to begin writing. For binary LOBs it is the number of bytes from 
the beginning of the lob from which to begin reading. The offset starts at 1.
src_offset (IN) - this is the absolute offset for the source LOB. 
For character LOBs it is the number of characters from the beginning of the 
LOB, for binary LOBs it is the number of bytes. Starts at 1.

See Also 
OCIErrorGet(), OCILobAppend(), OCILobWrite(), OCILobTrim()

OCILobCreateTemporary()

Name
OCI Lob Create Temporary

Purpose
Create a Temporary Lob

Syntax
sword OCILobCreateTemporary(OCISvcCtx          *svchp,
                            OCIError           *errhp,
                            OCILobLocator      *locp,
                            ub2                 csid,
                            ub1                 csfrm,
                            ub1                 lobtype,
                            boolean             cache,
                            OCIDuration         duration);


Comments
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a locator which points to the temporary Lob
csid (IN) - the character set id
csfrm(IN) - the character set form
lobtype (IN) - the lob type - one of the three constants OCI_TEMP_BLOB, 
               OCI_TEMP_CLOB and OCI_TEMP_NCLOB
cache(IN)-  TRUE if the temporary LOB goes through the cache; FALSE, if not.
duration(IN)- duration of the temporary LOB; Can be a valid duration id or one 
              of the values: OCI_DURATION_SESSION, OCI_DURATION_CALL
              Note: OCI_DURATION_TRANSACTION is NOT supported in 8.1
Related functions
OCILobFreeTemporary()
OCILobIsTemporary()

OCILobDisableBuffering()

Name
OCI Lob Disable Buffering

Purpose
Disable lob buffering for the input locator.


Syntax
sword OCILobDisableBuffering ( OCISvcCtx      *svchp,
                               OCIError       *errhp,
                               OCILobLocator  *locp);

Comments

Disable lob buffering for the input locator.  The next time data is
read/written from/to the lob through the input locator, the lob
buffering subsystem is *not* used.  Note that this call does *not*
implicitly flush the changes made in the buffering subsystem.  The 
user must explicitly call OCILobFlushBuffer() to do this.

Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a locator uniquely referencing the LOB. 

Related Functions
OCILobEnableBuffering()
OCIErrorGet()
OCILobFlushBuffer()




OCILobEnableBuffering()

Name
OCI Lob Enable Buffering

Purpose
Enable lob buffering for the input locator.


Syntax
sword OCILobEnableBuffering ( OCISvcCtx      *svchp,
                              OCIError       *errhp,
                              OCILobLocator  *locp);

Comments

Enable lob buffering for the input locator.  The next time data is
read/written from/to the lob through the input locator, the lob
buffering subsystem is used.  

Once lob buffering is enabled for a locator, if that locator is passed to 
one of the following routines, an error is returned:
        OCILobCopy, OCILobAppend, OCILobErase, OCILobGetLength, OCILobTrim

Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a locator uniquely referencing the LOB. 

Related Functions
OCILobDisableBuffering()
OCIErrorGet()
OCILobWrite()
OCILobRead()
OCILobFlushBuffer()




OCILobErase()

Name
OCI Lob ERase

Purpose
Erases a specified portion of the LOB data starting at a specified offset.

Syntax
sword OCILobErase ( OCISvcCtx       *svchp,
                  OCIError        *errhp,
                  OCILobLocator   *locp,
                  ub4             *amount,
                  ub4             offset );

Comments
Erases a specified portion of the LOB data starting at a specified offset.
The actual number of characters/bytes erased is returned. The actual number 
of characters/bytes and the requested number of characters/bytes will differ 
if the end of the LOB data is reached before erasing the requested number of 
characters/bytes.
If a section of data from the middle of the LOB data is erased, a hole is 
created. When data from that hole is read, 0's are returned. If the LOB is
NULL, this routine will indicate that 0 characters/bytes were erased.

Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
locp (IN/OUT) - the LOB for which to erase a section of data.
amount (IN/OUT) - On IN, the number of characters/bytes to erase. On OUT, 
the actual number of characters/bytes erased.
offset (IN) - absolute offset from the beginning of the LOB data from which 
to start erasing data. Starts at 1.

See Also
OCIErrorGet(), OCILobRead(), OCILobWrite()

OCILobOpen()

Name
OCI Lob Open

Purpose
Opens an internal or external Lob.

Syntax
sword OCILobOpen( OCISvcCtx        *svchp,
                  OCIError         *errhp,
                  OCILobLocator    *locp,
                  ub1               mode );

Comments
It is an error if the same lob is opened more than once in
the same transaction. Lobs are opened implicitly if they are
not opened before using them. A LOB has to be closed before
the transaction commits else the transaction is rolled back.
Open locators are closed if the transaction aborts. Multiple 
users can open the same lob on different locators.
Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
locp (IN/OUT) - locator points to the LOB to be opened
mode (IN) - mode in which to open the lob. The valid modes are
read-only - OCI_FILE_READONLY, read-write - OCI_FILE_READWRITE 

OCILobClose()

Name
OCI Lob Close

Purpose
Closes an open internal or external Lob.

Syntax
sword OCILobClose( OCISvcCtx        *svchp,
                   OCIError         *errhp,
                   OCILobLocator    *locp );


Comments
It is an error if the lob is not open at this time. All LOBs
that have been opened in a transaction have to be closed 
before the transaction commits, else the transaction gets
rolled back.

Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
locp  (IN)  - A locator that was opened using OCILobOpen()


OCILobFileClose()

Name
OCI Lob File CLoSe

Purpose
Closes a previously opened FILE.

Syntax
sword OCILobFileClose ( OCISvcCtx            *svchp,
                        OCIError             *errhp,
                        OCILobLocator        *filep );

Comments
Closes a previously opened FILE. It is an error if this function is called for
an internal LOB. No error is returned if the FILE exists but is not opened.
Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
filep (IN/OUT) - a pointer to a FILE locator to be closed.

See Also
OCIErrorGet(), OCILobFileOpen(), OCILobFileCloseAll(), OCILobFileIsOpen(),
OCILobFileExists(), CREATE DIRECTORY DDL




OCILobFileCloseAll()

Name
OCI LOB FILE Close All

Purpose
Closes all open FILEs on a given service context.

Syntax
sword OCILobFileCLoseAll ( OCISvcCtx *svchp, 
                           OCIError  *errhp );

Comments
Closes all open FILEs on a given service context.

Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.

See also
OCILobFileClose(),
OCIErrorGet(), OCILobFileOpen(), OCILobFileIsOpen(),
OCILobFileExists(), CREATE DIRECTORY DDL




OCILobFileExists()

Name
OCI LOB FILE exists

Purpose
Tests to see if the FILE exists on the server

Syntax
sword OCILobFileExists ( OCISvcCtx     *svchp,
                         OCIError      *errhp,
                         OCILobLocator *filep,
                         boolean       *flag );

Comments
Checks to see if a FILE exists for on the server.

Parameters
svchp (IN) - the OCI service context handle.
errhp (IN/OUT) - error handle. The OCI error handle. If there is an error, 
it is recorded in err and this function returns OCI_ERROR. Diagnostic 
information can be obtained by calling OCIErrorGet().
filep (IN) - pointer to the FILE locator that refers to the file.
flag (OUT) - returns TRUE if the FILE exists; FALSE if it does not.

See also
OCIErrorGet, CREATE DIRECTORY (DDL)




OCILobFileGetName()

Name
OCI LOB FILE Get file Name

Purpose
Gets the FILE locator's directory alias and file name.

Syntax
sword OCILobFileGetName ( OCIEnv                   *envhp,
                          OCIError                 *errhp, 
                          CONST OCILobLocator      *filep, 
                          OraText                     *dir_alias,
                          ub2                      *d_length, 
                          OraText                     *filename, 
                          ub2                      *f_length );

Comments
Returns the directory alias and file name associated with this file locator.  

Parameters
envhp (IN/OUT) - OCI environment handle initialized in object mode.
errhp (IN/OUT) -The OCI error handle. If there is an error, it is recorded in 
errhp and this function returns OCI_ERROR. Diagnostic information can be 
obtained by calling OCIErrorGet().
filep (IN) - FILE locator for which to get the directory alias and file name.
dir_alias (OUT) - buffer into which the directory alias name is placed. The 
caller must allocate enough space for the directory alias name and must not 
write into the space.
d_length (IN/OUT)                 
        - IN: length of the input dir_alias string;
        - OUT: length of the returned dir_alias string.
filename (OUT) - buffer into which the file name is placed. The caller must 
allocate enough space for the file name and must not write into the space.
f_length (IN/OUT) 
        - IN: length of the input filename string;
         - OUT: lenght of the returned filename string.

See also
OCILobFileSetName(), OCIErrorGet()




OCILobFileIsOpen()

Name
OCI LOB FILE Is Open?

Purpose
Tests to see if the FILE is open

Syntax
sword OCILobFileIsOpen ( OCISvcCtx *svchp,
                         OCIError  *errhp,
                         OCILobLocator *filep,
                         boolean       *flag );

Comments
Checks to see if the FILE on the server is open for a given LobLocator.

Parameters
svchp (IN) - the OCI service context handle.
errhp (IN/OUT) - error handle. The OCI error handle. If there is an error, it 
is recorded in err and this function returns OCI_ERROR. Diagnostic 
information can be obtained by calling OCIErrorGet().
filep (IN) - pointer to the FILE locator being examined. If the input file 
locator was never passed to OCILobFileOpen(), the file is considered not to 
be opened by this locator. However, a different locator may have opened the 
file. More than one file opens can be performed on the same file using 
different locators.
flag (OUT) - returns TRUE if the FILE is opened using this locator; FALSE if 
it is not. 

See also
OCIErrorGet, OCILobFileOpen, OCILobFileClose, OCILobFileCloseAll, CREATE 
DIRECTORY SQL command


OCILobFileOpen()

Name
OCI LOB FILE open

Purpose
Opens a FILE for read-only access

Syntax
sword OCILobFileOpen ( OCISvcCtx            *svchp,
                     OCIError             *errhp,
                     OCILobLocator        *filep,
                     ub1                  mode );

Comments
Opens a FILE. The FILE can be opened for read-only access only. FILEs may not 
be written to throough ORACLE.

Parameters 
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
filep (IN/OUT) - the FILE to open. Error if the locator does not refer to a 
FILE. 
mode (IN) - mode in which to open the file. The only valid mode is 
read-only - OCI_FILE_READONLY. 

See Also
OCILobFileClose, OCIErrorGet, OCILobFileCloseAll, OCILobFileIsOpen, 
OCILobFileSetName, CREATE DIRECTORY 




OCILobFileSetName()

Name
OCI Lob File Set NaMe

Purpose
Sets directory alias and file name in the FILE locator.

Syntax
sword OCILobFileSetName ( OCIEnv             *envhp,
                          OCIError           *errhp,
                          OCILobLocator      **filepp,
                          OraText               *dir_alias,
                          ub2                d_length, 
                          OraText               *filename, 
                          ub2                f_length );
Comments
Sets the directory alias and file name in the LOB file locator.  
Parameters
envhp (IN/OUT) - OCI environment handle initialized in object mode.
errhp (IN/OUT) - The OCI error handle. If there is an error, it is recorded 
in errhp and this function returns OCI_ERROR. Diagnostic information can be 
obtained by calling OCIErrorGet().
filepp (IN/OUT) - FILE locator for which to set the directory alias name.
The caller must have already allocated space for the locator by calling
OCIDescriptorAlloc().
dir_alias (IN) - buffer that contains the directory alias name to set in the 
locator.
d_length (IN) - length of the input dir_alias parameter.
filename (IN) - buffer that contains the file name is placed.
f_length (IN) - length of the input filename parameter.
See also
OCILobFileGetName, OCIErrorGet, CREATE DIRECTORY




OCILobFlushBuffer()

Name
OCI Lob Flush all Buffers for this lob.

Purpose
Flush/write all buffers for this lob to the server.


Syntax
sword OCILobFlushBuffer ( OCISvcCtx       *svchp,
                          OCIError        *errhp,
                          OCILobLocator   *locp,
                          ub4              flag);

Comments

Flushes to the server, changes made to the buffering subsystem that 
are associated with the lob referenced by the input locator.  This 
routine will actually write the data in the buffer to the lob in 
the database.  Lob buffering must have already been enabled for the 
input lob locator.

This routine, by default, does not free the buffer resources for
reallocation to another buffered LOB operation. However, if you
want to free the buffer explicitly, you can set the flag parameter
to OCI_LOB_BUFFER_FREE.

Parameters
svchp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a locator uniquely referencing the LOB. 
flag    (IN)     - to indicate if the buffer resources need to be freed
                   after a flush. Default value is OCI_LOB_BUFFER_NOFREE.
                   Set it to OCI_LOB_BUFFER_FREE if you want the buffer
                   resources to be freed.
Related Functions
OCILobEnableBuffering()
OCILobDisableBuffering()
OCIErrorGet()
OCILobWrite()
OCILobRead()


OCILobFreeTemporary()

Name
OCI Lob Free Temporary

Purpose
Free a temporary LOB

Syntax
sword OCILobFreeTemporary(OCISvcCtx          *svchp,
                          OCIError           *errhp,
                          OCILobLocator      *locp);

Comments
  Frees the contents of the temporary Lob this locator is pointing to. Note
  that the locator itself is not freed until a OCIDescriptorFree is done.

Parameters
svchp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a locator uniquely referencing the LOB

Related functions
OCILobCreateTemporary()
OCILobIsTemporary()


Name
OCI Lob/File Get Chunk Size

Purpose
When creating the table, the user can specify the chunking factor, which can
be a multiple of Oracle blocks. This corresponds to the chunk size used by the
LOB data layer when accessing/modifying the LOB value. Part of the chunk is
used to store system-related information and the rest stores the LOB value.
This function returns the amount of space used in the LOB chunk to store 
the LOB value.

Syntax
sword OCILobGetChunkSize ( OCISvcCtx      *svchp,
                           OCIError       *errhp,
                           OCILobLocator  *locp,
                           ub4            *chunksizep );

Comments
 Performance will be improved if the user issues read/write
requests using a multiple of this chunk size. For writes, there is an added 
benefit since LOB chunks are versioned and, if all writes are done on chunk
basis, no extra/excess versioning is done nor duplicated. Users could batch 
up the write until they have enough for a chunk instead of issuing several
write calls for the same chunk.

Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
locp (IN/OUT) - a LOB locator that uniquely references the LOB. For internal
LOBs, this locator must be a locator that was obtained from the server 
specified by svchp. For FILEs, this locator can be initialized by a Select or
OCILobFileSetName.
chunksizep (OUT) - On output, it is the length of the LOB if not NULL - for 
character LOBs it is the number of characters, for binary LOBs it is the 
number of bytes in the LOB.

Related Functions

OCILobGetLength()

Name
OCI Lob/File Length

Purpose
Gets the length of a LOB/FILE. 

Syntax
sword OCILobGetLength ( OCISvcCtx      *svchp,
                        OCIError       *errhp,
                        OCILobLocator  *locp,
                        ub4            *lenp );

Comments
Gets the length of a LOB/FILE. If the LOB/FILE is NULL, the length is 
undefined.

Parameters
svchp (IN) - the service context handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
locp (IN/OUT) - a LOB locator that uniquely references the LOB. For internal
LOBs, this locator must be a locator that was obtained from the server 
specified by svchp. For FILEs, this locator can be initialized by a Select or
OCILobFileSetName.
lenp (OUT) - On output, it is the length of the LOB if not NULL - for 
character LOBs it is the number of characters, for binary LOBs it is the 
number of bytes in the LOB.

Related Functions
OCIErrorGet, OCIFileSetName



OCILobIsEqual()

Name

OCI Lob Is Equal

Purpose
Compares two LOB locators for equality.

Syntax
sword OCILobIsEqual ( OCIEnv                  *envhp,
                      CONST OCILobLocator     *x,
                      CONST OCILobLocator     *y,
                      boolean                 *is_equal );

Comments
Compares the given LOB locators for equality.  Two LOB locators are equal if 
and only if they both refer to the same LOB data.
Two NULL locators are considered not equal by this function.
Parameters
envhp (IN) - the OCI environment handle.
x (IN) - LOB locator to compare.
y (IN) - LOB locator to compare.
is_equal (OUT) - TRUE, if the LOB locators are equal; FALSE if they are not.

See also
OCILobAssign, OCILobLocatorIsInit
OCILobLocatorAssign,
OCILobIsOpen()

Name

OCI Lob Is Open
sword OCILobIsOpen(svchp, errhp, locp, flag)
OCISvcCtx     *svchp;
OCIError      *errhp;
OCILobLocator *locp;
boolean       *flag;

Comments
   Checks if the LOB locator was opened before. flag is set to TRUE
   if opened; FALSE otherwise 


Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN) - the locator to test for temporary LOB
flag(OUT) - TRUE, if the LOB locator points to is open
                    FALSE, if not.

OCILobIsTemporary()

Name

OCI Lob Is Temporary

Purpose
  Tests if this locator points to a temporary LOB

Syntax
sword OCILobIsTemporary(OCIEnv            *envhp,
                        OCIError          *errhp,
                        OCILobLocator     *locp,
                        boolean           *is_temporary);

Comments
Tests the locator to determine if it points to a temporary LOB.
If so, is_temporary is set to TRUE. If not, is_temporary is set
to FALSE.

Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN) - the locator to test for temporary LOB
is_temporary(OUT) - TRUE, if the LOB locator points to a temporary LOB;
                    FALSE, if not.

See Also
OCILobCreateTemporary, OCILobFreeTemporary


OCILobLoadFromFile()

Name
OCI Lob Load From File

Purpose
Load/copy all or a portion of the file into an internal LOB.

Syntax
sword OCILobLoadFromFile ( OCISvcCtx        *svchp,
                           OCIError         *errhp,
                           OCILobLocator    *dst_locp,
                           OCILobLocator    *src_filep,
                           ub4              amount,
                           ub4              dst_offset,
                           ub4              src_offset );

Comments
Loads/copies a portion or all of a file value into an internal LOB as 
specified.  The data is copied from the source file to the destination 
internal LOB (BLOB/CLOB).  No character set conversions are performed 
when copying the bfile data to a clob/nclob.  The bfile data must already
be in the same character set as the clob/nclob in the database.  No
error checking is performed to verify this.
The source (src_filep) and the destination (dst_locp) LOBs must already exist.
If the data already exists at the destination's start position, it is 
overwritten with the source data. If the destination's start position is 
beyond the end of the current data, a hole is created from the end of the data
to the beginning of the newly written data from the source. The destination 
LOB is extended to accommodate the newly written data if it extends 
beyond the current length of the destination LOB. 
It is an error to extend the destination LOB beyond the maximum length 
allowed or to try to copy from a NULL LOB.
Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
dst_locp (IN/OUT) - a locator uniquely referencing the destination internal 
LOB which may be of type blob, clob, or nclob. 
src_filep (IN/OUT) - a locator uniquely referencing the source BFILE. 
amount (IN) - the number of bytes to be copied.
dst_offset (IN) - this is the absolute offset for the destination LOB. 
For character LOBs it is the number of characters from the beginning of the 
LOB at which to begin writing. For binary LOBs it is the number of bytes from 
the beginning of the lob from which to begin reading. The offset starts at 1.
src_offset (IN) - this is the absolute offset for the source BFILE.  It is 
the number of bytes from the beginning of the LOB.  The offset starts at 1.

See Also 
OCIErrorGet(), OCILobAppend(), OCILobWrite(), OCILobTrim(), OCILobCopy()

OCILobLocatorAssign()

Name
OCI Lob LOCATOR ASsiGn

Purpose
Assigns one LOB/FILE locator to another.

Syntax
sword OCILobLocatorAssign ( OCISvcCtx             *svchp, 
                            OCIError              *errhp, 
                            CONST OCILobLocator   *src_locp, 
                            OCILobLocator         **dst_locpp );

Comments
Assign source locator to destination locator.  After the assignment, both 
locators refer to the same LOB data.  For internal LOBs, the source locator's 
LOB data gets copied to the destination locator's LOB data only when the 
destination locator gets stored in the table.  Therefore, issuing a flush of 
the object containing the destination locator will copy the LOB data. For 
FILEs only the locator that refers to the OS file is copied to the table. The
OS file is not copied.
Note : the only difference between this and OCILobAssign is that this takes
a OCI service handle pointer instead of a OCI environment handle pointer

Parameters
svchp (IN/OUT) - OCI service handle initialized in object mode.
errhp (IN/OUT) - The OCI error handle. If there is an error, it is recorded 
in errhp and this function returns OCI_ERROR. Diagnostic information can be 
obtained by calling OCIErrorGet().
src_locp (IN) - LOB locator to copy from.
dst_locpp (IN/OUT) - LOB locator to copy to.  The caller must allocate space 
for the OCILobLocator by calling OCIDescriptorAlloc().

See also
OCIErrorGet()
OCILobIsEqual()
OCILobLocatorIsInit()
OCILobAssign()




OCILobLocatorIsInit()

Name
OCI LOB locator is initialized?

Purpose
Tests to see if a given LOB locator is initialized.

Syntax
sword OCILobLocatorIsInit ( OCIEnv   *envhp,
                            OCIError *errhp,
                            CONST OCILobLocator *locp,
                            boolean *is_initialized );

Comments
Tests to see if a given LOB locator is initialized.

Parameters
envhp (IN/OUT) - OCI environment handle initialized in object mode.
errhp (IN/OUT) - error handle. The OCI error handle. If there is an error, it 
is recorded in err and this function returns OCI_ERROR. Diagnostic 
information can be obtained by calling OCIErrorGet().
locp (IN) - the LOB locator being tested
is_initialized (OUT) - returns TRUE if the given LOB locator is initialized; 
FALSE if it is not.

See also
OCIErrorGet, OCILobIsEqual




OCILobRead()

Name
OCI Lob/File ReaD

Purpose
Reads a portion of a LOB/FILE as specified by the call into a buffer. 

Syntax
sword OCILobRead ( OCISvcCtx       *svchp,
                   OCIError        *errhp,
                   OCILobLocator   *locp,
                   ub4             offset,
                   ub4             *amtp,
                   dvoid           *bufp,
                   ub4             bufl,
                   dvoid           *ctxp,  
                   OCICallbackLobRead (cbfp)
                                   (
                                    dvoid         *ctxp,
                                    CONST dvoid   *bufp,
                                    ub4           len,
                                    ub1           piece )
                   ub2             csid,
                   ub1             csfrm );

Comments
Reads a portion of a LOB/FILE as specified by the call into a buffer. Data 
read from a hole is returned as 0s. It is an error to try to read from a NULL
LOB/FILE. The OS FILE must already exist on the server and must have been 
opened using the input locator. Oracle must hav epermission to read the OS 
file and user must have read permission on the directory object.

Parameters
svchp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
locp (IN/OUT) - a LOB locator that uniquely references a LOB. 
offset (IN) - On input, it is the absolute offset, for character LOBs in the 
number of characters from the beginning of the LOB, for binary LOBs it is the 
number of bytes. Starts from 1.
amtp (IN/OUT) - On input, the number of character or bytes to be read. On 
output, the actual number of bytes or characters read. 
If the amount of bytes to be read is larger than the buffer length it is 
assumed that the LOB is being read in a streamed mode. On input if this value 
is 0, then the data shall be read in streamed mode from the LOB until the end 
of LOB. If the data is read in pieces, *amtp always contains the length of 
the last piece read.  If a callback function is defined, then this callback 
function will be invoked each time bufl bytes are read off the pipe. Each 
piece will be written into bufp.
If the callback function is not defined, then OCI_NEED_DATA error code will 
be returned. The application must invoke the LOB read over and over again to 
read more pieces of the LOB until the OCI_NEED_DATA error code is not 
returned. The buffer pointer and the length can be different in each call 
if the pieces are being read into different sizes and location. 
bufp (IN) - the pointer to a buffer into which the piece will be read. The 
length of the allocated memory is assumed to be bufl. 
bufl (IN) - the length of the buffer in octets. 
ctxp (IN) - the context for the call back function. Can be NULL.
cbfp (IN) - a callback that may be registered to be called for each piece. If 
this is NULL, then OCI_NEED_DATA will be returned for each piece. 
The callback function must return OCI_CONTINUE for the read to continue. 
If any other error code is returned, the LOB read is aborted. 
  ctxp (IN) - the context for the call back function. Can be NULL.
  bufp (IN) - a buffer pointer for the piece.
  len (IN) - the length of length of current piece in bufp.
  piece (IN) - which piece - OCI_FIRST_PIECE, OCI_NEXT_PIECE or 
  OCI_LAST_PIECE.
csid - the character set ID of the buffer data
csfrm - the character set form of the buffer data

Related Functions
OCIErrorGet, OCILobWrite, OCILobFileOpen, OCILobFileSetName, CREATE DIRECTORY




OCILobTrim()

Name

OCI Lob  Trim

Purpose
Trims the lob value to a shorter length

Syntax
sword OCILobTrim ( OCISvcCtx       *svchp,
                 OCIError        *errhp,
                 OCILobLocator   *locp,
                 ub4             newlen );

Comments
Truncates LOB data to a specified shorter length. 

Parameters
svchp (IN) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a LOB locator that uniquely references the LOB. This locator 
must be a locator that was obtained from the server specified by svchp. 
newlen (IN) - the new length of the LOB data, which must be less than or equal
to the current length. 

Related Functions
OCIErrorGet, OCILobWrite, OCiLobErase, OCILobAppend, OCILobCopy





OCILobWrite()

Name
OCI Lob Write

Purpose
Writes a buffer into a LOB

Syntax
sword OCILobWrite ( OCISvcCtx       *svchp,
                    OCIError        *errhp,
                    OCILobLocator   *locp,
                    ub4             offset,
                    ub4             *amtp,
                    dvoid           *bufp, 
                    ub4             buflen,
                    ub1             piece,
                    dvoid           *ctxp,  
                    OCICallbackLobWrite   (cbfp)
                                    (
                                    dvoid    *ctxp,
                                    dvoid    *bufp,
                                    ub4      *lenp,
                                    ub1      *piecep ) 
                    ub2             csid
                    ub1             csfrm );


Comments
Writes a buffer into a LOB as specified. If LOB data already exists 
it is overwritten with the data stored in the buffer.
The buffer can be written to the LOB in a single piece with this call, or
it can be provided piecewise using callbacks or a standard polling method.
If this value of the piece parameter is OCI_FIRST_PIECE, data must be 
provided through callbacks or polling.
If a callback function is defined in the cbfp parameter, then this callback 
function will be invoked to get the next piece after a piece is written to 
the pipe. Each piece will be written from bufp.
If no callback function is defined, then OCILobWrite() returns the 
OCI_NEED_DATA error code. The application must all OCILobWrite() again 
to write more pieces of the LOB. In this mode, the buffer pointer and the 
length can be different in each call if the pieces are of different sizes and 
from different locations. A piece value of OCI_LAST_PIECE terminates the 
piecewise write. 

Parameters
svchp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a LOB locator that uniquely references a LOB. 
offset (IN) - On input, it is the absolute offset, for character LOBs in 
the number of characters from the beginning of the LOB, for binary LOBs it 
is the number of bytes. Starts at 1.
bufp (IN) - the pointer to a buffer from which the piece will be written. The 
length of the allocated memory is assumed to be the value passed in bufl. 
Even if the data is being written in pieces, bufp must contain the first 
piece of the LOB when this call is invoked.
bufl (IN) - the length of the buffer in bytes.
Note: This parameter assumes an 8-bit byte. If your platform uses a 
longer byte, the value of bufl must be adjusted accordingly.
piece (IN) - which piece of the buffer is being written. The default value for
this parameter is OCI_ONE_PIECE, indicating the buffer will be written in a 
single piece.
The following other values are also possible for piecewise or callback mode: 
OCI_FIRST_PIECE, OCI_NEXT_PIECE and OCI_LAST_PIECE.
amtp (IN/OUT) - On input, takes the number of character or bytes to be 
written. On output, returns the actual number of bytes or characters written. 
If the data is written in pieces, *amtp will contain the total length of the 
pieces written at the end of the call (last piece written) and is undefined in
between. 
(Note it is different from the piecewise read case)
ctxp (IN) - the context for the call back function. Can be NULL.
cbfp (IN) - a callback that may be registered to be called for each piece in 
a piecewise write. If this is NULL, the standard polling method will be used.
The callback function must return OCI_CONTINUE for the write to continue. 
If any other error code is returned, the LOB write is aborted. The 
callback takes the following parameters:
  ctxp (IN) - the context for the call back function. Can be NULL.
  bufp (IN/OUT) - a buffer pointer for the piece.
  lenp (IN/OUT) - the length of the buffer (in octets) and the length of 
  current piece in bufp (out octets).
  piecep (OUT) - which piece - OCI_NEXT_PIECE or OCI_LAST_PIECE.
csid - the character set ID of the buffer data
csfrm - the character set form of the buffer data
Related Functions

OCILobWriteAppend()

Name
OCI Lob Write Append

Purpose
Writes data to the end of a LOB value. This call provides the ability
to get the length of the data and append it to the end of the LOB in
a single round trip to the server.

Syntax
sword OCILobWriteAppend ( OCISvcCtx       *svchp,
                    OCIError        *errhp,
                    OCILobLocator   *locp,
                    ub4             *amtp,
                    dvoid           *bufp, 
                    ub4             buflen,
                    ub1             piece,
                    dvoid           *ctxp,  
                    OCICallbackLobWrite   (cbfp)
                                    (
                                    dvoid    *ctxp,
                                    dvoid    *bufp,
                                    ub4      *lenp,
                                    ub1      *piecep ) 
                    ub2             csid
                    ub1             csfrm );


Comments
Writes a buffer to the end of a LOB as specified. If LOB data already exists 
it is overwritten with the data stored in the buffer.
The buffer can be written to the LOB in a single piece with this call, or
it can be provided piecewise using callbacks or a standard polling method.
If this value of the piece parameter is OCI_FIRST_PIECE, data must be 
provided through callbacks or polling.
If a callback function is defined in the cbfp parameter, then this callback 
function will be invoked to get the next piece after a piece is written to the 
pipe. Each piece will be written from bufp.
If no callback function is defined, then OCILobWriteAppend() returns the 
OCI_NEED_DATA error code. The application must all OCILobWriteAppend() again 
to write more pieces of the LOB. In this mode, the buffer pointer and the 
length can be different in each call if the pieces are of different sizes and 
from different locations. A piece value of OCI_LAST_PIECE terminates the 
piecewise write. 

Parameters
svchp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
locp (IN/OUT) - a LOB locator that uniquely references a LOB. 
bufp (IN) - the pointer to a buffer from which the piece will be written. The 
length of the allocated memory is assumed to be the value passed in bufl. Even 
if the data is being written in pieces, bufp must contain the first piece of 
the LOB when this call is invoked.
bufl (IN) - the length of the buffer in bytes.
Note: This parameter assumes an 8-bit byte. If your platform uses a 
longer byte, the value of bufl must be adjusted accordingly.
piece (IN) - which piece of the buffer is being written. The default value for
this parameter is OCI_ONE_PIECE, indicating the buffer will be written in a 
single piece.
The following other values are also possible for piecewise or callback mode: 
OCI_FIRST_PIECE, OCI_NEXT_PIECE and OCI_LAST_PIECE.
amtp (IN/OUT) - On input, takes the number of character or bytes to be 
written. On output, returns the actual number of bytes or characters written. 
If the data is written in pieces, *amtp will contain the total length of the 
pieces written at the end of the call (last piece written) and is undefined in
between. 
(Note it is different from the piecewise read case)
ctxp (IN) - the context for the call back function. Can be NULL.
cbfp (IN) - a callback that may be registered to be called for each piece in a 
piecewise write. If this is NULL, the standard polling method will be used.
The callback function must return OCI_CONTINUE for the write to continue. 
If any other error code is returned, the LOB write is aborted. The 
callback takes the following parameters:
  ctxp (IN) - the context for the call back function. Can be NULL.
  bufp (IN/OUT) - a buffer pointer for the piece.
  lenp (IN/OUT) - the length of the buffer (in octets) and the length of 
  current piece in bufp (out octets).
  piecep (OUT) - which piece - OCI_NEXT_PIECE or OCI_LAST_PIECE.
csid - the character set ID of the buffer data
csfrm - the character set form of the buffer data
Related Functions



OCILogoff()
Name
OCI simplified Logoff
Purpose
This function is used to terminate a session created with OCILogon().
Syntax
sword OCILogoff ( OCISvcCtx      *svchp
                   OCIError       *errhp );
Comments
This call is used to terminate a session which was created with OCILogon().  
This call implicitly deallocates the server, authentication, and service 
context handles.
Note: For more information on logging on and off in an application, 
refer to the section "Application Initialization, Connection, and 
Authorization" on page 2-16.
Parameters
svchp (IN) - the service context handle which was used in the call to 
OCILogon().
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
See Also
OCILogon()






OCILogon()
Name
OCI Service Context Logon
Purpose
This function is used to create a simple logon session.
Syntax
sword OCILogon ( OCIEnv          *envhp,
                       OCIError        *errhp,
                       OCISvcCtx       *svchp,
                       CONST OraText      *username,
                       ub4             uname_len,
                       CONST OraText      *password,
                       ub4             passwd_len,
                       CONST OraText      *dbname,
                       ub4             dbname_len );
Comments
This function is used to create a simple logon session for an application. 
Note: Users requiring more complex session (e.g., TP monitor 
applications) should refer to the section "Application Initialization, 
Connection, and Authorization" on page 2-16.
This call allocates the error and service context handles which are passed to 
it. This call also implicitly allocates server and authentication handles 
associated with the session.  These handles can be retrieved by calling 
OCIAttrGet() on the service context handle.
Parameters
envhp (IN) - the OCI environment handle.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
svchp (OUT) - the service context pointer.
username (IN) - the username.
uname_len (IN) - the length of username.
password (IN) - the user's password.
passwd_len (IN) - the length of password.
dbname (IN) - the name of the database to connect to.
dbname_len (IN) - the length of dbname.
See Also
OCILogoff()





OCIMemoryFree()
Name
OCI FREE Memory
Purpose
Frees up storage associated with the pointer.
Syntax
void OCIMemoryFree ( CONST OCIStmt   *stmhp,
                     dvoid           *memptr);
Comments
Frees up dynamically allocated data pointers associated with the pointer using 
either the default memory free function or the registered memory free 
function, as the case may be.
A user-defined memory free function can be registered during the initial call 
to OCIInitialize(). 
This call is always successful. 
Parameters
stmhp (IN) - statement handle which returned this data buffer.
memptr (IN) - pointer to data allocated by the client library. 
Related Functions
OCIInitialize()





OCIParamGet()
Name
OCI Get PARaMeter
Purpose
Returns a descriptor of a parameter specified by position in the describe 
handle or statement handle.
Syntax
sword OCIParamGet ( CONST dvoid       *hndlp,
                  ub4         htype,
                  OCIError    *errhp,
                  dvoid    **parmdpp,
                  ub4         pos );
Comments
This call returns a descriptor of a parameter specified by position in the 
describe handle or statement handle. Parameter descriptors are always 
allocated internally by the OCI library. They are read-only.
OCI_NO_DATA may be returned if there are no parameter descriptors for this 
position. 
See Appendix B for more detailed information about parameter descriptor 
attributes.
Parameters
hndlp (IN) - a statement handle or describe handle. The OCIParamGet() 
function will return a parameter descriptor for this handle. 
htype (IN) - the type of the handle passed in the handle parameter. Valid 
types are OCI_HTYPE_DESCRIBE, for a describe handle OCI_HTYPE_STMT, for a 
statement handle
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
parmdpp (OUT) - a descriptor of the parameter at the position given in the pos 
parameter.
pos (IN) - position number in the statement handle or describe handle. A 
parameter descriptor will be returned for this position.
Note: OCI_NO_DATA may be returned if there are no parameter 
descriptors for this position. 
Related Functions
OCIAttrGet(), OCIAttrSet()





OCIParamSet()
Name
OCI Parameter Set in handle
Purpose
Used to set a complex object retrieval descriptor into a complex object 
retrieval handle.
Syntax
sword   OCIParamGet ( dvoid *hndlp,
                       ub4 htyp,
                      OCIError *errhp,
                      CONST dvoid *dscp,
                      ub4 dtyp,
                      ub4 pos );
Comments
This call sets a given complex object retrieval descriptor into a complex 
object retrieval handle.
The handle must have been previously allocated using OCIHandleAlloc(), and 
the descriptor must have been previously allocated using OCIDescAlloc(). 
Attributes of the descriptor are set using OCIAttrSet().
Parameters
hndlp (IN/OUT) - handle pointer.
htype (IN) - handle type.
errhp (IN/OUT) - error handle.
dscp (IN) - complex object retrieval descriptor pointer.
dtyp (IN) - 
pos (IN) - position number.
See Also





OCIPasswordChange()
Name
OCI Change PassWord
Purpose
This call allows the password of an account to be changed.
Syntax
sword OCIPasswordChange ( OCISvcCtx     *svchp,
                        OCIError      *errhp,
                        CONST OraText    *user_name,
                        ub4           usernm_len,
                        CONST OraText    *opasswd,
                        ub4           opasswd_len,
                        CONST OraText    *npasswd,
                        sb4           npasswd_len,
                        ub4           mode);
Comments
This call allows the password of an account to be changed. This call is 
similar to OCISessionBegin() with the following differences:
If the user authentication is already established, it authenticates 
the account using the old password and then changes the 
password to the new password
If the user authentication is not established, it establishes a user 
authentication and authenticates the account using the old 
password, then changes the password to the new password.
This call is useful when the password of an account is expired and 
OCISessionBegin() returns an error or warning which indicates that the 
password has expired. 
Parameters
svchp (IN/OUT) - a handle to a service context. The service context handle 
must be initialized and have a server context handle associated with it.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
user_name (IN) - specifies the user name. It points to a character string, 
whose length is specified in usernm_len. This parameter must be NULL if the 
service context has been initialized with an authentication handle.
usernm_len (IN) - the length of the user name string specified in user_name. 
For a valid user name string, usernm_len must be non-zero.
opasswd (IN) - specifies the user's old password. It points to a character 
string, whose length is specified in opasswd_len .
opasswd_len (IN) - the length of the old password string specified in opasswd. 
For a valid password string, opasswd_len must be non-zero.
npasswd (IN) - specifies the user's new password. It points to a character 
string, whose length is specified in npasswd_len which must be non-zero for a 
valid password string. If the password complexity verification routine is 
specified in the user's profile to verify the new password's complexity, the 
new password must meet the complexity requirements of the verification 
function.
npasswd_len (IN)  - then length of the new password string specified in 
npasswd. For a valid password string, npasswd_len must be non-zero.
mode - pass as OCI_DEFAULT.
Related Functions
OCISessionBegin()


----------------------------------OCIReset------------------------------------


OCIReset()
Name
OCI Reset
Purpose
Resets the interrupted asynchronous operation and protocol. Must be called
if a OCIBreak call had been issued while a non-blocking operation was in
progress.
Syntax
sword OCIReset ( dvoid      *hndlp,
                 OCIError   *errhp);
Comments
This call is called in non-blocking mode ONLY. Resets the interrupted
asynchronous operation and protocol. Must be called if a OCIBreak call 
had been issued while a non-blocking operation was in progress. 
Parameters
hndlp (IN) - the service context handle or the server context handle.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
Related Functions


OCIResultSetToStmt()
Name
OCI convert Result Set to Statement Handle
Purpose
Converts a descriptor to statement handle for fetching rows.
Syntax
sword OCIResultSetToStmt ( OCIResult     *rsetdp,
                         OCIError      *errhp );
Comments
Converts a descriptor to statement handle for fetching rows.
A result set descriptor can be allocated with a call to OCIDescAlloc().
Parameters
rsetdp (IN/OUT) - a result set descriptor pointer. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
Related Functions
OCIDescAlloc()




OCIServerAttach()
Name
OCI ATtaCH to server
Purpose
Creates an access path to a data source for OCI operations.
Syntax
sword OCIServerAttach ( OCIServer    *srvhp,
                      OCIError     *errhp,
                      CONST OraText   *dblink,
                      sb4          dblink_len,
                      ub4          mode);
Comments
This call is used to create an association between an OCI application and a 
particular server. 
This call initializes a server context handle, which must have been previously 
allocated with a call to OCIHandleAlloc().
The server context handle initialized by this call can be associated with a 
service context through a call to OCIAttrSet(). Once that association has been 
made, OCI operations can be performed against the server.
If an application is operating against multiple servers, multiple server 
context handles can be maintained. OCI operations are performed against 
whichever server context is currently associated with the service context.
Parameters
srvhp (IN/OUT) - an uninitialized server context handle, which gets 
initialized by this call. Passing in an initialized server handle causes an 
error. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
dblink (IN) - specifies the database (server) to use. This parameter points to
a character string which specifies a connect string or a service point. If the 
connect string is NULL, then this call attaches to the default host. The length
of connstr is specified in connstr_len. The connstr pointer may be freed by the
caller on return.
dblink_len (IN) - the length of the string pointed to by connstr. For a valid 
connect string name or alias, connstr_len must be non-zero.
mode (IN) - specifies the various modes of operation.  For release 8.0, pass as
OCI_DEFAULT - in this mode, calls made to the server on this server context 
are made in blocking mode. 
Example
See the description of OCIStmtPrepare() on page 13-96 for an example showing 
the use of OCIServerAttach().
Related Functions
OCIServerDetach()



OCIServerDetach()
Name
OCI DeTaCH server
Purpose
Deletes an access to a data source for OCI operations.
Syntax
sword OCIServerDetach ( OCIServer   *svrhp,
                      OCIError    *errhp,
                      ub4         mode); 
Comments
This call deletes an access to data source for OCI operations, which was 
established by a call to OCIServerAttach(). 
Parameters
srvhp (IN) - a handle to an initialized server context, which gets reset to 
uninitialized state. The handle is not de-allocated. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
mode (IN) - specifies the various modes of operation. The only valid mode is 
OCI_DEFAULT for the default mode. 
Related Functions
OCIServerAttach()



OCIServerVersion()
Name
OCI VERSion
Purpose
Returns the version string of the Oracle server.
Syntax
sword OCIServerVersion ( dvoid        *hndlp, 
                       OCIError     *errhp, 
                       OraText         *bufp,
                       ub4          bufsz
                       ub1          hndltype );
Comments
This call returns the version string of the Oracle server. 
For example, the following might be returned as the version string if your 
application is running against a 7.3.2 server:
Oracle7 Server Release 7.3.2.0.0 - Production Release
PL/SQL Release 2.3.2.0.0 - Production
CORE Version 3.5.2.0.0 - Production
TNS for SEQUENT DYNIX/ptx: Version 2.3.2.0.0 - Production
NLSRTL Version 3.2.2.0.0 - Production

Parameters
hndlp (IN) - the service context handle or the server context handle.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
bufp (IN) - the buffer in which the version information is returned.
bufsz (IN) - the length of the buffer.
hndltype (IN) - the type of handle passed to the function.
Related Functions





OCISessionBegin()
Name
OCI Session Begin and authenticate user
Purpose
Creates a user authentication and begins a user session for a given server.
Syntax
sword OCISessionBegin ( OCISvcCtx     *svchp,
                      OCIError      *errhp,
                      OCISession    *usrhp,
                      ub4           credt,
                      ub4           mode);

Comments
For Oracle8, OCISessionBegin() must be called for any given server handle 
before requests can be made against it. Also, OCISessionBegin() only supports 
authenticating the user for access to the Oracle server specified by the 
server handle in the service context. In other words, after OCIServerAttach() 
is called to initialize a server handle, OCISessionBegin() must be called to 
authenticate the user for that given server. 
When OCISessionBegin() is called for the first time for the given server 
handle, the initialized authentication handle is called a primary 
authentication context. A primary authentication context may not be created 
with the OCI_MIGRATE mode. Also, only one primary authentication context can 
be created for a given server handle and the primary authentication context c
an only ever be used with that server handle. If the primary authentication 
context is set in a service handle with a different server handle, then an 
error will result.
After OCISessionBegin() has been called for the server handle, and the primary 
authentication context is set in the service handle, OCISessionBegin() may be 
called again to initialize another authentication handle with different (or 
the same) credentials. When OCISessionBegin() is called with a service handle 
set with a primary authentication context, the returned authentication context
in authp is called a user authentication context. As many user authentication 
contexts may be initialized as desired.
User authentication contexts may be created with the OCI_MIGRATE mode. 
If the OCI_MIGRATE mode is not specified, then the user authentication 
context can only ever be used with the same server handle set in svchp. If 
OCI_MIGRATE mode is specified, then the user authentication may be set 
with different server handles. However, the user authentication context is 
restricted to use with only server handles which resolve to the same database 
instance and that have equivalent primary authentication contexts. Equivalent 
authentication contexts are those which were authenticated as the same 
database user.
OCI_SYSDBA, OCI_SYSOPER, and OCI_PRELIM_AUTH may only be used 
with a primary authentication context.
To provide credentials for a call to OCISessionBegin(), one of two methods are 
supported. The first is to provide a valid username and password pair for 
database authentication in the user authentication handle passed to 
OCISessionBegin(). This involves using OCIAttrSet() to set the 
OCI_ATTR_USERNAME and OCI_ATTR_PASSWORD attributes on the 
authentication handle. Then OCISessionBegin() is called with 
OCI_CRED_RDBMS.
Note: When the authentication handle is terminated using 
OCISessionEnd(), the username and password attributes remain 
unchanged and thus can be re-used in a future call to OCISessionBegin(). 
Otherwise, they must be reset to new values before the next 
OCISessionBegin() call.
The second type of credentials supported are external credentials. No 
attributes need to be set on the authentication handle before calling 
OCISessionBegin(). The credential type is OCI_CRED_EXT. This is equivalent 
to the Oracle7 `connect /' syntax. If values have been set for 
OCI_ATTR_USERNAME and OCI_ATTR_PASSWORD, then these are 
ignored if OCI_CRED_EXT is used.
Parameters
svchp (IN) - a handle to a service context. There must be a valid server 
handle set in svchp.
errhp (IN) - an error handle to the retrieve diagnostic information.
usrhp (IN/OUT) - a handle to an authentication context, which is initialized 
by this call.
credt (IN) - specifies the type of credentials to use for authentication. 
Valid values for credt are:
OCI_CRED_RDBMS - authenticate using a database username and 
password pair as credentials. The attributes OCI_ATTR_USERNAME 
and OCI_ATTR_PASSWORD should be set on the authentication 
context before this call.
OCI_CRED_EXT - authenticate using external credentials. No username 
or password is provided.
mode (IN) - specifies the various modes of operation. Valid modes are:
OCI_DEFAULT - in this mode, the authentication context returned may 
only ever be set with the same server context specified in svchp. This 
establishes the primary authentication context.
OCI_MIGRATE - in this mode, the new authentication context may be 
set in a service handle with a different server handle. This mode 
establishes the user authentication context. 
OCI_SYSDBA - in this mode, the user is authenticated for SYSDBA 
access.
OCI_SYSOPER - in this mode, the user is authenticated for SYSOPER 
access.
OCI_PRELIM_AUTH - this mode may only be used with OCI_SYSDBA 
or OCI_SYSOPER to authenticate for certain administration tasks.
Related Functions
OCISessionEnd()






OCISessionEnd()
Name
OCI Terminate user Authentication Context
Purpose
Terminates a user authentication context created by OCISessionBegin()
Syntax
sword OCISessionEnd ( OCISvcCtx       *svchp,
                    OCIError        *errhp,
                    OCISession      *usrhp,
                    ub4             mode);

Comments
The user security context associated with the service context is invalidated 
by this call. Storage for the authentication context is not freed. The 
transaction specified by the service context is implicitly committed. The 
transaction handle, if explicitly allocated, may be freed if not being used.
Resources allocated on the server for this user are freed.
The authentication handle may be reused in a new call to OCISessionBegin().
Parameters
svchp (IN/OUT) - the service context handle. There must be a valid server 
handle and user authentication handle associated with svchp.
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
usrhp (IN) - de-authenticate this user. If this parameter is passed as NULL, the 
user in the service context handle is de-authenticated.
mode (IN) - the only valid mode is OCI_DEFAULT.
Example
In this example, an authentication context is destroyed.
Related Functions
OCISessionBegin()




OCIStmtExecute()
Name
OCI EXECute
Purpose
This call associates an application request with a server.
Syntax
sword OCIStmtExecute ( OCISvcCtx           *svchp,
                     OCIStmt             *stmtp,
                     OCIError            *errhp,
                     ub4                 iters,
                     ub4                 rowoff,
                     CONST OCISnapshot   *snap_in,
                     OCISnapshot         *snap_out,
                     ub4                 mode );
Comments
This function  is used to execute a prepared SQL statement.
Using an execute call, the application associates a request with a server. On 
success, OCI_SUCCESS is returned.
If a SELECT statement is executed, the description of the select list follows 
implicitly as a response. This description is buffered on the client side for 
describes, fetches and define type conversions. Hence it is optimal to 
describe a select list only after an execute. 
Also for SELECT statements, some results are available implicitly. Rows will 
be received and buffered at the end of the execute. For queries with small row 
count, a prefetch causes memory to be released in the server if the end of 
fetch is reached, an optimization that may result in memory usage reduction. 
Set attribute call has been defined to set the number of rows to be prefetched
per result set.
For SELECT statements, at the end of the execute, the statement handle 
implicitly maintains a reference to the service context on which it is 
executed. It is the user's responsibility to maintain the integrity of the 
service context. If the attributes of a service context is changed for 
executing some operations on this service context, the service context must 
be restored to have the same attributes, that a statement was executed with, 
prior to a fetch on the statement handle. The implicit reference is maintained 
until the statement handle is freed or the fetch is cancelled or an end of 
fetch condition is reached.
Note: If output variables are defined for a SELECT statement before a 
call to OCIStmtExecute(), the number of rows specified by iters will be 
fetched directly into the defined output buffers and additional rows 
equivalent to the prefetch count will be prefetched. If there are no 
additional rows, then the fetch is complete without calling 
OCIStmtFetch().
The execute call will return errors if the statement has bind data types that 
are not supported in an Oracle7 server.
Parameters
svchp (IN/OUT) - service context handle. 
stmtp (IN/OUT) - an statement handle - defines the statement and the 
associated data to be executed at the server. It is invalid to pass in a 
statement handle that has bind of data types only supported in release 8.0 
when srvchp points to an Oracle7 server. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. If the statement is being 
batched and it is successful, then this handle will contain this particular 
statement execution specific errors returned from the server when the batch is 
flushed.
iters (IN) - the number of times this statement is executed for non-Select 
statements. For Select statements, if iters is non-zero, then defines must 
have been done for the statement handle. The execution fetches iters rows into 
these predefined buffers and prefetches more rows depending upon the prefetch 
row count. This function returns an error if iters=0 for non-SELECT 
statements.
rowoff (IN) - the index from which the data in an array bind is relevant for 
this multiple row execution. 
snap_in (IN) - this parameter is optional. if supplied, must point to a 
snapshot descriptor of type OCI_DTYPE_SNAP.  The contents of this descriptor 
must be obtained from the snap_out parameter of a previous call.  The 
descriptor is ignored if the SQL is not a SELECT.  This facility allows 
multiple service contexts to ORACLE to see the same consistent snapshot of the 
database's committed data.  However, uncommitted data in one context is not 
visible to another context even using the same snapshot.
snap_out (OUT) - this parameter optional. if supplied, must point to a 
descriptor of type OCI_DTYPE_SNAP. This descriptor is filled in with an 
opaque representation which is the current ORACLE "system change 
number" suitable as a snap_in input to a subsequent call to OCIStmtExecute().  
This descriptor should not be used any longer than necessary in order to avoid 
"snapshot too old" errors. 
mode (IN) - The modes are:
If OCI_DEFAULT_MODE, the default mode, is selected, the request is 
immediately executed. Error handle contains diagnostics on error if any. 
OCI_EXACT_FETCH - if the statement is a SQL SELECT, this mode is 
only valid if the application has set the prefetch row count prior to this 
call. In this mode, the OCI library will get up to the number of rows 
specified (i.e., prefetch row count plus iters). If the number of rows 
returned by the query is greater than this value, OCI_ERROR will be 
returned with ORA-01422 as the implementation specific error in a 
diagnostic record. If the number of rows returned by the query is 
smaller than the prefetch row count, OCI_SUCCESS_WITH_INFO will 
be returned with ORA-01403 as the implementation specific error. The 
prefetch buffer size is ignored and the OCI library tries to allocate all the 
space required to contain the prefetched rows. The exact fetch semantics 
apply to only the top level rows. No more rows can be fetched for this 
query at the end of the call. 
OCI_KEEP_FETCH_STATE - the result set rows (not yet fetched) of this 
statement executed in this transaction will be maintained when the 
transaction is detached for migration. By default, a query is cancelled 
when a transaction is detached for migration. This mode is the default 
mode when connected to a V7 server. 
Related Functions
OCIStmtPrepare()





OCIStmtFetch()
Name
OCI FetCH
Purpose
Fetches rows from a query.
Syntax
sword OCIStmtFetch ( OCIStmt     *stmtp,
                   OCIError    *errhp, 
                   ub4         nrows,
                   ub2         orientation,
                   ub4         mode);
Comments
The fetch call is a local call, if prefetched rows suffice. However, this is 
transparent to the application. If LOB columns are being read, LOB locators 
are fetched for subsequent LOB operations to be performed on these locators. 
Prefetching is turned off if LONG columns are involved. 
A fetch with nrows set to 0 rows effectively cancels the fetch for this 
statement.
Parameters
stmtp (IN) - a statement (application request) handle.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
nrows (IN) - number of rows to be fetched from the current position.
orientation (IN) - for release 8.0, the only acceptable value is 
OCI_FETCH_NEXT, which is also the default value. 
mode (IN) - for release 8.0, beta-1, the following mode is defined.
OCI_DEFAULT - default mode
OCI_EOF_FETCH - indicates that it is the last fetch from the result set. 
If nrows is non-zero, setting this mode effectively cancels fetching after 
retrieving nrows, otherwise it cancels fetching immediately. 
Related Functions
OCIAttrGet()





OCIStmtGetBindInfo()
Name
OCI Get Bind Parameters
Purpose
Gets the bind and indicator variable names.
Syntax
sword OCIStmtGetBindInfo ( OCIStmt      *stmtp,
                     OCIError     *errhp,
                     ub4          size,
                     ub4          startloc,
                     sb4          *found,
                     OraText         *bvnp[],
                     ub1          bvnl[],
                     OraText         *invp[],
                     ub1          inpl[],
                     ub1          dupl[],
                     OCIBind      *hndl[] );

Comments
Gets the bind and indicator variable names. It returns the information for all 
the bind variables (even the duplicate ones) and sets the found parameter to 
the total number of bind variables and not just the number of distinct bind 
variables.
The statement must have been prepared with a call to OCIStmtPrepare() prior 
to this call. 
This call is processed locally.
Parameters
stmtp (IN) - the statement handle.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
size (IN) - the number of elements in each array.
startloc (IN) - position of the  bind variable at which to start getting  bind 
information.
found (IN) - abs(found) gives the total number of bind variables in the 
statement irrespective of the start position. Positive value if the number of 
bind variables returned is less than the size provided, otherwise negative.
bvnp (OUT) - array of pointers to hold bind variable names.
bvnl (OUT) - array to hold the length of the each bvnp element.
invp (OUT) - array of pointers to hold indicator variable names.
inpl (OUT) - array of pointers to hold the length of the each invp element.
dupl (OUT) - an array whose element value is 0 or 1 depending on whether the 
bind position is duplicate of another.
hndl  (OUT) - an array which returns the bind handle if binds have been done 
for the bind position. No handle is returned for duplicates.
Related Functions
OCIStmtPrepare()



OCIStmtGetPieceInfo()
Name
OCI Get Piece Information
Purpose
Returns piece information for a piecewise operation.
Syntax
sword OCIStmtGetPieceInfo( CONST OCIStmt  *stmtp,
                         OCIError       *errhp,
                         dvoid          **hndlpp,
                         ub4            *typep,
                         ub1            *in_outp,
                         ub4            *iterp, 
                         ub4            *idxp,
                         ub1            *piecep );

Comments
When an execute/fetch call returns OCI_NEED_DATA to get/return a 
dynamic bind/define value or piece, OCIStmtGetPieceInfo() returns the 
relevant information: bind/define handle, iteration or index number and 
which piece.
See the section "Runtime Data Allocation and Piecewise Operations" on page 
5-16 for more information about using OCIStmtGetPieceInfo().
Parameters
stmtp (IN) - the statement when executed returned OCI_NEED_DATA. 
errhp (OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
hndlpp (OUT) - returns a pointer to the bind or define handle of the bind or 
define whose runtime data is required or is being provided.
typep (OUT) - the type of the handle pointed to by hndlpp: OCI_HTYPE_BIND 
(for a bind handle) or OCI_HTYPE_DEFINE (for a define handle).
in_outp (OUT) - returns OCI_PARAM_IN if the data is required for an IN bind 
value. Returns OCI_PARAM_OUT if the data is available as an OUT bind 
variable or a define position value.
iterp (OUT) - returns the row number of a multiple row operation.
idxp (OUT) - the index of an array element of a PL/SQL array bind operation.
piecep (OUT) - returns one of the following defined values - 
OCI_ONE_PIECE, OCI_FIRST_PIECE, OCI_NEXT_PIECE and 
OCI_LAST_PIECE. The default value is always OCI_ONE_PIECE. 
Related Functions
OCIAttrGet(), OCIAttrGet(), OCIStmtExecute(), OCIStmtFetch(), 
OCIStmtSetPieceInfo()




OCIStmtPrepare()
Name
OCI Statement REQuest
Purpose
This call defines the SQL/PLSQL statement to be executed.
Syntax
sword OCIStmtPrepare ( OCIStmt      *stmtp,
                     OCIError     *errhp,
                     CONST OraText   *stmt, 
                     ub4          stmt_len,
                     ub4          language,
                     ub4          mode);
Comments
This call is used to prepare a SQL or PL/SQL statement for execution. The 
OCIStmtPrepare() call defines an application request. 
This is a purely local call. Data values for this statement initialized in 
subsequent bind calls will be stored in a bind handle which will hang off this 
statement handle.
This call does not create an association between this statement handle and any 
particular server.
See the section "Preparing Statements" on page 2-21 for more information 
about using this call.
Parameters
stmtp (IN) - a statement handle.
errhp (IN) - an error handle to retrieve diagnostic information.
stmt (IN) - SQL or PL/SQL statement to be executed. Must be a null-
terminated string. The pointer to the OraText of the statement must be available 
as long as the statement is executed.
stmt_len (IN) - length of the statement. Must not be zero.
language (IN) - V7, V8, or native syntax. Possible values are:
OCI_V7_SYNTAX - V7 ORACLE parsing syntax
OCI_V8_SYNTAX - V8 ORACLE parsing syntax
OCI_NTV_SYNTAX - syntax depending upon the version of the server. 
mode (IN) - the only defined mode is OCI_DEFAULT for default mode. 
Example
This example demonstrates the use of OCIStmtPrepare(), as well as the OCI 
application initialization calls.
Related Functions
OCIAttrGet(), OCIStmtExecute()


OCIStmtSetPieceInfo()
Name
OCI Set Piece Information
Purpose
Sets piece information for a piecewise operation.
Syntax
sword OCIStmtSetPieceInfo ( dvoid             *hndlp,
                          ub4               type,
                          OCIError          *errhp,
                          CONST dvoid       *bufp,
                          ub4               *alenp, 
                          ub1               piece,
                          CONST dvoid       *indp, 
                          ub2               *rcodep ); 
Comments
When an execute call returns OCI_NEED_DATA to get a dynamic IN/OUT 
bind value or piece, OCIStmtSetPieceInfo() sets the piece information: the 
buffer, the length, the indicator and which piece is currently being processed.
For more information about using OCIStmtSetPieceInfo() see the section 
"Runtime Data Allocation and Piecewise Operations" on page 5-16.
Parameters
hndlp (IN/OUT) - the bind/define handle.
type (IN) - type of the handle. 
errhp (OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
bufp (IN/OUT) - bufp is a pointer to a storage containing the data value or 
the piece when it is an IN bind variable, otherwise bufp is a pointer to 
storage for getting a piece or a value for OUT binds and define variables. For
named data types or REFs, a pointer to the object or REF is returned.
alenp (IN/OUT) - the length of the piece or the value. 
piece (IN) - the piece parameter. The following are valid values: 
OCI_ONE_PIECE, OCI_FIRST_PIECE, OCI_NEXT_PIECE, or 
OCI_LAST_PIECE. 
The default value is OCI_ONE_PIECE. This parameter is used for IN bind 
variables only.
indp (IN/OUT) - indicator. A pointer to a sb2 value or pointer to an indicator 
structure for named data types (SQLT_NTY) and REFs (SQLT_REF), i.e., *indp 
is either an sb2 or a dvoid * depending upon the data type.
rcodep (IN/OUT) - return code. 
Related Functions
OCIAttrGet(), OCIAttrGet(), OCIStmtExecute(), OCIStmtFetch(), 
OCIStmtGetPieceInfo()


OCIFormatInit
Name
OCIFormat Package Initialize
Purpose
Initializes the OCIFormat package.
Syntax
sword OCIFormatInit(dvoid *hndl, OCIError *err);
Comments
This routine must be called before calling any other OCIFormat routine.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - OCI environment or session handle
err (IN/OUT) - OCI error handle
Related Functions
OCIFormatTerm()


OCIFormatString
Name
OCIFormat Package Format String
Purpose
Writes a text string into the supplied text buffer using the argument
list submitted to it and in accordance with the format string given.
Syntax
sword OCIFormatString(dvoid *hndl, OCIError *err, OraText *buffer,
                      sbig_ora bufferLength, sbig_ora *returnLength,
                      CONST OraText *formatString, ...);
Comments
The first call to this routine must be preceded by a call to the
OCIFormatInit routine that initializes the OCIFormat package
for use.  When this routine is no longer needed then terminate
the OCIFormat package by a call to the OCIFormatTerm routine.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl         (IN/OUT) - OCI environment or session handle
err          (IN/OUT) - OCI error handle
buffer       (OUT)    - text buffer for the string
bufferLength (IN)     - length of the text buffer
returnLength (OUT)    - length of the formatted string
formatString (IN)     - format specification string
...          (IN)     - variable argument list
Related Functions


OCIFormatTerm 
Name
OCIFormat Package Terminate
Purpose
Terminates the OCIFormat package.
Syntax
sword OCIFormatTerm(dvoid *hndl, OCIError *err);
Comments
It must be called after the OCIFormat package is no longer being used.
Returns OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR
Parameters
hndl (IN/OUT) - OCI environment or session handle
err (IN/OUT) - OCI error handle
Related Functions
OCIFormatInit()


OCIFormatTUb1
Name
OCIFormat Package ub1 Type
Purpose
Return the type value for the ub1 type.
Syntax
sword OCIFormatTUb1(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTUb2
Name
OCIFormat Package ub2 Type
Purpose
Return the type value for the ub2 type.
Syntax
sword OCIFormatTUb2(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTUb4
Name
OCIFormat Package ub4 Type
Purpose
Return the type value for the ub4 type.
Syntax
sword OCIFormatTUb4(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTUword
Name
OCIFormat Package uword Type
Purpose
Return the type value for the uword type.
Syntax
sword OCIFormatTUword(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTUbig_ora
Name
OCIFormat Package ubig_ora Type
Purpose
Return the type value for the ubig_ora type.
Syntax
sword OCIFormatTUbig_ora(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTSb1
Name
OCIFormat Package sb1 Type
Purpose
Return the type value for the sb1 type.
Syntax
sword OCIFormatTSb1(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTSb2
Name
OCIFormat Package sb2 Type
Purpose
Return the type value for the sb2 type.
Syntax
sword OCIFormatTSb2(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTSb4
Name
OCIFormat Package sb4 Type
Purpose
Return the type value for the sb4 type.
Syntax
sword OCIFormatTSb4(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTSword
Name
OCIFormat Package sword Type
Purpose
Return the type value for the sword type.
Syntax
sword OCIFormatTSword(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTSbig_ora
Name
OCIFormat Package sbig_ora Type
Purpose
Return the type value for the sbig_ora type.
Syntax
sword OCIFormatTSbig_ora(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTEb1
Name
OCIFormat Package eb1 Type
Purpose
Return the type value for the eb1 type.
Syntax
sword OCIFormatTEb1(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTEb2
Name
OCIFormat Package eb2 Type
Purpose
Return the type value for the eb2 type.
Syntax
sword OCIFormatTEb2(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTEb4
Name
OCIFormat Package eb4 Type
Purpose
Return the type value for the eb4 type.
Syntax
sword OCIFormatTEb4(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTEword
Name
OCIFormat Package eword Type
Purpose
Return the type value for the eword type.
Syntax
sword OCIFormatTEword(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTChar
Name
OCIFormat Package text Type
Purpose
Return the type value for the text type.
Syntax
sword OCIFormatTChar(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTText
Name
OCIFormat Package *text Type
Purpose
Return the type value for the *text type.
Syntax
sword OCIFormatTText(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTDouble
Name
OCIFormat Package double Type
Purpose
Return the type value for the double type.
Syntax
sword OCIFormatTDouble(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatDvoid
Name
OCIFormat Package dvoid Type
Purpose
Return the type value for the dvoid type.
Syntax
sword OCIFormatTDvoid(void);
Comments
None
Parameters
None
Related Functions
None


OCIFormatTEnd
Name
OCIFormat Package end Type
Purpose
Return the list terminator's "type".
Syntax
sword OCIFormatTEnd(void);
Comments
None
Parameters
None
Related Functions
None


OCISvcCtxToLda()
Name
OCI toggle SerVice context handle to Version 7 Lda_Def
Purpose
Toggles between a V8 service context handle and a V7 Lda_Def.
Syntax
sword OCISvcCtxToLda ( OCISvcCtx    *srvhp,
                     OCIError     *errhp,
                     Lda_Def      *ldap );
Comments
Toggles between an Oracle8 service context handle and an Oracle7 Lda_Def.
This function can only be called after a service context has been properly 
initialized.
Once the service context has been translated to an Lda_Def, it can be used in 
release 7.x OCI calls (e.g., obindps(), ofen()).
Note: If there are multiple service contexts which share the same server 
handle, only one can be in V7 mode at any time.
The action of this call can be reversed by passing the resulting Lda_Def to 
the OCILdaToSvcCtx() function.
Parameters
svchp (IN/OUT) - the service context handle. 
errhp (IN/OUT) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error. 
ldap (IN/OUT) - a Logon Data Area for V7-style OCI calls which is initialized 
by this call. 
Related Functions
OCILdaToSvcCtx()




OCITransCommit()
Name
OCI TX (transaction) CoMmit
Purpose
Commits the transaction associated with a specified service context.
Syntax
sword OCITransCommit ( OCISvcCtx    *srvcp,
                     OCIError     *errhp,
                     ub4          flags );
Comments
The transaction currently associated with the service context is committed. If 
it is a distributed transaction that the server cannot commit, this call 
additionally retrieves the state of the transaction from the database to be 
returned to the user in the error handle.
If the application has defined multiple transactions, this function operates 
on the transaction currently associated with the service context. If the 
application is working with only the implicit local transaction created when 
database changes are made, that implicit transaction is committed.
If the application is running in the object mode, then the modified or updated 
objects in the object cache for this transaction are also committed.
The flags parameter is used for one-phase commit optimization in distributed 
transactions. If the transaction is non-distributed, the flags parameter is 
ignored, and OCI_DEFAULT can be passed as its value. OCI applications 
managing global transactions should pass a value of 
OCI_TRANS_TWOPHASE to the flags parameter for a two-phase commit. The 
default is one-phase commit.
Under normal circumstances, OCITransCommit() returns with a status 
indicating that the transaction has either been committed or rolled back. With 
distributed transactions, it is possible that the transaction is now in-doubt 
(i.e., neither committed nor aborted). In this case, OCITransCommit() 
attempts to retrieve the status of the transaction from the server. 
The status is returned.
Parameters
srvcp (IN) - the service context handle.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
flags -see the "Comments" section above.
Related Functions
OCITransRollback()




OCITransDetach()
Name
OCI TX (transaction) DeTach
Purpose
Detaches a transaction.
Syntax
sword OCITransDetach ( OCISvcCtx    *srvcp,
                     OCIError     *errhp,
                     ub4          flags);
Comments
Detaches a global transaction from the service context handle. The transaction 
currently attached to the service context handle becomes inactive at the end 
of this call. The transaction may be resumed later by calling OCITransStart(), 
specifying  a flags value of OCI_TRANS_RESUME.
When a transaction is detached, the value which was specified in the timeout 
parameter of OCITransStart() when the transaction was started is used to 
determine the amount of time the branch can remain inactive before being 
deleted by the server's PMON process.
Note: The transaction can be resumed by a different process than the one 
that detached it, provided that the transaction has the same 
authorization.
Parameters
srvcp (IN) - the service context handle. 
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
flags (IN) - you must pass a value of OCI_DEFAULT for this parameter.
Related Functions
OCITransStart()



OCITransForget()
Name
OCI TX (transaction) ForGeT
Purpose
Causes the server to forget a heuristically completed global transaction.
Syntax
sword OCITransForget ( OCISvcCtx     *svchp, 
                     OCIError      *errhp,
                     ub4           flags);

Comments

Forgets a heuristically completed global transaction. The server deletes the 
status of the transaction from the system's pending transaction table.
The XID of the transaction to be forgotten is set as an attribute of the 
transaction handle (OCI_ATTR_XID).
Parameters
srvcp (IN) - the service context handle - the transaction is rolled back.
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
flags (IN) - you must pass OCI_DEFAULT for this parameter.
Related Functions
OCITransCommit(), OCITransRollback()


OCITransMultiPrepare() 
Name
OCI Trans(action) Multi-Branch Prepare
Purpose
Prepares a transaction with multiple branches in a single call.
Syntax
sword OCITransMultiPrepare ( OCISvcCtx    *svchp,
                             ub4           numBranches,
                             OCITrans     **txns,
                             OCIError     **errhp);

Comments

Prepares the specified global transaction for commit.
This call is valid only for distributed transactions.
This call is an advanced performance feature intended for use only in
situations where the caller is responsible for preparing all the branches
in a transaction. 
Parameters
srvcp (IN) - the service context handle. 
numBranches (IN) - This is the number of branches expected. It is also the
array size for the next two parameters.
txns (IN) - This is the array of transaction handles for the branches to
prepare. They should all have the OCI_ATTR_XID set. The global transaction
ID should be the same.
errhp (IN) - This is the array of error handles. If OCI_SUCCESS is not
returned, then these will indicate which branches received which errors.
Related Functions
OCITransPrepare()


OCITransPrepare()
Name
OCI TX (transaction) PREpare
Purpose
Prepares a transaction for commit.
Syntax
sword OCITransPrepare ( OCISvcCtx    *svchp, 
                      OCIError     *errhp,
                      ub4          flags);

Comments

Prepares the specified global transaction for commit.
This call is valid only for distributed transactions.
The call returns OCI_SUCCESS_WITH_INFO if the transaction has not made 
any changes. The error handle will indicate that the transaction is read-only. 
The flag parameter is not currently used. 
Parameters
srvcp (IN) - the service context handle. 
errhp (IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
flags (IN) - you must pass OCI_DEFAULT for this parameter.
Related Functions
OCITransCommit(), OCITransForget()




OCITransRollback()
Name
OCI TX (transaction) RoLlback
Purpose
Rolls back the current transaction.
Syntax
sword OCITransRollback ( dvoid        *svchp, 
                       OCIError     *errhp,
                       ub4          flags );
Comments
The current transaction- defined as the set of statements executed since the 
last OCITransCommit() or since OCISessionBegin()-is rolled back.
If the application is running under object mode then the modified or updated 
objects in the object cache for this transaction are also rolled back.
An error is returned if an attempt is made to roll back a global transaction 
that is not currently active.
Parameters
svchp (IN) - a service context handle. The transaction currently set in the 
service context handle is rolled back.
errhp -(IN) - an error handle which can be passed to OCIErrorGet() for 
diagnostic information in the event of an error.
flags - you must pass a value of OCI_DEFAULT for this parameter.
Related Functions
OCITransCommit()




OCITransStart()
Name
OCI TX (transaction) STart
Purpose
Sets the beginning of a transaction.
Syntax
sword OCITransStart ( OCISvcCtx    *svchp, 
                    OCIError     *errhp, 
                    uword        timeout,
                    ub4          flags);

Comments
This function sets the beginning of a global or serializable transaction. The 
transaction context currently associated with the service context handle is 
initialized at the end of the call if the flags parameter specifies that a new 
transaction should be started.
The XID of the transaction is set as an attribute of the transaction handle 
(OCI_ATTR_XID)
Parameters
svchp (IN/OUT) - the service context handle. The transaction context in the 
service context handle is initialized at the end of the call if the flag 
specified a new transaction to be started.
errhp (IN/OUT) - The OCI error handle. If there is an error, it is recorded in 
err and this function returns OCI_ERROR. Diagnostic information can be 
obtained by calling OCIErrorGet().
timeout (IN) - the time, in seconds, to wait for a transaction to become 
available for resumption when OCI_TRANS_RESUME is specified. When 
OCI_TRANS_NEW is specified, this value is stored and may be used later by 
OCITransDetach().
flags (IN) - specifies whether a new transaction is being started or an 
existing transaction is being resumed. Also specifies serializiability or 
read-only status. More than a single value can be specified. By default, 
a read/write transaction is started. The flag values are:
OCI_TRANS_NEW - starts a new transaction branch. By default starts a 
tightly coupled and migratable branch.
OCI_TRANS_TIGHT - explicitly specifies a tightly coupled branch
OCI_TRANS_LOOSE - specifies a loosely coupled branch
OCI_TRANS_RESUME - resumes an existing transaction branch. 
OCI_TRANS_READONLY - start a readonly transaction
OCI_TRANS_SERIALIZABLE - start a serializable transaction
Related Functions
OCITransDetach()





******************************************************************************/
/*-----------------------Dynamic Callback Function Pointers------------------*/
 
 
typedef sb4 (*OCICallbackInBind)(dvoid *ictxp, OCIBind *bindp, ub4 iter,
                                  ub4 index, dvoid **bufpp, ub4 *alenp,
                                  ub1 *piecep, dvoid **indp);
 
typedef sb4 (*OCICallbackOutBind)(dvoid *octxp, OCIBind *bindp, ub4 iter,
                                 ub4 index, dvoid **bufpp, ub4 **alenp,
                                 ub1 *piecep, dvoid **indp,
                                 ub2 **rcodep);
 
typedef sb4 (*OCICallbackDefine)(dvoid *octxp, OCIDefine *defnp, ub4 iter,
                                 dvoid **bufpp, ub4 **alenp, ub1 *piecep,
                                 dvoid **indp, ub2 **rcodep);

typedef sword (*OCIUserCallback)(dvoid *ctxp, dvoid *hndlp, ub4 type,
                                 ub4 fcode, ub4 when, sword returnCode,
                                 sb4 *errnop, va_list arglist);

typedef sword (*OCIEnvCallbackType)(OCIEnv *env, ub4 mode,
                                   size_t xtramem_sz, dvoid *usrmemp,
                                   OCIUcb *ucbDesc); 

typedef sb4 (*OCICallbackLobRead)(dvoid *ctxp, CONST dvoid *bufp,
                                             ub4 len, ub1 piece);

typedef sb4 (*OCICallbackLobWrite)(dvoid *ctxp, dvoid *bufp, 
                                          ub4 *lenp, ub1 *piece);

/*--------------------------Failover Callback Structure ---------------------*/
typedef sb4 (*OCICallbackFailover)(dvoid *svcctx, dvoid *envctx,
                                   dvoid *fo_ctx, ub4 fo_type,
                                   ub4 fo_event);

typedef struct
{
  OCICallbackFailover callback_function;
  dvoid *fo_ctx;
} 
OCIFocbkStruct;

/*****************************************************************************
                         ACTUAL PROTOTYPE DECLARATIONS
******************************************************************************/

sword   OCIInitialize   (ub4 mode, dvoid *ctxp, 
                 dvoid *(*malocfp)(dvoid *ctxp, size_t size),
                 dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
                 void   (*mfreefp)(dvoid *ctxp, dvoid *memptr) );

sword   OCITerminate( ub4 mode);

sword   OCIEnvCreate (OCIEnv **envp, ub4 mode, dvoid *ctxp,
                 dvoid *(*malocfp)(dvoid *ctxp, size_t size),
                 dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
                 void   (*mfreefp)(dvoid *ctxp, dvoid *memptr),
                 size_t xtramem_sz, dvoid **usrmempp);

sword   OCIFEnvCreate (OCIEnv **envp, ub4 mode, dvoid *ctxp,
                 dvoid *(*malocfp)(dvoid *ctxp, size_t size),
                 dvoid *(*ralocfp)(dvoid *ctxp, dvoid *memptr, size_t newsize),
                 void   (*mfreefp)(dvoid *ctxp, dvoid *memptr),
                 size_t xtramem_sz, dvoid **usrmempp, dvoid *fupg);

sword   OCIHandleAlloc(CONST dvoid *parenth, dvoid **hndlpp, CONST ub4 type, 
                       CONST size_t xtramem_sz, dvoid **usrmempp);

sword   OCIHandleFree(dvoid *hndlp, CONST ub4 type);


sword   OCIDescriptorAlloc(CONST dvoid *parenth, dvoid **descpp, 
                           CONST ub4 type, CONST size_t xtramem_sz, 
                           dvoid **usrmempp);

sword   OCIDescriptorFree(dvoid *descp, CONST ub4 type);

sword   OCIEnvInit (OCIEnv **envp, ub4 mode, 
                    size_t xtramem_sz, dvoid **usrmempp);

sword   OCIServerAttach  (OCIServer *srvhp, OCIError *errhp,
                          CONST OraText *dblink, sb4 dblink_len, ub4 mode);

sword   OCIServerDetach  (OCIServer *srvhp, OCIError *errhp, ub4 mode);

sword   OCISessionBegin  (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp,
                          ub4 credt, ub4 mode);

sword   OCISessionEnd   (OCISvcCtx *svchp, OCIError *errhp, OCISession *usrhp, 
                         ub4 mode);

sword   OCILogon (OCIEnv *envhp, OCIError *errhp, OCISvcCtx **svchp, 
		  CONST OraText *username, ub4 uname_len, 
		  CONST OraText *password, ub4 passwd_len, 
		  CONST OraText *dbname, ub4 dbname_len);

sword   OCILogoff (OCISvcCtx *svchp, OCIError *errhp);


sword   OCIPasswordChange   (OCISvcCtx *svchp, OCIError *errhp, 
                             CONST OraText *user_name, ub4 usernm_len, 
                             CONST OraText *opasswd, ub4 opasswd_len, 
                             CONST OraText *npasswd, ub4 npasswd_len, ub4 mode);

sword   OCIStmtPrepare   (OCIStmt *stmtp, OCIError *errhp, CONST OraText *stmt,
                          ub4 stmt_len, ub4 language, ub4 mode);

sword   OCIBindByPos  (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
		       ub4 position, dvoid *valuep, sb4 value_sz,
		       ub2 dty, dvoid *indp, ub2 *alenp, ub2 *rcodep,
		       ub4 maxarr_len, ub4 *curelep, ub4 mode);

sword   OCIBindByName   (OCIStmt *stmtp, OCIBind **bindp, OCIError *errhp,
			 CONST OraText *placeholder, sb4 placeh_len, 
                         dvoid *valuep, sb4 value_sz, ub2 dty, 
                         dvoid *indp, ub2 *alenp, ub2 *rcodep, 
                         ub4 maxarr_len, ub4 *curelep, ub4 mode);

sword   OCIBindObject  (OCIBind *bindp, OCIError *errhp, CONST OCIType *type, 
			dvoid **pgvpp, ub4 *pvszsp, dvoid **indpp, 
			ub4 *indszp);

sword   OCIBindDynamic   (OCIBind *bindp, OCIError *errhp, dvoid *ictxp,
			  OCICallbackInBind icbfp, dvoid *octxp,
			  OCICallbackOutBind ocbfp);

sword   OCIBindArrayOfStruct   (OCIBind *bindp, OCIError *errhp, 
                                ub4 pvskip, ub4 indskip,
                                ub4 alskip, ub4 rcskip);

sword   OCIStmtGetPieceInfo   (OCIStmt *stmtp, OCIError *errhp, 
                               dvoid **hndlpp, ub4 *typep,
                               ub1 *in_outp, ub4 *iterp, ub4 *idxp, 
                               ub1 *piecep);

sword   OCIStmtSetPieceInfo   (dvoid *hndlp, ub4 type, OCIError *errhp, 
                               CONST dvoid *bufp, ub4 *alenp, ub1 piece, 
                               CONST dvoid *indp, ub2 *rcodep);

sword   OCIStmtExecute  (OCISvcCtx *svchp, OCIStmt *stmtp, OCIError *errhp, 
                         ub4 iters, ub4 rowoff, CONST OCISnapshot *snap_in, 
                         OCISnapshot *snap_out, ub4 mode);

sword   OCIDefineByPos  (OCIStmt *stmtp, OCIDefine **defnp, OCIError *errhp,
			 ub4 position, dvoid *valuep, sb4 value_sz, ub2 dty,
			 dvoid *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode);

sword   OCIDefineObject  (OCIDefine *defnp, OCIError *errhp, 
                          CONST OCIType *type, dvoid **pgvpp, 
                          ub4 *pvszsp, dvoid **indpp, ub4 *indszp);

sword   OCIDefineDynamic   (OCIDefine *defnp, OCIError *errhp, dvoid *octxp,
                            OCICallbackDefine ocbfp);

sword   OCIDefineArrayOfStruct  (OCIDefine *defnp, OCIError *errhp, ub4 pvskip,
                                 ub4 indskip, ub4 rlskip, ub4 rcskip);

sword   OCIStmtFetch   (OCIStmt *stmtp, OCIError *errhp, ub4 nrows, 
                        ub2 orientation, ub4 mode);

sword   OCIStmtGetBindInfo   (OCIStmt *stmtp, OCIError *errhp, ub4 size, 
                              ub4 startloc,
                              sb4 *found, OraText *bvnp[], ub1 bvnl[],
                              OraText *invp[], ub1 inpl[], ub1 dupl[],
                              OCIBind *hndl[]);

sword   OCIDescribeAny  (OCISvcCtx *svchp, OCIError *errhp, 
                         dvoid *objptr, 
                         ub4 objnm_len, ub1 objptr_typ, ub1 info_level,
			 ub1 objtyp, OCIDescribe *dschp);

sword   OCIParamGet (CONST dvoid *hndlp, ub4 htype, OCIError *errhp, 
                     dvoid **parmdpp, ub4 pos);

sword   OCIParamSet(dvoid *hdlp, ub4 htyp, OCIError *errhp, CONST dvoid *dscp,
                    ub4 dtyp, ub4 pos);

sword   OCITransStart  (OCISvcCtx *svchp, OCIError *errhp, 
                        uword timeout, ub4 flags );

sword   OCITransDetach  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags );

sword   OCITransCommit  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

sword   OCITransRollback  (OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

sword   OCITransPrepare (OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

sword   OCITransMultiPrepare (OCISvcCtx *svchp, ub4 numBranches, 
                              OCITrans **txns, OCIError **errhp);

sword   OCITransForget (OCISvcCtx *svchp, OCIError *errhp, ub4 flags);

sword   OCIErrorGet   (dvoid *hndlp, ub4 recordno, OraText *sqlstate,
                       sb4 *errcodep, OraText *bufp, ub4 bufsiz, ub4 type);

sword   OCILobAppend  (OCISvcCtx *svchp, OCIError *errhp, 
                       OCILobLocator *dst_locp,
                       OCILobLocator *src_locp);

sword   OCILobAssign (OCIEnv *envhp, OCIError *errhp, 
                      CONST OCILobLocator *src_locp, 
                      OCILobLocator **dst_locpp);

sword   OCILobCharSetForm (OCIEnv *envhp, OCIError *errhp, 
                           CONST OCILobLocator *locp, ub1 *csfrm);

sword   OCILobCharSetId (OCIEnv *envhp, OCIError *errhp, 
                         CONST OCILobLocator *locp, ub2 *csid);

sword   OCILobCopy (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *dst_locp,
                    OCILobLocator *src_locp, ub4 amount, ub4 dst_offset, 
                    ub4 src_offset);

sword OCILobCreateTemporary(OCISvcCtx          *svchp,
                            OCIError           *errhp,
                            OCILobLocator      *locp,
                            ub2                 csid,
                            ub1                 csfrm,
                            ub1                 lobtype,
                            boolean             cache,
                            OCIDuration         duration);


sword OCILobClose( OCISvcCtx        *svchp,
                   OCIError         *errhp,
                   OCILobLocator    *locp );


sword   OCILobDisableBuffering (OCISvcCtx      *svchp,
                                OCIError       *errhp,
                                OCILobLocator  *locp);

sword   OCILobEnableBuffering (OCISvcCtx      *svchp,
                               OCIError       *errhp,
                               OCILobLocator  *locp);

sword   OCILobErase (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                      ub4 *amount, ub4 offset);

sword   OCILobFileClose (OCISvcCtx *svchp, OCIError *errhp, 
                         OCILobLocator *filep);

sword   OCILobFileCloseAll (OCISvcCtx *svchp, OCIError *errhp);

sword   OCILobFileExists (OCISvcCtx *svchp, OCIError *errhp, 
			  OCILobLocator *filep,
			  boolean *flag);

sword   OCILobFileGetName (OCIEnv *envhp, OCIError *errhp, 
                           CONST OCILobLocator *filep, 
                           OraText *dir_alias, ub2 *d_length, 
                           OraText *filename, ub2 *f_length);

sword   OCILobFileIsOpen (OCISvcCtx *svchp, OCIError *errhp, 
                          OCILobLocator *filep,
                          boolean *flag);

sword   OCILobFileOpen (OCISvcCtx *svchp, OCIError *errhp, 
                        OCILobLocator *filep,
                        ub1 mode);

sword   OCILobFileSetName (OCIEnv *envhp, OCIError *errhp, 
                           OCILobLocator **filepp, 
                           CONST OraText *dir_alias, ub2 d_length, 
                           CONST OraText *filename, ub2 f_length);

sword   OCILobFlushBuffer (OCISvcCtx       *svchp,
                           OCIError        *errhp,
                           OCILobLocator   *locp,
                           ub4              flag);

sword OCILobFreeTemporary(OCISvcCtx          *svchp,
                          OCIError           *errhp,
                          OCILobLocator      *locp);

sword OCILobGetChunkSize(OCISvcCtx         *svchp,
                         OCIError          *errhp,
                         OCILobLocator     *locp,
                         ub4               *chunksizep);

sword   OCILobGetLength  (OCISvcCtx *svchp, OCIError *errhp, 
                          OCILobLocator *locp,
                          ub4 *lenp);

sword   OCILobIsEqual  (OCIEnv *envhp, CONST OCILobLocator *x, 
                        CONST OCILobLocator *y, 
                        boolean *is_equal);

sword OCILobIsOpen( OCISvcCtx     *svchp,
                    OCIError      *errhp,
                    OCILobLocator *locp,
                    boolean       *flag);

sword OCILobIsTemporary(OCIEnv            *envp,
                        OCIError          *errhp,
                        OCILobLocator     *locp,
                        boolean           *is_temporary);

sword   OCILobLoadFromFile (OCISvcCtx *svchp, OCIError *errhp, 
                            OCILobLocator *dst_locp,
       	                    OCILobLocator *src_filep, 
                            ub4 amount, ub4 dst_offset, 
                            ub4 src_offset);

sword   OCILobLocatorAssign  (OCISvcCtx *svchp, OCIError *errhp, 
                            CONST OCILobLocator *src_locp, 
                            OCILobLocator **dst_locpp);


sword   OCILobLocatorIsInit (OCIEnv *envhp, OCIError *errhp, 
                             CONST OCILobLocator *locp, 
                             boolean *is_initialized);

sword   OCILobOpen( OCISvcCtx        *svchp,
                   OCIError         *errhp,
                   OCILobLocator    *locp,
                   ub1               mode );
 
sword   OCILobRead  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                     ub4 *amtp, ub4 offset, dvoid *bufp, ub4 bufl, 
                     dvoid *ctxp, sb4 (*cbfp)(dvoid *ctxp, 
                                              CONST dvoid *bufp, 
                                              ub4 len, 
                                              ub1 piece),
                     ub2 csid, ub1 csfrm);

sword   OCILobTrim  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                     ub4 newlen);

sword   OCILobWrite  (OCISvcCtx *svchp, OCIError *errhp, OCILobLocator *locp,
                      ub4 *amtp, ub4 offset, dvoid *bufp, ub4 buflen, 
                      ub1 piece, dvoid *ctxp, 
                      sb4 (*cbfp)(dvoid *ctxp, 
                                  dvoid *bufp, 
                                  ub4 *len, 
                                  ub1 *piece),
                      ub2 csid, ub1 csfrm);

sword OCILobWriteAppend(OCISvcCtx *svchp, OCIError *errhp, 
                        OCILobLocator *lobp, ub4 *amtp, 
                        dvoid *bufp, ub4 bufl, ub1 piece, dvoid *ctxp,
                       sb4 (*cbfp)(dvoid *ctxp, dvoid *bufp, ub4 *len, 
                       ub1 *piece), 
                      ub2 csid, ub1 csfrm);

sword   OCIBreak (dvoid *hndlp, OCIError *errhp);

sword   OCIReset (dvoid *hndlp, OCIError *errhp);

sword   OCIServerVersion  (dvoid *hndlp, OCIError *errhp, OraText *bufp, 
                           ub4 bufsz,
                           ub1 hndltype);


sword   OCIAttrGet (CONST dvoid *trgthndlp, ub4 trghndltyp, 
                    dvoid *attributep, ub4 *sizep, ub4 attrtype, 
                    OCIError *errhp);

sword   OCIAttrSet (dvoid *trgthndlp, ub4 trghndltyp, dvoid *attributep,
                    ub4 size, ub4 attrtype, OCIError *errhp);

sword   OCISvcCtxToLda (OCISvcCtx *svchp, OCIError *errhp, Lda_Def *ldap);

sword   OCILdaToSvcCtx (OCISvcCtx **svchpp, OCIError *errhp, Lda_Def *ldap);

sword   OCIResultSetToStmt (OCIResult *rsetdp, OCIError *errhp);

sword OCIFileClose ( dvoid  *hndl, OCIError *err, OCIFileObject *filep );

sword	OCIUserCallbackRegister(dvoid *hndlp, ub4 type, dvoid *ehndlp,
                                    OCIUserCallback callback, dvoid *ctxp,
                                    ub4 fcode, ub4 when, OCIUcb *ucbDesc);

sword	OCIUserCallbackGet(dvoid *hndlp, ub4 type, dvoid *ehndlp,
                               ub4 fcode, ub4 when, OCIUserCallback *callbackp,
                               dvoid **ctxpp, OCIUcb *ucbDesc);

sword   OCISharedLibInit(dvoid *metaCtx, dvoid *libCtx, ub4 argfmt, sword argc,
                         dvoid *argv[], OCIEnvCallbackType envCallback);

sword OCIFileExists ( dvoid  *hndl, OCIError *err, OraText *filename,
                     OraText *path, ub1 *flag  );

sword OCIFileFlush( dvoid *hndl, OCIError *err, OCIFileObject *filep  );


sword OCIFileGetLength( dvoid *hndl, OCIError *err, OraText *filename,
                        OraText *path, ubig_ora *lenp  );

sword OCIFileInit ( dvoid *hndl, OCIError *err );

sword OCIFileOpen ( dvoid *hndl, OCIError *err, OCIFileObject **filep,
                    OraText *filename, OraText *path, ub4 mode, ub4 create, 
                    ub4 type );

sword OCIFileRead ( dvoid *hndl, OCIError *err, OCIFileObject *filep,
                    dvoid *bufp, ub4 bufl, ub4 *bytesread );

sword OCIFileSeek ( dvoid *hndl, OCIError *err, OCIFileObject *filep,
                     uword origin, ubig_ora offset, sb1 dir );

sword OCIFileTerm ( dvoid *hndl, OCIError *err );


sword OCIFileWrite ( dvoid *hndl, OCIError *err, OCIFileObject   *filep,
                     dvoid *bufp, ub4 buflen, ub4 *byteswritten );


/*
 ** Initialize the security package
 */
sword   OCISecurityInitialize (OCISecurity *sechandle, OCIError *error_handle);

sword   OCISecurityTerminate (OCISecurity *sechandle, OCIError *error_handle);

sword OCISecurityOpenWallet(OCISecurity *osshandle,
                            OCIError *error_handle,
                            size_t wrllen,
                            OraText *wallet_resource_locator,
                            size_t pwdlen,
                            OraText *password,
                            nzttWallet *wallet);

sword OCISecurityCloseWallet(OCISecurity *osshandle,
                             OCIError *error_handle,
                             nzttWallet *wallet);

sword OCISecurityCreateWallet(OCISecurity *osshandle,
                              OCIError *error_handle,
                              size_t wrllen,
                              OraText *wallet_resource_locator,
                              size_t pwdlen,
                              OraText *password,
                              nzttWallet *wallet);

sword OCISecurityDestroyWallet(OCISecurity *osshandle,
                               OCIError *error_handle,
                               size_t wrllen,
                               OraText *wallet_resource_locator,
                               size_t pwdlen,
                               OraText *password);

sword OCISecurityStorePersona(OCISecurity *osshandle,
                              OCIError *error_handle,
                              nzttPersona **persona,
                              nzttWallet *wallet);

sword OCISecurityOpenPersona(OCISecurity *osshandle,
                             OCIError *error_handle,
                             nzttPersona *persona);

sword OCISecurityClosePersona(OCISecurity *osshandle,
                              OCIError *error_handle,
                              nzttPersona *persona);

sword OCISecurityRemovePersona(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttPersona **persona);

sword OCISecurityCreatePersona(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttIdentType identity_type,
                               nzttCipherType cipher_type,
                               nzttPersonaDesc *desc,
                               nzttPersona **persona);

sword OCISecuritySetProtection(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttPersona *persona,
                               nzttcef crypto_engine_function,
                               nztttdufmt data_unit_format,
                               nzttProtInfo *protection_info);

sword OCISecurityGetProtection(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttPersona *persona,
                               nzttcef crypto_engine_function,
                               nztttdufmt * data_unit_format_ptr,
                               nzttProtInfo *protection_info);

sword OCISecurityRemoveIdentity(OCISecurity *osshandle,
                                OCIError *error_handle,
                                nzttIdentity **identity_ptr);

sword OCISecurityCreateIdentity(OCISecurity *osshandle,
                                OCIError *error_handle,
                                nzttIdentType type,
                                nzttIdentityDesc *desc,
                                nzttIdentity **identity_ptr);

sword OCISecurityAbortIdentity(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttIdentity **identity_ptr);

sword OCISecurityFreeIdentity(OCISecurity *osshandle,
			      OCIError *error_handle,
			      nzttIdentity **identity_ptr);


sword OCISecurityStoreTrustedIdentity(OCISecurity *osshandle,
                                      OCIError *error_handle,
                                      nzttIdentity **identity_ptr,
                                      nzttPersona *persona);

sword OCISecuritySign(OCISecurity *osshandle,
                      OCIError *error_handle,
                      nzttPersona *persona,
                      nzttces signature_state,
                      size_t input_length,
                      ub1 *input,
                      nzttBufferBlock *buffer_block);

sword OCISecuritySignExpansion(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttPersona *persona,
                               size_t inputlen,
                               size_t *signature_length);

sword OCISecurityVerify(OCISecurity *osshandle,
                        OCIError *error_handle,
                        nzttPersona *persona,
                        nzttces signature_state,
                        size_t siglen,
                        ub1 *signature,
                        nzttBufferBlock *extracted_message,
                        boolean *verified,
                        boolean *validated,
                        nzttIdentity **signing_party_identity);

sword OCISecurityValidate(OCISecurity *osshandle,
                          OCIError *error_handle,
                          nzttPersona *persona,
                          nzttIdentity *identity,
                          boolean *validated);

sword OCISecuritySignDetached(OCISecurity *osshandle,
                              OCIError *error_handle,
                              nzttPersona *persona,
                              nzttces signature_state,
                              size_t input_length,
                              ub1 * input,
                              nzttBufferBlock *signature);

sword OCISecuritySignDetExpansion(OCISecurity *osshandle,
                                  OCIError *error_handle,
                                  nzttPersona *persona,
                                  size_t input_length,
                                  size_t *required_buffer_length);

sword OCISecurityVerifyDetached(OCISecurity *osshandle,
                                OCIError *error_handle,
                                nzttPersona *persona,
                                nzttces signature_state,
                                size_t data_length,
                                ub1 *data,
                                size_t siglen,
                                ub1 *signature,
                                boolean *verified,
                                boolean *validated,
                                nzttIdentity **signing_party_identity);

sword OCISecurity_PKEncrypt(OCISecurity *osshandle,
                            OCIError *error_handle,
                            nzttPersona *persona,
                            size_t number_of_recipients,
                            nzttIdentity *recipient_list,
                            nzttces encryption_state,
                            size_t input_length,
                            ub1 *input,
                            nzttBufferBlock *encrypted_data);

sword OCISecurityPKEncryptExpansion(OCISecurity *osshandle,
                                    OCIError *error_handle,
                                    nzttPersona *persona,
                                    size_t number_recipients,
                                    size_t input_length,
                                    size_t *buffer_length_required);

sword OCISecurityPKDecrypt(OCISecurity *osshandle,
                           OCIError *error_handle,
                           nzttPersona *persona,
                           nzttces encryption_state,
                           size_t input_length,
                           ub1 *input,
                           nzttBufferBlock *encrypted_data);

sword OCISecurityEncrypt(OCISecurity *osshandle,
                         OCIError *error_handle,
                         nzttPersona *persona,
                         nzttces encryption_state,
                         size_t input_length,
                         ub1 *input,
                         nzttBufferBlock *encrypted_data);

sword OCISecurityEncryptExpansion(OCISecurity *osshandle,
                                  OCIError *error_handle,
                                  nzttPersona *persona,
                                  size_t input_length,
                                  size_t *encrypted_data_length);

sword OCISecurityDecrypt(OCISecurity *osshandle,
                         OCIError *error_handle,
                         nzttPersona *persona,
                         nzttces decryption_state,
                         size_t input_length,
                         ub1 *input,
                         nzttBufferBlock *decrypted_data);

sword OCISecurityEnvelope(OCISecurity *osshandle,
                          OCIError *error_handle,
                          nzttPersona *persona,
                          size_t number_of_recipients,
                          nzttIdentity *identity,
                          nzttces encryption_state,
                          size_t input_length,
                          ub1 *input,
                          nzttBufferBlock *enveloped_data);

sword OCISecurityDeEnvelope(OCISecurity *osshandle,
                            OCIError *error_handle,
                            nzttPersona *persona,
                            nzttces decryption_state,
                            size_t input_length,
                            ub1 *input,
                            nzttBufferBlock *output_message,
                            boolean *verified,
                            boolean *validated,
                            nzttIdentity **sender_identity);

sword OCISecurityKeyedHash(OCISecurity *osshandle,
                           OCIError *error_handle,
                           nzttPersona *persona,
                           nzttces hash_state,
                           size_t input_length,
                           ub1 *input,
                           nzttBufferBlock *keyed_hash);

sword OCISecurityKeyedHashExpansion(OCISecurity *osshandle,
                                    OCIError *error_handle,
                                    nzttPersona *persona,
                                    size_t input_length,
                                    size_t *required_buffer_length);

sword OCISecurityHash(OCISecurity *osshandle,
                      OCIError *error_handle,
                      nzttPersona *persona,
                      nzttces hash_state,
                      size_t input,
                      ub1 *input_length,
                      nzttBufferBlock *hash);

sword OCISecurityHashExpansion(OCISecurity *osshandle,
                               OCIError *error_handle,
                               nzttPersona *persona,
                               size_t input_length,
                               size_t *required_buffer_length);

sword OCISecuritySeedRandom(OCISecurity *osshandle,
                            OCIError *error_handle,
                            nzttPersona *persona,
                            size_t seed_length,
                            ub1 *seed);

sword OCISecurityRandomBytes(OCISecurity *osshandle,
                             OCIError *error_handle,
                             nzttPersona *persona,
                             size_t number_of_bytes_desired,
                             nzttBufferBlock *random_bytes);

sword OCISecurityRandomNumber(OCISecurity *osshandle,
                              OCIError *error_handle,
                              nzttPersona *persona,
                              uword *random_number_ptr);

sword OCISecurityInitBlock(OCISecurity *osshandle,
                           OCIError *error_handle,
                           nzttBufferBlock *buffer_block);

sword OCISecurityReuseBlock(OCISecurity *osshandle,
                            OCIError *error_handle,
                            nzttBufferBlock *buffer_block);

sword OCISecurityPurgeBlock(OCISecurity *osshandle,
                            OCIError *error_handle,
                            nzttBufferBlock *buffer_block);

sword OCISecuritySetBlock(OCISecurity *osshandle,
                          OCIError *error_handle,
                          uword flags_to_set,
                          size_t buffer_length,
                          size_t used_buffer_length,
                          ub1 *buffer,
                          nzttBufferBlock *buffer_block);

sword OCISecurityGetIdentity(OCISecurity   *osshandle,
                             OCIError      *error_handle,
                             size_t         namelen,
                             OraText          *distinguished_name,
                             nzttIdentity **identity);

sword OCIAQEnq(OCISvcCtx *svchp, OCIError *errhp, OraText *queue_name,
		 OCIAQEnqOptions *enqopt, OCIAQMsgProperties *msgprop,
		 OCIType *payload_tdo, dvoid **payload, dvoid **payload_ind, 
		 OCIRaw **msgid, ub4 flags); 

sword OCIAQDeq(OCISvcCtx *svchp, OCIError *errhp, OraText *queue_name,
		 OCIAQDeqOptions *deqopt, OCIAQMsgProperties *msgprop,
		 OCIType *payload_tdo, dvoid **payload, dvoid **payload_ind, 
		 OCIRaw **msgid, ub4 flags); 

sword OCIAQListen(/*_ OCISvcCtx *svchp, OCIError *errhp, 
		      OCIAQAgent **agent_list, ub4 num_agents,
		      sb4 wait, OCIAQAgent **agent,
		      ub4 flags _*/);

sword OCIExtractInit(dvoid *hndl, OCIError *err);

sword OCIExtractTerm(dvoid *hndl, OCIError *err);

sword OCIExtractReset(dvoid *hndl, OCIError *err);

sword OCIExtractSetNumKeys(dvoid *hndl, OCIError *err, uword numkeys);

sword OCIExtractSetKey(dvoid *hndl, OCIError *err, CONST OraText *name, 
                       ub1 type, ub4 flag, CONST dvoid *defval, 
                       CONST sb4 *intrange, CONST OraText *CONST *strlist);

sword OCIExtractFromFile(dvoid *hndl, OCIError *err, ub4 flag, 
                         OraText *filename);

sword OCIExtractFromStr(dvoid *hndl, OCIError *err, ub4 flag, OraText *input);

sword OCIExtractToInt(dvoid *hndl, OCIError *err, OraText *keyname, 
                      uword valno, sb4 *retval);

sword OCIExtractToBool(dvoid *hndl, OCIError *err, OraText *keyname, 
                       uword valno, ub1 *retval);

sword OCIExtractToStr(dvoid *hndl, OCIError *err, OraText *keyname, uword valno, 
                      OraText *retval, uword buflen);

sword OCIExtractToOCINum(dvoid *hndl, OCIError *err, OraText *keyname, 
                         uword valno, OCINumber *retval);

sword OCIExtractToList(dvoid *hndl, OCIError *err, uword *numkeys);

sword OCIExtractFromList(dvoid *hndl, OCIError *err, uword index, 
			 OraText **name, 
                         ub1 *type, uword *numvals, dvoid ***values);

/* Memory Related Service Interfaces */

sword OCIMemoryAlloc(dvoid *hdl, OCIError *err, dvoid **mem,
	OCIDuration dur, ub4 size, ub4 flags);

sword OCIMemoryResize(dvoid *hdl, OCIError *err, dvoid **mem,
			ub4 newsize, ub4 flags);
			
sword OCIMemoryFree(dvoid *hdl, OCIError *err, dvoid *mem);

sword OCIContextSetValue(dvoid *hdl, OCIError *err, OCIDuration duration,
			ub1 *key, ub1 keylen, dvoid *ctx_value);

sword OCIContextGetValue(dvoid *hdl, OCIError *err, ub1 *key,
			ub1 keylen, dvoid **ctx_value);

sword OCIContextClearValue(dvoid *hdl, OCIError *err, ub1 *key, 
			ub1 keylen);

sword OCIContextGenerateKey(dvoid *hdl, OCIError *err, ub4 *key);

sword OCIMemorySetCurrentIDs(dvoid *hdl, OCIError *err, 
	ub4 curr_session_id, ub4 curr_trans_id, ub4 curr_stmt_id);

sword OCIPicklerTdsCtxInit(OCIEnv *env, OCIError *err, 
                           OCIPicklerTdsCtx **tdsc);

sword OCIPicklerTdsCtxFree(OCIEnv *env, OCIError *err, OCIPicklerTdsCtx *tdsc);

sword OCIPicklerTdsInit(OCIEnv *env, OCIError *err, OCIPicklerTdsCtx *tdsc, 
					OCIPicklerTds **tdsh);

sword OCIPicklerTdsFree(OCIEnv *env, OCIError *err, OCIPicklerTds *tdsh);

sword OCIPicklerTdsCreateElementNumber(OCIEnv *env, OCIError *err, 
					OCIPicklerTds *tdsh, ub1 prec, 
					sb1 scale, OCIPicklerTdsElement *elt);

sword OCIPicklerTdsCreateElementChar(OCIEnv *env, OCIError *err, 
				     OCIPicklerTds *tdsh, ub2 len, 
				     OCIPicklerTdsElement *elt);

sword OCIPicklerTdsCreateElementVarchar(OCIEnv *env, OCIError *err, 
                                        OCIPicklerTds *tdsh, ub2 len, 
                                        OCIPicklerTdsElement *elt);

sword OCIPicklerTdsCreateElementRaw(OCIEnv *env, OCIError *err, 
                                    OCIPicklerTds *tdsh, ub2 len, 
                                    OCIPicklerTdsElement *elt);

sword OCIPicklerTdsCreateElement(OCIEnv *env, OCIError *err, 
                                 OCIPicklerTds *tdsh, OCITypeCode dty, 
                                 OCIPicklerTdsElement *elt);

sword OCIPicklerTdsAddAttr(OCIEnv *env, OCIError *err, 
                           OCIPicklerTds *tdsh, OCIPicklerTdsElement elt);

sword OCIPicklerTdsGenerate(OCIEnv *env, OCIError *err, 
				OCIPicklerTds *tdsh);

sword OCIPicklerTdsGetAttr(OCIEnv *env, OCIError *err, 
				CONST OCIPicklerTds *tdsh, ub1  attrno,
				OCITypeCode *typ, ub2  *len);

sword OCIPicklerFdoInit(OCIEnv *env, OCIError *err, 
				OCIPicklerFdo **fdoh);

sword OCIPicklerFdoFree(OCIEnv *env, OCIError *err, 
				OCIPicklerFdo *fdoh);

sword OCIPicklerImageInit(OCIEnv *env, OCIError *err, 
                          OCIPicklerFdo *fdoh, 
                          OCIPicklerTds *tdsh,
			  OCIPicklerImage **imgh);

sword OCIPicklerImageFree(OCIEnv *env, OCIError *err, 
				OCIPicklerImage *imgh);
 
sword OCIPicklerImageAddScalar(OCIEnv *env, OCIError *err,  
                               OCIPicklerImage *imgh, dvoid *scalar, ub4  len);
 
sword OCIPicklerImageAddNullScalar(OCIEnv *env, OCIError *err, 
				OCIPicklerImage *imgh);
 
sword OCIPicklerImageGenerate(OCIEnv *env, OCIError *err, 
				OCIPicklerImage *imgh);
 
sword OCIPicklerImageGetScalarSize(OCIEnv *env, OCIError *err, 
				OCIPicklerImage *imgh, 
				ub4  attrno, ub4  *size);
 
sword OCIPicklerImageGetScalar(OCIEnv *env, OCIError *err, 
				OCIPicklerImage *imgh, ub4  attrno, 
				dvoid *buf, ub4  *len, OCIInd *ind);

sword OCIPicklerImageCollBegin(OCIEnv *env, OCIError *err,
                OCIPicklerImage *imgh, CONST OCIPicklerTds *colltdsh);
 
sword OCIPicklerImageCollAddScalar( OCIEnv *env, OCIError *err,
                OCIPicklerImage *imgh, dvoid *scalar,
                ub4 buflen, OCIInd ind);
 
sword OCIPicklerImageCollEnd(OCIEnv *env, OCIError *err,
                OCIPicklerImage *imgh);
 
/* should take svcctx for locator stuff */
sword OCIPicklerImageCollBeginScan(OCIEnv *env, OCIError *err,
                OCIPicklerImage *imgh, CONST OCIPicklerTds *coll_tdsh,
                ub4 attrnum, ub4 startidx, OCIInd *ind);
 
sword OCIPicklerImageCollGetScalarSize(OCIEnv *env, OCIError *err,
                CONST OCIPicklerTds *coll_tdsh, ub4 *size);
 
sword OCIPicklerImageCollGetScalar(OCIEnv *env, OCIError *err,
                                   OCIPicklerImage *imgh, dvoid *buf,
                                   ub4 *buflen, OCIInd *ind);

sword OCIFormatInit(dvoid *hndl, OCIError *err);

sword OCIFormatString(dvoid *hndl, OCIError *err, OraText *buffer,
                      sbig_ora bufferLength, sbig_ora *returnLength,
                      CONST OraText *formatString, ...);

sword OCIFormatTerm(dvoid *hndl, OCIError *err);

sword OCIFormatTUb1(void);
sword OCIFormatTUb2(void);
sword OCIFormatTUb4(void);
sword OCIFormatTUword(void);
sword OCIFormatTUbig_ora(void);
sword OCIFormatTSb1(void);
sword OCIFormatTSb2(void);
sword OCIFormatTSb4(void);
sword OCIFormatTSword(void);
sword OCIFormatTSbig_ora(void);
sword OCIFormatTEb1(void);
sword OCIFormatTEb2(void);
sword OCIFormatTEb4(void);
sword OCIFormatTEword(void);
sword OCIFormatTChar(void);
sword OCIFormatTText(void);
sword OCIFormatTDouble(void);
sword OCIFormatTDvoid(void);
sword OCIFormatTEnd(void);

/*-------------------------- Extensions to XA interface ---------------------*/
/* ------------------------- xaosvch ----------------------------------------*/
/*
   NAME
     xaosvch  -  XA Oracle get SerViCe Handle
   DESCRIPTION
     Given a database name return the service handle that is used by the
     XA library
   NOTE
     This macro has been provided for backward compatibilty with 8.0.2
*/
OCISvcCtx *xaosvch(OraText *dbname);

/* ------------------------- xaoSvcCtx --------------------------------------*/
/*
   NAME
     xaoSvcCtx  -  XA Oracle get SerViCe ConTeXt
   DESCRIPTION
     Given a database name return the service handle that is used by the
     XA library
   NOTE
     This routine has been provided for APs to get access to the service
     handle that XA library uses. Without this routine APs must use SQLLIB
     routine sqlld2 to get access to the Logon data area registered by the
     XA library
*/
OCISvcCtx *xaoSvcCtx(OraText *dbname);

/* ------------------------- xaoEnv -----------------------------------------*/
/*
   NAME
     xaoEnv  -  XA Oracle get ENvironment Handle
   DESCRIPTION
     Given a database name return the environment handle that is used by the
     XA library
   NOTE
     This routine has been provided for APs to get access to the environment
     handle that XA library uses. Without this routine APs must use SQLLIB
     routine sqlld2 to get access to the Logon data area registered by the
     XA library
*/
OCIEnv *xaoEnv(OraText *dbname);

/* ------------------------- xaosterr ---------------------------------------*/
/*
   NAME
     xaosterr  -  XA Oracle get xa STart ERRor code
   DESCRIPTION
     Given an oracle error code return the XA error code
 */
int xaosterr(OCISvcCtx *svch, sb4 error);
/*-------------------------- End Extensions ---------------------------------*/
/*---------------------- Extensions to NLS cartridge service ----------------*/
/* ----------------------- OCINlsGetInfo ------------------------------------*/
/*
   NAME
     OCINlsGetInfo - Get NLS info from OCI environment handle
   REMARKS
     This function generates language information specified by item from OCI 
     environment handle envhp into an array pointed to by buf within size 
     limitation as buflen.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE, or OCI_ERROR on wrong item.
   envhp(IN/OUT)
     OCI environment handle.
   errhp(IN/OUT)
     The OCI error handle. If there is an error, it is record in errhp and 
     this function returns a NULL pointer. Diagnostic information can be 
     obtained by calling OCIErrorGet().
   buf(OUT)
     Pointer to the destination buffer.
   buflen(IN)
     The size of destination buffer. The maximum length for each information
     is 32 bytes.
   item(IN)
     It specifies to get which item in OCI environment handle and can be one
     of following values:
       OCI_NLS_DAYNAME1 : Native name for Monday.
       OCI_NLS_DAYNAME2 : Native name for Tuesday.
       OCI_NLS_DAYNAME3 : Native name for Wednesday.
       OCI_NLS_DAYNAME4 : Native name for Thursday.
       OCI_NLS_DAYNAME5 : Native name for Friday.
       OCI_NLS_DAYNAME6 : Native name for for Saturday.
       OCI_NLS_DAYNAME7 : Native name for for Sunday.
       OCI_NLS_ABDAYNAME1 : Native abbreviated name for Monday.
       OCI_NLS_ABDAYNAME2 : Native abbreviated name for Tuesday.
       OCI_NLS_ABDAYNAME3 : Native abbreviated name for Wednesday.
       OCI_NLS_ABDAYNAME4 : Native abbreviated name for Thursday.
       OCI_NLS_ABDAYNAME5 : Native abbreviated name for Friday.
       OCI_NLS_ABDAYNAME6 : Native abbreviated name for for Saturday.
       OCI_NLS_ABDAYNAME7 : Native abbreviated name for for Sunday.
       OCI_NLS_MONTHNAME1 : Native name for January.
       OCI_NLS_MONTHNAME2 : Native name for February.
       OCI_NLS_MONTHNAME3 : Native name for March.
       OCI_NLS_MONTHNAME4 : Native name for April.
       OCI_NLS_MONTHNAME5 : Native name for May.
       OCI_NLS_MONTHNAME6 : Native name for June.
       OCI_NLS_MONTHNAME7 : Native name for July.
       OCI_NLS_MONTHNAME8 : Native name for August.
       OCI_NLS_MONTHNAME9 : Native name for September.
       OCI_NLS_MONTHNAME10 : Native name for October.
       OCI_NLS_MONTHNAME11 : Native name for November.
       OCI_NLS_MONTHNAME12 : Native name for December.
       OCI_NLS_ABMONTHNAME1 : Native abbreviated name for January.
       OCI_NLS_ABMONTHNAME2 : Native abbreviated name for February.
       OCI_NLS_ABMONTHNAME3 : Native abbreviated name for March.
       OCI_NLS_ABMONTHNAME4 : Native abbreviated name for April.
       OCI_NLS_ABMONTHNAME5 : Native abbreviated name for May.
       OCI_NLS_ABMONTHNAME6 : Native abbreviated name for June.
       OCI_NLS_ABMONTHNAME7 : Native abbreviated name for July.
       OCI_NLS_ABMONTHNAME8 : Native abbreviated name for August.
       OCI_NLS_ABMONTHNAME9 : Native abbreviated name for September.
       OCI_NLS_ABMONTHNAME10 : Native abbreviated name for October.
       OCI_NLS_ABMONTHNAME11 : Native abbreviated name for November.
       OCI_NLS_ABMONTHNAME12 : Native abbreviated name for December.
       OCI_NLS_YES : Native string for affirmative response.
       OCI_NLS_NO : Native negative response. 
       OCI_NLS_AM : Native equivalent string of AM.
       OCI_NLS_PM : Native equivalent string of PM.
       OCI_NLS_AD : Native equivalent string of AD.
       OCI_NLS_BC : Native equivalent string of BC.
       OCI_NLS_DECIMAL : decimal character.
       OCI_NLS_GROUP : group separator.
       OCI_NLS_DEBIT : Native symbol of debit.
       OCI_NLS_CREDIT : Native sumbol of credit.
       OCI_NLS_DATEFORMAT : Oracle date format.
       OCI_NLS_INT_CURRENCY: International currency symbol.
       OCI_NLS_LOC_CURRENCY : Locale currency symbol.
       OCI_NLS_LANGUAGE : Language name.
       OCI_NLS_ABLANGUAGE : Abbreviation for language name.
       OCI_NLS_TERRITORY : Territory name.
       OCI_NLS_CHARACTER_SET : Character set name.
       OCI_NLS_LINGUISTIC : Linguistic name.
       OCI_NLS_CALENDAR : Calendar name.
       OCI_NLS_DUAL_CURRENCY : Dual currency symbol.
*/
sword OCINlsGetInfo(dvoid *envhp, OCIError *errhp, OraText *buf,
                    size_t buflen, ub2 item);


/* -------------------- OCIMultiByteToWideChar ------------------------------*/
/*
   NAME
     OCIMultiByteToWideChar - Convert a null terminated multibyte string into 
                              wchar
   REMARKS
     This routine converts an entire null-terminated string into the wchar 
     format. The wchar output buffer will be null-terminated.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     OCI environment handle to determine the character set of string.
   dst (OUT)
     Destination buffer for wchar.
   src (IN)
     Source string to be converted.
   rsize (OUT)
     Number of characters converted including null-terminator.
     If it is a NULL pointer, no number return
*/
sword OCIMultiByteToWideChar(dvoid *envhp, OCIWchar *dst, CONST OraText *src,
                             size_t *rsize);


/* --------------------- OCIMultiByteInSizeToWideChar -----------------------*/
/*
   NAME
     OCIMultiByteInSizeToWideChar - Convert a mulitbyte string in length into
                                    wchar
   REMARKS
     This routine converts part of string into the wchar format. It will
     convert as many complete characters as it can until it reaches output
     buffer size or input buffer size or it reaches a null-terminator in
     source string. The output buffer will be null-terminated if space permits.
     If dstsz is zero, this function will only return number of characters not
     including ending null terminator for converted string.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     OCI environment handle to determine the character set of string.
   dst (OUT)
     Pointer to a destination buffer for wchar. It can be NULL pointer when
     dstsz is zero.
   dstsz(IN)
     Destination buffer size in character. If it is zero, this function just
     returns number of characters will be need for the conversion.
   src (IN)
     Source string to be converted.
   srcsz(IN)
     Length of source string in byte.
   rsize(OUT)
     Number of characters written into destination buffer, or number of
     characters for converted string is dstsz is zero.
     If it is NULL pointer, nothing to return.
*/
sword OCIMultiByteInSizeToWideChar(dvoid *envhp, OCIWchar *dst,
                                  size_t dstsz, CONST OraText *src, 
                                  size_t srcsz, size_t *rsize);


/* ---------------------- OCIWideCharToMultiByte ----------------------------*/
/*
   NAME
     OCIWideCharToMultiByte - Convert a null terminated wchar string into
                              multibyte
   REMARKS
     This routine converts an entire null-terminated wide character string into
     multi-byte string. The output buffer will be null-terminated.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     OCI environment handle to determine the character set of string.
   dst (OUT)
     Destination buffer for multi-byte string.
   src (IN)
     Source wchar string to be converted.
   rsize (OUT)
     Number of bytes written into the destination buffer.
     If it is NULL pointer, nothing to return.
*/
sword OCIWideCharToMultiByte(dvoid *envhp, OraText *dst, CONST OCIWchar *src,
                             size_t *rsize);


/* ---------------------- OCIWideCharInSizeToMultiByte ----------------------*/
/*
   NAME
     OCIWideCharInSizeToMultiByte - Convert a wchar string in length into 
                                    mulitbyte
   REMARKS
     This routine converts part of wchar string into the multi-byte format.
     It will convert as many complete characters as it can until it reaches
     output buffer size or input buffer size or it reaches a null-terminator
     in source string. The output buffer will be null-terminated if space
     permits. If dstsz is zero, the function just returns the size of byte not 
     including ending null-terminator need to store the converted string.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     OCI environment handle to determine the character set of string.
   dst (OUT)
     Destination buffer for multi-byte. It can be NULL pointer if dstsz is zero.
   dstsz(IN)
     Destination buffer size in byte. If it is zero, it just returns the size
     of bytes need for converted string.
   src (IN)
     Source wchar string to be converted.
   srcsz(IN)
     Length of source string in character.
   rsize(OUT)
     Number of bytes written into destination buffer, or number of bytes need
     to store the converted string if dstsz is zero.
     If it is NULL pointer, nothing to return.
*/
sword OCIWideCharInSizeToMultiByte(dvoid *envhp, OraText *dst,
                                    size_t dstsz, CONST OCIWchar *src,
                                    size_t srcsz, size_t *rsize);



/* ----------------------- OCIWideCharIsAlnum -------------------------------*/
/*
   NAME
     OCIWideCharIsAlnum - test whether wc is a letter or decimal digit
   REMARKS
     It tests whether wc is a letter or decimal digit.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsAlnum(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsAlpha -------------------------------*/
/*
   NAME
     OCIWideCharIsAlpha - test whether wc is an alphabetic letter
   REMARKS
     It tests whether wc is an alphabetic letter
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsAlpha(dvoid *envhp, OCIWchar wc);


/* --------------------- OCIWideCharIsCntrl ---------------------------------*/
/*
   NAME
     OCIWideCharIsCntrl - test whether wc is a control character
   REMARKS
     It tests whether wc is a control character.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsCntrl(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsDigit -------------------------------*/
/*
   NAME
     OCIWideCharIsDigit - test whether wc is a decimal digit character
   REMARKS
     It tests whether wc is a decimal digit character.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsDigit(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsGraph -------------------------------*/
/*
   NAME
     OCIWideCharIsGraph - test whether wc is a graph character
   REMARKS
     It tests whether wc is a graph character. A graph character is character
     with a visible representation and normally includes alphabetic letter,
     decimal digit, and punctuation.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsGraph(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsLower -------------------------------*/
/*
   NAME
     OCIWideCharIsLower - test whether wc is a lowercase letter
   REMARKS
     It tests whether wc is a lowercase letter.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsLower(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsPrint -------------------------------*/
/*
   NAME
     OCIWideCharIsPrint - test whether wc is a printable character
   REMARKS
     It tests whether wc is a printable character.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsPrint(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsPunct -------------------------------*/
/*
   NAME
     OCIWideCharIsPunct - test whether wc is a punctuation character
   REMARKS
     It tests whether wc is a punctuation character.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsPunct(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsSpace -------------------------------*/
/*
   NAME
     OCIWideCharIsSpace - test whether wc is a space character
   REMARKS
     It tests whether wc is a space character. A space character only causes
     white space in displayed text(for example, space, tab, carriage return,
     newline, vertical tab or form feed).
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsSpace(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharIsUpper -------------------------------*/
/*
   NAME
     OCIWideCharIsUpper - test whether wc is a uppercase letter
   REMARKS
     It tests whether wc is a uppercase letter.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsUpper(dvoid *envhp, OCIWchar wc);


/*----------------------- OCIWideCharIsXdigit -------------------------------*/
/*
   NAME
     OCIWideCharIsXdigit - test whether wc is a hexadecimal digit
   REMARKS
     It tests whether wc is a hexadecimal digit ( 0-9, A-F, a-f ).
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsXdigit(dvoid *envhp, OCIWchar wc);


/* --------------------- OCIWideCharIsSingleByte ----------------------------*/
/*
   NAME
     OCIWideCharIsSingleByte - test whether wc is a single-byte character
   REMARKS
     It tests whether wc is a single-byte character when converted into
     multi-byte.
   RETURNS
     TRUE or FLASE.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for testing.
*/
boolean OCIWideCharIsSingleByte(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharToLower -------------------------------*/
/*
   NAME
     OCIWideCharToLower - Convert a wchar into the lowercase
   REMARKS
     If there is a lower-case character mapping for wc in the specified locale,
     it will return the lower-case in wchar, else return wc itself.
   RETURNS
     A wchar
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for lowercase mapping.
*/
OCIWchar OCIWideCharToLower(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharToUpper -------------------------------*/
/*
   NAME
     OCIWideCharToUpper - Convert a wchar into the uppercase
   REMARKS
     If there is a upper-case character mapping for wc in the specified locale,
     it will return the upper-case in wchar, else return wc itself.
   RETURNS
     A wchar
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar for uppercase mapping.
*/
OCIWchar OCIWideCharToUpper(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIWideCharStrcmp --------------------------------*/
/*
   NAME
     OCIWideCharStrcmp - compare two null terminated wchar string
   REMARKS
     It compares two wchar string in binary ( based on wchar encoding value ),
     linguistic, or case-insensitive.
   RETURNS
     0, if wstr1 == wstr2.
     Positive, if wstr1 > wstr2.
     Negative, if wstr1 < wstr2.
   envhp(IN/OUT)
     OCI environment handle to determine the character set.
   wstr1(IN)
     Pointer to a null-terminated wchar string.
   wstr2(IN)
     Pointer to a null-terminated wchar string.
   flag(IN)
     It is used to decide the comparison method. It can be taken one of the
     following values:
       OCI_NLS_BINARY : for the binary comparison, this is default value.
       OCI_NLS_LINGUISTIC : for linguistic comparison specified in the locale.
     This flag can be ORed with OCI_NLS_CASE_INSENSITIVE for case-insensitive
     comparison.
*/
int OCIWideCharStrcmp(dvoid *envhp, CONST OCIWchar *wstr1, 
                CONST OCIWchar *wstr2, int flag);


/* ----------------------- OCIWideCharStrncmp -------------------------------*/
/*
   NAME
     OCIWideCharStrncmp - compare twe wchar string in length
   REMARKS
     This function is similar to OCIWideCharStrcmp(), except that at most len1
     characters from wstr1 and len2 characters from wstr1 are compared. The
     null-terminator will be taken into the comparison.
   RETURNS
     0, if wstr1 = wstr2
     Positive, if wstr1 > wstr2
     Negative, if wstr1 < wstr2
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wstr1(IN)
     Pointer to the first wchar string
   len1(IN)
     The length for the first string for comparison
   wstr2(IN)
     Pointer to the second wchar string
   len2(IN)
     The length for the second string for comparison.
   flag(IN)
     It is used to decide the comparison method. It can be taken one of the
     following values:
       OCI_NLS_BINARY : for the binary comparison, this is default value.
       OCI_NLS_LINGUISTIC : for linguistic comparison specified in the locale.
     This flag can be ORed with OCI_NLS_CASE_INSENSITIVE for case-insensitive 
     comparison.
*/
int OCIWideCharStrncmp(dvoid *envhp, CONST OCIWchar *wstr1, size_t len1,
                 CONST OCIWchar *wstr2, size_t len2, int flag);


/* ----------------------- OCIWideCharStrcat --------------------------------*/
/*
   NAME
     OCIWideCharStrcat - concatenate two wchar strings
   REMARKS
     This function appends a copy of the wchar string pointed to by wsrcstr,
     including the null-terminator to the end of wchar string pointed to by
     wdststr. It returns the number of character in the result string not
     including the ending null-terminator.
   RETURNS
     number of characters in the result string not including the ending
     null-terminator.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wdststr(IN/OUT)
     Pointer to the destination wchar string for appending.
   wsrcstr(IN)
     Pointer to the source wchar string to append.
*/
size_t OCIWideCharStrcat(dvoid *envhp, OCIWchar *wdststr,
                         CONST OCIWchar *wsrcstr);


/* ----------------------- OCIWideCharStrchr --------------------------------*/
/*
   NAME
     OCIWideCharStrchr - Search the first occurrence of wchar in a wchar string
   REMARKS
     This function searchs for the first occurrence of wc in the wchar string
     pointed to by wstr. It returns a pointer to the whcar if successful, or
     a null pointer.
   RETURNS
     wchar pointer if successful, otherwise a null pointer.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wstr(IN)
     Pointer to the wchar string to search
   wc(IN)
     Wchar to search for.
*/
OCIWchar *OCIWideCharStrchr(dvoid *envhp, CONST OCIWchar *wstr,
                            OCIWchar wc);


/* ----------------------- OCIWideCharStrcpy --------------------------------*/
/*
   NAME
     OCIWideCharStrcpy - copy a wchar string
   REMARKS
     This function copies the wchar string pointed to by wsrcstr, including the 
     null-terminator, into the array pointed to by wdststr. It returns the
     number of character copied not including the ending null-terminator.
   RETURNS
     number of characters copied not including the ending null-terminator.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wdststr(OUT)
     Pointer to the destination wchar buffer.
   wsrcstr(IN)
     Pointer to the source wchar string.
*/
size_t OCIWideCharStrcpy(dvoid *envhp, OCIWchar *wdststr,
                         CONST OCIWchar *wsrcstr);


/* ----------------------- OCIWideCharStrlen --------------------------------*/
/*
   NAME
     OCIWideCharStrlen - Return number of character in a wchar string
   REMARKS
     This function computes the number of characters in the wchar string
     pointed to by wstr, not including the null-terminator, and returns
    this number. 
   RETURNS
     number of characters not including ending null-terminator.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wstr(IN)
     Pointer to the source wchar string.
*/
size_t OCIWideCharStrlen(dvoid *envhp, CONST OCIWchar *wstr);


/* ----------------------- OCIWideCharStrncat -------------------------------*/
/*
   NAME
     OCIWideCharStrncat - Concatenate wchar string in length
   REMARKS
     This function is similar to OCIWideCharStrcat(), except that at most n
     characters from wsrcstr are appended to wdststr. Note that the
     null-terminator in wsrcstr will stop appending. wdststr will be
     null-terminated.. 
   RETURNS
     Number of characters in the result string not including the ending
     null-terminator.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wdststr(IN/OUT)
     Pointer to the destination wchar string for appending.
   wsrcstr(IN)
     Pointer to the source wchar string to append.
   n(IN)
     Number of characters from wsrcstr to append.
*/
size_t OCIWideCharStrncat(dvoid *envhp, OCIWchar *wdststr,
                          CONST OCIWchar *wsrcstr, size_t n);


/* ----------------------- OCIWideCharStrncpy -------------------------------*/
/*
   NAME
     OCIWideCharStrncpy - Copy wchar string in length
   REMARKS
     This function is similar to OCIWideCharStrcpy(), except that at most n
     characters are copied from the array pointed to by wsrcstr to the array
     pointed to by wdststr. Note that the null-terminator in wdststr will
     stop coping and result string will be null-terminated.
   RETURNS
     number of characters copied not including the ending null-terminator.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wdststr(OUT)
     Pointer to the destination wchar buffer.
   wsrcstr(IN)
     Pointer to the source wchar string.
   n(IN)
     Number of characters from wsrcstr to copy.
*/
size_t OCIWideCharStrncpy(dvoid *envhp, OCIWchar *wdststr,
                          CONST OCIWchar *wsrcstr, size_t n);


/* ----------------------- OCIWideCharStrrchr -------------------------------*/
/*
   NAME
     OCIWideCharStrrchr - search the last occurrence of a wchar in wchar string
   REMARKS
     This function searchs for the last occurrence of wc in the wchar string
     pointed to by wstr. It returns a pointer to the whcar if successful, or
     a null pointer.
   RETURNS
     wchar pointer if successful, otherwise a null pointer.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wstr(IN)
     Pointer to the wchar string to search
   wc(IN)
     Wchar to search for.
*/
OCIWchar *OCIWideCharStrrchr(dvoid *envhp, CONST OCIWchar *wstr,
                             OCIWchar wc);


/* --------------------- OCIWideCharStrCaseConversion -----------------------*/
/*
   NAME
     OCIWideCharStrCaseConversion - convert a wchar string into lowercase or
                                    uppercase
   REMARKS
     This function convert the wide char string pointed to by wsrcstr into the
     uppercase or lowercase specified by flag and copies the result into the
     array pointed to by wdststr. The result string will be null-terminated.
   RETURNS
     number of characters for result string not including null-terminator.
   envhp(IN/OUT)
     OCI environment handle.
   wdststr(OUT)
     Pointer to destination array.
   wsrcstr(IN)
     Pointer to source string.
   flag(IN)
     Specify the case to convert:
       OCI_NLS_UPPERCASE : convert to uppercase.
       OCI_NLS_LOWERCASE: convert to lowercase.
     This flag can be ORed with OCI_NLS_LINGUISTIC to specify that the
     linguistic setting in the locale will be used for case conversion.
*/
size_t OCIWideCharStrCaseConversion(dvoid *envhp, OCIWchar *wdststr,
                                    CONST OCIWchar *wsrcstr, ub4 flag);


/*---------------------- OCIWideCharDisplayLength ---------------------------*/
/*
   NAME
     OCIWideCharDisplayLength - Calculate the display length for a wchar
   REMARKS
     This function determines the number of column positions required for wc
     in display. It returns number of column positions, or 0 if wc is 
     null-terminator.
   RETURNS
     Number of display positions.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar character.
*/
size_t OCIWideCharDisplayLength(dvoid *envhp, OCIWchar wc );


/*---------------------- OCIWideCharMultiByteLength -------------------------*/
/*
   NAME
     OCIWideCharMultiByteLength - Determine byte size in multi-byte encoding
   REMARKS
     This function determines the number of byte required for wc in multi-byte
     encoding. It returns number of bytes in multi-byte for wc.
   RETURNS
     Number of bytes.
   envhp(IN/OUT)
     OCI environment handle to determine the character set .
   wc(IN)
     Wchar character.
*/
size_t OCIWideCharMultiByteLength(dvoid *envhp, OCIWchar wc);


/* ----------------------- OCIMultiByteStrcmp -------------------------------*/
/*
   NAME
     OCIMultiByteStrcmp - Compare two multi-byte strings
   REMARKS
     It compares two multi-byte strings in binary ( based on encoding value ),
     linguistic, or case-insensitive.
   RETURNS
     0, if str1 == str2.
     Positive, if str1 > str2.
     Negative, if str1 < str2.
   envhp(IN/OUT)
     OCI environment handle to determine the character set.
   str1(IN)
     Pointer to a null-terminated string.
   str2(IN)
     Pointer to a null-terminated string.
   flag(IN)
     It is used to decide the comparison method. It can be taken one of the
     following values:
       OCI_NLS_BINARY: for the binary comparison, this is default value.
       OCI_NLS_LINGUISTIC: for linguistic comparison specified in the locale.
     This flag can be ORed with OCI_NLS_CASE_INSENSITIVE for case-insensitive 
     comparison.
*/
int OCIMultiByteStrcmp(dvoid *envhp, CONST OraText *str1,
                       CONST OraText *str2, int flag);


/*----------------------- OCIMultiByteStrncmp -------------------------------*/
/*
   NAME
     OCIMultiByteStrncmp - compare two strings in length
   REMARKS
     This function is similar to OCIMultiBytestrcmp(), except that at most len1 bytes
     from str1 and len2 bytes from str2 are compared. The null-terminator will
     be taken into the comparison.
   RETURNS
     0, if str1 = str2
     Positive, if str1 > str2
     Negative, if str1 < str2
   envhp(IN/OUT)
     OCI environment handle to determine the character set.
   str1(IN)
     Pointer to the first string
   len1(IN)
     The length for the first string for comparison
   str2(IN)
     Pointer to the second string
   len2(IN)
     The length for the second string for comparison.
   flag(IN)
     It is used to decide the comparison method. It can be taken one of the
     following values:
       OCI_NLS_BINARY: for the binary comparison, this is default value.
       OCI_NLS_LINGUISTIC: for linguistic comparison specified in the locale.
     This flag can be ORed with OCI_NLS_CASE_INSENSITIVE for case-insensitive 
     comparison.
*/
int OCIMultiByteStrncmp(dvoid *envhp, CONST OraText *str1, size_t len1,
                        OraText *str2, size_t len2, int flag);


/*----------------------- OCIMultiByteStrcat --------------------------------*/
/*
   NAME
     OCIMultiByteStrcat - concatenate multibyte strings
   REMARKS
     This function appends a copy of the multi-byte string pointed to by
     srcstr, including the null-terminator to the end of string pointed to by
     dststr. It returns the number of bytes in the result string not including
     the ending null-terminator.
   RETURNS
     number of bytes in the result string not including the ending
     null-terminator.
   envhp(IN/OUT)
     Pointer to OCI environment handle
   dststr(IN/OUT)
     Pointer to the destination multi-byte string for appending.
   srcstr(IN)
     Pointer to the source string to append.
*/
size_t OCIMultiByteStrcat(dvoid *envhp, OraText *dststr,
                          CONST OraText *srcstr);


/*------------------------- OCIMultiByteStrcpy ------------------------------*/
/*
   NAME
     OCIMultiByteStrcpy - copy multibyte string
   REMARKS
     This function copies the multi-byte string pointed to by srcstr,
     including the null-terminator, into the array pointed to by dststr. It
     returns the number of bytes copied not including the ending 
     null-terminator.
   RETURNS
     number of bytes copied not including the ending null-terminator.
   envhp(IN/OUT)
     Pointer to the OCI environment handle.
   srcstr(OUT)
     Pointer to the destination buffer.
   dststr(IN)
     Pointer to the source multi-byte string.
*/
size_t OCIMultiByteStrcpy(dvoid *envhp, OraText *dststr,
                          CONST OraText *srcstr);


/*----------------------- OCIMultiByteStrlen --------------------------------*/
/*
   NAME
     OCIMultiByteStrlen - Calculate multibyte string length
   REMARKS
     This function computes the number of bytes in the multi-byte string
     pointed to by str, not including the null-terminator, and returns this
     number. 
   RETURNS
     number of bytes not including ending null-terminator.
   str(IN)
     Pointer to the source multi-byte string.
*/
size_t OCIMultiByteStrlen(dvoid *envhp, CONST OraText *str);


/*----------------------- OCIMultiByteStrncat -------------------------------*/
/*
   NAME
     OCIMultiByteStrncat - concatenate string in length
   REMARKS
     This function is similar to OCIMultiBytestrcat(), except that at most n
     bytes from srcstr are appended to dststr. Note that the null-terminator in
     srcstr will stop appending and the function will append as many character
     as possible within n bytes. dststr will be null-terminated. 
   RETURNS
     Number of bytes in the result string not including the ending
     null-terminator.
   envhp(IN/OUT)
     Pointer to OCI environment handle.
   srcstr(IN/OUT)
     Pointer to the destination multi-byte string for appending.
   dststr(IN)
     Pointer to the source multi-byte string to append.
   n(IN)
     Number of bytes from srcstr to append.
*/
size_t OCIMultiByteStrncat(dvoid *envhp, OraText *dststr,
                           CONST OraText *srcstr, size_t n);


/*----------------------- OCIMultiByteStrncpy -------------------------------*/
/*
   NAME
     OCIMultiByteStrncpy - copy multibyte string in length
   REMARKS
     This function is similar to OCIMultiBytestrcpy(), except that at most n
     bytes are copied from the array pointed to by srcstr to the array pointed
     to by dststr. Note that the null-terminator in srcstr will stop coping and
     the function will copy as many character as possible within n bytes. The 
     result string will be null-terminated.
   RETURNS
     number of bytes copied not including the ending null-terminator.
   envhp(IN/OUT)
     Pointer to a OCI environment handle.
   dststr(IN)
     Pointer to the source multi-byte string.
   srcstr(OUT)
     Pointer to the destination buffer.
   n(IN)
     Number of bytes from srcstr to copy.
*/
size_t OCIMultiByteStrncpy(dvoid *envhp, OraText *dststr,
                           CONST OraText *srcstr, size_t n);


/*----------------------- OCIMultiByteStrnDisplayLength ---------------------*/
/*
   NAME
     OCIMultiByteStrnDisplayLength - calculate the display length for a
                                     multibyt string
   REMARKS
     This function returns the number of display positions occupied by the 
     complete characters within the range of n bytes.
   RETURNS
     number of display positions.
   envhp(IN/OUT)
     OCI environment handle.
   str(IN)
     Pointer to a multi-byte string.
   n(IN)
     Number of bytes to examine.
*/
size_t OCIMultiByteStrnDisplayLength(dvoid *envhp, CONST OraText *str1,
                                     size_t n);


/*---------------------- OCIMultiByteStrCaseConversion  ---------------------*/
/*
   NAME
     OCIMultiByteStrCaseConversion
   REMARKS
     This function convert the multi-byte string pointed to by srcstr into the
     uppercase or lowercase specified by flag and copies the result into the
     array pointed to by dststr. The result string will be null-terminated.
   RETURNS
     number of bytes for result string not including null-terminator.
   envhp(IN/OUT)
     OCI environment handle.
   dststr(OUT)
     Pointer to destination array.
   srcstr(IN)
     Pointer to source string.
   flag(IN)
     Specify the case to convert:
       OCI_NLS_UPPERCASE: convert to uppercase.
       OCI_NLS_LOWERCASE: convert to lowercase.
     This flag can be ORed with OCI_NLS_LINGUISTIC to specify that the 
     linguistic setting in the locale will be used for case conversion.
*/
size_t OCIMultiByteStrCaseConversion(dvoid *envhp, OraText *dststr,
                                     CONST OraText *srcstr, ub4 flag);


/*------------------------- OCICharSetToUnicode -----------------------------*/
/*
   NAME
     OCICharSetToUnicode - convert multibyte string into Unicode as UCS2
   REMARKS
     This function converts a multi-byte string pointed to by src to Unicode 
     into the array pointed to by dst. The conversion will stop when it reach
     to the source limitation or destination limitation. 
     The function will return number of characters converted into Unicode.
     If dstlen is zero, it will just return the number of characters for the
     result without real conversion.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     Pointer to an OCI environment handle
   dst(OUT)
     Pointer to a destination buffer
   dstlen(IN)
     Size of destination buffer in character
   src(IN)
     Pointer to multi-byte source string.
   srclen(IN)
     Size of source string in bytes.
   rsize(OUT)
     Number of characters converted.
     If it is a NULL pointer, nothing to return.
*/
sword OCICharSetToUnicode(dvoid *envhp, ub2 *dst, size_t dstlen,
                          CONST OraText *src, size_t  srclen, size_t *rsize);


/*------------------------- OCIUnicodeToCharSet -----------------------------*/
/*
   NAME
     OCIUnicodeToCharSet - convert Unicode into multibyte
   REMARKS
     This function converts a Unicode string pointed to by src to multi-byte
     into the array pointed to by dst. The conversion will stop when it reach
     to the source limitation or destination limitation. The function will
     return number of bytes converted into multi-byte. If dstlen is zero, it
     will just return the number of bytes for the result without real 
     conversion. If a Unicode character is not convertible for the character
     set specified in OCI environment handle, a replacement character will be
     used for it. In this case, OCICharSetConversionIsReplacementUsed() will
     return ture.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     Pointer to an OCI environment handle.
   dst(OUT)
     Pointer to a destination buffer.
   dstlen(IN)
     Size of destination buffer in byte.
   src(IN)
     Pointer to a Unicode string.
   srclen(IN)
     Size of source string in characters.
   rsize(OUT)
     Number of bytes converted.
     If it is a NULL pointer, nothing to return.
*/
sword OCIUnicodeToCharSet(dvoid *envhp, OraText *dst, size_t dstlen, 
                           CONST ub2 *src, size_t srclen, size_t *rsize);


/* ------------------- OCICharsetConversionIsReplacementUsed ----------------*/
/*
   NAME
     OCICharsetConversionIsReplacementUsed - chech if replacement is used in 
                                             conversion
   REMARKS
     This function indicates whether or not the replacement character was used
     for nonconvertible characters in character set conversion in last invoke
     of OCICharsetUcs2ToMb().
   RETURNS
     TRUE is the replacement character was used in last OCICharsetUcs2ToMb()
     invoking, else FALSE.
   envhp(IN/OUT)
     OCI environment handle. This should be the first handle passed to 
     OCICharsetUcs2ToMb().
*/
boolean OCICharSetConversionIsReplacementUsed(dvoid *envhp);


/*------------------------- OCIMessageOpen ----------------------------------*/
/*
   NAME
     OCIMessageOpen - open a locale message file
   REMARKS
     This function opens a message handle for facility of product in a language
     pointed to by envhp. It first try to open the message file corresponding
     to envhp for the facility. If it successes, it will use that file to
     initialize a message handle, else it will use the default message file
     which is for American language for the facility. The function return a
     pointer pointed to a message handle into msghp parameter.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     A pointer to OCI environment handle for message language.
   errhp(IN/OUT)
     The OCI error handle. If there is an error, it is record in errhp and this
     function returns a NULL pointer. Diagnostic information can be obtained by
     calling OCIErrorGet().
   msghp(OUT)
     a message handle for return
   product(IN)
     A pointer to a product name. Product name is used to locate the directory
     for message in a system dependent way. For example, in Solaris, the
     directory of message files for the product `rdbms' is
     `${ORACLE_HOME}/rdbms'.
   facility(IN)
     A pointer to a facility name in the product. It is used to construct a
     message file name. A message file name follows the conversion with 
     facility as prefix. For example, the message file name for facility
     `img' in American language will be `imgus.msb' where `us' is the 
     abbreviation of American language and `msb' as message binary file 
     extension.
   dur(IN)
     Duration for memory allocation for the return message handle. It can be
     the following values:
        OCI_DURATION_CALL
        OCI_DURATION_STATEMENT
        OCI_DURATION_SESSION
        OCI_DURATION_TRANSACTION
     For the detail description, please refer to Memory Related Service
     Interfaces section.
*/
sword OCIMessageOpen(dvoid *envhp, OCIError *errhp, OCIMsg **msghp,
                      CONST OraText *product, CONST OraText *facility, 
                      OCIDuration dur);
 

/*------------------------- OCIMessageGet -----------------------------------*/
/*
   NAME
     OCIMessageGet - get a locale message from a message handle
   REMARKS
     This function will get message with message number identified by msgno and
     if buflen is not zero, the function will copy the message into the buffer
     pointed to by msgbuf. If buflen is zero, the message will be copied into
     a message buffer inside the message handle pointed to by msgh. For both
     cases. it will return the pointer to the null-terminated message string.
     If it cannot get the message required, it will return a NULL pointer.
   RETURNS
     A pointer to a null-terminated message string on success, otherwise a NULL
     pointer.
   msgh(IN/OUT)
     Pointer to a message handle which was previously opened by OCIMessageOpen().
   msgno(IN)
     The message number for getting message.
   msgbuf(OUT)
     Pointer to a destination buffer to the message retrieved. If buflen is
     zero, it can be NULL pointer.
   buflen(IN)
     The size of the above destination buffer.
*/
OraText *OCIMessageGet(OCIMsg *msgh, ub4 msgno, OraText *msgbuf, size_t buflen);


/*------------------------- OCIMessageClose ---------------------------------*/
/*
   NAME
     OCIMessageClose - close a message handle
   REMARKS
     This function closes a message handle pointed to by msgh and frees any
     memory associated with this handle.
   RETURNS
     OCI_SUCCESS, OCI_INVALID_HANDLE or OCI_ERROR
   envhp(IN/OUT)
     A pointer to OCI environment handle for message language.
   errhp(IN/OUT)
     The OCI error handle. If there is an error, it is record in errhp and this
     function returns a NULL pointer. Diagnostic information can be obtained by
     calling OCIErrorGet().
   msghp(IN/OUT)
     A pointer to a message handle which was previously opened by
     OCIMessageOpen().
*/
sword OCIMessageClose(dvoid *envhp, OCIError *errhp, OCIMsg *msghp);

/*--------------- End of Extensions to NLS cartridge service ----------------*/


/*----------------- Extensions to OCI Thread interface ---------------------*/
/*****************************************************************************
                              DESCRIPTION
******************************************************************************
1 Threads Interface

The OCIThread package provides a number of commonly used threading
primitives for use by Oracle customers.  It offers a portable interface to
threading capabilities native to various platforms.  It does not implement
threading on platforms which do not have native threading capability.

OCIThread does not provide a portable implementation of multithreaded 
facilities.  It only serves as a set of portable covers for native 
multithreaded facilities.  Therefore, platforms that do not have native 
support for multi-threading will only be able to support a limited 
implementation of OCIThread.  As a result, products that rely on all of 
OCIThread's functionality will not port to all platforms.  Products that must 
port to all platforms must use only a subset of OCIThread's functionality.  
This issue is discussed further in later sections of this document.

The OCIThread API is split into four main parts.  Each part is described
briefly here.  The following subsections describe each in greater detail.

 1. Initialization and Termination Calls

     These calls deal with the initialization and termination of OCIThread.
     Initialization of OCIThread initializes the OCIThread context which is
     a member of the OCI environment or session handle.  This context is 
     required for other OCIThread calls.

 2. Passive Threading Primitives

     The passive threading primitives include primitives to manipulate mutual
     exclusion (mutex) locks, thread ID's, and thread-specific data keys.

     The reason that these primitives are described as 'passive' is that while
     their specifications allow for the existence of multiple threads, they do
     not require it.  This means that it is possible for these primitives to
     be implemented according to specification in both single-threaded and
     multi-threaded environments.

     As a result, OCIThread clients that use only these primitives will not
     require the existence of multiple threads in order to work correctly,
     i.e., they will be able to work in single-threaded environments without 
     branching code.

 3. Active Threading Primitives

     Active threading primitives include primitives dealing with the creation,
     termination, and other manipulation of threads.

     The reason that these primitives are described as 'active' is that they
     can only be used in true multi-threaded environments.  Their
     specifications explicitly require that it be possible to have multiple
     threads.  If you need to determine at runtime whether or not you are in a
     multi-threaded environment, call OCIThreadIsMulti() before calling an
     OCIThread active primitive.


1.1 Initialization & Termination
==================================

The types and functions described in this section are associated with the
initialization and termination of the OCIThread package.  OCIThread must
be properly initialized before any of its functionality can be used.
OCIThread's process initialization function, 'OCIThreadProcessInit()',
must be called with care; see below.

The observed behavior of the initialization and termination functions is the
same regardless of whether OCIThread is in single-threaded or multi-threaded
environment.  It is OK to call the initialization functions from both generic
and operating system specific (OSD) code.

1.1.1 Types

  OCIThreadContext - OCIThread Context
  -------------------------------------

    Most calls to OCIThread functions take the OCI environment or session 
    handle as a parameter.  The OCIThread context is part of the OCI 
    environment or session handle and it must be initialized by calling 
    'OCIThreadInit()'.  Termination of the OCIThread context occurs by calling
    'OCIThreadTerm()'.

    The OCIThread context is a private data structure.  Clients must NEVER
    attempt to examine the contents of the context.

1.1.2  OCIThreadProcessInit

  OCIThreadProcessInit - OCIThread Process INITialization
  --------------------------------------------------------

    Description

      This function should be called to perform OCIThread process
      initialization.

    Prototype

      void OCIThreadProcessInit();

    Returns

      Nothing.

    Notes

      Whether or not this function needs to be called depends on how OCI
      Thread is going to be used.

        * In a single-threaded application, calling this function is optional.
          If it is called at all, the first call to it must occur before calls
          to any other OCIThread functions.  Subsequent calls can be made
          without restriction; they will not have any effect.

        * In a multi-threaded application, this function MUST be called.  The
          first call to it MUST occur 'strictly before' any other OCIThread
          calls; i.e., no other calls to OCIThread functions (including other
          calls to this one) can be concurrent with the first call.
          Subsequent calls to this function can be made without restriction;
          they will not have any effect.


1.1.3 OCIThreadInit

  OCIThreadInit - OCIThread INITialize
  -------------------------------------

    Description

      This initializes OCIThread context.

    Prototype

      sword OCIThreadInit(dvoid *hndl, OCIError *err);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is illegal for OCIThread clients to try an examine the memory
      pointed to by the returned pointer.

      It is safe to make concurrent calls to 'OCIThreadInit()'.  Unlike
      'OCIThreadProcessInit()',  there is no need to have a first call
      that occurs before all the others.

      The first time 'OCIThreadInit()' is called, it initilaizes the OCI
      Thread context.  It also saves a pointer to the context in some system
      dependent manner.  Subsequent calls to 'OCIThreadInit()' will return
      the same context.

      Each call to 'OCIThreadInit()' must eventually be matched by a call to
      'OCIThreadTerm()'.

  OCIThreadTerm - OCIThread TERMinate
  ------------------------------------

    Description

      This should be called to release the OCIThread context.  It should be
      called exactly once for each call made to 'OCIThreadInit()'.

    Prototype

      sword OCIThreadTerm(dvoid *hndl, OCIError *err);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is safe to make concurrent calls to 'OCIThreadTerm()'.

      'OCIThreadTerm()' will not do anything until it has been called as
      many times as 'OCIThreadInit()' has been called.  When that happens,
      it terminates the OCIThread layer and frees the memory allocated for
      the context.  Once this happens, the context should not be re-used.
      It will be necessary to obtain a new one by calling 'OCIThreadInit()'.


  OCIThreadIsMulti - OCIThread Is Multi-Threaded?
  ------------------------------------------------

    Description

      This tells the caller whether the application is running in a
      multi-threaded environment or a single-threaded environment.

    Prototype
      boolean OCIThreadIsMulti(void);

    Returns

      TRUE if the environment is multi-threaded;
      FALSE if the environment is single-threaded.


1.2 Passive Threading Primitives
==================================

1.2.1 Types

The passive threading primitives deal with the manipulation of mutex, 
thread ID's, and thread-specific data.  Since the specifications of these 
primitives do not require the existence of multiple threads, they can be 
used both on multithreaded and single-threaded platforms.

1.2.1.1  OCIThreadMutex - OCIThread Mutual Exclusion Lock
-----------------------------------------------------------

  The type 'OCIThreadMutex' is used to represent a mutual exclusion lock
  (mutex).  A mutex is typically used for one of two purposes: (i) to
  ensure that only one thread accesses a given set of data at a time, or
  (ii) to ensure that only one thread executes a given critical section of
  code at a time.

  Mutexes pointer can be declared as parts of client structures or as 
  stand-alone variables.  Before they can be used, they must be initialized 
  using 'OCIThreadMutexInit()'.  Once they are no longer needed, they must be
  destroyed using 'OCIThreadMutexDestroy()'.  A mutex pointer must NOT be 
  used after it is destroyed.

  A thread can acquire a mutex by using either 'OCIThreadMutexAcquire()' or
  'OCIThreadMutexTry()'.  They both ensure that only one thread at a time is
  allowed to hold a given mutex.  A thread that holds a mutex can release it
  by calling 'OCIThreadMutexRelease()'.


1.2.1.2  OCIThreadKey - OCIThread Key for Thread-Specific Data
----------------------------------------------------------------

  A key can be thought of as a process-wide variable that has a
  thread-specific value.  What this means is that all the threads in a
  process can use any given key.  However, each thread can examine or modify
  that key independently of the other threads.  The value that a thread sees
  when it examines the key will always be the same as the value that it last
  set for the key.  It will not see any values set for the key by the other
  threads.

  The type of the value held by a key is a 'dvoid *' generic pointer.

  Keys can be created using 'OCIThreadKeyInit()'.  When a key is created, its
  value is initialized to 'NULL' for all threads.

  A thread can set a key's value using 'OCIThreadKeySet()'.  A thread can
  get a key's value using 'OCIThreadKeyGet()'.

  The OCIThread key functions will save and retrieve data SPECIFIC TO THE
  THREAD.  When clients maintain a pool of threads and assign the threads to
  different tasks, it *may not* be appropriate for a task to use OCIThread
  key functions to save data associated with it.  Here is a scenario of how
  things can fail: A thread is assigned to execute the initialization of a
  task.  During the initialization, the task stored some data related to it
  in the thread using OCIThread key functions.  After the initialization,
  the thread is returned back to the threads pool.  Later, the threads pool
  manager assigned another thread to perform some operations on the task,
  and the task needs to retrieve those data it stored earlier in
  initialization.  Since the task is running in another thread, it will not
  be able to retrieve the same data back!  Applications that use thread
  pools should be aware of this and be cautious when using OCIThread key
  functions.


1.2.1.3  OCIThreadKeyDestFunc - OCIThread Key Destructor Function Type
------------------------------------------------------------------------

  This is the type of a pointer to a key's destructor routine.  Keys can be
  associated with a destructor routine when they are created (see
  'OCIThreadKeyInit()').

  A key's destructor routine will be called whenever a thread that has a
  non-NULL value for the key terminates.

  The destructor routine returns nothing and takes one parameter.  The
  parameter will be the value that was set for key when the thread
  terminated.

  The destructor routine is guaranteed to be called on a thread's value
  in the key after the termination of the thread and before process
  termination.  No more precise guarantee can be made about the timing
  of the destructor routine call; thus no code in the process may assume
  any post-condition of the destructor routine.  In particular, the
  destructor is not guaranteed to execute before a join call on the
  terminated thread returns.


1.2.1.4  OCIThreadId - OCIThread Thread ID
--------------------------------------------

  Type 'OCIThreadId' is the type that will be used to identify a thread.
  At any given time, no two threads will ever have the same 'OCIThreadId'.
  However, 'OCIThreadId' values can be recycled; i.e., once a thread dies,
  a new thread may be created that has the same 'OCIThreadId' as the one
  that died.  In particular, the thread ID must uniquely identify a thread
  T within a process, and it must be consistent and valid in all threads U
  of the process for which it can be guaranteed that T is running
  concurrently with U.  The thread ID for a thread T must be retrievable
  within thread T.  This will be done via OCIThreadIdGet().

  The 'OCIThreadId' type supports the concept of a NULL thread ID: the NULL
  thread ID will never be the same as the ID of an actual thread.



1.2.2 Function prototypes for passive primitives
--------------------------------------------------

1.2.2.1 Mutex functions
-------------------------

  OCIThreadMutexInit - OCIThread MuteX Initialize
  -----------------------------------------------

    Description

      This allocate and initializes a mutex.  All mutexes must be 
      initialized prior to use.

    Prototype

      sword OCIThreadMutexInit(dvoid *hndl, OCIError *err, 
                               OCIThreadMutex **mutex);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        mutex(OUT):  The mutex to initialize.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      Multiple threads must not initialize the same mutex simultaneously.
      Also, a mutex must not be reinitialized until it has been destroyed (see
      'OCIThreadMutexDestroy()').

  OCIThreadMutexDestroy - OCIThread MuteX Destroy
  -----------------------------------------------

    Description

      This destroys and deallocate a mutex.  Each mutex must be destroyed 
      once it is no longer needed.

    Prototype

      sword OCIThreadMutexDestroy(dvoid *hndl, OCIError *err,
                                  OCIThreadMutex **mutex);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        mutex(IN/OUT):   The mutex to destroy.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is not legal to destroy a mutex that is uninitialized or is currently
      held by a thread.  The destruction of a mutex must not occur concurrently
      with any other operations on the mutex.  A mutex must not be used after
      it has been destroyed.


  OCIThreadMutexAcquire - OCIThread MuteX Acquire
  -----------------------------------------------

    Description

      This acquires a mutex for the thread in which it is called.  If the mutex
      is held by another thread, the calling thread is blocked until it can
      acquire the mutex.

    Prototype

     sword OCIThreadMutexAcquire(dvoid *hndl, OCIError *err,
                                 OCIThreadMutex *mutex);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error, it is 
                     recorded in err and this function returns OCI_ERROR.  
                     Diagnostic information can be obtained by calling 
                     OCIErrorGet().

        mutex(IN/OUT):   The mutex to acquire.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is illegal to attempt to acquire an uninitialized mutex.

      This function's behavior is undefined if it is used by a thread to
      acquire a mutex that is already held by that thread.



  OCIThreadMutexRelease - OCIThread MuteX Release
  -----------------------------------------------

    Description

      This releases a mutex.  If there are any threads blocked on the mutex,
      one of them will acquire it and become unblocked.

    Prototype

      sword OCIThreadMutexRelease(dvoid *hndl, OCIError *err,
                                  OCIThreadMutex *mutex);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        mutex(IN/OUT):   The mutex to release.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is illegal to attempt to release an uninitialized mutex.  It is also
      illegal for a thread to release a mutex that it does not hold.


  OCIThreadKeyInit - OCIThread KeY Initialize
  -------------------------------------------

    Description

      This creates a key.  Each call to this routine allocate and generates 
      a new key that is distinct from all other keys.

    Prototype

      sword OCIThreadKeyInit(dvoid *hndl, OCIError *err, OCIThreadKey **key,
                             OCIThreadKeyDestFunc destFn);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        key(OUT):    The 'OCIThreadKey' in which to create the new key.

        destFn(IN):  The destructor for the key.  NULL is permitted.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      Once this function executes successfully, a pointer to an allocated and 
      initialized key is return.  That key can be used with 'OCIThreadKeyGet()'
      and 'OCIThreadKeySet()'.  The initial value of the key will be 'NULL' for
      all threads.

      It is illegal for this function to be called more than once to create the
      same key (i.e., to be called more than once with the same value for the
      'key' parameter).

      If the 'destFn' parameter is not NULL, the routine pointed to by 'destFn'
      will be called whenever a thread that has a non-NULL value for the key
      terminates.  The routine will be called with one parameter.  The
      parameter will be the key's value for the thread at the time at which the
      thread terminated.
      If the key does not need a destructor function, pass NULL for 'destFn'.


  OCIThreadKeyDestroy - OCIThread KeY DESTROY
  -------------------------------------------

   Description

     Destroy and deallocate the key pointed to by 'key'.

    Prototype

      sword OCIThreadKeyDestroy(dvoid *hndl, OCIError *err, 
                                OCIThreadKey **key);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        key(IN/OUT):  The 'OCIThreadKey' in which to destroy the key.
 
    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      This is different from the destructor function callback passed to the
      key create routine.  This new destroy function 'OCIThreadKeyDestroy' is 
      used to terminate any resources OCI THREAD acquired when it created 
      'key'.  [The 'OCIThreadKeyDestFunc' callback type is a key VALUE 
      destructor; it does in no way operate on the key itself.]

      This must be called once the user has finished using the key.  Not
      calling the key destroy function may result in memory leaks.




1.2.2.2 Thread Key operations
-------------------------------

  OCIThreadKeyGet - OCIThread KeY Get value
  -----------------------------------------

    Description

      This gets the calling thread's current value for a key.

    Prototype

      sword OCIThreadKeyGet(dvoid *hndl, OCIError *err, OCIThreadKey *key,
                            dvoid **pValue);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        key(IN):          The key.

        pValue(IN/OUT):   The location in which to place the thread-specific
                          key value.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is illegal to use this function on a key that has not been created
      using 'OCIThreadKeyInit()'.

      If the calling thread has not yet assigned a value to the key, 'NULL' is
      placed in the location pointed to by 'pValue'.


  OCIThreadKeySet - OCIThread KeY Set value
  -----------------------------------------

    Description

      This sets the calling thread's value for a key.

    Prototype

      sword OCIThreadKeySet(dvoid *hndl, OCIError *err, OCIThreadKey *key,
                            dvoid *value);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        key(IN/OUT): The key.

        value(IN):   The thread-specific value to set in the key.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      It is illegal to use this function on a key that has not been created
      using 'OCIThreadKeyInit()'.

1.2.2.3  Thread Id
--------------------

  OCIThreadIdInit - OCIThread Thread Id INITialize
  --------------------------------------------------

    Description

      Allocate and initialize the thread id 'tid'.

    Prototype

      sword OCIThreadIdInit(dvoid *hndl, OCIError *err, OCIThreadId **tid);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tid (OUT):   Pointer to the thread ID to initialize.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.


  OCIThreadIdDestroy - OCIThread Thread Id DESTROY
  --------------------------------------------------

    Description

      Destroy and deallocate the thread id 'tid'.

    Prototype

      sword OCIThreadIdDestroy(dvoid *hndl, OCIError *err, OCIThreadId **tid);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tid(IN/OUT):        Pointer to the thread ID to destroy.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Note

      'tid' should be initialized by OCIThreadIdInit().


  OCIThreadIdSet - OCIThread Thread Id Set
  -----------------------------------------

    Description

      This sets one 'OCIThreadId' to another.

    Prototype

      sword OCIThreadIdSet(dvoid *hndl, OCIError *err, 
                           OCIThreadId *tidDest,
                           OCIThreadId *tidSrc);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tidDest(OUT):   This should point to the location of the 'OCIThreadId'
                        to be set to.

        tidSrc(IN):     This should point to the 'OCIThreadId' to set from.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      'tid' should be initialized by OCIThreadIdInit().


  OCIThreadIdSetNull - OCIThread Thread Id Set Null
  ---------------------------------------------------------

    Description

      This sets the NULL thread ID to a given 'OCIThreadId'.

    Prototype

      sword OCIThreadIdSetNull(dvoid *hndl, OCIError *err,
                               OCIThreadId *tid);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error, it is 
                     recorded in err and this function returns OCI_ERROR.  
                     Diagnostic information can be obtained by calling 
                     OCIErrorGet().

        tid(OUT):    This should point to the 'OCIThreadId' in which to put
                     the NULL thread ID.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      'tid' should be initialized by OCIThreadIdInit().


  OCIThreadIdGet - OCIThread Thread Id Get
  ------------------------------------------

    Description

      This retrieves the 'OCIThreadId' of the thread in which it is called.

    Prototype

      sword OCIThreadIdGet(dvoid *hndl, OCIError *err,
                           OCIThreadId *tid);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tid(OUT):    This should point to the location in which to place the
                     ID of the calling thread.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      'tid' should be initialized by OCIThreadIdInit().

      When OCIThread is used in a single-threaded environment,
      OCIThreadIdGet() will always place the same value in the location
      pointed to by 'tid'.  The exact value itself is not important.  The
      important thing is that it is not the same as the NULL thread ID and
      that it is always the same value.


  OCIThreadIdSame - OCIThread Thread Ids Same?
  ----------------------------------------------

    Description

      This determines whether or not two 'OCIThreadId's represent the same
      thread.

    Prototype

      sword OCIThreadIdSame(dvoid *hndl, OCIError *err,
                            OCIThreadId *tid1, OCIThreadId *tid2,
                            boolean *result);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tid1(IN):   Pointer to the first 'OCIThreadId'.

        tid2(IN):   Pointer to the second 'OCIThreadId'.
	
	result(IN/OUT): Pointer to the result.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      If 'tid1' and 'tid2' represent the same thread, 'result' is set to TRUE.
      Otherwise, 'result' is set to FALSE.

      'result' is set to TRUE if both 'tid1' and 'tid2' are the NULL thread ID.

      'ti1d' and 'tid2' should be initialized by OCIThreadIdInit().


  OCIThreadIdNull - OCIThread Thread Id NULL?
  ---------------------------------------------

    Description

      This determines whether or not a given 'OCIThreadId' is the NULL thread
      ID.

    Prototype

      sword OCIThreadIdNull(dvoid *hndl, OCIError *err,
                            OCIThreadId *tid,
                            boolean *result);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tid(IN):    Pointer to the 'OCIThreadId' to check.

	result(IN/OUT): Pointer to the result.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      If 'tid' is the NULL thread ID, 'result' is set to TRUE.  Otherwise, 
      'result' is set to FALSE.

      'tid' should be initialized by OCIThreadIdInit().


1.3 Active Threading Primitives
=================================

The active threading primitives deal with the manipulation of actual
threads.  Because the specifications of most of these primitives require
that it be possible to have multiple threads, they work correctly only in
the enabled OCIThread; In the disabled OCIThread, they always return
failure.  The exception is OCIThreadHandleGet(); it may be called in a 
single-threaded environment, in which case it will have no effect.

Active primitives should only be called by code running in a multi-threaded
environment.  You can call OCIThreadIsMulti() to determine whether the 
environment is multi-threaded or single-threaded.


1.3.1  Types
--------------

1.3.1.1    OCIThreadHandle - OCIThread Thread Handle
------------------------------------------------------

  Type 'OCIThreadHandle' is used to manipulate a thread in the active
  primitives:  OCIThreadJoin()and OCIThreadClose().  A thread handle opened by
  OCIThreadCreate() must be closed in a matching call to
  OCIThreadClose().  A thread handle is invalid after the call to
  OCIThreadClose().

  The distinction between a thread ID and a thread handle in OCIThread usage
  follows the distinction between the thread ID and the thread handle on
  Windows NT.  On many platforms, the underlying native types are the same.


1.3.2  Functions
------------------

  OCIThreadHndInit - OCIThread HaNDle Initialize
  ----------------------------------------------

    Description

      Allocate and initialize the thread handle.

    Prototype

      sword OCIThreadHndInit(dvoid *hndl, OCIError *err, 
                             OCIThreadHandle **thnd);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        thnd(OUT):   The address of pointer to the thread handle to initialize.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.


  OCIThreadHndDestroy - OCIThread HaNDle Destroy
  ----------------------------------------------

    Description

      Destroy and deallocate the thread handle.

    Prototype

      sword OCIThreadHndDestroy(dvoid *hndl, OCIError *err, 
                                OCIThreadHandle **thnd);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        thnd(IN/OUT):  The address of pointer to the thread handle to destroy.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      'thnd' should be initialized by OCIThreadHndInit().


  OCIThreadCreate - OCIThread Thread Create
  -----------------------------------------

    Description

      This creates a new thread.

    Prototype

      sword OCIThreadCreate(dvoid *hndl, OCIError *err,
                            void (*start)(dvoid *), dvoid *arg,
                            OCIThreadId *tid, OCIThreadHandle *tHnd);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        start(IN):    The function in which the new thread should begin
                      execution.

        arg(IN):      The argument to give the function pointed to by 'start'.

        tid(IN/OUT):  If not NULL, gets the ID for the new thread.

        tHnd(IN/OUT): If not NULL, gets the handle for the new thread.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      The new thread will start by executing a call to the function pointed
      to by 'start' with the argument given by 'arg'.  When that function
      returns, the new thread will terminate.  The function should not
      return a value and should accept one parameter, a 'dvoid *'.

      The call to OCIThreadCreate() must be matched by a call to
      OCIThreadClose() if and only if tHnd is non-NULL.

      If tHnd is NULL, a thread ID placed in *tid will not be valid in the
      calling thread because the timing of the spawned thread's termination
      is unknown.

      'tid' should be initialized by OCIThreadIdInit().

      'thnd' should be initialized by OCIThreadHndInit().



  OCIThreadJoin - OCIThread Thread Join
  -------------------------------------

    Description

      This function allows the calling thread to 'join' with another thread.
      It blocks the caller until the specified thread terminates.

    Prototype

      sword OCIThreadJoin(dvoid *hndl, OCIError *err, OCIThreadHandle *tHnd);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tHnd(IN):    The 'OCIThreadHandle' of the thread to join with.

    Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      'thnd' should be initialized by OCIThreadHndInit().

      The result of multiple threads all trying to join with the same thread is
      undefined.


  OCIThreadClose - OCIThread Thread Close
  ---------------------------------------

   Description

     This function should be called to close a thread handle.

   Prototype

     sword OCIThreadClose(dvoid *hndl, OCIError *err, OCIThreadHandle *tHnd);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tHnd(IN/OUT):    The OCIThread thread handle to close.

   Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

   Notes

      'thnd' should be initialized by OCIThreadHndInit().

      Both thread handle and the thread ID that was returned by the same call
      to OCIThreadCreate() are invalid after the call to OCIThreadClose().



  OCIThreadHandleGet - OCIThread Thread Get Handle
  ------------------------------------------------

    Description

      Retrieve the 'OCIThreadHandle' of the thread in which it is called.

    Prototype

      sword OCIThreadHandleGet(dvoid *hndl, OCIError *err,
                               OCIThreadHandle *tHnd);

        hndl(IN/OUT): The OCI environment or session handle.

        err(IN/OUT): The OCI error handle.  If there is an error and OCI_ERROR
                     is returned, the error is recorded in err and diagnostic 
                     information can be obtained by calling OCIErrorGet().

        tHnd(IN/OUT):      If not NULL, the location to place the thread
                           handle for the thread.

     Returns

      OCI_SUCCESS, OCI_ERROR or OCI_INVALID_HANDLE.

    Notes

      'thnd' should be initialized by OCIThreadHndInit().   

      The thread handle 'tHnd' retrieved by this function must be closed 
      with OCIThreadClose() and destroyed by OCIThreadHndDestroy() after it
      is used.




1.4 Using OCIThread
=====================

This section summarizes some of the more important details relating to the use
of OCIThread.

  * Process initialization

    OCIThread only requires that the process initialization function
    ('OCIThreadProcessInit()') be called when OCIThread is being used in a
    multi-threaded application.  Failing to call 'OCIThreadProcessInit()' in
    a single-threaded application is not an error.

  * OCIThread initialization

    Separate calls to 'OCIThreadInit()' will all return the same OCIThread
    context.

    Also, remember that each call to 'OCIThreadInit()' must eventually be
    matched by a call to 'OCIThreadTerm()'.

  * Active vs. Passive Threading primitives

    OCIThread client code written without using any active primitives can be
    compiled and used without change on both single-threaded and
    multi-threaded platforms.

    OCIThread client code written using active primitives will only work
    correctly on multi-threaded platforms.  In order to write a version of the
    same application to run on single-threaded platform, it is necessary to 
    branch the your code, whether by branching versions of the source file or 
    by branching at runtime with the OCIThreadIsMulti() call.

******************************************************************************/

/*****************************************************************************
                         ACTUAL PROTOTYPE DECLARATIONS
******************************************************************************/

void OCIThreadProcessInit();

sword OCIThreadInit(dvoid *hndl, OCIError *err);

sword OCIThreadTerm(dvoid *hndl, OCIError *err);

boolean OCIThreadIsMulti();

sword OCIThreadMutexInit(dvoid *hndl, OCIError *err,  
                         OCIThreadMutex **mutex); 

sword OCIThreadMutexDestroy(dvoid *hndl, OCIError *err, 
                            OCIThreadMutex **mutex);

sword OCIThreadMutexAcquire(dvoid *hndl, OCIError *err,
                            OCIThreadMutex *mutex);

sword OCIThreadMutexRelease(dvoid *hndl, OCIError *err,
                            OCIThreadMutex *mutex);

sword OCIThreadKeyInit(dvoid *hndl, OCIError *err, OCIThreadKey **key,
                       OCIThreadKeyDestFunc destFn);

sword OCIThreadKeyDestroy(dvoid *hndl, OCIError *err, 
                          OCIThreadKey **key);

sword OCIThreadKeyGet(dvoid *hndl, OCIError *err, OCIThreadKey *key,
                      dvoid **pValue);

sword OCIThreadKeySet(dvoid *hndl, OCIError *err, OCIThreadKey *key,
                      dvoid *value);

sword OCIThreadIdInit(dvoid *hndl, OCIError *err, OCIThreadId **tid);

sword OCIThreadIdDestroy(dvoid *hndl, OCIError *err, OCIThreadId **tid);

sword OCIThreadIdSet(dvoid *hndl, OCIError *err, 
                     OCIThreadId *tidDest, OCIThreadId *tidSrc);

sword OCIThreadIdSetNull(dvoid *hndl, OCIError *err, OCIThreadId *tid);

sword OCIThreadIdGet(dvoid *hndl, OCIError *err, OCIThreadId *tid);

sword OCIThreadIdSame(dvoid *hndl, OCIError *err,
                      OCIThreadId *tid1, OCIThreadId *tid2,
                      boolean *result);

sword OCIThreadIdNull(dvoid *hndl, OCIError *err,
                      OCIThreadId *tid, boolean *result);

sword OCIThreadHndInit(dvoid *hndl, OCIError *err, OCIThreadHandle **thnd);

sword OCIThreadHndDestroy(dvoid *hndl, OCIError *err, OCIThreadHandle **thnd);

sword OCIThreadCreate(dvoid *hndl, OCIError *err,
                      void (*start)(dvoid *), dvoid *arg,
                      OCIThreadId *tid, OCIThreadHandle *tHnd);

sword OCIThreadJoin(dvoid *hndl, OCIError *err, OCIThreadHandle *tHnd);

sword OCIThreadClose(dvoid *hndl, OCIError *err, OCIThreadHandle *tHnd);

sword OCIThreadHandleGet(dvoid *hndl, OCIError *err, OCIThreadHandle *tHnd);
/*----------------- End OCI Thread interface Extensions ---------------------*/

/*--------------- Begin OCI Client Notification Interfaces ------------------*/

typedef ub4 (*OCISubscriptionNotify)(dvoid *ctx, OCISubscription *subscrhp, 
				     dvoid *pay, ub4 payl, 
				     dvoid *desc, ub4 mode);

sword OCISubscriptionRegister(OCISvcCtx *svchp, OCISubscription **subscrhpp, 
			      ub2 count, OCIError *errhp, ub4 mode);


sword OCISubscriptionPost(OCISvcCtx *svchp, OCISubscription **subscrhpp, 
			      ub2 count, OCIError *errhp, ub4 mode);

sword OCISubscriptionUnRegister(OCISvcCtx *svchp, OCISubscription *subscrhp, 
			      OCIError *errhp, ub4 mode);

sword OCISubscriptionDisable(OCISubscription *subscrhp, 
			   OCIError *errhp, ub4 mode);

sword OCISubscriptionEnable(OCISubscription *subscrhp, 
			  OCIError *errhp, ub4 mode);

/*------------------- End OCI Publish/Subscribe Interfaces ------------------*/

/*----------------- Extensions to Datetime interfaces -----------------------*/
/*--------------------- Actual Prototypes -----------------------------------*/
sword OCIDateTimeGetTime(dvoid *hndl, OCIError *err, OCIDateTime *datetime, 
			 ub1 *hr, ub1 *mm, ub1 *ss, ub4 *fsec);

sword OCIDateTimeGetDate(dvoid *hndl, OCIError *err,  CONST OCIDateTime *date, 
		 sb2 *yr, ub1 *mnth, ub1 *dy );

sword OCIDateTimeGetTimeZoneOffset(dvoid *hndl,OCIError *err,
		  CONST OCIDateTime   *datetime,sb1 *hr,sb1 *mm);

sword OCIDateTimeConstruct(dvoid  *hndl,OCIError *err,OCIDateTime *datetime,
               sb2 yr,ub1 mnth,ub1 dy,ub1 hr,ub1 mm,ub1 ss,ub4 fsec,
               OraText  *timezone,size_t timezone_length);

sword OCIDateTimeSysTimeStamp(dvoid *hndl, OCIError *err, 
			      OCIDateTime *sys_date );

sword OCIDateTimeAssign(dvoid *hndl, OCIError *err, CONST OCIDateTime *from, 
		       OCIDateTime *to);

sword OCIDateTimeToText(dvoid *hndl, OCIError *err, CONST OCIDateTime *date, 
                        CONST OraText *fmt, ub1 fmt_length, ub1 fsprec, 
                        CONST OraText *lang_name, size_t lang_length, 
                        size_t *buf_size, OraText *buf );

sword OCIDateTimeFromText(dvoid *hndl, OCIError *err, CONST OraText *date_str, 
             size_t dstr_length, CONST OraText *fmt, ub1 fmt_length,
             CONST OraText *lang_name, size_t lang_length, OCIDateTime *date );

sword OCIDateTimeCompare(dvoid *hndl, OCIError *err, CONST OCIDateTime *date1, 
                     CONST OCIDateTime *date2,  sword *result );

sword OCIDateTimeCheck(dvoid *hndl, OCIError *err, CONST OCIDateTime *date, 
		 ub4 *valid );

sword OCIDateTimeConvert(dvoid *hndl, OCIError *err, OCIDateTime *indate,
				OCIDateTime *outdate);

sword OCIDateTimeSubtract(dvoid *hndl, OCIError *err, OCIDateTime *indate1, 
		OCIDateTime *indate2,	 OCIInterval *inter);

sword OCIDateTimeIntervalAdd(dvoid *hndl, OCIError *err, OCIDateTime *datetime,
	 OCIInterval *inter, OCIDateTime *outdatetime);

sword OCIDateTimeIntervalSub(dvoid *hndl, OCIError *err, OCIDateTime *datetime,
	 OCIInterval *inter, OCIDateTime *outdatetime);

sword OCIIntervalSubtract(dvoid *hndl, OCIError *err, OCIInterval *minuend, 
			    OCIInterval *subtrahend, OCIInterval *result );

sword OCIIntervalAdd(dvoid *hndl, OCIError *err, OCIInterval *addend1, 
			OCIInterval *addend2, OCIInterval *result );

sword OCIIntervalMultiply(dvoid *hndl, OCIError *err, CONST OCIInterval *inter,
			OCINumber *nfactor, OCIInterval *result );

sword OCIIntervalDivide(dvoid *hndl, OCIError *err, OCIInterval *dividend, 
		OCINumber *divisor, OCIInterval *result );

sword OCIIntervalCompare(dvoid *hndl, OCIError *err, OCIInterval *inter1, 
			OCIInterval *inter2, sword *result );

sword OCIIntervalFromNumber(dvoid *hndl, OCIError *err, OCIInterval *inter, 
			OCINumber *number);

sword OCIIntervalFromText( dvoid *hndl, OCIError *err, CONST OraText *inpstr, 
		size_t str_len, OCIInterval *result );

sword OCIIntervalToText( dvoid *hndl, OCIError *err, CONST OCIInterval *inter,
                         ub1 lfprec, ub1 fsprec, 
			OraText *buffer, size_t buflen, size_t *resultlen );

sword OCIIntervalToNumber(dvoid *hndl, OCIError *err,const OCIInterval *inter, 
			OCINumber *number);

sword OCIIntervalCheck(dvoid *hndl, OCIError *err, CONST OCIInterval *interval,
			 ub4 *valid );

sword OCIIntervalAssign(dvoid *hndl, OCIError *err, CONST OCIInterval *ininter,
			OCIInterval *outinter );

sword OCIIntervalSetYearMonth(dvoid *hndl, OCIError *err, sb4 yr, sb4 mnth,
                        OCIInterval *result );

sword OCIIntervalGetYearMonth(dvoid *hndl, OCIError *err, sb4 *yr, sb4 *mnth,
                        CONST OCIInterval *result );

sword OCIIntervalSetDaySecond(dvoid *hndl, OCIError *err, sb4 dy, sb4 hr,
                        sb4 mm, sb4 ss, sb4 fsec, OCIInterval *result );

sword OCIIntervalGetDaySecond(dvoid *hndl, OCIError *err, sb4 *dy, sb4 *hr,
                     sb4 *mm, sb4 *ss, sb4 *fsec, CONST OCIInterval *result );

/*----------------- End Datetime interface Extensions -----------------------*/


/*---------------------------------------------------------------------------
                          PRIVATE FUNCTIONS
  ---------------------------------------------------------------------------*/

            /* the following functions are depracated and should not be used */
#ifdef NEVER
sword   OCIStmtBindByPos  (OCIStmt *stmtp, OCIBind *bindp, OCIError *errhp,
                  ub4 position, dvoid *valuep, sb4 value_sz,
                  ub2 dty, dvoid *indp, ub2 *alenp, ub2 *rcodep,
                  ub4 maxarr_len, ub4 *curelep, ub4 mode);


sword   OCIStmtBindByName  (OCIStmt *stmtp, OCIBind *bindp, OCIError *errhp,
                  CONST OraText *placeholder, sb4 placeh_len, dvoid *valuep, 
                  sb4 value_sz, ub2 dty, dvoid *indp, ub2 *alenp, 
                  ub2 *rcodep, ub4 maxarr_len, ub4 *curelep, ub4 mode);

sword   ocidefn  (OCIStmt *stmtp, OCIDefine *defnp, OCIError *errhp,
                  ub4 position, dvoid *valuep, sb4 value_sz, ub2 dty,
                  dvoid *indp, ub2 *rlenp, ub2 *rcodep, ub4 mode);
#endif /* NEVER */

#endif                                                       /* OCIAP_ORACLE */
