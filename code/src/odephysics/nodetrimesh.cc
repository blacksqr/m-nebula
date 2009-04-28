#define N_IMPLEMENTS nOdeTriMesh
//--------------------------------------------------------------------
//  nodetrimesh.cc
//
//  (c) 2003  Vadim Macagon
//
//  nOdeTriMesh is licensed under the terms of the Nebula License.
//--------------------------------------------------------------------
#include "kernel/nfileserver2.h"
#include "odephysics/nodetrimesh.h"
#include "mathlib/vector.h"
#include "mathlib/triangle.h"
#include "mathlib/line.h"
#include "gfx/ngfxserver.h"
#include "odephysics/nodeserver.h"
#include "odephysics/nodetrimeshshape.h"

//------------------------------------------------------------------------------
/**
*/
nOdeTriMesh::nOdeTriMesh( const char* id ) 
  : nHashNode(id), refCount(0),
    numVertices(0), numFaces(0),
    vertexData(0), faceData(0),
    isLoaded(false), meshDataId(0)
{
  this->odeServer = (nOdeServer*)nOdeServer::kernelServer->
                    Lookup( "/sys/servers/ode" );
  n_assert( this->odeServer );
}

//------------------------------------------------------------------------------
/**
*/
nOdeTriMesh::~nOdeTriMesh()
{
  if ( this->meshDataId )
  {
    dGeomTriMeshDataDestroy( this->meshDataId );
  }
  if ( this->vertexData )
  {
    n_free( this->vertexData );
    this->vertexData = 0;
  }
  if ( this->faceData )
  {
    n_free( this->faceData );
    this->faceData = 0;
  }
  n_assert( 0 == this->refCount );
}

//------------------------------------------------------------------------------
/**
*/
int nOdeTriMesh::AddRef()
{
  return ++refCount;
}

//------------------------------------------------------------------------------
/**
*/
int nOdeTriMesh::RemRef()
{
  n_assert( refCount > 0 );
  return --refCount;
}

//------------------------------------------------------------------------------
/**
*/
int nOdeTriMesh::GetRef()
{
  return refCount;
}

