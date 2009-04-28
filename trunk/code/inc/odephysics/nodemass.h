#ifndef N_ODEMASS_H
#define N_ODEMASS_H
//------------------------------------------------------------------------------
/**
  @class nOdeMass
  @ingroup NODEPhysicsContribModule
  @brief Defines a mass for an ODE rigid body.

  (c) 2003 Vadim Macagon
  
  nOdeMass is licensed under the terms of the Nebula License.
*/  
//------------------------------------------------------------------------------
#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#ifndef N_ODEUTIL_H
#include "odephysics/odeutil.h"
#endif

//------------------------------------------------------------------------------
class nOdeMass
{
  public:
    friend class nOdeBody;
  
    nOdeMass();
    nOdeMass( const nOdeMass& m );
    ~nOdeMass();
    
    /// set all mass parameters to zero
    void Reset();
    
    void SetParameters( float themass, const vector3& centerOfGravity,
                        const matrix33& inertia );
    void GetParameters( float* themass, vector3* centerOfGravity,
                        matrix33* inertia );
                        
    float GetTotalMass();
    void SetCenterOfGravity( const vector3& );
    void GetCenterOfGravity( vector3* );
    void SetInertia( const matrix33& );
    void GetInertia( matrix33* );
    
    void SetSphere( float density, float radius );
    void SetSphereTotal( float totalMass, float radius );
    void SetCapsule( float density, float radius, float length, int direction = 3 );
    void SetCapsuleTotal( float totalMass, float radius, float length, 
                          int direction = 3 );
    void SetCylinder( float density, float radius, float length, int direction = 3 );
    void SetCylinderTotal( float totalMass, float radius, float length, 
                           int direction = 3 );
    void SetBox( float density, float lx, float ly, float lz );
    void SetBoxTotal( float totalMass, float lx, float ly, float lz );
    void AdjustTo( float newmass );
    
    void Translate( const vector3& vec );
    void Rotate( const matrix33& rot );
    void Rotate( const matrix44& rot );
    void Rotate( const quaternion& rot );
    
    nOdeMass& operator+=( const nOdeMass& m );
    friend nOdeMass operator+( const nOdeMass& m1, const nOdeMass& m2 );
  
  private:
    dMass mass;
};

inline 
nOdeMass operator+( const nOdeMass& m1, const nOdeMass& m2 ) 
{
  nOdeMass res( m1 );
  dMassAdd( &res.mass, &m2.mass );
  return res;
}

//------------------------------------------------------------------------------
inline
nOdeMass::nOdeMass()
{
}

//------------------------------------------------------------------------------
inline
nOdeMass::nOdeMass( const nOdeMass& m )
{
  this->mass = m.mass;
}

//------------------------------------------------------------------------------
inline
nOdeMass::~nOdeMass()
{
}
    
//------------------------------------------------------------------------------
inline
nOdeMass& nOdeMass::operator+=( const nOdeMass& m )
{
  dMassAdd( &this->mass, &m.mass );
  return *this;
}

//------------------------------------------------------------------------------
inline
void nOdeMass::Reset()
{
  dMassSetZero( &this->mass );
}

