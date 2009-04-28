#--------------------------------------------------------------------
#   classtemplates.tcl
#	----------------
#   templates for classbuilder.tcl
#
#   (C)	2002	leaf
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#   nclassname.h
#--------------------------------------------------------------------
set header "#ifndef $N_CLASSNAME_H
#define $N_CLASSNAME_H
//------------------------------------------------------------------------------
/**
    @class $nClassName

    @brief a brief description of the class

    a detailed description of the class

    (C)	$year	$user
*/
#ifndef $N_SUPERCLASSNAME_H
#include \"$superdir/$nsuperclassname.h\"
#endif

#undef N_DEFINES
#define N_DEFINES $nClassName
#include \"kernel/ndefdllclass.h\"

//------------------------------------------------------------------------------
class $nClassName : public $nSuperClassName
{
public:
    /// constructor
    $nClassName\();
    /// destructor
    virtual ~$nClassName\();
    /// persistency
    virtual bool SaveCmds(nPersistServer* persistServer);

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;
};
//------------------------------------------------------------------------------
#endif
"

#--------------------------------------------------------------------
#   nclassname_cmds.cc
#--------------------------------------------------------------------
set cmds "#define N_IMPLEMENTS $nClassName
//------------------------------------------------------------------------------
//  (C) $year	$user
//------------------------------------------------------------------------------
#include \"$subdir/$nclassname.h\"
#include \"kernel/npersistserver.h\"

static void n_xxx(void* slf, nCmd* cmd);

//------------------------------------------------------------------------------
/**
    @scriptclass
    $nclassname
    
    @superclass
    $nsuperclassname

    @classinfo
    A detailed description of what the class does (written for script programmers!) 
*/
void
n_initcmds(nClass* clazz)
{
    clazz->BeginCmds();
    clazz->AddCmd(\"v_xxx_v\", 'XXXX', n_xxx);
    clazz->EndCmds();
}

//------------------------------------------------------------------------------
/**
    @cmd
    xxx

    @input
    v

    @output
    v

    @info
    detailed description (for script programmers! no C++ babble)
*/
static
void
n_xxx(void* slf, nCmd* cmd)
{
    $nClassName\* self = ($nClassName\*) slf;
    self->XXX();
}

//------------------------------------------------------------------------------
/**
    @param  ps          writes the nCmd object contents out to a file.
    @return             success or failure
*/
bool
$nClassName\::SaveCmds(nPersistServer* ps)
{
    if ($nSuperClassName\::SaveCmds(ps))
    {
        nCmd* cmd = ps->GetCmd(this, 'XXXX');
        ps->PutCmd(cmd);

        return true;
    }
    return false;
}
"
#--------------------------------------------------------------------
#   nclassname_main.h
#--------------------------------------------------------------------
set main "#define N_IMPLEMENTS $nClassName
//------------------------------------------------------------------------------
//  (C) $year	$user
//------------------------------------------------------------------------------
#include \"$subdir/$nclassname.h\"

nNebulaScriptClass($nClassName, \"$nsuperclassname\");

//------------------------------------------------------------------------------
/**
*/
$nClassName\::$nClassName\()
{
}

//------------------------------------------------------------------------------
/**
*/
$nClassName\::~$nClassName\()
{
}
"

#--------------------------------------------------------------------
#   Package File
#--------------------------------------------------------------------
set pakfile "#--------------------------------------------------------------------
#   $npackagename.pak
#   ============
#   Description of your class/package
#
#   $date   $user	created
#--------------------------------------------------------------------

#--------------------------------------------------------------------
beginmodule $nclassname
    setdir $subdir
    setfiles {
        $nclassname\_main $nclassname\_cmds
    }
    setheaders {
        $nclassname
    }
endmodule

#--------------------------------------------------------------------
begintarget $npackagename
    settype $targettype
    setmods {
        $nclassname
    }
    setdepends { $depends }
    setlibs_win32 { $win32libs }
    setlibs_unix  { $unixlibs}
endtarget

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
"        

#--------------------------------------------------------------------
#   EOF
#--------------------------------------------------------------------
