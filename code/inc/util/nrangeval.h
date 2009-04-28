#ifndef N_RANGEVAL_H
#define N_RANGEVAL_H
//------------------------------------------------------------------------------
/**
A stack template class.

(C) 2002 RadonLabs GmbH
*/
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif


template<class TYPE> class nRangeVal
{	
public:
	nRangeVal() { this->min = this->max = this->val = (TYPE)0;}
	nRangeVal(TYPE a, TYPE b, TYPE v = (TYPE)0) { this->set_range(a, b); this->set(v);}
	TYPE min;
	TYPE max;
	TYPE val;

	//TYPE operator=(const nRangeVal<TYPE>& v) { return v.val; }

	operator TYPE() const { return this->val; }

	nRangeVal<TYPE>& operator=(TYPE v)
	{
		this->set(v);
		return *this;
	}

	nRangeVal<TYPE>& operator=(nRangeVal<TYPE>& rng)
	{		
		if (rng.width())
			this->set(this->min + (rng.val-rng.min)*this->width()/rng.width());
		else
			this->set(rng.val);
		return *this;
	}

	void set_range(TYPE a, TYPE b) 
	{
		if (b < a)			
		{
			this->min = b;
			this->max = a;
		}
		else
		{
			this->min = a;
			this->max = b;
		}
	}
	
	TYPE width() const { return n_abs(this->max - this->min); }

	void set(TYPE v) 
	{
		if (v < this->min) this->val = this->min;
		else if (v > this->max) this->val = this->max;
		else this->val = v;
	}
};
#endif