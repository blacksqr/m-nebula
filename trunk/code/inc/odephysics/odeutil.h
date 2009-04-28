#ifndef N_ODEUTIL_H
#define N_ODEUTIL_H
//------------------------------------------------------------------------------
/**
  @ingroup NODEPhysicsContribModule
  
  Various functions for dealing with ODE types.
*/
//------------------------------------------------------------------------------
#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_ODE_H
#define N_ODE_H
#include <ode/ode.h>
#endif

//#ifndef __OPCODE_H__
//#include "opcode/opcode.h"
//#endif

//using namespace Opcode;

//------------------------------------------------------------------------------
namespace nOdeUtil
{
  template<class T> const T& dcMAX(const T& x, const T& y)
  {
    return x > y ? x : y;
  }

  template<class T> const T& dcMIN(const T& x, const T& y)
  {
    return x < y ? x : y;
  }

  //------------------------------------------------------------------------------
  /**
    @brief Decomposes an ODE matrix into 3 vectors.
  
    Ripped from ODE Tri-Collider.
  */
  inline 
  void DecomposeMatrix( const dMatrix3 Matrix, 
                        dVector3 Right, dVector3 Up, dVector3 Direction )
  {
    Right[0] = Matrix[0 * 4 + 0];
    Right[1] = Matrix[1 * 4 + 0];
    Right[2] = Matrix[2 * 4 + 0];
    Right[3] = Matrix[3 * 4 + 0];
    Up[0] = Matrix[0 * 4 + 1];
    Up[1] = Matrix[1 * 4 + 1];
    Up[2] = Matrix[2 * 4 + 1];
    Up[3] = Matrix[3 * 4 + 1];
    Direction[0] = Matrix[0 * 4 + 2];
    Direction[1] = Matrix[1 * 4 + 2];
    Direction[2] = Matrix[2 * 4 + 2];
    Direction[3] = Matrix[3 * 4 + 2];
  }
  
  // Outputs a matrix to 3 vectors
  inline 
  void DecomposeMatrix( const dMatrix3 Matrix, dVector3 Vectors[3] )
  {
    DecomposeMatrix( Matrix, Vectors[0], Vectors[1], Vectors[2] );
  }
  
  //------------------------------------------------------------------------------
  /**
    @brief Convert an ODE rotation + position into an OPCODE matrix.
  
    Ripped from ODE Tri-Collider.
  */
  /*
  inline
  void OdeMatrixToOpcode( const dVector3 Position, const dMatrix3 Rotation, 
                          Matrix4x4& Out )
  {
    Out.m[0][0] = Rotation[0];
    Out.m[1][0] = Rotation[1];
    Out.m[2][0] = Rotation[2];

    Out.m[0][1] = Rotation[4];
    Out.m[1][1] = Rotation[5];
    Out.m[2][1] = Rotation[6];

    Out.m[0][2] = Rotation[8];
    Out.m[1][2] = Rotation[9];
    Out.m[2][2] = Rotation[10];

    Out.m[3][0] = Position[0];
    Out.m[3][1] = Position[1];
    Out.m[3][2] = Position[2];

    Out.m[0][3] = 0.0f;
    Out.m[1][3] = 0.0f;
    Out.m[2][3] = 0.0f;
    Out.m[3][3] = 1.0f;
  }
  */
  //------------------------------------------------------------------------------
  /**
    @brief Convert an ODE rotation + position of a geom into an OPCODE matrix.
  
    Ripped from ODE Tri-Collider.
  */
  /*
  inline 
  void GetOpcodeMatrix( dxGeom* g, Matrix4x4* Out )
  {
    const dVector3& Position = *(const dVector3*)dGeomGetPosition( g );
    const dMatrix3& Rotation = *(const dMatrix3*)dGeomGetRotation( g );
    OdeMatrixToOpcode( Position, Rotation, *Out );
  }
  */
  //------------------------------------------------------------------------------
  /**
    @brief Convert an ODE rotation + position into a matrix44.
  */
  inline
  void OdeMatrixToNeb( const dVector3 Position, const dMatrix3 Rotation, 
                       matrix44& out )
  {
    out.M11 = Rotation[0];
    out.M21 = Rotation[1];
    out.M31 = Rotation[2];

    out.M12 = Rotation[4];
    out.M22 = Rotation[5];
    out.M32 = Rotation[6];

    out.M13 = Rotation[8];
    out.M23 = Rotation[9];
    out.M33 = Rotation[10];

    out.M41 = Position[0];
    out.M42 = Position[1];
    out.M43 = Position[2];

    out.M14 = 0.0f;
    out.M24 = 0.0f;
    out.M34 = 0.0f;
    out.M44 = 1.0f;
  }
  
