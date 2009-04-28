#ifndef _CSGMESH_H
#define _CSGMESH_H

#include "csgmath/csgtriangle.h"
#include "csgmath/csgedge.h"
#include "csgmath/csgprims.h"

#define A_IN 0x1
#define B_IN 0x2
#define C_IN 0x4

#define TRI_OUT  0x0
#define TRI_IN  ( A_IN | B_IN | C_IN )

/**
    @class csgmesh
    @brief basic mesh interface;
    
*/
//-------------------------------------------------------------------
class csgMesh {
public:   
	
    vertices_v  vertices;
    faces_v     faces;
    edges_v     edges;

public:
	csgMesh() : in_begin(false){};
    // start mesh description
    virtual void Begin() {
        assert(!this->in_begin && "error: mesh already in begin");
        this->in_begin = true;
        this->edges.clear();        
        this->vertices.clear();
        this->faces.Begin();
        
    }

    virtual void AddVertex(const vector3& v) {
        assert(this->in_begin && "error: mesh not started");
        this->vertices.push_back(v);
    }

    virtual void AddVertices(const vertices_v& vs) {
        assert(this->in_begin && "error: mesh not started");
        for (vertices_v::const_iterator i = vs.begin(); i != vs.end(); i++)
            this->vertices.push_back(*i);
    }

    virtual void AddFace(long a, long b, long c) {
        assert(this->in_begin && "error: mesh not started");
        this->faces.Add(a, b, c);
    }

    virtual void AddFace(const face_t& f) {
        assert(this->in_begin && "error: mesh not started");
        this->faces.Add(f);
    }

    // finish mesh description
    virtual void End() {
        assert(this->in_begin && "error: mesh not started");
        this->in_begin = false;
        //this->unique();
        //this->remove_unused();
        this->faces.End();
        
        this->edges.Begin();
        for(const face_t* i = this->fbegin(); i != this->fend(); i++) {
            this->edges.Add(i->a, i->b);
            this->edges.Add(i->b, i->c);
            this->edges.Add(i->c, i->a);
        }
        this->edges.End();        
    }

    // call those functions from implementation. MUST
    virtual long GetVerticesNum()   const { assert(!this->in_begin && "error: mesh is not complete"); return this->vertices.size(); }
    virtual long GetFacesNum()      const { assert(!this->in_begin && "error: mesh is not complete"); return this->faces.size(); }
    virtual long GetEdgesNum()      const { assert(!this->in_begin && "error: mesh is not complete"); return this->edges.size(); }
	
