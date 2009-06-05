#ifndef N_VARIABLECONTEXT_H
#define N_VARIABLECONTEXT_H
//------------------------------------------------------------------------------
/**
    A variable context is a container for nVariable objects. Variables
    are accessed by their global nVariableHandle.
    
    (C) 2002 RadonLabs GmbH
*/
#ifndef N_VARIABLE_H
#include "variable/nvariable.h"
#endif

//------------------------------------------------------------------------------
class nVariableContext : public std::vector<nVariable>
{
public:
    /// constructor
    nVariableContext();
    /// destructor
	virtual ~nVariableContext() {}
    /// clear the variable context
	void Clear();
    /// add a variable to the context
	void AddVariable(const nVariable& var) {this->push_back(var);}
    /// get a variable object by its handle
    nVariable* GetVariable(nVariable::Handle handle);
    /// get number of variables in context
	int GetNumVariables() const { return (int)this->size(); }
    /// get variable at index (this is NOT the variable handle!)
	nVariable& GetVariableAt(int index) { return this->at(index); };
};

//------------------------------------------------------------------------------
/**
*/
inline
nVariableContext::nVariableContext() 
{	
    // empty
}

//------------------------------------------------------------------------------
/**
	clear the variable context
*/
inline
void 
nVariableContext::Clear() 
{
	for (std::vector<nVariable>::iterator i = this->begin(); i != this->end(); i++)
		i->Clear();
	this->clear();
}



//------------------------------------------------------------------------------
/**
    @param  h       the variable handle
    @return         pointer to variable object, or 0 if variable doesn't exist
                    in this context
*/
inline
nVariable*
nVariableContext::GetVariable(nVariable::Handle h)
{
    int size = this->size();
    
    for (int i = 0; i < size; i++)
    {
        if (this->at(i).GetHandle() == h)
        {
            return &(this->at(i));
        }
    }
    // fallthrough: variable not found
    return 0;
}


//------------------------------------------------------------------------------
#endif