  inline
  void OdeMatrixToNeb( const dMatrix3 Rotation, matrix44& out )
  {
    out.M11 = Rotation[0];
    out.M21 = Rotation[1];
    out.M31 = Rotation[2];

    out.M12 = Rotation[4];
    out.M22 = Rotation[5];
    out.M32 = Rotation[6];

    out.M13 = Rotation[8];
    out.M23 = Rotation[9];
    out.M33 = Rotation[10];

    out.M41 = 0.0f;
    out.M42 = 0.0f;
    out.M43 = 0.0f;

    out.M14 = 0.0f;
    out.M24 = 0.0f;
    out.M34 = 0.0f;
    out.M44 = 1.0f;
  }
  
  inline
  void OdeMatrixToNeb( const dMatrix3 Rotation, matrix33& out )
  {
    out.M11 = Rotation[0];
    out.M21 = Rotation[1];
    out.M31 = Rotation[2];

    out.M12 = Rotation[4];
    out.M22 = Rotation[5];
    out.M32 = Rotation[6];

    out.M13 = Rotation[8];
    out.M23 = Rotation[9];
    out.M33 = Rotation[10];
  }
  
  //------------------------------------------------------------------------------
  /**
    @brief Convert an ODE rotation + position of a geom into a matrix44.
  */
  inline
  void GetNebMatrix( dxGeom* g, matrix44* out )
  {
    const dVector3& Position = *(const dVector3*)dGeomGetPosition( g );
    const dMatrix3& Rotation = *(const dMatrix3*)dGeomGetRotation( g );
    OdeMatrixToNeb( Position, Rotation, *out );
  }
  
  inline
  void NebMatrixToODE( const matrix44& mat, dVector3 position, dMatrix3 rotation )
  {
    position[0] = mat.M41;
    position[1] = mat.M42;
    position[2] = mat.M43;
    position[3] = 0.0f;
    
    rotation[0]   = mat.M11;
    rotation[1]   = mat.M21;
    rotation[2]   = mat.M31;
    rotation[3]   = 0.0f;
    rotation[4]   = mat.M12;
    rotation[5]   = mat.M22;
    rotation[6]   = mat.M32;
    rotation[7]   = 0.0f;
    rotation[8]   = mat.M13;
    rotation[9]   = mat.M23;
    rotation[10]  = mat.M33;
    rotation[11]  = 0.0f;
  }
  
  inline
  void NebMatrixToODE( const matrix33& mat, dMatrix3 rotation )
  {
    rotation[0]   = mat.M11;
    rotation[1]   = mat.M21;
    rotation[2]   = mat.M31;
    rotation[3]   = 0.0f;
    rotation[4]   = mat.M12;
    rotation[5]   = mat.M22;
    rotation[6]   = mat.M32;
    rotation[7]   = 0.0f;
    rotation[8]   = mat.M13;
    rotation[9]   = mat.M23;
    rotation[10]  = mat.M33;
    rotation[11]  = 0.0f;
  }
  
  inline
  void NebMatrixToODE( const matrix44& mat, dMatrix3 rotation )
  {
    rotation[0]   = mat.M11;
    rotation[1]   = mat.M21;
    rotation[2]   = mat.M31;
    rotation[3]   = 0.0f;
    rotation[4]   = mat.M12;
    rotation[5]   = mat.M22;
    rotation[6]   = mat.M32;
    rotation[7]   = 0.0f;
    rotation[8]   = mat.M13;
    rotation[9]   = mat.M23;
    rotation[10]  = mat.M33;
    rotation[11]  = 0.0f;
  }
  
  /** 
    @brief Convert ODE quaternion to a Nebula one.
  */
  inline
  void OdeQuatToNeb( const dQuaternion source, quaternion* res )
  {
    res->set( source[1], source[2], source[3], source[0] );
  }
  
  /**
    @brief Convert Nebula quaternion to ODE one.
  */
  inline
  void NebQuatToOde( const quaternion& source, dQuaternion res )
  {
    res[0] = source.w;
    res[1] = source.x;
    res[2] = source.y;
    res[3] = source.z;
  }
  
  //------------------------------------------------------------------------------
  /**
    @brief Fetches a contact from the contacts array.
  
    Ripped from ODE Tri-Collider.
  */
  inline 
  dContactGeom* FetchContact( int flags, dContactGeom* contacts, int index, int stride )
  {
    n_assert( index >= 0 && index < (flags & 0x0ffff) );
    return ((dContactGeom*)(((char*)contacts) + (index * stride)));
  }
  
} // namespace nOdeUtil

#endif // N_ODEUTIL_H
