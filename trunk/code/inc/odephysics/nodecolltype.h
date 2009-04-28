#ifndef N_ODECOLLTYPE_H
#define N_ODECOLLTYPE_H
//------------------------------------------------------------------------------
/**
    @ingroup NODEPhysicsContribModule
    Define the collision types for collision queries.

    (c) 2003  Vadim Macagon
*/

enum nOdeCollType {
    ODE_COLLTYPE_IGNORE     = 0,    ///< no collisions
    ODE_COLLTYPE_QUICK      = 1,    ///< quick sphere-2-sphere collision
    ODE_COLLTYPE_CONTACT    = 2,    ///< first contact only
    ODE_COLLTYPE_EXACT      = 3,    ///< all contacts
};

/**
    @ingroup NODEPhysicsContribModule
    special case values for the CollClass check
*/
enum
{
    ODE_COLLCLASS_ALWAYS_IGNORE  = -1,
    ODE_COLLCLASS_ALWAYS_QUICK   = -2,
    ODE_COLLCLASS_ALWAYS_CONTACT = -3,
    ODE_COLLCLASS_ALWAYS_EXACT   = -4,
};

//------------------------------------------------------------------------------
#endif
