#ifndef N_ODESURFACE_H
#define N_ODESURFACE_H
//------------------------------------------------------------------------------
/**
    @class nOdeSurface
    @ingroup NODEPhysicsContribModule
    @brief Defines properties for colliding surfaces.

    This corresponds to the ODE dSurfaceParameters structure.

    (C) 2003  Vadim Macagon
    
    nOdeSurface is licensed under the terms of the Nebula License.
*/
//------------------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//------------------------------------------------------------------------------
class nOdeSurface
{
  public:
    friend class nOdePhysicsContext;
  
    nOdeSurface();
    ~nOdeSurface();
  
    void SetName( const char* );
    const char* GetName();
  
    void SetMode( int flags );
    int GetMode();
    void ToggleFlag( int flag );
    void EnableFlag( int flag );
    
    void SetMU( float mu );
    float GetMU();
    
    void SetMU2( float mu2 );
    float GetMU2();
    
    void SetBounce( float value );
    float GetBounce();
  
    void SetBounceVel( float value );
    float GetBounceVel();
    
    void SetSoftERP( float value );
    float GetSoftERP();
    
    void SetSoftCFM( float value );
    float GetSoftCFM();
    
    void SetMotion1( float value );
    float GetMotion1();
    void SetMotion2( float value );
    float GetMotion2();
    
    void SetSlip1( float value );
    float GetSlip1();
    void SetSlip2( float value );
    float GetSlip2();
    
  private:
    dSurfaceParameters surface;
    const char* name;
};

//------------------------------------------------------------------------------
inline
nOdeSurface::nOdeSurface()
  : name( 0 )
{
  surface.mode = 0;
}

//------------------------------------------------------------------------------
inline
nOdeSurface::~nOdeSurface()
{
  if ( this->name )
    n_free( (void*)this->name );
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetName( const char* n )
{
  if ( this->name )
    n_free( (void*)this->name );
  this->name = n_strdup( n );
}

//------------------------------------------------------------------------------
inline    
const char* nOdeSurface::GetName()
{
  return this->name;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetMode( int flags )
{
  this->surface.mode = flags;
}

//------------------------------------------------------------------------------
inline
int nOdeSurface::GetMode()
{
  return this->surface.mode;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::ToggleFlag( int flag )
{
  if ( this->surface.mode & flag )  // if on, turn it off
    this->surface.mode &= ~flag;
  else // turn it on
    this->surface.mode |= flag;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::EnableFlag( int flag )
{
  this->surface.mode |= flag;
}

//------------------------------------------------------------------------------
/**
  @brief 
  
  A value less than zero is interpreted as infinity.
*/
inline
void nOdeSurface::SetMU( float mu )
{
  if ( mu < 0.0f )
      this->surface.mu = (dReal)dInfinity;
  else
    this->surface.mu = mu;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetMU()
{
  return this->surface.mu;
}

//------------------------------------------------------------------------------
/**
  @brief 
  
  A value less than zero is interpreted as infinity.
*/
inline
void nOdeSurface::SetMU2( float mu2 )
{
  if ( mu2 < 0.0f )
    this->surface.mu2 = (dReal)dInfinity;
  else
    this->surface.mu2 = mu2;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetMU2()
{
  return this->surface.mu2;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetBounce( float value )
{
  this->surface.bounce = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetBounce()
{
  return this->surface.bounce;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetBounceVel( float value )
{
  this->surface.bounce_vel = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetBounceVel()
{
  return this->surface.bounce_vel;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetSoftERP( float value )
{
  this->surface.soft_erp = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetSoftERP()
{
  return this->surface.soft_erp;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetSoftCFM( float value )
{
  this->surface.soft_cfm = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetSoftCFM()
{
  return this->surface.soft_cfm;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetMotion1( float value )
{
  this->surface.motion1 = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetMotion1()
{
  return this->surface.motion1;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetMotion2( float value )
{
  this->surface.motion2 = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetMotion2()
{
  return this->surface.motion2;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetSlip1( float value )
{
  this->surface.slip1 = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetSlip1()
{
  return this->surface.slip1;
}

//------------------------------------------------------------------------------
inline
void nOdeSurface::SetSlip2( float value )
{
  this->surface.slip2 = value;
}

//------------------------------------------------------------------------------
inline
float nOdeSurface::GetSlip2()
{
  return this->surface.slip2;
}

//------------------------------------------------------------------------------
#endif // N_ODESURFACE_H
