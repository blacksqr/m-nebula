#define N_IMPLEMENTS nTimeServer
#define N_KERNEL
//-------------------------------------------------------------------
//  ntime_main.cc
//  (C) 1998 Andre Weissflog
//-------------------------------------------------------------------
#include "kernel/ntimeserver.h"
#include "kernel/nscriptserver.h"

#if defined(__LINUX__) || defined(__MACOSX__)
#define tv2micro(x) ((long long int)x.tv_sec * (long long int)N_MICROSEC_INT + (long long int)x.tv_usec);
#endif

//-------------------------------------------------------------------
/**
     - 07-Dec-98   floh    created
     - 16-May-99   floh    DeltaT Handling
     - 12-Aug-99   floh    + WaitT Handling
     - 26-Jan-00   floh    + FrameTime Handling
*/
//-------------------------------------------------------------------
nTimeServer::nTimeServer() : refScriptServer(ks, this)
{
    this->stopped       = false;
	this->asleep		= true;
    this->frame_enabled = false; 
    this->time_stop     = 0;
    this->frame_time    = 0.0;
    this->frame_time_scale = 1.0f;
    this->lock_delta_t  = 0.0;
    this->wait_delta_t  = 0.0;
    this->lock_time     = 0.0;

	this->refScriptServer = "/sys/servers/script";

#   ifdef __WIN32__
    if (!QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_diff))) {
        n_warn("QueryPerformanceCounter() NOT SUPPORTED!!!");
    };    
#   elif defined(__LINUX__) || defined(__MACOSX__)
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
#   else
    n_error("Not implemented.");
#   endif
}

//-------------------------------------------------------------------
/**
     - 07-Dec-98   floh    created
*/
//-------------------------------------------------------------------
nTimeServer::~nTimeServer()
{
    // Profilers killen
    nProfiler *p;
    while ((p = (nProfiler*)this->prof_list.RemHead())) n_delete p;
	nTimer *tm;
	while ((tm = (nTimer*)this->timers_list.RemHead())) n_delete tm;
}

//-------------------------------------------------------------------
/**
     - 07-Dec-98   floh    created
*/
//-------------------------------------------------------------------
void nTimeServer::ResetTime(void)
{
    this->lock_time = 0;

#   ifdef __WIN32__
    QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_diff));
#   elif __LINUX__
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
#   else
    n_error("Not implemented.");
#   endif
}

//-------------------------------------------------------------------
/**
     - 01-Feb-99   floh    created
     - 18-May-01   floh    + also update "frame_time"
*/
//-------------------------------------------------------------------
void nTimeServer::SetTime(double t)
{
    this->lock_time = t;
    if (this->frame_enabled) this->frame_time = t;

#   ifdef __WIN32__
    // t nach Ticks umrechnen
    LONGLONG freq;
    if (QueryPerformanceFrequency((LARGE_INTEGER *)&freq)) {
        LONGLONG td = (LONGLONG) (t * ((double)freq));
        QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_diff));
        this->time_stop = this->time_diff;
        this->time_diff -= td;
    }
#   elif __LINUX__
    // t nach Microsecs umrechnen
    long long td = (long long int) (t * N_MICROSEC_FLOAT);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    this->time_diff = tv2micro(tv);
    this->time_stop = this->time_diff;
    this->time_diff -= td;
#   else
    n_error("Not implemented.");
#   endif
}

//-------------------------------------------------------------------
/**
     - 11-Jan-99   floh    created
*/
//-------------------------------------------------------------------
void nTimeServer::StopTime(void)
{
    if (this->stopped) {
        n_printf("Time already stopped!\n");
    } else {
        this->stopped = true;

#   ifdef __WIN32__
        QueryPerformanceCounter((LARGE_INTEGER *)&(this->time_stop));
#   elif __LINUX__
        struct timeval tv;
        gettimeofday(&tv,NULL);
        this->time_stop = tv2micro(tv);
#   else
        n_error("Not implemented.!");
#   endif
    }
}

//-------------------------------------------------------------------
/**
     - 11-Jan-99   floh    created
*/
//-------------------------------------------------------------------
void nTimeServer::StartTime(void)
{
    if (!this->stopped) {
        n_printf("Time already started!\n");
    } else {
        this->stopped = false;
        
#   ifdef __WIN32__
        LONGLONG time,td;
        QueryPerformanceCounter((LARGE_INTEGER *)&time);
        td = time - this->time_stop;
        this->time_diff += td;
#   elif __LINUX__
        long long int time;
        long long int td;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        time = tv2micro(tv);
        td = time - this->time_stop;
        this->time_diff += td;
#   else
        n_error("Not implemented.!");
#   endif
    }
}

