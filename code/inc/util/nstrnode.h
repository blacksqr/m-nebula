#ifndef N_STRNODE_H
#define N_STRNODE_H
//------------------------------------------------------------------------------
/**
    A node in a nStrList.
    
    (C) 2002 RadonLabs GmbH
*/
#include <string.h>

#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#ifndef N_STL_H
#include "util/nstl.h"
#endif

//------------------------------------------------------------------------------
class nStrNode: public nNode 
{
public:
    /// default constructor
    nStrNode();
    /// constructor providing custom data pointer
    nStrNode(void* ptr);
    /// constructor providing node name
    nStrNode(const char* str);
    /// constructor providing custom data pointer and node name
    nStrNode(const char* str, void* ptr);
    /// set the name of this node
    void SetName(const char* str);
    /// get the name of this node
    const char* GetName() const;
    /// get next node in list
    nStrNode* GetSucc() const;
    /// get previous node in list
    nStrNode* GetPred() const;

private:
    stl_string name;
};

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode::nStrNode()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode::nStrNode(void* ptr) :
    nNode(ptr)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode::nStrNode(const char* str) :
    name(str)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode::nStrNode(const char* str, void* ptr) :
    nNode(ptr),
    name(str)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
nStrNode::SetName(const char* str)
{
    this->name = str;
}

//------------------------------------------------------------------------------
/**
*/
inline
const char*
nStrNode::GetName() const
{
    return this->name.empty() ? 0 : this->name.c_str();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrNode::GetSucc() const
{
    return (nStrNode *) nNode::GetSucc();
}

//------------------------------------------------------------------------------
/**
*/
inline
nStrNode*
nStrNode::GetPred() const
{
    return (nStrNode *) nNode::GetPred();
}

//------------------------------------------------------------------------------
#endif
