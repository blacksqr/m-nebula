#define N_IMPLEMENTS nPrimitiveServer
//------------------------------------------------------------------------------
//  (C) 2003  Leaf Garland & Vadim Macagon
//
//  nPrimitiveServer is licensed under the terms of the Nebula License.
//------------------------------------------------------------------------------
#include "gfx/nprimitiveserver.h"

nNebulaScriptClass(nPrimitiveServer, "nroot");

#ifndef N_GFXSERVER_H
#include "gfx/ngfxserver.h"
#endif

#ifndef N_COLOR_H
#include "mathlib/color.h"
#endif

//------------------------------------------------------------------------------
/**
*/
nPrimitiveServer::nPrimitiveServer()
    : refGfx( kernelServer, this ),     
    dynVB(kernelServer, this),
    VB(0),
    maxVerts(0),
	curColor(1),
    inBegin(false),
	internal_ps(this),
	ref_ibuf(this),
	cur_ps(0),
	cur_ta(0)
{
	this->refGfx = "/sys/servers/gfx";

	this->internal_psdesc.SetDiffuse( color4(1, 1, 1, 1) );
	this->internal_psdesc.SetLightEnable( false );
	this->internal_psdesc.SetZWriteEnable( true );
	this->internal_psdesc.SetZFunc( N_CMP_LESS );
	this->internal_psdesc.SetCullMode(N_CULL_NONE);	
}