//-------------------------------------------------------------------
/**
     - 07-Dec-98   floh    created
     - 16-May-99   floh    + LockDeltaT()-Handling
*/
//-------------------------------------------------------------------
double nTimeServer::GetTime(void)
{
    if (this->lock_delta_t > 0.0) return this->lock_time;
    else {
#       ifdef __WIN32__
        LONGLONG time,freq;
        if (QueryPerformanceCounter((LARGE_INTEGER *)&time)) {
            if (this->stopped) time = this->time_stop;
            if (QueryPerformanceFrequency((LARGE_INTEGER *)&freq)) {
                LONGLONG td = time - this->time_diff;
                double d_time = ((double)td) / ((double)freq);
                return d_time;
            }
        }
        return 0.0;
    }    
#   elif __LINUX__
        long long int time;
        long long int td;
        double d_time;
        if (this->stopped) time = this->time_stop;
        else {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            time = tv2micro(tv);
        }
        td = time - this->time_diff;
        d_time = ((double)td) / N_MICROSEC_FLOAT;
        return d_time;
    }
#   else
        n_error("Not implemented.");
        return 0.0;
    }
#   endif
}

//-------------------------------------------------------------------
/**
- 24-Apr-2008   Ilya    created
*/
nTimeServer::nTimer* nTimeServer::NewTimer(const char* name, float delay, const char* command, const char* param)
{
	n_assert(name);
	nTimer* tm = n_new nTimer(name, this->GetTime());
	if (tm)
	{
		tm->delay = delay;
		tm->command = command;
		tm->param = param;
		this->timers_list.AddTail(tm);
	}		
	return tm;
}

//-------------------------------------------------------------------
/**
- 24-Apr-2008   Ilya    created
*/
void nTimeServer::ReleaseTimer(nTimer* tm)
{
	n_assert(tm);
	tm->Remove();
	n_delete tm;
}

//-------------------------------------------------------------------
/**
- 24-Apr-2008   Ilya    created
*/
bool nTimeServer::ReleaseTimer(const char* name)
{
	nTimer* tm = (nTimer*)this->timers_list.Find(name);
	if (tm)
	{
		tm->Remove();
		n_delete tm;
		return true;
	}
	return false;	
}

//-------------------------------------------------------------------
/**
- 11-May-2008   Ilya    created
*/
double nTimeServer::GetTimerRemainder(const char* name) 
{
	nTimer* tm = (nTimer*)this->timers_list.Find(name);
	if (tm)
	{
		return tm->remainder(this->GetTime());		
	}
	return -1.0;	
}

//-------------------------------------------------------------------
/**
- 24-Apr-2008   Ilya    created
*/
nStrList* nTimeServer::GetTimers(void)
{
	return &(this->timers_list);
}

//-------------------------------------------------------------------
/**
     - 07-Dec-98   floh    created
     - 19-Feb-99   floh    umbenannt nach NewProfiler()
     - 30-Sep-99   floh    Profiler muessen jetzt eindeutig benannt sein
*/
//-------------------------------------------------------------------
nProfiler *nTimeServer::NewProfiler(const char *name)
{
    n_assert(name);
    nProfiler *p;
    p = n_new nProfiler(ks,this,name);
    this->prof_list.AddTail(p);
    return p;
}

//-------------------------------------------------------------------
/**
     - 07-Dec-99   floh    created
     - 30-Sep-99   floh    kein Usecount fuer Profiler mehr...
*/
//-------------------------------------------------------------------
void nTimeServer::ReleaseProfiler(nProfiler *p)
{
    n_assert(p);
    p->Remove();
    n_delete p;
}

//-------------------------------------------------------------------
/**
     - 07-Dec-99   floh    created
*/
//-------------------------------------------------------------------
nStrList *nTimeServer::GetProfilers(void)
{
    return &(this->prof_list);
}

//-------------------------------------------------------------------
/**
     - 16-May-99   floh    created
     - 12-Apr-02   peter   handle 0.0 case
*/
//-------------------------------------------------------------------
void nTimeServer::LockDeltaT(double dt)
{
    n_assert(dt >= 0.0);
    if (dt == 0.0) {
        this->SetTime(this->lock_time);
    } else {
        this->lock_time = this->GetTime();
    }
    this->lock_delta_t = dt;
}

