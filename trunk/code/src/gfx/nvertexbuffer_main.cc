#define N_IMPLEMENTS nVertexBuffer
//-------------------------------------------------------------------
//  nvertexbuffer_main.cc
//  (C) 2000 RadonLabs GmbH -- A.Weissflog
//-------------------------------------------------------------------
#include "gfx/nvertexbuffer.h"
#include "gfx/nvertexpool.h"
#include "gfx/nvertexbubble.h"
#include "gfx/nvertexpoolmanager.h"
#include "gfx/nindexbuffer.h"
#include "mathlib/triangle.h"

nNebulaClass(nVertexBuffer, "nroot");

//-------------------------------------------------------------------
//  ~nVertexBuffer()
//  01-Sep-00   floh    created
//-------------------------------------------------------------------
nVertexBuffer::~nVertexBuffer()
{
    n_assert(0 == this->lock_count);

    // releasing the vertex bubble is a bit tricky, since
    // we first have to find the responsible vertex pool,
    // and then the vertex pool manager which is responsible
    // for the vertex pool...
    if (this->vbubble) {
        nVertexPool *vp = this->vbubble->GetVertexPool();
        n_assert(vp);
        nVertexPoolManager *vpm = vp->GetVertexPoolManager();
        n_assert(vpm);
        vpm->ReleaseVertexBubble(this->vbubble);
        this->vbubble = NULL;
    }
}

//-------------------------------------------------------------------
/**
    Lock the vertex bubble and validate public pointers.

    History:
     - 01-Sep-00   floh    created
     - 16-Oct-00   floh    readonly buffers can now be locked multiple
                           times, non-readonly buffer are protected from
                           being locked
     - 23-Oct-00   floh    + JointIndices, Weights
*/
//-------------------------------------------------------------------
bool nVertexBuffer::LockVertices(void)
{
    n_assert(this->vbubble);

    // read only vertex buffers can be locked multiple times
    if ((this->lock_count>0) && 
        (N_VBTYPE_READONLY != this->vbubble->GetVertexPool()->GetVBufType())) 
    {        
        n_error("Trying to lock non-readonly vbuffer '%s' multiple times!\n",
                 this->GetFullName().c_str());
    }

    nVertexPool *vp = this->vbubble->GetVertexPool();
    n_assert(vp);
    
    if (vp->LockBubble(this->vbubble)) {
        this->coord_ptr = vp->GetCoordPtr();
        this->norm_ptr  = vp->GetNormPtr();
        this->color_ptr = vp->GetColorPtr();
        int i;
        for (i=0; i<N_MAXNUM_TEXCOORDS; i++) {
            this->uv_ptr[i] = vp->GetUvPtr(i);
        };
        this->j_ptr   = vp->GetJointIndexPtr();
        this->w_ptr   = vp->GetWeightPtr();
        this->stride4 = vp->GetStride4();
        this->num_v   = this->vbubble->GetBubbleSize();
        this->lock_count++;
        return true;
    } else {
        return false;
    }
}

//-------------------------------------------------------------------
/**
    Unlock the vertex bubble and invalidate public array pointers.

    History:
     - 01-Sep-00   floh    created
*/
//-------------------------------------------------------------------
void nVertexBuffer::UnlockVertices(void)
{
    n_assert(this->vbubble);
    n_assert(this->lock_count > 0);

    nVertexPool *vp = this->vbubble->GetVertexPool();
    n_assert(vp);
    vp->UnlockBubble(this->vbubble);

    this->lock_count--;

    this->coord_ptr = NULL;
    this->norm_ptr  = NULL;
    this->color_ptr = NULL;
    this->j_ptr     = NULL;
    this->w_ptr     = NULL;
    n_memset(this->uv_ptr,0,sizeof(this->uv_ptr));
    this->stride4 = 0;
    this->num_v   = 0;
}

float nVertexBuffer::GetRayIntersection(nIndexBuffer* ib, const line3& ray)
{		
	n_assert("No Index Buffer porvided" && ib);

	if (!this->bbox.intersect(ray)) return -1.0f;

	this->LockVertices();      /*! lock vertices to get access; */
	ib->Lock();

	float*      coords  = this->coord_ptr;  /*! pointer to vertices */

	float*      cur_v;			/*! pointer to current vertex */

	vector3     a, b, c;		/*! face coords */
	triangle    face;			/*! face        */ 
	long        i, j1, j2, j3;  /*! indexes     */		

	float  t, t_min =  FLT_MAX;  	
	long              ni = ib->GetNumIndices() - 2;
	unsigned short*   idxs = ib->GetIndexPointer();


	switch (ib->GetPrimType()) 
	{
	case N_PTYPE_TRIANGLE_LIST:     
		for (i = 0; i < ni; i += 3) 
		{
			j1 = idxs[i + 0];
			j2 = idxs[i + 1];
			j3 = idxs[i + 2];
			cur_v = coords + j1 * (this->stride4);  a.set(cur_v[0], cur_v[1], cur_v[2]); 
			cur_v = coords + j2 * (this->stride4);  b.set(cur_v[0], cur_v[1], cur_v[2]); 
			cur_v = coords + j3 * (this->stride4);  c.set(cur_v[0], cur_v[1], cur_v[2]);      
			face.set(a, b, c);

			if (face.intersect(ray, t))
			{
				if ( t >= 0 && t < t_min ) 
					t_min = t;				
			}
		}   
		break;
	case N_PTYPE_TRIANGLE_FAN:
		j1 = idxs[0];
		cur_v = coords + j1*(this->stride4);
		a.set(cur_v[0], cur_v[1], cur_v[2]);

		for (i = 0; i < ni; i++) 
		{
			j2 = idxs[i + 1];
			j3 = idxs[i + 2];
			cur_v = coords + j2 * (this->stride4);  b.set(cur_v[0], cur_v[1], cur_v[2]); 
			cur_v = coords + j3 * (this->stride4);  c.set(cur_v[0], cur_v[1], cur_v[2]);      
			face.set(a, b, c);

			if (face.intersect(ray, t))
			{
				if ( t >= 0 && t < t_min ) 
					t_min = t;				
			}
		}   
		break;
	case N_PTYPE_TRIANGLE_STRIP:    
		for (i = 0; i < ni; i++) 
		{
			j1 = idxs[i];            
			j2 = idxs[i + 1 + (i&0x1)];
			j3 = idxs[i + 2 - (i&0x1)];
			cur_v = coords + j1 * (this->stride4);  a.set(cur_v[0], cur_v[1], cur_v[2]); 
			cur_v = coords + j2 * (this->stride4);  b.set(cur_v[0], cur_v[1], cur_v[2]); 
			cur_v = coords + j3 * (this->stride4);  c.set(cur_v[0], cur_v[1], cur_v[2]);      
			face.set(a, b, c);

			if (face.intersect(ray, t))
			{
				if ( t >= 0 && t < t_min ) 
					t_min = t;				
			}
		}
		break;	
	}

	ib->Unlock();

	this->UnlockVertices();

	if (t_min < FLT_MAX/2.0f) 
		return t_min;
	else
		return -2.0f;    
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
