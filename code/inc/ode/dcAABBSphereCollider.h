#ifndef __DCAABBSPHERETREECOLLIDER_INCLUDED__
#define __DCAABBSPHERETREECOLLIDER_INCLUDED__

class __declspec(dllexport) dcAABBSphereTreeCollider{
	Sphere sphere;

	Point* LeafVerts[3];
	udword LeafIndex;
public:
	/* In */
	const dcVector3*& Vertices;
	const int*& Indices;

	/* In/Out */
	dArray<const AABBNoLeafNode*>* TCData;

	static test;

	/* Out */
	dArray<int> Contacts;

	/* Constructor/destructor */
	dcAABBSphereTreeCollider(const dcVector3*& Vertices, const int*& Indices);
	~dcAABBSphereTreeCollider();

	/* Collide */
	void Collide(const AABBNoLeafTree* Tree, const Sphere& sphere, Matrix4x4 &ms, Matrix4x4 &mt);
	void GenerateTC(const AABBNoLeafTree* Tree, const Sphere& sphere);
private:
	
	AABBSphereCollider collider;
	
    static void opcodeTriCallback(udword triangleindex, VertexPointers& triangle, udword userdata)
	{
		dcAABBSphereTreeCollider* self = (dcAABBSphereTreeCollider*) userdata;
	    const int *indexPtr = &(self->Indices[3 * triangleindex]);

		triangle.Vertex[0] = (Point*)&(self->Vertices[indexPtr[0]]);
		triangle.Vertex[1] = (Point*)&(self->Vertices[indexPtr[1]]);
		triangle.Vertex[2] = (Point*)&(self->Vertices[indexPtr[2]]);
			
		printf("Index: %d", triangleindex);
		test = triangleindex;

	}

	// Opcode fills this in for us
	CollisionFaces outFaces;

	void _CollideTriBox();
	void _Collide(const AABBNoLeafNode* a);

	void _GenerateTC(const AABBNoLeafNode* a);

	/* General */
	bool BoxBoxOverlap(const Point& a, const Point& Pa);
	bool TriBoxOverlap();
	
	void InitQuery(const Sphere& sphere);
};
	
#endif // __DCAABBTREECOLLIDER_INCLUDED__
