#ifndef N_PRIMITIVESERVER_H
#define N_PRIMITIVESERVER_H
//------------------------------------------------------------------------------
/**
    @class nPrimitiveServer
    @brief OpenGL style immediate mode rendering and primitives for debugging purposes.

    nPrimitiveServer is primarily for debugging purposes. It allows for
    OpenGL style immediate mode rendering and provides functions for drawing
    common shapes like capsules, spheres, boxes, cylinders, etc. 
    The interface is somewhat similar to GLUT, so
    in order to rotate the primitives you'll need to manipulate the
    MODELVIEW matrix in the gfx server.

    (C) 2003  Leaf Garland & Vadim Macagon

    nPrimitiveServer is licensed under the terms of the Nebula License.
*/

#ifndef N_AUTOREF_H
#include "kernel/nautoref.h"
#endif

#ifndef N_PIXELSHADER_H
#include "gfx/npixelshader.h"
#endif

#ifndef N_DYNVERTEXBUFFER_H
#include "gfx/ndynvertexbuffer.h"
#endif

#include "util/nstl.h"
#include "mathlib/vector.h"

#ifndef N_ROOT_H
#include "kernel/nroot.h"
#endif

#undef N_DEFINES
#define N_DEFINES nPrimitiveServer
#include "kernel/ndefdllclass.h"

class nGfxServer;
//------------------------------------------------------------------------------
class N_PUBLIC nPrimitiveServer : public nRoot
{
public:
    /// constructor
    nPrimitiveServer();
    /// destructor
    virtual ~nPrimitiveServer();
    /// persistency
    virtual bool SaveCmds( nPersistServer* persistServer );

    // OpenGL style immediate mode rendering functions.
    virtual void Begin(nPrimType, nPixelShader* ps = 0, nTextureArray* texarr = 0);
    virtual void Coord(float x, float y, float z) { this->Coord(vector3(x, y, z)); }
	virtual void Coord(const vector3& v); 
    virtual void Norm(float x, float y, float z) { this->Norm(vector3(x, y, z)); }
	virtual void Norm(const vector3& n); 
	virtual void Rgba(float r, float g, float b, float a) {this->Rgba(vector4(r, g, b, a));}
	virtual void Rgba(const vector4& c);
	virtual void Uv(float t, float u) {this->Uv(vector2(t, u));}	
	virtual void Uv(const vector2& t);

    virtual void End(void); 

    // Prefab shapes for your pleasure
    virtual void WirePlane( float planeExtent = 25.0f, float gridSize = 1.0f );
    virtual void SolidPlane( float planeExtent = 25.0f );

	virtual void WirePlaneXY( float planeExtent = 25.0f, float gridSize = 1.0f );
    virtual void SolidPlaneXY( float planeExtent = 25.0f );
	virtual void WirePlaneXZ( float planeExtent = 25.0f, float gridSize = 1.0f ) 
	{
		this->WirePlane(planeExtent, gridSize);
	}

    virtual void SolidPlaneXZ( float planeExtent = 25.0f ) {this->SolidPlane(planeExtent);}
	virtual void WirePlaneYZ( float planeExtent = 25.0f, float gridSize = 1.0f );
    virtual void SolidPlaneYZ( float planeExtent = 25.0f );

    virtual void WireSphere( float radius, int slices, int stacks );
    virtual void SolidSphere( float radius, int slices, int stacks );
    virtual void WireCone( float base, float height, bool closed, int slices, int stacks );
    virtual void SolidCone( float base, float height, bool closed, int slices, int stacks );
    virtual void WireCapsule( float radius, float length, int slices, int stacks );
    virtual void SolidCapsule( float radius, float length, int slices, int stacks );
    virtual void WireCylinder( float radius, float length, 
		bool closed = true, int slices = 16, int stacks = 1 );
    virtual void SolidCylinder( float radius, float length, 
		bool closed = true, int slices = 16, int stacks = 1 );
    virtual void WireCube( float size = 1.0f );
    virtual void SolidCube( float size = 1.0f );
    virtual void WireBox( float lx, float ly, float lz );
    virtual void SolidBox( float lx, float ly, float lz );
    virtual void WireTorus( float innerRadius, float outerRadius, int sides, int rings );
    virtual void SolidTorus( float innerRadius, float outerRadius, int sides, int rings );
    virtual void CheckerPlane(float planeExtent, float gridSize);
    
    // State management
    void EnableLighting( bool enable );
    virtual void SetColor( float r, float g, float b, float a );
	virtual void SetColor(const vector4& c) { this->SetColor(c.x, c.y, c.z, c.w); }
   // void SetPixelShader( nPixelShader* ps );
    //virtual nPixelShader* GetPixelShader();	

    /// pointer to nKernelServer
    static nKernelServer* kernelServer;

protected:
    nAutoRef<nGfxServer> refGfx;

    nRef<nPixelShader>	internal_ps;
    nPixelShaderDesc	internal_psdesc;

	nPixelShader*		cur_ps;
	nTextureArray*		cur_ta;

    nDynVertexBuffer dynVB;
    nVertexBuffer* VB;
    nRef<nIndexBuffer> ref_ibuf;

    nPrimType primType;

    size_t  maxVerts;
    bool	inBegin;
    ulong	curColor;


	std::vector<vector3>	vertices;
	std::vector<vector3>	normals;
	std::vector<vector4>	colors;
	std::vector<vector2>	texels;
};

//------------------------------------------------------------------------------
#endif