//-------------------------------------------------------------------
/**
     - 12-Aug-99   floh    created
*/
//-------------------------------------------------------------------
void nTimeServer::WaitDeltaT(double dt)
{
    n_assert(dt >= 0.0);
    this->wait_delta_t = dt;
}

//-------------------------------------------------------------------
/**
     - 12-Aug-99   floh    created
*/
//-------------------------------------------------------------------
double nTimeServer::GetLockDeltaT(void)
{
    return this->lock_delta_t;
}

//-------------------------------------------------------------------
/**
     - 12-Aug-99   floh    created
*/
//-------------------------------------------------------------------
double nTimeServer::GetWaitDeltaT(void)
{
    return this->wait_delta_t;
}

//-------------------------------------------------------------------
/**
     - 16-May-99   floh    created
     - 30-Sep-99   floh    Profiler-Env-Variablen werden geupdated...
     - 30-Dec-99   floh    + beachtet jetzt das 'time stopped' Flag
     - 26-Jan-00   floh    + FrameTime handling
*/
//-------------------------------------------------------------------
void nTimeServer::Trigger(void)
{
    if (this->lock_delta_t > 0.0)
	{
        if (!this->stopped) 
		{
            this->lock_time += this->lock_delta_t;
        }
    }

	double current_time = this->GetTime();
    if (this->frame_enabled) 
		this->frame_time = current_time; 

    // update profile watcher variables      
    for (nProfiler* p = (nProfiler*)this->prof_list.GetHead(); p; p = (nProfiler*)p->GetSucc())
    {
        p->Rewind();
    }

	// update profile watcher variables      
	nTimer* tm = (nTimer*)this->timers_list.GetHead();
    while ( tm )
    {
        if (tm->check(current_time))
		{
			n_printf("Timer %s signaled command %s param %s\n", tm->GetName(), 
				tm->command.c_str(), tm->param.c_str());
			if (this->refScriptServer.isvalid()) 
			{
				nScriptServer* script = this->refScriptServer.get();
				stl_string cmd("");
				cmd += script->GetCmdBegin();
				cmd += tm->command;
				cmd += script->GetCmdParamBegin();
				cmd += "\"";
				cmd += tm->GetName();
				cmd += "\"";
				cmd += script->GetCmdParamDelim();
				cmd += "\"";
				cmd += tm->param;
				cmd += "\"";
				cmd += script->GetCmdParamEnd();
				cmd += script->GetCmdEnd();
				nTimer* old = tm;
				tm = (nTimer*)tm->GetSucc();
				this->ReleaseTimer(old);

				const char* resstr;
				script->Run(cmd.c_str(), resstr);
			}			
		} 
		else
			tm = (nTimer*)tm->GetSucc();
    }

	if (this->GetWaitDeltaT() > 0.0 && this->GetSleep()) 
		n_sleep(this->wait_delta_t);
	else
		n_sleep(0.0);
}

//-------------------------------------------------------------------
/**
     - 26-Jan-00   floh    created
*/
//-------------------------------------------------------------------
void nTimeServer::EnableFrameTime(void)
{
    this->frame_enabled = true;
    this->frame_time = this->GetTime();
}

//-------------------------------------------------------------------
/**
     - 26-Jan-00   floh    created
*/
//-------------------------------------------------------------------
void nTimeServer::DisableFrameTime(void)
{
    this->frame_enabled = false;
}

//-------------------------------------------------------------------
/**
    The purpose of GetFrameTime() is to get an exactly identical 
    frame-timestamp within two nTimeServer::Trigger() calls
    (whereas a nTimeServer::GetTime() will return a different
    timestamp each time it is called).
	26-Jan-00   floh    created
	07-Oct-02   Zeev Clockwise add time scale
*/
//-------------------------------------------------------------------
double nTimeServer::GetFrameTime(void)
{
    if (this->frame_enabled) return this->frame_time * this->frame_time_scale;
    else                     return this->GetTime()  * this->frame_time_scale;
}

//-------------------------------------------------------------------
/**
	GetScale()
	07-Oct-02   Zeev Clockwise add time scale
*/
//-------------------------------------------------------------------
float nTimeServer::GetScale(void)
{
    return this->frame_time_scale;
}

//-------------------------------------------------------------------
/**
	GetScale()
	07-Oct-02   Zeev Clockwise add time scale
*/
//-------------------------------------------------------------------
void nTimeServer::SetScale(float a)
{
    this->frame_time_scale = a; 
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
