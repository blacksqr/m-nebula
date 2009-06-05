#define N_IMPLEMENTS nBlob
//--------------------------------------------------------------------
//  IMPLEMENTATION
//  nblob.cc
//  (C) 1999 A.Weissflog
//--------------------------------------------------------------------
#include "kernel/nkernelserver.h"
#include "misc/nblob.h"


nNebulaClass(nBlob, "nroot");
//--------------------------------------------------------------------
/**
    - 07-Feb-99   floh    created
*/
//--------------------------------------------------------------------
nBlob::nBlob()
{
    this->blob = NULL;
    this->size = 0;
}

//--------------------------------------------------------------------
/**
    - 07-Feb-99   floh    created
*/
//--------------------------------------------------------------------
nBlob::~nBlob()
{
    if (this->blob) n_free(this->blob);
}

//--------------------------------------------------------------------
/**
    @brief Copy the data from the address given.

    - 07-Feb-99   floh    created
    - 15-May-00   floh    + bugfix: if null pointer blob came in,
                            previous blob pointer has not been 
                            set to NULL
*/
//--------------------------------------------------------------------
bool nBlob::Set(void *b, int s)
{
    bool retval = true;
    if (this->blob) {
        n_free(this->blob);
        this->blob = NULL;
    }
    if (b) {
        this->blob = n_malloc(s);
        if (this->blob) memcpy(this->blob,b,s);
        else            retval = false;
    }
    return retval;
}

//--------------------------------------------------------------------
/**
    @brief Return the pointer to the stored data.
    
    - 07-Feb-99   floh    created
*/
//--------------------------------------------------------------------
void *nBlob::Get(void)
{
    return this->blob;
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

