#ifndef N_3DNODE_H
#define N_3DNODE_H
//------------------------------------------------------------------------------
/**
    @class n3DNode
    @ingroup NebulaVisnodeModule

    @brief Group and transform objects in the 3d world.

    n3DNode objects are both grouping nodes and transform nodes (as they
    are usually called). A n3DNode is usually the root object of 
    a hierarchy of nVisNode subclass objects, which form together
    a visible/audible object floating in 3d space. This can range
    from the most simplest static textured object (requiring a n3DNode,
    a nMeshNode, nShaderNode and nTextureArrayNode) to
    very complex animated 3d hierarchies, or joint skeletons that
    influence a weighted vertex skin. In any way, at the top is
    always a n3DNode.

    n3DNode have a few more features then just building hierarchies:

    - can lock position on viewer
    - sprite/billboard behaviour (automatic viewer relative orientation)
    - a simple "active/inactive" switch
    - lod computation, plus a "max lod" threshold
    - an optional quaternion interface
    - has a "high detail" flag, which can activate/deactivate the
      node based on the value in the "detail" channel

    (C) 2001 A.Weissflog
*/
#ifndef N_VISNODE_H
#include "node/nvisnode.h"
#endif

#ifndef N_BBOX_H
#include "mathlib/bbox.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_QUATERNION_H
#include "mathlib/quaternion.h"
#endif

#undef N_DEFINES
#define N_DEFINES n3DNode
#include "kernel/ndefdllclass.h"
//--------------------------------------------------------------------
class nSceneGraph2;
class N_PUBLIC n3DNode : public nVisNode 
{
public:
    /// constructor
    n3DNode();

    /// persistency
    virtual bool SaveCmds(nPersistServer* fileServer);
    /// attach to scene
    virtual bool Attach(nSceneGraph2* sceneGraph);
    /// update internal state and set scene graph node transform
    virtual void Compute(nSceneGraph2*);
    /// collect required channels through nVisNode hierarchie
    virtual void AttachChannels(nChannelSet*);

    /// set active flag
    void SetActive(bool b);
    /// get active flag
    bool GetActive() const;
    /// set max lod threshold
    void SetMaxLod(float lod);
    /// get max lod threshold
    float GetMaxLod() const;

    /// set sprite behaviour
    void SetSprite(bool b);
    /// get sprite behaviour
    bool GetSprite() const;
    /// set billboard behaviour
    void SetBillboard(bool b);
    /// get billboard behaviour
    bool GetBillboard() const;
    /// lock position on viewer
    void SetLockViewer(bool b);
    /// get lock viewer status
    bool GetLockViewer() const;
    /// set/unset high detail flag
    void SetHighDetail(bool b);
    /// get the high detail flag
    bool GetHighDetail() const;
    /// set view space flag
    void SetViewSpace(bool b);
    /// get view space flag
    bool GetViewSpace() const;
	/// lock to screen 
	void SetLockScreen(bool b);  // Add by Zeev Clockwise
	/// get lock to screen status
	bool GetLockScreen() const;  // Add by Zeev Clockwise
	/// Align to screen 
	void SetAlignScreen(bool b); // Add by Zeev Clockwise
	/// get align to screen status
	bool GetAlignScreen() const; // Add by Zeev Clockwise
	
    /// directly set matrix, overrides Txyz(), Rxyz() etc...
    void M(const matrix44& m);
    /// set xyz translation
    void Txyz(float x, float y, float z);
	/// set xyz translation
	void Txyz(const vector3& _t);
    /// set x translation
    void Tx(float x);
    /// set y translation
    void Ty(float y);
    /// set z translation
    void Tz(float z);
    /// set xyz rotation (rotation order is always x->y->z)
    void Rxyz(float x, float y, float z);
	/// set xyz rotation (rotation order is always x->y->z)
    void Rxyz(const vector3& _r);
    /// set x rotation
    void Rx(float x);
    /// set y rotation
    void Ry(float y);
    /// set z rotation
    void Rz(float z);
    /// set xyz scale
    void Sxyz(float x, float y, float z);
	/// set xyz scale
    void Sxyz(const vector3& _s);
    /// set x scale
    void Sx(float x);
    /// set y scale
    void Sy(float y);
    /// set z scale
    void Sz(float z);
    /// set quaternion
    void Qxyzw(float x, float y, float z, float w);
	/// set quaternion
    void Qxyzw(const quaternion& _q);

    /// get resulting matrix
    const matrix44& GetM();
    /// get translation
    void GetT(float& x, float& y, float& z) const;
    /// get rotation
    void GetR(float& x, float& y, float& z) const;
    /// get scale
    void GetS(float& x, float& y, float& z) const;
    /// get quaternion
    void GetQ(float& x, float& y, float& z, float& w) const;   

	/// get translation
	const vector3& GetT() const {return this->t;}
    /// get rotation
	const vector3& GetR() const {return this->r;}
    /// get scale
    const vector3& GetS() const {return this->s;}
    /// get quaternion
	const quaternion& GetQ() const {return this->q;}

