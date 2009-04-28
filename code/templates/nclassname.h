#ifndef N_CLASSNAME_H
#define N_CLASSNAME_H
//------------------------------------------------------------------------------
/**
    @class classname

    @brief a brief description of the class

    a detailed description of the class

    (C) 2001 RadonLabs GmbH
*/
#ifndef N_SUPERCLASSNAME_H
#include "subdir/nsuperclassname.h"
#endif

#undef N_DEFINES
#define N_DEFINES nClassName
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nClassName : public nSuperClassName
{
public:
    /// constructor
    nClassName();
    /// destructor
    virtual ~nClassName();
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};
//------------------------------------------------------------------------------
#endif
