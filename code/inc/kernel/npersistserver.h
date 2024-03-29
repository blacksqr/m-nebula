#ifndef N_PERSISTSERVER_H
#define N_PERSISTSERVER_H
//------------------------------------------------------------------------------
/**
    OVERVIEW
    Interface to the file system for serializing objects.

    Nebula Persistent Object File Format
    ------------------------------------
    The actual format of serialized objects is not specified.
    Instead script server objects are used to translate nCmd objects
    emitted by objects into actual script statements that will
    be saved to files. However, since the file server needs to
    know which script server to use for reading serialized object
    files, there has to be some meta information in the file
    itself which names the class of the script server which 
    understands it.

    Within the first 128 Bytes of a serialized object file the 
    following strings must be embedded:

    $parser:classname$

    Where 'classname' is the script server's class which has
    to be used to read that file. 

    *** NOTE *** UNFOLDED OBJECTS ARE NO LONGER SUPPORTED FOR
    WRITING, AND READING IS ONLY SUPPORTED FOR "FLAT" FILE
    HIERARCHIES (i.e. a "nKernelServer::Load("bla.n") is allowed
    if bla.n is a directory, and contains a _main file).

    Folded and unfolded objects
    ---------------------------
    Serialized objects can be saved in 2 ways, folded and unfolded.
    Folded means, the object itself and all of it's children will
    be saved into a single file. Unfolded means that for each
    object, a directory will be create in the host filesystem,
    so that a saved object hierarchy will result in a
    corresponding directory hierarchie.

    (C) 2002 A.Weissflog
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_STACK_H
#include "util/nstack.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_DYNAUTOREF_H
#include "kernel/ndynautoref.h"
#endif

#undef N_DEFINES
#define N_DEFINES nPersistServer
#include "kernel/ndefdllclass.h"

//------------------------------------------------------------------------------
class nScriptServer;
class nFileServer2;
class nFile;
class N_PUBLIC nPersistServer : public nRoot 
{
public:
    static nKernelServer* kernelServer;
 
	/// Savemodes
    enum nSaveMode 
    {
        SAVEMODE_FOLD,  
        SAVEMODE_CLONE,  
    };

	/// constructor
    nPersistServer();
	/// destructor
    virtual ~nPersistServer();
    /// set the save mode
    void SetSaveMode(nSaveMode);
    /// get the save mode
    nSaveMode GetSaveMode(void);
    /// set the save level
    void SetSaveLevel(int);
    /// get the save level
    int GetSaveLevel(void);
    /// set the script server class which should be used for saving
    bool SetSaverClass(const char* saverClass);
    /// get the script server class which is currently used for saving
    const char* GetSaverClass();
    /// begin a persistent object
    bool BeginObject(nRoot *, const char *);
    /// begin a persistent object with constructor command
    bool BeginObjectWithCmd(nRoot *, nCmd *, const char *);
    /// create a nCmd object
    nCmd *GetCmd(nRoot *, uint id);
    /// put a cmd into persistency stream
    bool PutCmd(nCmd *);
    /// put a cmd into persistency stream if savelevel permits
    bool PutCmd(int, nCmd *);
    /// finish a persistent object
    bool EndObject(void);
    /// return cloned object
    nRoot *GetClone(void);
    /// load a object from persistency stream
    nRoot *LoadObject(const char *name);
    /// runs a persistency scipt against the provided nRoot*
    bool ParseFile(nRoot *o, const char *fname);
private:   
    /// internal
    bool BeginFoldedObject(nRoot *obj, nCmd *, const char *name, bool sel_only);
    /// internal
    bool BeginCloneObject(nRoot *obj, const char *name);
    /// internal
    bool EndFoldedObject(void);
    /// internal
    bool EndCloneObject(void);
    /// internal
    nRoot *LoadFoldedObject(const char *fname, const char *obj_name);
    /// internal
    char *ReadEmbeddedString(const char *fname, const char *keyword, char *buf, int buf_size);
    /// internal
    nFile * PutFoldedObjectHeader(nScriptServer *saver, const char *fname, nRoot* obj);
    /// get a valid loader script server from class name
    nScriptServer* GetLoader(const char* loaderClass);

    nAutoRef<nFileServer2> refFileServer;
    nDynAutoRef<nScriptServer> refSaver;
    nFile *file;
    nStack<nRoot*> objectStack;
    nSaveMode saveMode;     
    
    nRoot *cloneTarget;
    nRoot *origCwd;

    int saveLevel;
};
//--------------------------------------------------------------------
#endif
