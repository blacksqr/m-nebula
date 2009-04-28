#ifndef N_ODE_COLLIDEOBJECT_H
#define N_ODE_COLLIDEOBJECT_H
//-------------------------------------------------------------------
//  CLASS
//  nCollideObject -- collission system object
//
//  OVERVIEW
//  nCollideObject is an actual collission system object which can
//  be positioned and oriented in space. It points to an
//  nCollideShape which describes the actual shape of the
//  object.
//
//  nCollideObjects are kept in sorted list (one for each dimension)
//  by the nCollideContext they belong to.
//-------------------------------------------------------------------
#ifndef N_COLLIDEOBJECT_H
#include "collide/ncollideobject.h"
#endif

#include "nemesis/node_physics.h"
#include "nemesis/node_collideshapes.h"
#include "kernel/ntimeserver.h"

#include "ode/contact.h"
//-------------------------------------------------------------------
class nOdeCollideContext;
class nCollideReport;
// enum for contact paramaters
// Thanks!
enum {
  dCParamMu,
  dCParamMu2,
  dCParamBounce,
  dCParamBounceVel,
  dCParamSoftErp,
  dCParamSoftCfm,
  dCParamMotion1,
  dCParamMotion2,
  dCParamSlip1,
  dCParamSlip2
};


class N_PUBLIC nOdeCollideObject : public nCollideObject {
protected:
    friend nOdeCollideContext;

	// From a geomID we can get a BodyID.  All objects in the collision space have
	// a GeomID, only non-static ones will have a BodyID.
	//
	// Shapes should hold BodyID's and static/nonstatic information -- but now they 
	// can no longer be shared as this will force all shapes to be tied to their objects... 
	//
	//
	
	// SO instead -- Objects hold the static/nonstatic information, shapes hold the
	// geometric, mass, inertia, and geocentric data information used to create the 
	// actual ODE object.  On SetShape, this will happen.

	bool isStatic;

	dGeomID	myOdeGeometryId;
	dSurfaceParameters mySurfaceParameters;
	
public:
    nOdeCollideObject();
    ~nOdeCollideObject();
	
	dGeomID getOdeGeometryID()
	{ return myOdeGeometryId; }

	inline void incrementCollision()
	{
		this->num_colls++;
	}

	// Set this object's contact (surface) parameters
	// See the ODE documentation for more information
	dReal GetContactParam( int param );
	void SetContactParam( int param, dReal value );
	dSurfaceParameters *GetSurfaceParameters(){ return &mySurfaceParameters; };

	// Sets the static nature of this object (static means no forces (gravity) will move this object
	// only positional transforms will move it.
	void setStatic( bool doStatic );

    void SetShape(nCollideShape *s);

    nCollideShape *GetShape(void) {
        return this->shape;
    };

	// Forces (Accumulated for each frame, emptied at the end of every frame.)
	void addForce( vector3 *force );
	vector3 getForce();

	// Forces (Accumulated for each frame, emptied at the end of every frame.)
	void addTorque( vector3 *torque );
	vector3 getTorque();

	void addRelForce(vector3 *force);
	void addRelTorque(vector3 *torque);
	void addForceAtPos(vector3 *force, vector3 *pos);
	void addForceAtRelPos(vector3 *force, vector3 *pos);
	void addRelForceAtPos(vector3 *force, vector3 *pos);
	void addRelForceAtRelPos(vector3 *force, vector3 *pos);

	vector3 getRelPointPos(vector3 *pos);
	vector3 getRelPointVel(vector3 *pos);

	void setFiniteRotationMode(int mode);
	void setFiniteRotationAxis(vector3 *axis);

	int getFiniteRotationMode();
	vector3 getFiniteRotationAxis();

	int getNumJoints();
	dJointID getJoint (int index);

	vector3 getLinearVelocity();

	vector3 getAngularVelocity();
	
	// Now will cue a repositioning of the object in ODE space
	virtual void Reset(matrix44& m);

    //---------------------------------------------------------------
    //  Transform()
    //  Transform the object to its new position/orientation, update
    //  the dimensional nodes and the bounding box.
    //
    //  26-Jun-00   floh    created
    //  19-Jul-00   floh    + bounding box now covers the range
    //                        of motion between the last position
    //                        and the current position
    //  14-Oct-00   floh    + added the max velocity position exception
    //                        (this fixes the position-set problem at
    //                        the case of an collission if an object
    //                        travels at near-infinity speed (it will
    //                        basically be reset to its previous pos).
    //---------------------------------------------------------------
	
	// Question -- why would we call this with ODE managing the motions?
	//
	// Answers -- To bypass ODE and reposition the object in space?
	//		      To synchronize Nemesis with ODE?
	//			  To inform Nemesis of the new object's position?
	//			  
	//
	// users of this class will now poll the nCollideObject each frame 
	// for the position using "GetTransform()" to sync with their associated 
	// 3D object.  They will also add forces, torques, and set positions 
	// using "Reset()" but otherwise I think this "transform" method is now bunk.
    void Transform(double t, matrix44& m);

	// Useful little function, perhaps move to mathlib.
	inline float DISTANCE(const vector3 a, const vector3 b)
	{ 
		return sqrt( (a.x-b.x)*(a.x-b.x) + 
					 (a.y-b.y)*(a.y-b.y) +
					 (a.z-b.z)*(a.z-b.z) ); 
	};
    const matrix44& GetTransform(void);

	// NOTE: Contact() is now bunk as it is handled by the dSpaceCollide()
	//		 within the CONTEXT this object resides.  There is a callback method
	//		 that gives us a hook for "Near Hits" which is now the
	//		 "nearCallback()" method.
	//
	// WRONG -- Contact is for object->object tests and needs to be allowed
	//			still.  Reimplement with ODE tests.
	bool Contact(nCollideObject *other,     // the other object
	             nCollType ct,
		         nCollideReport& cr);
    
	
	//---------------------------------------------------------------
    //  Collide()
    //  For each overlapping object in all 3 dimensions, 
    //  which doesn't fall into the ignore_types category,
    //  do a collission check, and if the check is positive,
    //  record collission by doing an AddCollission().
    //  26-Jun-00   floh    created
	//
	//
	//	Now bunk - handled by the context like it should have in the
	//			   first place!
    //---------------------------------------------------------------
    void Collide(void) {
		n_printf("nOdeCollideObject::Collide() -- Why!?");
    };
};
//-------------------------------------------------------------------
#endif
