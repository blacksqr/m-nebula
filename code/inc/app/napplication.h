#ifndef N_APPLICATION_H
#define N_APPLICATION_H
//------------------------------------------------------------------------------
/**
    @class nApplication

    @application class, 
	provides "skeleton" for application
	creates infrastructure, required for run, such as logs
	provides API for "servers" maintenance, add and automatically removes in 
	reverse order (LIFO)

    (C)	2009	plushy
*/
#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_STL_H
#include "util/nstl.h"
#endif

#define LOG

#undef N_DEFINES
#define N_DEFINES nApplication
#include "kernel/ndefdllclass.h"

#ifdef LOG
#ifdef __WIN32__
class nWin32LogHandler;
#endif
class nFileLogHandler;
#endif
class nScriptServer;
//------------------------------------------------------------------------------
class N_PUBLIC nApplication : public nRoot
{
public:
    /// constructor
    nApplication();
    /// destructor
    virtual ~nApplication();

	virtual void Initialize(void);
    /// persistence
    virtual bool SaveCmds(nPersistServer* persistServer);

public:
    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

public:
	/** 
		Main loop function, 
		inside calls begin(), 
		if success "mainloop()" callback provided as parameter, called
		and the end() called when mainloop() exit.
	*/
	virtual int Run(int (*mainloop)(nKernelServer* ks, nApplication* app));

	nRoot* AddServer(nKernelServer* _ks, const char* class_name, const char* path)
	{
		nRoot* server = _ks->New(class_name, path);
		if (server)
			this->servers.push_back(server);
		return server;
	}
	/// Add servers in right order, they will be automatically removed at the destructor in LIFO order	
	template <class Type>
	nRoot* AddServer(nKernelServer* _ks, const char* class_name, const char* path, nAutoRef<Type>* ref = 0)		
	{	
		nRoot* server = _ks->New(class_name, path);
		if (server)
			this->servers.push_back(server);
		if (ref)
			(*ref) = path;
		return server;
	}
	
protected:
	//------------------------------------------------------------------------------
	/**
	  those 4 functions supposed to be called ...??? ##
	  from descendants virtual implementations
	*/
	/// provide and initialize calls here and don't forget to call ascendant initialize()
	virtual bool initialize();
	/// called in Run before mainloop callback
	virtual bool begin();
	/// called in Run after mainloop callback
	virtual void end();	
	/// processing command line for switches
	virtual bool process_command_line();
	/// running startup scripts
	virtual bool run_init_script();
	/// running startup scripts
	virtual bool run_startup_script();

protected:
	bool				in_begin;	
	bool				noinit;	
	
	std::list<nRoot*>	servers;
	stl_string			startup_script;	

	typedef std::map<stl_string, stl_string> str2str_map;
	str2str_map ext_script_map;

#ifdef LOG
#ifdef __WIN32__
	nWin32LogHandler* winlog;
#endif
	nFileLogHandler* filelog; 
#endif

public:
	nAutoRef<nScriptServer> refScript;
	bool					noremote;
	stl_string				app_name;
	stl_string				port_name;		
	stl_string				script_class;	
};


//------------------------------------------------------------------------------
#endif

