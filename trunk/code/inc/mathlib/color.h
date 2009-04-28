#ifndef N_COLOR4_H
#define N_COLOR4_H

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif
//------------------------------------------------------------------------------
/**
	class color4
*/
typedef t_vector4<float> vector4f;

class color4 : public vector4f
{
public:
	//-- constructors -----------------------------------------------
	color4() : vector4f() {}
    color4(float nr, float ng, 
		float nb, float na) : vector4f(nr, ng, nb, na) { this->saturate();}
    color4(const color4& o)	: vector4f(o.r, o.g, o.b, o.a) {}
	color4(const float* p) : vector4f(p) { this->saturate(); }	

	color4(long uc) 
	{
		if (uc > 0)
		{
			unsigned char ur = (unsigned char)uc; uc >>= 8;
			unsigned char ug = (unsigned char)uc; uc >>= 8;
			unsigned char ub = (unsigned char)uc; uc >>= 8;
			unsigned char ua = (unsigned char)uc; 
			this->set(ur, ug, ub, ua);
		}
		else
			vector4f::set(0.0f, 0.0f, 0.0f, 1.0f);
	}
		

	void set(unsigned char ur, unsigned char ug, unsigned char ub, unsigned char ua)
	{
		vector4f::set(((float)ur)/255.0f, ((float)ug)/255.0f, 
			((float)ub)/255.0f, ((float)ua)/255.0f);
	}

	// - color4 specific
	color4& operator= (const color4& o) { vector4f::set(o.r, o.g, o.b, o.a); return *this; }

	void saturate() 
	{
		n_saturate(r);
		n_saturate(g);
		n_saturate(b);
		n_saturate(a);	
	}
};
#endif
//-------------------------------------------------------------------