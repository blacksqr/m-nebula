#ifndef N_AABBOX_H
#define N_AABBOX_H

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef N_vector_H
#include "mathlib/vector.h"
#endif

#ifndef N_MATRIX_H
#include "mathlib/matrix.h"
#endif

#ifndef N_LINE_H
#include "mathlib/line.h"
#endif

// TBD: Merge AxisAlignedBox and bbox3?

#define vector_MIN FLT_MIN
#define vector_MAX FLT_MAX
#define vector_EPSILON FLT_EPSILON

#define	ANGLE_EPSILON    float(       0.0000005  ) // are angles equal?
#define	DIST_EPSILON     float(       0.0005     ) // are distances equal?
#define EQUAL_EPSILON    float(       0.00005    ) // are points equal?
#define ON_EPSILON       float(       0.005      ) // is point on plane?
#define MAX_WORLD_vector float( 10000.0          ) // unlikely large vector point

struct AxisAlignedBox {

	AxisAlignedBox() :
		boxMin(vector_MAX, vector_MAX, vector_MAX),
		boxMax(-vector_MAX, -vector_MAX, -vector_MAX)
		{
		}

	AxisAlignedBox(const int bbox[6]) {
		boxMin.x = (float)bbox[0];
		boxMin.y = (float)bbox[1];
		boxMin.z = (float)bbox[2];
		boxMax.x = (float)bbox[3];
		boxMax.y = (float)bbox[4];
		boxMax.z = (float)bbox[5];
	}

	AxisAlignedBox(const float bbox[6]) {
		boxMin.x = bbox[0];
		boxMin.y = bbox[1];
		boxMin.z = bbox[2];
		boxMax.x = bbox[3];
		boxMax.y = bbox[4];
		boxMax.z = bbox[5];
	}

	vector3 boxMin;
	vector3 boxMax;

	float operator [] (int i) const {
		switch (i)
		{
			case 0:
				return boxMin.x;
			case 1:
				return boxMin.y;
			case 2:
				return boxMin.z;
			case 3:
				return boxMax.x;
			case 4:
				return boxMax.y;
			case 5:
				return boxMax.z;
			default:
				return 0;
		}
	}

	void set(const vector3& point) {
		boxMin = point;
		boxMax = point;
	}

	void clear() {
		boxMin.x = boxMin.y = boxMin.z = vector_MAX;
		boxMax.x = boxMax.y = boxMax.z = -vector_MAX;
	}

	void extendToFit(const AxisAlignedBox& box) {
		if (boxMin.x > box.boxMax.x) boxMin.x = box.boxMin.x; 
		if (boxMin.y > box.boxMin.y) boxMin.y = box.boxMin.y;
		if (boxMin.z > box.boxMin.z) boxMin.z = box.boxMin.z;

		if (boxMax.x < box.boxMax.x) boxMax.x = box.boxMax.x; 
		if (boxMax.y < box.boxMax.y) boxMax.y = box.boxMax.y;
		if (boxMax.z < box.boxMax.z) boxMax.z = box.boxMax.z;
	}

	void extendToFit(const vector3& point) {
		if (boxMin.x > point.x) boxMin.x = point.x; 
		if (boxMin.y > point.y) boxMin.y = point.y;
		if (boxMin.z > point.z) boxMin.z = point.z;

		if (boxMax.x < point.x) boxMax.x = point.x; 
		if (boxMax.y < point.y) boxMax.y = point.y;
		if (boxMax.z < point.z) boxMax.z = point.z;
	}

	void inflate(const float& distance) {
		boxMin.x -= distance;
		boxMin.y -= distance;
		boxMin.z -= distance;
		boxMax.x += distance;
		boxMax.y += distance;
		boxMax.z += distance;
	}

	bool isWellFormed() const {
		return
			boxMin.x != vector_MAX &&
			boxMin.y != vector_MAX &&
			boxMin.z != vector_MAX &&
			boxMax.x != -vector_MAX &&
			boxMax.y != -vector_MAX &&
			boxMax.z != -vector_MAX &&
			boxMax.x >= boxMin.x &&
			boxMax.y >= boxMin.y &&
			boxMax.z >= boxMin.z
			;
	}

	bool contains(const vector3 point) const {
		return
			point.x >= boxMin.x &&
			point.y >= boxMin.y &&
			point.z >= boxMin.z &&

			point.x <= boxMax.x &&
			point.y <= boxMax.y &&
			point.z <= boxMax.z;
	}
};

//-------------------------------------------------------------------
#endif