//------------------------------------------------------------------------------
/**
*/
nPrimitiveServer::~nPrimitiveServer()
{
	release_ref(this->internal_ps);		
	release_ref(this->ref_ibuf);
	this->vertices.clear();
	this->normals.clear();
	this->texels.clear();
	this->colors.clear();
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::EnableLighting( bool enable )
{    
	if (this->cur_ps)
		this->cur_ps->GetShaderDesc()->SetLightEnable( enable );
}

//------------------------------------------------------------------------------
/**
*/  
void nPrimitiveServer::SetColor( float r, float g, float b, float a )
{
    ulong c;
    if (N_COLOR_RGBA == this->refGfx->GetColorFormat()) 
        c = n_f2rgba(r,g,b,a);
    else
        c = n_f2bgra(r,g,b,a);
    this->curColor = c;
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Begin(nPrimType ptype, nPixelShader* ps, nTextureArray* ta)
{	
    n_assert(!this->inBegin);
    
	if (!this->internal_ps.isvalid())
	{
		nPixelShader* nps = this->refGfx->NewPixelShader( "nprimitiveserver" );
		if ( nps ) 
		{
			this->internal_ps = nps;
			nps->SetShaderDesc( &this->internal_psdesc );
		}
	}

	n_assert(this->internal_ps.isvalid());

	if (ps)
	{		
		this->cur_ps = ps;
	}
	else
	{
		this->cur_ps = this->internal_ps.get();
	}

	this->cur_ta = ta;

	this->primType = ptype;
	
	this->vertices.clear();
	this->normals.clear();
	this->texels.clear();
	this->colors.clear();
	this->inBegin = true;   
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Coord(const vector3& v)
{ 
    n_assert(this->inBegin);
	this->vertices.push_back(v);    
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Norm(const vector3& n)
{
	n_assert(this->inBegin);  
	this->normals.push_back(n);
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Rgba(const vector4& c)
{
	n_assert(this->inBegin);
	this->colors.push_back(c);    
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::Uv(const vector2& t)
{
    n_assert(this->inBegin);  
	this->texels.push_back(t);  
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::End(void)
{
	size_t vnum = this->vertices.size();
	if (!vnum)
		return;
	// check buffers.
    if (!this->dynVB.IsValid()) 
    {    	
		this->dynVB.Initialize( N_VT_COORD | N_VT_NORM | N_VT_RGBA | N_VT_UV0 , 0);

        n_assert(!this->ref_ibuf.isvalid());

        nIndexBuffer *ibuf = this->refGfx->FindIndexBuffer("nprimitiveserver_ibuf");
        if (!ibuf) 
		{
            ibuf = this->refGfx->NewIndexBuffer("nprimitiveserver_ibuf");
            
			this->maxVerts = (this->dynVB.GetNumVertices() / 3) * 3;

            ibuf->Begin(N_IBTYPE_WRITEONLY, this->primType, this->maxVerts);            
            for (size_t i = 0; i < this->maxVerts; i++) 
            {
                ibuf->Index(i, (ushort)i);
            }
            ibuf->End();
        }
        n_assert(ibuf);
        this->ref_ibuf = ibuf;
    }

	n_assert(this->ref_ibuf.isvalid());
	n_assert(this->dynVB.IsValid());

	ref_ibuf->SetPrimType(this->primType);
    
    this->VB = this->dynVB.Begin(this->ref_ibuf.get(), this->cur_ps, this->cur_ta);
	n_assert(this->VB);

	size_t nnum = this->normals.size();
	size_t cnum = this->colors.size();
	size_t tnum = this->texels.size();

	n_assert("nPrimitiveServer::End inconsistent number of normals" && (!nnum || nnum == vnum));
	n_assert("nPrimitiveServer::End inconsistent number of colors" && (!cnum || cnum == vnum));
	n_assert("nPrimitiveServer::End inconsistent number of texels" && (!tnum || tnum == vnum));	

	bool rgba = (N_COLOR_RGBA == this->refGfx->GetColorFormat());	

	vector3 def_norm(1.0f, 0.0f, 0.0f);
	vector2 def_tex(1.0f, 0.0f);

	size_t i=0;
	
	for (size_t gi = 0; gi < vnum; gi++)	
	{
		i = gi%this->maxVerts;
		if (i == 1 && gi > this->maxVerts)
		{
			this->VB = this->dynVB.Swap(this->maxVerts, this->maxVerts);
			n_assert(this->VB);
		}

		this->VB->Coord(i, this->vertices[gi]);
		if (cnum)
		{			
			vector4& c = this->colors[gi];			
			this->VB->Color(i, (rgba ? n_f2rgba(c.x, c.y, c.z, c.w) : n_f2bgra(c.x, c.y, c.z, c.w)));
		}
		else
		{
			this->VB->Color(i, this->curColor);
		}

		if (nnum)
		{
			this->VB->Norm(i, this->normals[gi]);    
		}
		//else
		//{
		//	this->VB->Norm(i, def_norm);
		//}

		if (tnum)
		{
			this->VB->Uv(i, 0, this->texels[i]);
		}
		//else
		//{
		//	this->VB->Uv(i, 0, def_tex);
		//}		
	}	

	i++;
	
	

    this->dynVB.End(i, i);
    this->VB = 0;    
    this->inBegin = false;
}


//------------------------------------------------------------------------------
/**
    @brief Render square wireframe grid/plane.
    @param planeExtent Ideal distance from centre of plane to one of the 'edges'.
    @param gridSize Size of each grid square.
*/
void nPrimitiveServer::WirePlane( float planeExtent, float gridSize )
{
    float extent = planeExtent;//float((int)(planeExtent / gridSize));
    this->Begin( N_PTYPE_LINE_LIST );
    for ( float p = -extent; p <= +extent; p += gridSize )
    {
        this->Coord(+p, 0.0f, -planeExtent);
        this->Coord(+p, 0.0f, +planeExtent);
        this->Coord(-planeExtent, 0.0f, +p);
        this->Coord(+planeExtent, 0.0f, +p);
    }
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render square plane (really just a square).
    @param planeExtent Distance from centre of plane to one of the 'edges'.
*/
void nPrimitiveServer::SolidPlane( float planeExtent )
{
    this->Begin( N_PTYPE_TRIANGLE_STRIP );	
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( planeExtent, 0.0f, -planeExtent );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( -planeExtent, 0.0f, -planeExtent );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( planeExtent, 0.0f, planeExtent );
    this->Norm( 0.0f, 1.0f, 0.0f );
    this->Coord( -planeExtent, 0.0f, planeExtent );
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render square wireframe grid/plane.
    @param planeExtent Ideal distance from centre of plane to one of the 'edges'.
    @param gridSize Size of each grid square.
*/
void nPrimitiveServer::WirePlaneXY( float planeExtent, float gridSize )
{
    float extent = planeExtent;//float((int)(planeExtent / gridSize));
    this->Begin( N_PTYPE_LINE_LIST );
    for ( float p = -extent; p <= +extent; p += gridSize )
    {
        this->Coord(+p, -planeExtent, 0.0f);
        this->Coord(+p, +planeExtent, 0.0f);
        this->Coord(-planeExtent, +p, 0.0f);
        this->Coord(+planeExtent, +p, 0.0f);
    }
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render square plane (really just a square).
    @param planeExtent Distance from centre of plane to one of the 'edges'.
*/
void nPrimitiveServer::SolidPlaneXY( float planeExtent )
{
    this->Begin( N_PTYPE_TRIANGLE_STRIP );	
    this->Norm( 0.0f, 0.0f, 1.0f);
    this->Coord(+planeExtent, -planeExtent, 0.0f);
    this->Norm( 0.0f, 0.0f, 1.0f);
    this->Coord(-planeExtent, -planeExtent, 0.0f);
    this->Norm( 0.0f, 0.0f, 1.0f);
    this->Coord(+planeExtent, +planeExtent, 0.0f);
    this->Norm( 0.0f, 0.0f, 1.0f);
    this->Coord(-planeExtent, +planeExtent, 0.0f);
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render square wireframe grid/plane.
    @param planeExtent Ideal distance from centre of plane to one of the 'edges'.
    @param gridSize Size of each grid square.
*/
void nPrimitiveServer::WirePlaneYZ( float planeExtent, float gridSize )
{
    float extent = planeExtent;//float((int)(planeExtent / gridSize));
    this->Begin( N_PTYPE_LINE_LIST );
    for ( float p = -extent; p <= +extent; p += gridSize )
    {
        this->Coord(0.0f, +p, -planeExtent);
        this->Coord(0.0f, +p, +planeExtent);
        this->Coord(0.0f, -planeExtent, +p);
        this->Coord(0.0f, +planeExtent, +p);
    }
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render square plane (really just a square).
    @param planeExtent Distance from centre of plane to one of the 'edges'.
*/
void nPrimitiveServer::SolidPlaneYZ( float planeExtent )
{
    this->Begin( N_PTYPE_TRIANGLE_STRIP );	
    this->Norm(1.0f, 0.0f, 0.0f);
    this->Coord(0.0f, +planeExtent, -planeExtent);
    this->Norm(1.0f, 0.0f, 0.0f);
    this->Coord(0.0f, -planeExtent, -planeExtent);
    this->Norm(1.0f, 0.0f, 0.0f);
    this->Coord(0.0f, +planeExtent, +planeExtent);
    this->Norm(1.0f, 0.0f, 0.0f);
    this->Coord(0.0f, -planeExtent, +planeExtent);
    this->End();
}



//------------------------------------------------------------------------------
/**
    @brief Render a wireframe sphere centred at the origin.
    @param radius Sphere radius.
    @param slices Number of slices.
    @param stacks Number of stacks.
*/
void nPrimitiveServer::WireSphere( float sphereRad, int slices, int stacks )
{
    /*
    FIXME: This doesn't actually work properly yet :|, feel free to
    replace with a working version.
    */

    float s, t, sinT, x, y, radius;
    float divS = (float)N_PI / (float)slices;
    float divT = (float)N_DOUBLE_PI / (float)stacks;
    float extent = (float)N_PI - divT;

    // draw rings/stacks about the z-axis
    for ( t = -extent; t <= extent; t += divT )
    {
        radius = n_cos(t) * sphereRad;
        sinT = n_sin(t);
        this->Begin( N_PTYPE_LINE_STRIP );
        for ( int i = 0; i <= slices; i++ )
        {
            s = (float)i * divS;
            x = radius * n_cos(s);
            y = radius * n_sin(s);
            this->Coord( x, y, sinT );
        }
        this->End();
    }

    // draw the rest of the rings/slices
    for ( int i = 0; i < slices; i++ )
    {
        s = (float)i * divS;
        this->Begin( N_PTYPE_LINE_STRIP );
        for ( t = -extent; t <= extent; t += divT )
        {
            radius = n_cos(t) * sphereRad;
            x = radius * n_cos(s);
            y = radius * n_sin(s);
            this->Coord( x, y, n_sin(t) );
        }
        this->End();
    }
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid sphere centred at the origin.
*/
void nPrimitiveServer::SolidSphere( float radius, int slices, int stacks )
{
    float t, nextT;
    float divS = (float)N_DOUBLE_PI / (float)slices;
    float divT = (float)N_DOUBLE_PI / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    float cosS = 1;
    float sinS = 0;
    float cosNextS = n_cos(nextS);
    float sinNextS = n_sin(nextS);
    float cosT, sinT, cosNextT, sinNextT;
    vector2 p0, p1, p2, p3;
    vector3 n0, n1, n2, n3;

    this->Begin( N_PTYPE_TRIANGLE_LIST );
    int i, j;
    for ( i = 0; i < slices; i++ )
    {
        t = -(float)N_PI;
        nextT = t + divT;
        cosT = 1;
        sinT = 0;
        cosNextT = n_cos(nextT);
        sinNextT = n_sin(nextT);
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( cosT * cosS, cosT * sinS, sinT ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( cosT * cosNextS, cosT * sinNextS, sinT ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );
            n2.set( cosNextT * cosNextS, cosNextT * sinNextS, sinNextT ); n2.norm();
            p2.set( radius * n2.x, radius * n2.y );
            n3.set( cosNextT * cosS, cosNextT * sinS, sinNextT ); n3.norm();
            p3.set( radius * n3.x, radius * n3.y );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, radius * sinT );
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            // tri 2
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            this->Norm( n3.x, n3.y, n3.z );
            this->Coord( p3.x, p3.y, radius * sinNextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );

            t = nextT;
            nextT += divT;
            cosT = cosNextT;
            cosNextT = n_cos(nextT);
            sinT = sinNextT;
            sinNextT = n_sin(nextT);
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        cosNextS = n_cos(nextS);
        sinS = sinNextS;
        sinNextS = n_sin(nextS);
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cone.
    @param base Radius of the cone base.
    @param height Height of cone.
    @param closed Draw the cone base?
    @param slices Number of slices.
    @param stacks Number of stacks.

    Render a cone oriented along the z-axis with base at z = 0 and the
    top at z = height. The cone is subdivided around the z-axis into
    slices and along the z-axis into stacks.
*/
void nPrimitiveServer::WireCone( float base, float height, bool closed,
                                int slices, int stacks )
{
    float s, t;
    float nextS, nextX, nextY;
    float x = base;
    float y = 0.0f;
    float gradient = -base / height;
    float divS = (float)N_DOUBLE_PI / (float)slices;
    float divT = height / (float)stacks;
    float radius = base;

    this->Begin( N_PTYPE_LINE_LIST );

    // draw vertical side lines & optionally the base
    for ( int i = 0; i < slices; i++ )
    {
        s = (float)i * divS;
        x = base * n_cos(s);
        y = base * n_sin(s);
        this->Coord( x, y, 0 );
        this->Coord( 0, 0, height );
        // draw part of base
        if ( closed )
        {
            this->Coord( x, y, 0 );
            this->Coord( 0, 0, 0 );
        }
    }

    // draw rings
    for ( int j = 0; j < stacks; j++ )
    {
        t = (float)j * divT;
        radius = gradient * t + base;
        x = radius;
        y = 0.0f;
        for ( int i = 0; i < slices; i++ )
        {
            nextS = (float)(i + 1) * divS;
            nextX = radius * n_cos(nextS);
            nextY = radius * n_sin(nextS);
            // draw arc
            this->Coord( x, y, t );
            this->Coord( nextX, nextY, t ); 
            x = nextX;
            y = nextY;
        }
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cone.
*/
void nPrimitiveServer::SolidCone( float base, float height, bool closed,
                                 int slices, int stacks )
{
    float t, nextT;
    float gradient = -base / height;
    float divS = (float)N_DOUBLE_PI / (float)slices;
    float divT = height / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    float rad, nextRad;
    float cosS = 1;
    float sinS = 0;
    float cosNextS = n_cos(nextS);
    float sinNextS = n_sin(nextS);
    vector2 p0, p1, p2, p3;
    vector3 n0, n1;

    this->Begin( N_PTYPE_TRIANGLE_LIST );
    int i, j;
    for ( i = 0; i < slices; i++ )
    {
        t = 0.0f;
        nextT = divT;
        rad = base;
        nextRad = gradient * divT + base;
        n0.set( divT * cosS, divT * sinS, -gradient ); n0.norm();
        n1.set( divT * cosNextS, divT * sinNextS, -gradient ); n1.norm();
        for ( j = 0; j < stacks - 1; j++ )
        {
            p0.set( rad * cosS, rad * sinS );
            p1.set( rad * cosNextS, rad * sinNextS );
            p2.set( nextRad * cosNextS, nextRad * sinNextS );
            p3.set( nextRad * cosS, nextRad * sinS );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, t );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, t );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p2.x, p2.y, nextT );
            // tri 2
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p2.x, p2.y, nextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p3.x, p3.y, nextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, t );

            t = nextT;
            nextT += divT;
            rad = nextRad;
            nextRad += gradient * divT;
        }

        // tri on the tip
        p0.set( rad * cosS, rad * sinS );
        p1.set( rad * cosNextS, rad * sinNextS );
        this->Norm( n0.x, n0.y, n0.z );
        this->Coord( p0.x, p0.y, t );
        this->Norm( n1.x, n1.y, n1.z );
        this->Coord( p1.x, p1.y, t );
        this->Norm( 0, 0, 1 ); // what's the normal at the very tip?
        this->Coord( 0, 0, height );

        // draw a 'base' tri
        if ( closed )
        {
            p0.set( base * cosS, base * sinS );
            p1.set( base * cosNextS, base * sinNextS );
            // base tri
            this->Norm( 0, 0, -1 );
            this->Coord( 0, 0, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p1.x, p1.y, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p0.x, p0.y, 0 );
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        sinS = sinNextS;
        cosNextS = n_cos(nextS);
        sinNextS = n_sin(nextS);
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe capsule.
    @param radius Capsule radius.
    @param length Capsule length.
    @param slices Number of slices.
    @param stacks Number of stacks.

    Render a capsule oriented along the z-axis with one end at z = 0 and
    the other at z = length + 2 * radius.
*/
void nPrimitiveServer::WireCapsule( float /*radius*/, float /*length*/, 
                                   int /*slices*/, int /*stacks*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid capsule.
*/
void nPrimitiveServer::SolidCapsule( float radius, float length, 
                                    int slices, int stacks )
{
    this->SolidCylinder( radius, length, false, slices, 1 );

    float t, nextT;
    float divS = (float)N_DOUBLE_PI / (float)slices;
    float divT = (float)N_PI / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    float cosS = 1;
    float sinS = 0;
    float cosNextS = n_cos(nextS);
    float sinNextS = n_sin(nextS);
    float cosT, sinT, cosNextT, sinNextT;
    vector2 p0, p1, p2, p3;
    vector3 n0, n1, n2, n3;
    int i, j;

    this->Begin( N_PTYPE_TRIANGLE_LIST );  

    // render one end hemi-sphere
    for ( i = 0; i < slices; i++ )
    {
        t = -(float)N_PI;
        nextT = t + divT;
        cosT = -1;
        sinT = 0;
        cosNextT = n_cos(nextT);
        sinNextT = n_sin(nextT);
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( cosT * cosS, cosT * sinS, sinT ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( cosT * cosNextS, cosT * sinNextS, sinT ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );
            n2.set( cosNextT * cosNextS, cosNextT * sinNextS, sinNextT ); n2.norm();
            p2.set( radius * n2.x, radius * n2.y );
            n3.set( cosNextT * cosS, cosNextT * sinS, sinNextT ); n3.norm();
            p3.set( radius * n3.x, radius * n3.y );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, radius * sinT );
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            // tri 2
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT );
            this->Norm( n3.x, n3.y, n3.z );
            this->Coord( p3.x, p3.y, radius * sinNextT );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT );

            t = nextT;
            nextT += divT;
            cosT = cosNextT;
            cosNextT = n_cos(nextT);
            sinT = sinNextT;
            sinNextT = n_sin(nextT);
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        cosNextS = n_cos(nextS);
        sinS = sinNextS;
        sinNextS = n_sin(nextS);
    }

    s = 0.0f;
    nextS = divS;
    cosS = 1;
    sinS = 0;
    cosNextS = n_cos(nextS);
    sinNextS = n_sin(nextS);

    // render the other hemi-sphere
    for ( i = 0; i < slices; i++ )
    {
        t = 0.0f;
        nextT = t + divT;
        cosT = 1;
        sinT = 0;
        cosNextT = n_cos(nextT);
        sinNextT = n_sin(nextT);
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( cosT * cosS, cosT * sinS, sinT ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( cosT * cosNextS, cosT * sinNextS, sinT ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );
            n2.set( cosNextT * cosNextS, cosNextT * sinNextS, sinNextT ); n2.norm();
            p2.set( radius * n2.x, radius * n2.y );
            n3.set( cosNextT * cosS, cosNextT * sinS, sinNextT ); n3.norm();
            p3.set( radius * n3.x, radius * n3.y );

            // tri 1
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT + length );
            this->Norm( n1.x, n1.y, n1.z );
            this->Coord( p1.x, p1.y, radius * sinT + length );
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT + length );
            // tri 2
            this->Norm( n2.x, n2.y, n2.z );
            this->Coord( p2.x, p2.y, radius * sinNextT + length );
            this->Norm( n3.x, n3.y, n3.z );
            this->Coord( p3.x, p3.y, radius * sinNextT + length );
            this->Norm( n0.x, n0.y, n0.z );
            this->Coord( p0.x, p0.y, radius * sinT + length );

            t = nextT;
            nextT += divT;
            cosT = cosNextT;
            cosNextT = n_cos(nextT);
            sinT = sinNextT;
            sinNextT = n_sin(nextT);
        }

        s = nextS;
        nextS += divS;
        cosS = cosNextS;
        cosNextS = n_cos(nextS);
        sinS = sinNextS;
        sinNextS = n_sin(nextS);
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cylinder.
    @param radius Cylinder radius.
    @param length Cylinder length.
    @param closed Draw the cylinder ends?
    @param slices Number of slices.
    @param stacks Number of stacks.

    Render a capsule oriented along the z-axis with one end at z = 0 and
    the other at z = length. The cylinder is subdivided around the z-axis into
    slices and along the z-axis into stacks.
*/
void nPrimitiveServer::WireCylinder( float radius, float length, 
                                    bool closed, int slices, int stacks )
{
    float s, t;
    float nextS, nextX, nextY;
    float x = radius;
    float y = 0.0f;
    float divS = (float)N_DOUBLE_PI / (float)slices;
    float divT = length / (float)stacks;

    this->Begin( N_PTYPE_LINE_LIST );
    for ( int i = 0; i < slices; i++ )
    {
        nextS = (float)(i + 1) * divS;
        nextX = radius * n_cos(nextS);
        nextY = radius * n_sin(nextS);

        // draw arcs along the cylinder axis
        for ( int j = 0; j <= stacks; j++ )
        {
            t = (float)j * divT;
            this->Coord( x, y, t );
            this->Coord( nextX, nextY, t );
        }

        // draw a side line
        this->Coord( x, y, 0 );
        this->Coord( x, y, length );

        // draw part of the cylinder 'ends'
        if ( closed )
        {
            this->Coord( x, y, 0 );
            this->Coord( 0, 0, 0 );
            this->Coord( x, y, length );
            this->Coord( 0, 0, length );
        }

        s = nextS;
        x = nextX;
        y = nextY;
    }
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid cylinder.
*/
void nPrimitiveServer::SolidCylinder( float radius, float length, 
                                     bool closed, int slices, int stacks )
{
    float t, nextT;
    float divS = (float)N_DOUBLE_PI / (float)slices;
    float divT = length / (float)stacks;
    float s = 0.0f;
    float nextS = divS;
    vector2 p0, p1, n0, n1;

    /*
    Could probably be optimised to use triangle strips, but
    I couldn't be bothered.
    */

    this->Begin( N_PTYPE_TRIANGLE_LIST );
    int i, j;
    for ( i = 0; i < slices; i++ )
    {
        t = 0.0f;
        nextT = divT;
        for ( j = 0; j < stacks; j++ )
        {
            n0.set( n_cos(s), n_sin(s) ); n0.norm();
            p0.set( radius * n0.x , radius * n0.y );
            n1.set( n_cos(nextS), n_sin(nextS) ); n1.norm();
            p1.set( radius * n1.x, radius * n1.y );

            // tri 1
            this->Norm( n0.x, n0.y, 0 );
            this->Coord( p0.x, p0.y, t );
            this->Norm( n1.x, n1.y, 0 );
            this->Coord( p1.x, p1.y, t );
            this->Norm( n1.x, n1.y, 0 );
            this->Coord( p1.x, p1.y, nextT );
            // tri 2
            this->Norm( n1.x, n1.y, 0 );
            this->Coord( p1.x, p1.y, nextT );
            this->Norm( n0.x, n0.y, 0 );
            this->Coord( p0.x, p0.y, nextT );
            this->Norm( n0.x, n0.y, 0 );
            this->Coord( p0.x, p0.y, t );

            t = nextT;
            nextT += divT;
        }

        // draw two 'end' tris
        if ( closed )
        {
            p0.set( radius * n_cos(s), radius * n_sin(s) );
            p1.set( radius * n_cos(nextS), radius * n_sin(nextS) );
            // tri on one end
            this->Norm( 0, 0, -1 );
            this->Coord( 0, 0, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p1.x, p1.y, 0 );
            this->Norm( 0, 0, -1 );
            this->Coord( p0.x, p0.y, 0 );
            // tri on the other end
            this->Norm( 0, 0, 1 );
            this->Coord( p0.x, p0.y, length );
            this->Norm( 0, 0, 1 );
            this->Coord( p1.x, p1.y, length );
            this->Norm( 0, 0, 1 );
            this->Coord( 0, 0, length );
        }

        s = nextS;
        nextS += divS;
    }

    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe cube centred at the origin.
    @param size Length of each of the cube's sides.
*/
void nPrimitiveServer::WireCube( float size )
{
    float half = n_half(size);
    // top
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -half, half, -half );
    this->Coord( half, half, -half );
    this->Coord( half, half, half );
    this->Coord( -half, half, half );
    this->Coord( -half, half, -half );
    this->End();
    // bottom
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -half, -half, -half );
    this->Coord( half, -half, -half );
    this->Coord( half, -half, half );
    this->Coord( -half, -half, half );
    this->Coord( -half, -half, -half );
    this->End();
    // sides
    this->Begin( N_PTYPE_LINE_LIST );
    this->Coord( -half, -half, -half ); this->Coord( -half, half, -half );
    this->Coord( half, -half, -half );  this->Coord( half, half, -half );
    this->Coord( half, -half, half );   this->Coord( half, half, half );
    this->Coord( -half, -half, half );  this->Coord( -half, half, half );
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid cube.
*/
void nPrimitiveServer::SolidCube( float /*size*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe box centred at the origin.
    @param lx Length of box in the x-dimension.
    @param ly Length of box in the y-dimension.
    @param lz Length of box in the z-dimension.
*/
void nPrimitiveServer::WireBox( float lx, float ly, float lz )
{
    float x = n_half(lx);
    float y = n_half(ly);
    float z = n_half(lz);
    // top
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -x, y, -z );
    this->Coord( x, y, -z );
    this->Coord( x, y, z );
    this->Coord( -x, y, z );
    this->Coord( -x, y, -z );
    this->End();
    // bottom
    this->Begin( N_PTYPE_LINE_STRIP );
    this->Coord( -x, -y, -z );
    this->Coord( x, -y, -z );
    this->Coord( x, -y, z );
    this->Coord( -x, -y, z );
    this->Coord( -x, -y, -z );
    this->End();
    // sides
    this->Begin( N_PTYPE_LINE_LIST );
    this->Coord( -x, -y, -z );  this->Coord( -x, y, -z );
    this->Coord( x, -y, -z );   this->Coord( x, y, -z );
    this->Coord( x, -y, z );    this->Coord( x, y, z );
    this->Coord( -x, -y, z );   this->Coord( -x, y, z );
    this->End();
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid box centred at the origin.
*/
void nPrimitiveServer::SolidBox( float /*lx*/, float /*ly*/, float /*lz*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a wireframe torus.
    @param innerRadius The inner radius of the torus.
    @param outerRadius The outer radius of the torus.
    @param sides
    @param rings

    Render a torus (doughnut) centred at the origin with its central
    axis aligned with the z-axis. The torus is subdivided into 'rings'
    segments along its circular centre line and into 'sides' segments
    along that line.
*/
void nPrimitiveServer::WireTorus( float /*innerRadius*/, float /*outerRadius*/, 
                                 int /*sides*/, int /*rings*/ )
{
}

//------------------------------------------------------------------------------
/**
    @brief Render a solid torus.
*/
void nPrimitiveServer::SolidTorus( float /*innerRadius*/, float /*outerRadius*/, 
                                  int /*sides*/, int /*rings*/ )
{
}

//------------------------------------------------------------------------------
/**
*/
void nPrimitiveServer::CheckerPlane(float planeExtent, float gridSize)
{
    float extent = float((int)(planeExtent / gridSize));
    
    int inc_y = 0;
    int inc_x = 0;
    for (float y = -extent; y < extent; y += gridSize)
    {
        for (float x = -extent; x < extent; x += gridSize)
        {
            this->Begin(N_PTYPE_TRIANGLE_STRIP);
            if ((inc_x + inc_y) % 2) 
                this->Rgba(1.0f,0.4f,0.4f,0.3f);
            else
                this->Rgba(0.4f,0.4f,1.0f,0.3f);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x+gridSize, 0.0f, y);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x, 0.0f, y);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x+gridSize, 0.0f, y+gridSize);
            this->Norm(0.0f, 1.0f, 0.0f);
            this->Coord(x, 0.0f, y+gridSize);
            this->End();
            inc_x++;
        }        
        inc_y++;
    }   
}


