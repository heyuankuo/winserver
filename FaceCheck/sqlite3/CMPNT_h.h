

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jun 22 18:12:21 2017
 */
/* Compiler settings for CMPNT.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CMPNT_h_h__
#define __CMPNT_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFaceDBOS_FWD_DEFINED__
#define __IFaceDBOS_FWD_DEFINED__
typedef interface IFaceDBOS IFaceDBOS;

#endif 	/* __IFaceDBOS_FWD_DEFINED__ */


#ifndef __HykSqliteSys_FWD_DEFINED__
#define __HykSqliteSys_FWD_DEFINED__

#ifdef __cplusplus
typedef class HykSqliteSys HykSqliteSys;
#else
typedef struct HykSqliteSys HykSqliteSys;
#endif /* __cplusplus */

#endif 	/* __HykSqliteSys_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_CMPNT_0000_0000 */
/* [local] */ 

typedef /* [public][public][public][public] */ struct __MIDL___MIDL_itf_CMPNT_0000_0000_0001
    {
    unsigned char _name[ 16 ];
    unsigned char _gender[ 8 ];
    int _id;
    int _feat_len;
    BYTE *_feats;
    } 	UserInfo;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_CMPNT_0000_0000_0002
    {
    UserInfo *first_user[ 100 ];
    int sum_users;
    } 	UserInfos;



extern RPC_IF_HANDLE __MIDL_itf_CMPNT_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_CMPNT_0000_0000_v0_0_s_ifspec;

#ifndef __IFaceDBOS_INTERFACE_DEFINED__
#define __IFaceDBOS_INTERFACE_DEFINED__

/* interface IFaceDBOS */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IFaceDBOS;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("32bb8325-b41b-11cf-a6bb-0080c7b2d682")
    IFaceDBOS : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryFace( 
            /* [out] */ UserInfos **users) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InserData( 
            /* [in] */ UserInfo *user,
            /* [in] */ int recoder_id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LoadData( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFaceDBOSVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFaceDBOS * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFaceDBOS * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFaceDBOS * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryFace )( 
            IFaceDBOS * This,
            /* [out] */ UserInfos **users);
        
        HRESULT ( STDMETHODCALLTYPE *InserData )( 
            IFaceDBOS * This,
            /* [in] */ UserInfo *user,
            /* [in] */ int recoder_id);
        
        HRESULT ( STDMETHODCALLTYPE *LoadData )( 
            IFaceDBOS * This);
        
        END_INTERFACE
    } IFaceDBOSVtbl;

    interface IFaceDBOS
    {
        CONST_VTBL struct IFaceDBOSVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFaceDBOS_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFaceDBOS_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFaceDBOS_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFaceDBOS_QueryFace(This,users)	\
    ( (This)->lpVtbl -> QueryFace(This,users) ) 

#define IFaceDBOS_InserData(This,user,recoder_id)	\
    ( (This)->lpVtbl -> InserData(This,user,recoder_id) ) 

#define IFaceDBOS_LoadData(This)	\
    ( (This)->lpVtbl -> LoadData(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFaceDBOS_INTERFACE_DEFINED__ */



#ifndef __ServerLib_LIBRARY_DEFINED__
#define __ServerLib_LIBRARY_DEFINED__

/* library ServerLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ServerLib;

EXTERN_C const CLSID CLSID_HykSqliteSys;

#ifdef __cplusplus

class DECLSPEC_UUID("0c092c2b-882c-11cf-a6bb-0080c7b2d682")
HykSqliteSys;
#endif
#endif /* __ServerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


