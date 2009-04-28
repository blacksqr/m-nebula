//--------------------------------------------------------------------
//  odetests.cc - Test suite for OdePhysics module.
//
//  (C) 2003 Vadim Macagon
//--------------------------------------------------------------------

#include "kernel/nkernelserver.h"
//#include "lua/nluaserver.h"
#include "script/ntclserver.h"
#include "kernel/nfileserver2.h"

int main( int argc, char **argv )
{
  int retval = 0;
  //char* scriptServer = "nluaserver";
  char* scriptServer = "ntclserver";
  char* fileServer = "nfileserver2";
    
  nKernelServer* ks = new nKernelServer();
  if ( ks )
  {
    // create a fileserver2
    if ( ks->New( fileServer, "/sys/servers/file2" ) )
    {
      // create script server
      //nScriptServer* ss = (nLuaServer*)ks->New( scriptServer, "/sys/servers/script" );
	  nScriptServer* ss = (nTclServer*)ks->New( scriptServer, "/sys/servers/script" );
      if ( ss )
      { 
        const char* res = 0;
        //ss->RunScript( "../../data/odetests/start.lua", res );
		ss->RunScript( "../../data/odetests/start.tcl", res );
        if ( res ) 
          printf( "%s\n", res );
        ss->Release();
      }
      else
      {
        retval = 10;
        printf( "Could not create script server object!\n" );
      }
    }
    else 
    {
      retval = 10;
      printf( "Could not create file server 2 object!\n" );
    }
    delete ks;
  }
  else
  {
    retval = 10;
    printf( "Could not create nKernelServer object!\n" );
  }
  getchar();
  return retval;
}