    virtual vector3& v(long i) { 
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetVerticesNum() && "vertex index run out of boundaries");
        return this->vertices[i]; 
    }

    virtual face_t& f(long i) {
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetFacesNum() && "face index run out of boundaries");
        return this->faces[i]; 
    }

    virtual edge_t& e(long i) { 
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetEdgesNum() && "edge index run out of boundaries");
        return this->edges[i]; 
    }
    
    virtual const vector3& GetVertex(long i) const { 
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetVerticesNum() && "vertex index run out of boundaries");
        return this->vertices[i]; 
    }

    virtual const face_t& GetFace(long i) const {
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetFacesNum() && "face index run out of boundaries");
        return this->faces[i]; 
    }

    virtual const edge_t& GetEdge(long i) const { 
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetEdgesNum() && "edge index run out of boundaries");
        return this->edges[i]; 
    }

    virtual const vector3* GetVertexPtr(long i) const { 
		assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetVerticesNum() && "vertex index run out of boundaries");
		if (i < this->GetVerticesNum())
			return &(this->vertices[i]); 
		else 
			return 0;
	}

	virtual const face_t* GetFacePtr(long i) const { 
		assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetFacesNum() && "face index run out of boundaries");
		if (i < this->GetFacesNum())
			return &(this->faces[i]); 
		else 
			return 0;
	}

	virtual const edge_t* GetEdgePtr(long i) const { 
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetEdgesNum() && "edge index run out of boundaries");
		if (i < this->GetEdgesNum())
			return &(this->edges[i]); 
		else 
			return 0;
    }    
    
    virtual const vector3*  vbegin() {assert(!this->in_begin && "error: mesh is not complete"); return vertices.begin();}
    virtual const face_t*   fbegin() {assert(!this->in_begin && "error: mesh is not complete"); return faces.begin();}
    virtual const edge_t*   ebegin() {assert(!this->in_begin && "error: mesh is not complete"); return edges.begin();}

    virtual const vector3*  vend() {assert(!this->in_begin && "error: mesh is not complete"); return vertices.end();}
    virtual const face_t*   fend() {assert(!this->in_begin && "error: mesh is not complete"); return faces.end();}
    virtual const edge_t*   eend() {assert(!this->in_begin && "error: mesh is not complete"); return edges.end();}

    virtual bool GetEdgeLine(long i, line3& l) const {
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetEdgesNum() && "edge index run out of boundaries");
        const edge_t* e = this->GetEdgePtr(i);
        if (e) {
            l.set(this->GetVertex(e->a), this->GetVertex(e->b));
            return true;
        } else 
            return false;        
    }

    virtual bool GetFaceTriangle(long i, csgtriangle& t) const {
        assert(!this->in_begin && "error: mesh is not complete"); 
        assert(i < this->GetFacesNum() && "face index run out of boundaries");
		const face_t* f = this->GetFacePtr(i);
        if (f) {
            t.set(this->GetVertex(f->a), this->GetVertex(f->b), this->GetVertex(f->c));
            return true;
        } else 
            return false;        
    }

    bool RayIntersect(const line3& l, floats_q& times) const {
        assert(!this->in_begin && "error: mesh is not complete");
        times.clear();        

        size_t num = this->GetFacesNum();
        for (size_t i = 0; i < num; i++) {		
            csgtriangle tri;
            this->GetFaceTriangle(i, tri);

            float t;

            if (tri.intersect_both_sides(l, t)) {
                if (times.size()) {
                    if (t <= times.front())
                        times.push_front(t);
                    else
                        times.push_back(t);
                } else {
                    times.push_back(t);
                }
            }
			std::sort(times.begin(), times.end());
        }	
        return times.size() != 0;
    }

    bool Surface() const {
        assert(!this->in_begin && "error: mesh is not complete");
        return this->edges.Open();
    }

    bool Inside(const vector3& v) const {
        assert(!this->in_begin && "error: mesh is not complete");
        if (this->Surface())
            return false;

        long tnum = this->GetFacesNum();        
        
        floats_q times;
        if (this->RayIntersect(line3(v, v + vector3(1.f, 1.f, 1.f )), times)){
            return (times.size() & (size_t)0x1) != 0;
        }            
        
        return false;
    }    

    // returns 1 if inside, 0 if outside and -1 if cuts
    long TriTest(const csgtriangle& t) const {
        long res = TRI_OUT;

        if (this->Inside(t.va()))
            res |= A_IN;
        
        if (this->Inside(t.vb()))
            res |= B_IN;

        if (this->Inside(t.vc()))
            res |= C_IN;
        
        return res;
    }

    virtual bool Intersect(const csgMesh* mesh) const {
        assert(!this->in_begin && "error: mesh is not complete"); 
        long tnum = mesh->GetFacesNum();        
        csgtriangle t;
        for (long i = 0; i < tnum; i++) {
            if (mesh->GetFaceTriangle(i, t)) {
                long lnum = this->GetEdgesNum();                
                for (long j = 0; j < lnum; j++) {
                    float tm;
                    line3 l;
                    this->GetEdgeLine(j, l);
                    if (t.intersect_both_sides_seg_with_ends(l, tm)){
                        return true;
                    }
                }
            }
        }
        return false;        
    }

    bool GetSufaceCuts(const csgMesh& cutmesh, csgMesh& inner, csgMesh& out) const {
        assert(!this->in_begin && "error: mesh is not complete"); 
        if (!this->Surface() || cutmesh.Surface())
            return false;

        inner.Begin();
        out.Begin();
        inner.AddVertices(this->vertices);
        out.AddVertices(this->vertices);        

        longs_v cut_faces;
        longs_v code_faces;

        size_t fnum = this->GetFacesNum();
        for (size_t i = 0; i < fnum; i++) {
            csgtriangle t;
            this->GetFaceTriangle(i, t);
            long tt = cutmesh.TriTest(t);
            if (tt == TRI_OUT)
                out.AddFace(this->GetFace(i));
            else if (tt = TRI_IN)
                inner.AddFace(this->GetFace(i));
            else {
                cut_faces.push_back(i);
                code_faces.push_back(tt);
            }
        }

        size_t cfnum = cut_faces.size();
        if (cfnum) {
            for (size_t i = 0; i < cfnum; i++) {
                long ti = cut_faces[i];
                const face_t& f = this->GetFace(ti);
                long tt = code_faces[i];
                csgtriangle t;
                this->GetFaceTriangle(ti, t);
                long cmfnum = cutmesh.GetFacesNum();

                for (long j = 0; j < cmfnum; j++) {                    
                    csgtriangle cmt;
                    cutmesh.GetFaceTriangle(j, cmt);
                    line3 l;
                    if (t.intersect(cmt, l) == 1) {

                    }
                }
            }
        }                

        out.End();
        inner.End();        
    }
	
protected:
    

    long find_vertex(const vector3* v) const {
        for (vertices_v::const_iterator i = this->vertices.begin(); i != this->vertices.end(); i++) {
            
        }
    }

    //bool substitute(long i, long j) {

    //}

//    void unique() {
        
  //  }

    void remove_unused() {
        
    }

	bool in_begin;

};
//-------------------------------------------------------------------
#endif
