#ifndef N_ODECOLLIDEREPORT_H
#define N_ODECOLLIDEREPORT_H
//------------------------------------------------------------------------------
/**
    @class nOdeCollideReportHandler 
    @ingroup NODEPhysicsContribModule
    @brief collect and manage collision pairs

    Manages a set of collision pairs. Makes sure that each collision
    between 2 shapes is only tested/reported once, to help 
    the nOdeCollideContext avoid redundant checks.
    
    (c) 2003  Vadim Macagon
    Based on Radon Labs nCollideReportHandler and friends.
    
    nOdeCollideReportHandler is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeCollideReport
    @ingroup NODEPhysicsContribModule
    @brief Describes a contact between 2 nOdeCollideShapes.
    
    A collide report holds one or more contacts between a pair of collide 
    shapes. The number of stored contacts between the two shapes can be
    obtained through nOdeCollideReport::GetNumContacts() and individual
    contacts can be retrieved via nOdeCollideContext::GetContact(),
    by supplying an index in the range [0, number of contacts).
    
    nOdeCollideReport is licensed under the terms of the Nebula License.
*/
/**
    @class nOdeCollideContact
    @ingroup NODEPhysicsContribModule
    @brief Stores a contact point, contact normal and depth of 
           interpenetration.
    
    nOdeCollideContact is licensed under the terms of the Nebula License.
*/

#ifndef N_KEYARRAY_H
#include "util/nkeyarray.h"
#endif

#ifndef N_VECTOR_H
#include "mathlib/vector.h"
#endif

//------------------------------------------------------------------------------
class nOdeCollideShape;
//class nOdeCollideReportHandler;

//------------------------------------------------------------------------------
class nOdeCollideContact
{
  public:
    /// the point of contact 
    vector3 contact;
    /// unit length collision normal vector points "in" to body 1
    vector3 normal;
    /// penetration depth
    float depth;
};

//------------------------------------------------------------------------------
class nOdeCollideReport
{
  public:
    friend class nOdeCollideContext;
    friend class nOdeCollideReportHandler;
  
    void Clear();
    int GetNumContacts();
    nOdeCollideContact* GetContact( int relindex );
        
    /// the first shape involved in the collision
    nOdeCollideShape *cs1; 
    /// the second shape involved in the collision
    nOdeCollideShape *cs2; 
    /// the timestamp at which the collision occured
    double tstamp;
    
  private:
    int GetAbsContactIndex( int relindex );
  
    /// the collide report handler this report belongs to
    nOdeCollideReportHandler* handler;
    /// index of first contact
    int firstContact;
    /// index of last contact
    int lastContact;
};

//------------------------------------------------------------------------------
class nOdeCollideReportHandler 
{
  public:
    enum { MAX_REPORTS_PER_OBJECT = 256, N_MAX_COLLIDE_CONTACTS = 1024 };
    
    nOdeCollideReportHandler();
    
    /// get a storage slot for a contact
    nOdeCollideContact* NewCollideContact( int& absIndex );
    /// get collide contact stored at 'absIndex'
    nOdeCollideContact* GetCollideContact( int absIndex );
    /// initialize nKeyArray for new collision frame
    void Clear(); // used to be BeginFrame()
    /// check if a collision has already been reported
    bool CollissionExists( int id1, int id2 );
    /// add a new collision 
    void AddCollission( nOdeCollideReport& cr, int id1, int id2 );
    /// get overall number of collision reports recorded
    int GetNumCollissions();
    int GetCollissions( nOdeCollideShape* cs, nOdeCollideReport**& cr_ptr );
    int GetAllCollissions( nOdeCollideReport**& cr_ptr );
    int GetClosestCollission( const vector3& v, nOdeCollideReport **& crPtr );

  private:
    bool GetMergedId( int id1, int id2, int& mrg );
    
    nKeyArray<nOdeCollideReport> coll_pairs;
    nOdeCollideReport* report_array[MAX_REPORTS_PER_OBJECT];
    int nextCollideContact;
    nOdeCollideContact collideContacts[N_MAX_COLLIDE_CONTACTS];
};

//------------------------------------------------------------------------------
inline
void nOdeCollideReport::Clear()
{
  this->cs1 = NULL;
  this->cs2 = NULL;
  this->tstamp = 0.0;
  this->firstContact = -1;
  this->lastContact = -1;
}

//------------------------------------------------------------------------------
inline
int nOdeCollideReport::GetNumContacts()
{
  return (this->lastContact - this->firstContact) + 1;
}

//------------------------------------------------------------------------------
inline
nOdeCollideContact* nOdeCollideReport::GetContact( int relindex )
{
  n_assert( this->handler );
  return this->handler->GetCollideContact( this->GetAbsContactIndex( relindex ) );
}

//------------------------------------------------------------------------------
inline
int nOdeCollideReport::GetAbsContactIndex( int relindex )
{
  int res = this->firstContact + relindex;
  n_assert( res <= this->lastContact );
  return res;
}

//------------------------------------------------------------------------------
inline
nOdeCollideReportHandler::nOdeCollideReportHandler() 
  : coll_pairs(128,128)
{        
}

//------------------------------------------------------------------------------
/** 
  Merge the 2 object id's into 1 32 bit id, order them, so that any 
  combination of 2 id's results in the same merged id. Return true if
  a swap happened (because other attributes may have to be swapped as well).
*/
inline
bool nOdeCollideReportHandler::GetMergedId( int id1, int id2, int& mrg ) 
{
  if ( id1 > id2 ) 
  {
    mrg = ((id2 & 0xffff)<<16) | (id1 & 0xffff);
    return true;
  } 
  else 
  {
    mrg = ((id1 & 0xffff)<<16) | (id2 & 0xffff);
    return false;
  }
};