    /// nKernelServer dependency.
    static nKernelServer* kernelServer;

	/// Get updated matrix in viewer space
	virtual const matrix44& GetResMX() const { return this->resMatrix;	}

	///Get object to world matrix
	virtual const matrix44& GetObject2WorldMX() {return this->object2worldmx; }
	
    ///     set computed flag for meshcluster optimization
    void    SetComputed(bool f);
    ///     get computed flag for meshcluster optimization
    bool    GetComputed() const;
	///     set dummy flag for general optimization
    void    SetDummy(bool f);
    ///     get dummy flag for general optimization
    bool    GetDummy() const;
	///     set geometry root flag
	void    SetGeoRoot(bool f);
	///     get geometry root flag
	bool    GetGeoRoot() const;

	/// Return radius of bounding sphere
	float GetRadius() const { return (this->bbox.vmax - this->bbox.vmin).len()/2.0f; }
	/// Return center of bounding sphere
	vector3 GetCenter() const { return (this->bbox.vmax + this->bbox.vmin)/2.0f; }
	/// Set bounding box
	void SetBBox(const bbox3& box) { this->bbox.set(box.vmin, box.vmax);}
	/// Grow bounding box
	void GrowBBox(const bbox3& box) { this->bbox.grow(box);}
	/// Return bounding box
	const bbox3& GetBBox() const { return this->bbox;}
	
protected:
	/// reset transform
	void ident() 
	{
		this->q.ident();
		this->r.set(0.0f, 0.0f, 0.0f);
		this->t.set(0.0f, 0.0f, 0.0f);
		this->s.set(1.0f, 1.0f, 1.0f);
	}

protected:
	/// flags enum
    enum 
	{
        MXDIRTY     = (1<<0),
        OVERRIDE    = (1<<1),
        SPRITE      = (1<<2),
        BILLBOARD   = (1<<3),
        LOCKVWR     = (1<<4),
        USEQUAT     = (1<<5),   // use (q) to validate mx, not (r)
        ACTIVE      = (1<<6),   // currently active?
        HIGHDETAIL  = (1<<7),   // high detail node?
        VIEWSPACE   = (1<<8),   // this nodes lives in viewer space
        LOCKSCR     = (1<<9),   // Lock to screen - Add by Zeev Clockwise
        ALIGNSCR    = (1<<10),  // Align to screen - Add by Zeev Clockwise
		INTERNAL	= (1<<11),	// Additional internal directly set matrix - Add by Ilya Insightec
		CULLED		= (1<<12),	// Failed on culling test - Add by Ilya Insightec       
		COMPUTED    = (1<<13),	// node was just computed
		DUMMY		= (1<<14),	// node dummy transform flag
		GEOROOT		= (1<<15),	// geometry/shape root node flag
    };

	matrix44 resMatrix;			// matrix resulting from (t,r,s) OR (t,q,s)
	matrix44 object2worldmx;
    matrix44 mx;				// matrix resulting from (t,r,s) OR (t,q,s)
    vector3 t;					// translate vector
    vector3 r;					// rotation angles (rotation order x->y->z)
    vector3 s;					// scale
    quaternion q;				// quaternion (alternative to r)    

	///	 bounding box
	bbox3					  bbox;	

    float maxLod;    
    int n3dFlags;
    int lodChn;
    int detailChn;    
	
    void updateMatrix();	

	void set_up_channels(nSceneGraph2* sceneGraph, float& curLod, float& curDetail);
    void bboard_lookat(matrix44& m, vector3& to, vector3& up);

	void set  (int f)			{ n_setbit  (f, this->n3dFlags); }
	void unset(int f)			{ n_unsetbit(f, this->n3dFlags); }	
	void bset (bool b, int f)   { n_bsetbit (b, f, this->n3dFlags); }