//------------------------------------------------------------------------------
/**
*/
bool nOdeTriMesh::IsLoaded()
{
  return this->isLoaded;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMesh::Begin( int numVerts, int numTris )
{
  n_assert( !this->isLoaded );
  n_assert( !this->vertexData );
  n_assert( !this->faceData );
    
  this->numVertices = numVerts;
  this->numFaces    = numTris;
  this->vertexData  = (float*) n_malloc(numVerts * 3 * sizeof(float));
  this->faceData    = (int*)   n_malloc(numTris  * 3 * sizeof(int));
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMesh::SetVertex( int index, vector3& v )
{
  n_assert( this->vertexData );
  n_assert( (index >= 0) && (index < this->numVertices) );

  float* ptr = this->vertexData + 3 * index;
  ptr[0] = v.x;
  ptr[1] = v.y;
  ptr[2] = v.z;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMesh::SetTriangle( int index, int p0Index, int p1Index, int p2Index )
{
  n_assert( this->faceData );
  n_assert( (index >= 0) && (index < this->numFaces) );
  n_assert( (p0Index >= 0) && (p0Index < this->numVertices) );
  n_assert( (p1Index >= 0) && (p1Index < this->numVertices) );
  n_assert( (p2Index >= 0) && (p2Index < this->numVertices) );

  int* ptr = this->faceData + 3 * index;
  ptr[0] = p0Index;
  ptr[1] = p1Index;
  ptr[2] = p2Index;
}

//------------------------------------------------------------------------------
/**
*/
void nOdeTriMesh::End()
{
  n_assert( (this->numVertices > 0) && (this->numFaces > 0) );
  n_assert( this->faceData && this->vertexData );
  n_assert( !this->meshDataId );

  this->meshDataId = dGeomTriMeshDataCreate();
  dGeomTriMeshDataBuildSingle( this->meshDataId, 
                               this->vertexData, sizeof(float) * 3, 
                               this->numVertices, this->faceData, 
                               this->numFaces * 3, sizeof(int) * 3 );


  /*
  OPCODECREATE opcc;
  opcc.NbTris    = this->numFaces;
  opcc.NbVerts   = this->numVertices;
  opcc.Tris      = (const unsigned int*) this->faceData;
  opcc.Verts     = (const Point*) this->vertexData;
  opcc.Rules     = SPLIT_COMPLETE | SPLIT_SPLATTERPOINTS;   // ???
  opcc.NoLeaf    = false; // true;
  opcc.Quantized = false; // true;
  this->opcModel.Build( opcc );
  */
  
  n_assert( !this->isLoaded );
  this->isLoaded = true;
}

//------------------------------------------------------------------------------
/**
    OPCODE uses a callback function to actually get triangle data for the
    collision test.
*/
/*
void nOdeTriMesh::collCallback( udword triangleIndex, VertexPointers& triangle, 
                                udword userData )
{
  nOdeTriMesh* self = (nOdeTriMesh*)userData;
  int* indexPtr = &(self->faceData[3 * triangleIndex]);
  triangle.Vertex[0] = (Point*)&(self->vertexData[3 * indexPtr[0]]);
  triangle.Vertex[1] = (Point*)&(self->vertexData[3 * indexPtr[1]]);
  triangle.Vertex[2] = (Point*)&(self->vertexData[3 * indexPtr[2]]);
}
*/

//------------------------------------------------------------------------------
/**
  @brief Obtain the AABB for the mesh.
*/
/*
void nOdeTriMesh::ComputeAABB( const dMatrix3& R, const dVector3& P, dReal aabb[6] )
{
  const AABBCollisionTree* tree = (const AABBCollisionTree*)this->opcModel.GetTree();
  const AABBCollisionNode* node = tree->GetNodes();

  dReal xrange = dFabs( R[0] * node->mAABB.mExtents.x ) 
               + dFabs( R[1] * node->mAABB.mExtents.y ) 
               + dFabs( R[2] * node->mAABB.mExtents.z );
  dReal yrange = dFabs( R[4] * node->mAABB.mExtents.x ) 
               + dFabs( R[5] * node->mAABB.mExtents.y ) 
               + dFabs( R[6] * node->mAABB.mExtents.z );
  dReal zrange = dFabs( R[8] * node->mAABB.mExtents.x ) 
               + dFabs( R[9] * node->mAABB.mExtents.y ) 
               + dFabs( R[10] * node->mAABB.mExtents.z );
                              
  aabb[0] = P[0] - xrange;
  aabb[1] = P[0] + xrange;
  aabb[2] = P[1] - yrange;
  aabb[3] = P[1] + yrange;
  aabb[4] = P[2] - zrange;
  aabb[5] = P[2] + zrange;
}
*/

//------------------------------------------------------------------------------
/**
*/
/*
void nOdeTriMesh::GetAABB( dGeomID g, dReal aabb[6] )
{
  const dVector3& P = *(const dVector3*)dGeomGetPosition( g );
  const dMatrix3& R = *(const dMatrix3*)dGeomGetRotation( g );
  nOdeTriMeshShape* shape = (nOdeTriMeshShape*)dGeomGetData( g );
  shape->GetTriMesh()->ComputeAABB( R, P, aabb );
}
*/

//------------------------------------------------------------------------------
/**
*/
/*
int nOdeTriMesh::TestAABB( dGeomID g1, dGeomID g2, dReal aabb2[6] )
{
  // not used or necessary atm.
  
  return 1;
  //return ((nOdeCollideObject*)dGeomGetData( g1 ))->GetShape()->TestAABB( aabb2 );
}
*/

//------------------------------------------------------------------------------
/**
*/
bool nOdeTriMesh::Load( nFileServer2* fs, const char* fname )
{
  if ( strstr( fname, ".n3d" ) )
  {
    return this->LoadN3D( fs, fname );
  }
  else if ( strstr( fname, ".nvx" ) )
  {
    return this->LoadNVX( fs, fname );
  }
  else
  {
    n_printf( "nOdeTriMesh: Unknown file format '%s'\n", fname );
    return false;
  }
}

//------------------------------------------------------------------------------
/**
    Render a AABBCollisionNode and recurse.
*/
/*
void nOdeTriMesh::VisualizeAABBCollisionNode( nGfxServer* gs, 
                                              const AABBCollisionNode* node )
{
  n_assert(gs && node);

  vector3 center( node->mAABB.mCenter.x, node->mAABB.mCenter.y, 
                  node->mAABB.mCenter.z );
  vector3 extent( node->mAABB.mExtents.x, node->mAABB.mExtents.y, 
                  node->mAABB.mExtents.z );

  vector3 v00(center.x - extent.x, center.y - extent.y, center.z - extent.z);
  vector3 v01(center.x - extent.x, center.y - extent.y, center.z + extent.z);
  vector3 v02(center.x + extent.x, center.y - extent.y, center.z + extent.z);
  vector3 v03(center.x + extent.x, center.y - extent.y, center.z - extent.z);
  vector3 v10(center.x - extent.x, center.y + extent.y, center.z - extent.z);
  vector3 v11(center.x - extent.x, center.y + extent.y, center.z + extent.z);
  vector3 v12(center.x + extent.x, center.y + extent.y, center.z + extent.z);
  vector3 v13(center.x + extent.x, center.y + extent.y, center.z - extent.z);

  // render ground rect
  gs->Coord(v00.x, v00.y, v00.z); gs->Coord(v01.x, v01.y, v01.z);
  gs->Coord(v01.x, v01.y, v01.z); gs->Coord(v02.x, v02.y, v02.z);
  gs->Coord(v02.x, v02.y, v02.z); gs->Coord(v03.x, v03.y, v03.z);
  gs->Coord(v03.x, v03.y, v03.z); gs->Coord(v00.x, v00.y, v00.z);

  // render top rect
  gs->Coord(v10.x, v10.y, v10.z); gs->Coord(v11.x, v11.y, v11.z);
  gs->Coord(v11.x, v11.y, v11.z); gs->Coord(v12.x, v12.y, v12.z);
  gs->Coord(v12.x, v12.y, v12.z); gs->Coord(v13.x, v13.y, v13.z);
  gs->Coord(v13.x, v13.y, v13.z); gs->Coord(v10.x, v10.y, v10.z);

  // render vertical lines
  gs->Coord(v00.x, v00.y, v00.z); gs->Coord(v10.x, v10.y, v10.z);
  gs->Coord(v01.x, v01.y, v01.z); gs->Coord(v11.x, v11.y, v11.z);
  gs->Coord(v02.x, v02.y, v02.z); gs->Coord(v12.x, v12.y, v12.z);
  gs->Coord(v03.x, v03.y, v03.z); gs->Coord(v13.x, v13.y, v13.z);

  if (!node->IsLeaf())
  {
    const AABBCollisionNode* neg = node->GetNeg();
    const AABBCollisionNode* pos = node->GetPos();
    this->VisualizeAABBCollisionNode( gs, neg );
    this->VisualizeAABBCollisionNode( gs, pos );
  }
}
*/
//------------------------------------------------------------------------------
/**
    Renders the collide model's triangle soup through the provided 
    gfx server.
*/
void nOdeTriMesh::Visualize( nGfxServer* gs )
{
  n_assert( gs );
  n_assert( this->vertexData && this->faceData );

  // render the triangle soup
  gs->Begin( N_PTYPE_LINE_LIST );
  int i;
  for ( i = 0; i < this->numFaces; i++ )
  {
    int* indexPtr = this->faceData + 3 * i;
    float* v0 = this->vertexData + 3 * indexPtr[0];
    float* v1 = this->vertexData + 3 * indexPtr[1];
    float* v2 = this->vertexData + 3 * indexPtr[2];
        
    gs->Coord(v0[0], v0[1], v0[2]); gs->Coord(v1[0], v1[1], v1[2]);
    gs->Coord(v1[0], v1[1], v1[2]); gs->Coord(v2[0], v2[1], v2[2]);
    gs->Coord(v2[0], v2[1], v2[2]); gs->Coord(v0[0], v0[1], v0[2]);
  }
  gs->End();
  
  /*
  // render the AABB tree
  gs->Rgba(0.5f, 0.0f, 0.0f, 1.0f);
  gs->Begin(N_PTYPE_LINE_LIST);
  const AABBCollisionTree* tree = (const AABBCollisionTree*) this->opcModel.GetTree();
  this->VisualizeAABBCollisionNode(gs, tree->GetNodes());
  gs->End();
  */
}

//------------------------------------------------------------------------------
/**
    Load a geometry file in n3d format as collide shape.
*/
bool nOdeTriMesh::LoadN3D( nFileServer2* fileServer, const char* filename )
{
  n_assert( fileServer );
  n_assert( filename );

  // load wavefront file
  int numVertices;
  int numFaces;
  char line[1024];

  nFile* file = fileServer->NewFileObject();
  n_assert( file );
  if ( !file->Open( filename, "r" ) )
  {
    n_printf( "nOdeTriMesh::Load(): Could not open file '%s'!\n", filename );
    return false;
  }

  // Pass1, count vertices and faces
  numVertices = 0;
  numFaces    = 0;
  while ( file->GetS( line, sizeof(line) ) ) 
  {
    char *kw = strtok( line, " \t\n" );
    if ( kw ) 
    {
      if ( strcmp( kw, "v" ) == 0 )      numVertices++;
      else if ( strcmp( kw, "f" ) == 0 ) numFaces++;
    }
  }

  // any errors?
  if ( 0 == numVertices ) 
  {
    n_printf( "nOdeTriMesh::Load(): file '%s' has no vertices!\n", filename );
    file->Close();
    delete file;
    return false;
  }
  if ( 0 == numFaces ) 
  {
    n_printf( "nOdeTriMesh::Load(): file '%s' has no triangles!\n", filename );
    file->Close();
    delete file;
    return false;
  }

  // read vertices, add triangles...
  this->Begin( numVertices, numFaces );
  file->Seek( 0, nFile::START );
  int actVertex = 0;
  int actFace   = 0;
  vector3 vec3;
  while ( file->GetS( line, sizeof(line) ) ) 
  {
    char *kw = strtok( line, " \t\n" );
    if ( kw ) 
    {
      if ( strcmp( kw, "v" ) == 0 ) 
      {
        // a vertex definition
        char *xs = strtok( NULL, " \t\n" );
        char *ys = strtok( NULL, " \t\n" );
        char *zs = strtok( NULL, " \t\n" );
        n_assert( xs && ys && zs );
        float x = (float)atof( xs );
        float y = (float)atof( ys );
        float z = (float)atof( zs );
        vec3.set( x, y, z );
        this->SetVertex( actVertex++, vec3 );
      } 
      else if ( strcmp( kw, "f" ) == 0 ) 
      {
        // a face definition only read first index in 
        // corner definition, read only 3 corners, ignore all others
        ulong i;
        int indices[3];
        for ( i = 0; i < 3; i++ ) 
        {
          char *tmp = strtok( NULL, " \t\n" );
          if ( tmp ) 
          {
            char *slash;
            if ( (slash = strchr( tmp, '/' )) ) 
              *slash = 0;
            int curIndex = atoi( tmp ) - 1;
            n_assert( curIndex < numVertices );
            indices[i] = curIndex;
          }
        }
        // add the triangle to the geometry
        this->SetTriangle( actFace++, indices[0], indices[1], indices[2] );
      }
    }
  }

  // finish adding geometry
  this->End();
  file->Close();
  delete file;
  return true;
}

//------------------------------------------------------------------------------
/**
    Reads a 32 bit int from file. FIXME: Endian correction!
*/
int nOdeTriMesh::readInt( nFile* file )
{
  n_assert( file );
  int var;
  file->Read( &var, sizeof(var) );
  return var;
}

//------------------------------------------------------------------------------
/**
*/
bool nOdeTriMesh::LoadNVX( nFileServer2* fileServer, const char* filename )
{
  n_assert( fileServer );
  n_assert( filename );

  // open file
  nFile* file = fileServer->NewFileObject();
  n_assert( file );
  if ( !file->Open( filename, "rb" ) )
  {
    n_printf( "nOdeTriMesh::LoadNVX(): could not load file '%s'\n", filename );
    return false;
  }
    
  // read header
  int magicNumber = this->readInt(file);
  if ( magicNumber != 'NVX1' )
  {
    n_printf( "nOdeTriMesh::LoadNVX(): '%s' is not a NVX1 file!\n", filename );
    file->Close();
    delete file;
    return false;
  }
  int numVertices        = this->readInt( file );
  int numIndices         = this->readInt( file );
  int numWingedEdges     = this->readInt( file );
  nVertexType vertexType = (nVertexType)this->readInt( file );
  int dataStart          = this->readInt( file );
  int dataSize           = this->readInt( file );

  // compute vertex stride
  n_assert( vertexType & N_VT_COORD );
  int vertexStride = 3;
  if ( vertexType & N_VT_NORM )
  {
    vertexStride += 3;
  }
  if ( vertexType & N_VT_RGBA )
  {
    vertexStride += 1;
  }
  if ( vertexType & N_VT_UV0 )
  {
    vertexStride += 2;
  }
  if ( vertexType & N_VT_UV1 )
  {
    vertexStride += 2;
  }
  if ( vertexType & N_VT_UV2 )
  {
    vertexStride += 2;
  }
  if ( vertexType & N_VT_UV3 )
  {
    vertexStride += 2;
  }
  if ( vertexType & N_VT_JW )
  {
    vertexStride += 6;
  }

  // read the complete data block into a memory buffer and close file
  char* buffer = (char*)n_malloc( dataSize );
  file->Seek( dataStart, nFile::START );
  int num = file->Read( buffer, dataSize );
  file->Close();
  delete file;
  file = 0;
  n_assert( num == dataSize );

  // get pointer to beginning of vertices and to beginning of indices
  float* vertexData = (float*)buffer;
  int wingedEdgeStride  = 2;
  ushort* indexData = (ushort*)(vertexData + (numVertices * vertexStride) 
                      + (numWingedEdges * wingedEdgeStride));

  // iterate over triangle list and add triangles to collide shape
  int numTriangles = numIndices / 3;
  this->Begin( numVertices, numTriangles );

  // add vertices
  vector3 vec3;
  int i;
  for ( i = 0; i < numVertices; i++ )
  {
    float* vPtr = vertexData + (i * vertexStride);
    vec3.set( vPtr[0], vPtr[1], vPtr[2] );
    this->SetVertex( i, vec3 );
  }

  // add triangles
  for ( i = 0; i < numTriangles; i++ )
  {
    int i0 = *indexData++;
    int i1 = *indexData++;
    int i2 = *indexData++;
    this->SetTriangle( i, i0, i1, i2 );
  }

  this->End();
  n_free( buffer );
  return true;
}

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