//------------------------------------------------------------------------------
inline
nOdeCollideContact* nOdeCollideReportHandler::NewCollideContact( int& absIndex )
{
  if ( this->nextCollideContact < this->N_MAX_COLLIDE_CONTACTS )
  {
    absIndex = this->nextCollideContact++;
    return &(this->collideContacts[absIndex]);
  }
  else
  {
    n_error( "nCollideReportHandler::NewCollideContact(): pool exhausted" );
    return NULL;
  }
}

//------------------------------------------------------------------------------
inline
nOdeCollideContact* nOdeCollideReportHandler::GetCollideContact( int absIndex )
{
  n_assert( absIndex < this->N_MAX_COLLIDE_CONTACTS );
  return &(this->collideContacts[absIndex]);
}

//------------------------------------------------------------------------------
inline
void nOdeCollideReportHandler::Clear()
{
  this->coll_pairs.Clear();
  memset( this->report_array, 0, sizeof(this->report_array) );
  // reset to start of collide contacts pool
  this->nextCollideContact = 0;
}

//------------------------------------------------------------------------------
inline
bool nOdeCollideReportHandler::CollissionExists( int id1, int id2 ) 
{
  // generate the merged 32 bit id, and query key array
  // for the collision
  int key;
  this->GetMergedId( id1, id2, key );
  nOdeCollideReport* cr;
  if ( this->coll_pairs.FindPtr( key, cr ) ) 
    return true;
  else 
    return false;
}

//------------------------------------------------------------------------------
inline
void nOdeCollideReportHandler::AddCollission( nOdeCollideReport& cr, 
                                              int id1, int id2 ) 
{
  // generate the merged 32 bit id and add collision report
  int key;
  this->GetMergedId( id1, id2, key );
  this->coll_pairs.Add( key, cr );
}

//------------------------------------------------------------------------------
inline
int nOdeCollideReportHandler::GetNumCollissions() 
{
  return this->coll_pairs.Size();
}

//--------------------------------------------------------------------------
/**
  @brief Report collisions for a specific shape. 
  @return Number of collision reports for this shape and a pointer to an 
          array of collision report pointers into the nKeyArray.
*/
inline
int nOdeCollideReportHandler::GetCollissions( nOdeCollideShape* cs, 
                                              nOdeCollideReport**& cr_ptr ) 
{
  // fill report array with all collisions which this
  // shape is involved in.
  n_assert( cs );
  int num_reports = 0;
  int i;
  int num = this->coll_pairs.Size();
        
  if ( num > MAX_REPORTS_PER_OBJECT )
    num = MAX_REPORTS_PER_OBJECT;
  
  for ( i = 0; i < num; i++ ) 
  {
    nOdeCollideReport* cr = &(this->coll_pairs.GetElementAt( i ));
    if ( (cr->cs1 == cs) || (cr->cs2 == cs) ) 
    {
      cr->handler = this;
      this->report_array[num_reports++] = cr;
    }
  }

  cr_ptr = this->report_array;
  return num_reports;
}

//--------------------------------------------------------------------------
/**
  @brief  Get all recorded collision reports.
*/
inline
int nOdeCollideReportHandler::GetAllCollissions( nOdeCollideReport**& cr_ptr ) 
{
  int num = this->coll_pairs.Size();
  int i;

  if ( num > MAX_REPORTS_PER_OBJECT )
    num = MAX_REPORTS_PER_OBJECT;
  
  for ( i = 0; i < num; i++ ) 
  {
    nOdeCollideReport* cr = &(this->coll_pairs.GetElementAt( i ));
    cr->handler = this;
    this->report_array[i] = cr;
  }
 
  cr_ptr = this->report_array;
  return num;
}

//--------------------------------------------------------------------------
/**
  @brief Get the collision closest to given point.
  @param  v       [in] origin coordinate
  @param  crPtr   [out] pointer to collide report pointer array
  @return         Number of entries in collide report pointer array (0 or 1)
*/
inline
int nOdeCollideReportHandler::GetClosestCollission( const vector3& v, 
                                                    nOdeCollideReport **& crPtr )
{
  int num = this->coll_pairs.Size();
  if ( 0 == num )
  {
    crPtr = 0;
    return 0;
  }

  vector3 distVec;
  nOdeCollideReport* minPtr = &(this->coll_pairs.GetElementAt( 0 ));
  bool initDist = true;
  float minDist;
  nOdeCollideContact* cc;
      
  // find the closest contact in the first report
  int i;
  for ( i = minPtr->firstContact; i <= minPtr->lastContact; i++ )
  {
    cc = this->GetCollideContact( i );
    if ( initDist )
    {
      minDist = vector3(cc->contact - v).len();
      continue;
    }
    distVec = cc->contact - v;
    float dist = distVec.len();
    if ( dist < minDist )
      minDist = dist;
  }
      
  // now find the closest contact overall
  for ( i = 1; i < num; i++ )
  {
    nOdeCollideReport* curPtr = &(this->coll_pairs.GetElementAt( i ));
        
    for ( int j = curPtr->firstContact; j <= curPtr->lastContact; j++ )
    {
      cc = this->GetCollideContact( j );
      distVec = cc->contact - v;
      float dist = distVec.len();
      if ( dist < minDist )
      {
        minDist = dist;
        minPtr = curPtr;
      }
    }
  }
  minPtr->handler = this;
  this->report_array[0] = minPtr;
  crPtr = this->report_array;
  return 1;
}

//-------------------------------------------------------------------
#endif
