#ifndef N_CSGPRIMS_H
#define N_CSGPRIMS_H

#ifdef _NEBULA_DEVICE_
#include "util/nstl.h"
#else
#include <vector>
#include <algorithm>
#endif

template <class vtype>
class t_vertices_v : public std::vector<vtype> 
{
public:
	t_vertices_v() {this->clear();}
	size_t add_unique(const vtype& v)  {
		size_t i = this->find(v);
		if (i != UINT_MAX) {
			return i;
		} else {
			this->push_back(v);
			return this->size()-1;
		}
	}

	size_t find(const vtype& v) 
	{
		size_t num = this->size();
		if (num) {
			for (size_t i = 0; i < num; i++) 
			{
				if (this->v(i) == v) 
				{
					return i;
				}
			}
		}		
		return UINT_MAX;
	}
	
	vtype& v(size_t i) 
	{
		return (*this)[i];
	}

	const vtype& v(size_t i) const {
		return (*this)[i];
	}

	vtype* get_buffer() 
	{
		return &((*this)[0]);
	}

	const vtype* get_buffer() const
	{
		return &((*this)[0]);
	}
};


#ifndef N_DEBUG_H
#include <assert.h>
#define n_assert	assert
#endif

#include "mathlib/vector.h"
#include "mathlib/line.h"
#include "mathlib/triangle.h"

typedef std::deque<vector3>			vertices_q;
typedef std::list<vector3>			vertices_l;
typedef std::vector<line3>			lines_v;
typedef std::vector<triangle>		triangles_v;

typedef t_vertices_v<vector3>		vertices_v;
typedef t_vertices_v<vector2>		vertices2_v;
typedef std::vector<vertices_v>		vec_vertices_v;
#endif