//------------------------------------------------------------------------------
inline    
void nOdeMass::SetParameters( float theMass, const vector3& cog,
                              const matrix33& inertia )
{
  dMassSetParameters( &this->mass, theMass, cog.x, cog.y, cog.z,
                      inertia.M11, inertia.M22, inertia.M33,
                      inertia.M12, inertia.M13, inertia.M23 );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::GetParameters( float* themass, vector3* centerOfGravity,
                              matrix33* inertia )
{
  *themass = this->mass.mass;
  centerOfGravity->set( this->mass.c[0], this->mass.c[1], this->mass.c[2] );
  nOdeUtil::OdeMatrixToNeb( this->mass.I, *inertia );
}

//------------------------------------------------------------------------------
inline
float nOdeMass::GetTotalMass()
{
  return this->mass.mass;
}

//------------------------------------------------------------------------------
inline
void nOdeMass::SetCenterOfGravity( const vector3& cog )
{
  this->mass.c[0] = cog.x;
  this->mass.c[1] = cog.y;
  this->mass.c[2] = cog.z;
}

//------------------------------------------------------------------------------
inline
void nOdeMass::GetCenterOfGravity( vector3* cog )
{
  cog->set( this->mass.c[0], this->mass.c[1], this->mass.c[2] );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::SetInertia( const matrix33& inertia )
{
  nOdeUtil::NebMatrixToODE( inertia, this->mass.I );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::GetInertia( matrix33* inertia )
{
  nOdeUtil::OdeMatrixToNeb( this->mass.I, *inertia );
}

//------------------------------------------------------------------------------
inline    
void nOdeMass::SetSphere( float density, float radius )
{
  dMassSetSphere( &this->mass, density, radius );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::SetSphereTotal( float totalMass, float radius )
{
  dMassSetSphereTotal( &this->mass, totalMass, radius );
}

//------------------------------------------------------------------------------
/**
  @brief Set mass parameters to represent a capsule.
  @param density
  @param radius
  @param length Length of capsule (excluding the radii of the end hemi-spheres)
  @param direction The body axis along which the capsule is oriented.
                   1 = x, 2 = y, 3 = z. Default is z-axis since that is how
                   nPrimitiveServer draws them.
*/
inline
void nOdeMass::SetCapsule( float density, float radius, float length, 
                           int direction )
{
  dMassSetCappedCylinder( &this->mass, density, direction, radius, length );
}

//------------------------------------------------------------------------------
/**
  @brief Set mass parameters to represent a capsule.
  @param totalMass
  @param radius
  @param length Length of capsule (excluding the radii of the end hemi-spheres)
  @param direction The body axis along which the capsule is oriented.
                   1 = x, 2 = y, 3 = z. Default is z-axis since that is how
                   nPrimitiveServer draws them.
*/
inline
void nOdeMass::SetCapsuleTotal( float totalMass, float radius, float length, 
                      int direction )
{
  dMassSetCappedCylinderTotal( &this->mass, totalMass, direction, radius, length );
}

//------------------------------------------------------------------------------
/**
  @brief Set mass parameters to represent a cylinder.
  @param density
  @param radius
  @param length Length of cylinder
  @param direction The body axis along which the cylinder is oriented.
                   1 = x, 2 = y, 3 = z. Default is z-axis since that is how
                   nPrimitiveServer draws them.
*/
inline
void nOdeMass::SetCylinder( float density, float radius, float length,
                            int direction )
{
  dMassSetCylinder( &this->mass, density, direction, radius, length );
}

//------------------------------------------------------------------------------
/**
  @brief Set mass parameters to represent a cylinder.
  @param totalMass
  @param radius
  @param length Length of cylinder
  @param direction The body axis along which the cylinder is oriented.
                   1 = x, 2 = y, 3 = z. Default is z-axis since that is how
                   nPrimitiveServer draws them.
*/
inline
void nOdeMass::SetCylinderTotal( float totalMass, float radius, float length, 
                                 int direction )
{
  dMassSetCylinderTotal( &this->mass, totalMass, direction, radius, length );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::SetBox( float density, float lx, float ly, float lz )
{
  dMassSetBox( &this->mass, density, lx, ly, lz );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::SetBoxTotal( float totalMass, float lx, float ly, float lz )
{
  dMassSetBoxTotal( &this->mass, totalMass, lx, ly, lz );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::AdjustTo( float newmass )
{
  dMassAdjust( &this->mass, newmass );
}
    
//------------------------------------------------------------------------------
inline
void nOdeMass::Translate( const vector3& vec )
{
  dMassTranslate( &this->mass, vec.x, vec.y, vec.z );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::Rotate( const matrix33& rot )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( rot, rotation );
  dMassRotate( &this->mass, rotation );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::Rotate( const matrix44& rot )
{
  dMatrix3 rotation;
  nOdeUtil::NebMatrixToODE( rot, rotation );
  dMassRotate( &this->mass, rotation );
}

//------------------------------------------------------------------------------
inline
void nOdeMass::Rotate( const quaternion& rot )
{
  dQuaternion quat;
  nOdeUtil::NebQuatToOde( rot, quat );
  dMatrix3 mat;
  dQtoR( quat, mat );
  dMassRotate( &this->mass, mat );
}

//------------------------------------------------------------------------------
#endif // N_ODEMASS_H