	bool get  (int f) const		{ return n_getbit(f, this->n3dFlags); }
};

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetViewSpace(bool b)
{
	unset(DUMMY);
    this->bset(b, VIEWSPACE);    
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetViewSpace() const
{
    return this->get(VIEWSPACE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetActive(bool b)
{
    this->bset(b, ACTIVE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetActive() const
{
    return this->get(ACTIVE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetMaxLod(float lod)
{
    this->maxLod = lod;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
n3DNode::GetMaxLod() const
{
    return maxLod;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetSprite(bool b) 
{
	unset(DUMMY);
    this->bset(b, SPRITE);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3DNode::GetSprite(void) const
{
    return this->get(SPRITE);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::SetBillboard(bool b) 
{
	unset(DUMMY);
    this->bset(b, BILLBOARD);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetBillboard(void) const
{
    return this->get(BILLBOARD);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::SetLockViewer(bool b) 
{
	unset(DUMMY);
    this->bset(b, LOCKVWR);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool 
n3DNode::GetLockViewer(void) const
{
    return get(LOCKVWR);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::SetHighDetail(bool b)
{
    this->NotifyChannelSetDirty();
    this->bset(b, HIGHDETAIL);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
n3DNode::GetHighDetail() const
{
    return get(HIGHDETAIL);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::M(const matrix44& m) 
{
    set(MXDIRTY);
	unset(DUMMY);	
    set(OVERRIDE);
    mx = m;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Txyz(float x, float y, float z) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    t.set(x,y,z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Txyz(const vector3& _t) {
	set(MXDIRTY);
	unset(DUMMY);
	unset(OVERRIDE);	
	this->t = _t;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Tx(float x) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    t.x = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Ty(float y) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    t.y = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Tz(float z) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    t.z = z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rxyz(float x, float y, float z) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    unset(USEQUAT);
    r.set(n_deg2rad(x),n_deg2rad(y),n_deg2rad(z));
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rxyz(const vector3& _r) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    unset(USEQUAT);
    this->r = _r;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rx(float x) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    unset(USEQUAT);
    r.x = n_deg2rad(x);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Ry(float y) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    unset(USEQUAT);
    r.y = n_deg2rad(y);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Rz(float z) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    unset(USEQUAT);
    r.z = n_deg2rad(z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sxyz(float x, float y, float z) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    s.set(x,y,z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sxyz(const vector3& _s) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    this->s = _s;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sx(float x) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    s.x = x;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sy(float y) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    s.y = y;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Sz(float z) 
{
    set(MXDIRTY);
	unset(DUMMY);
    unset(OVERRIDE);	
    s.z = z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Qxyzw(float x, float y, float z, float w) 
{
    set(MXDIRTY);
	unset(DUMMY);
    set(USEQUAT);
    unset(OVERRIDE);	
    q.set(x,y,z,w);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::Qxyzw(const quaternion& _q)
{
    set(MXDIRTY);
	unset(DUMMY);
    set(USEQUAT);
    unset(OVERRIDE);	
    this->q = _q;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
n3DNode::GetT(float& x, float& y, float& z) const 
{
    x = t.x;
    y = t.y;
    z = t.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::GetR(float& x, float& y, float& z) const 
{	
	n_assert(!get(OVERRIDE));
    x = n_rad2deg(r.x);
    y = n_rad2deg(r.y);
    z = n_rad2deg(r.z);
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::GetS(float& x, float& y, float& z) const 
{
    x = s.x;
    y = s.y;
    z = s.z;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
n3DNode::GetQ(float& x, float& y, float& z, float& w) const
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

//------------------------------------------------------------------------------
/**
*/
inline
const matrix44&
n3DNode::GetM() 
{	
	if (this->get(MXDIRTY) && !this->get(OVERRIDE) && !this->get(DUMMY))
		this->updateMatrix();
    return this->mx;
}

//------------------------------------------------------------------------------
/**
    Add by Zeev Clockwise
*/
inline
void 
n3DNode::SetLockScreen(bool b) 
{
	unset(DUMMY);
    this->bset(b, LOCKSCR);
}

//------------------------------------------------------------------------------
/**
    Add by Zeev Clockwise
*/
inline
bool 
n3DNode::GetLockScreen(void) const
{
    return get(LOCKSCR);
}

//------------------------------------------------------------------------------
/**
    Add by Zeev Clockwise
*/
inline
void 
n3DNode::SetAlignScreen(bool b) 
{
	unset(DUMMY);
    this->bset(b, ALIGNSCR);    
}

//------------------------------------------------------------------------------
/**
    Add by Zeev Clockwise
*/
inline
bool 
n3DNode::GetAlignScreen(void) const
{
    return get(ALIGNSCR);
}

//------------------------------------------------------------------------------
/**
	set computed flag for meshcluster optimization
*/
inline
void n3DNode::SetComputed(bool b) 
{
	this->bset(b, COMPUTED);	
}

//------------------------------------------------------------------------------
/**
	get computed flag for meshcluster optimization
*/
inline
bool 
n3DNode::GetComputed() const 
{
	return get(COMPUTED); 
}

//------------------------------------------------------------------------------
/**
	set dummy flag for general optimization
*/
inline
void n3DNode::SetDummy(bool b) 
{ 
	if (b)
		this->unset(MXDIRTY);
	this->bset(b, DUMMY);	
}

//------------------------------------------------------------------------------
/**
	get dummy flag for general optimization
*/
inline
bool 
n3DNode::GetDummy() const 
{
	return get(DUMMY); 
}

//------------------------------------------------------------------------------
/**
	set geometry root flag
*/
inline
void n3DNode::SetGeoRoot(bool b) 
{
	if (b) unset(DUMMY);
	this->bset(b, GEOROOT);	
}

//------------------------------------------------------------------------------
/**
	get geometry root flag
*/
inline
bool 
n3DNode::GetGeoRoot() const 
{	
	return get(GEOROOT); 
}

//-----------------------------------------------------------------------------
/**
    
*/
inline 
matrix44 GetNodeToNodeTransform(n3DNode* from, n3DNode* to)
{
	matrix44 from_m(from->GetObject2WorldMX());
	matrix44 to_m(to->GetObject2WorldMX());
	to_m.invert();
	from_m.mult_simple(to_m);
	return from_m;
}
//--------------------------------------------------------------------
#endif
