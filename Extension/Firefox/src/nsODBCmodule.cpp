#include "mozilla/ModuleUtils.h"
#include "nsIClassInfoImpl.h"

//#include "nsIGenericFactory.h"
#include "nsODBCStorageService.h"

#if 0
//#include "nsWorkerUtils.h"

NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsODBCStorageService,
                                         nsODBCStorageService::GetSingleton)

//NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsWorkerUtils,
//                                         nsWorkerUtils::GetSingleton)

static nsModuleComponentInfo components[] =
{
    {
       IODBCSTORAGESERVICE_CLASSNAME, 
       IODBCSTORAGESERVICE_CID,
       IODBCSTORAGESERVICE_CONTRACTID,
       nsODBCStorageServiceConstructor,
    },

//    {
//
//       WEBSQLSERVICE_CLASSNAME, 
//       WEBSQLSERVICE_CID,
//       WEBSQLSERVICE_CONTRACTID,
//       nsWorkerUtilsConstructor,
//    }
};

NS_IMPL_NSGETMODULE("IODBCModule", components) 

#else


NS_GENERIC_FACTORY_SINGLETON_CONSTRUCTOR(nsODBCStorageService,
                                         nsODBCStorageService::GetSingleton)

// The following line defines a kNS_SAMPLE_CID CID variable.
NS_DEFINE_NAMED_CID(IODBCSTORAGESERVICE_CID);

// Build a table of ClassIDs (CIDs) which are implemented by this module. CIDs
// should be completely unique UUIDs.
// each entry has the form { CID, service, factoryproc, constructorproc }
// where factoryproc is usually NULL.
static const mozilla::Module::CIDEntry kIODBC_CIDs[] = {
   { &kIODBCSTORAGESERVICE_CID, false, NULL, nsODBCStorageServiceConstructor },
   { NULL }
};

// Build a table which maps contract IDs to CIDs.
// A contract is a string which identifies a particular set of functionality. In some
// cases an extension component may override the contract ID of a builtin gecko component
// to modify or extend functionality.
static const mozilla::Module::ContractIDEntry kIODBC_Contracts[] = {
    { IODBCSTORAGESERVICE_CONTRACTID, &kIODBCSTORAGESERVICE_CID },
    { NULL }
};


static const mozilla::Module kIODBC_Module = {
    mozilla::Module::kVersion,
    kIODBC_CIDs,
    kIODBC_Contracts,
    NULL
};

// The following line implements the one-and-only "NSModule" symbol exported from this
// shared library.
NSMODULE_DEFN(nsIODBC_Module) = &kIODBC_Module;

// The following line implements the one-and-only "NSGetModule" symbol
// for compatibility with mozilla 1.9.2. You should only use this
// if you need a binary which is backwards-compatible and if you use
// interfaces carefully across multiple versions.
NS_IMPL_MOZILLA192_NSGETMODULE(&kIODBC_Module)


#endif

