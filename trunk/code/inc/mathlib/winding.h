#if !defined(WINDING_H_0F23211B_E8D4_4e99_9062_1E0628438684)
#define WINDING_H_0F23211B_E8D4_4e99_9062_1E0628438684

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector.h"
#include <vector>

class TexCoord2 {
public:
	TexCoord2()
		: u(0), v(0) {}
	TexCoord2(float u, float v)
		: u(u), v(v) {}

	float u, v;
};

class TexCoord3 {
public:
	TexCoord3()
		: u(0), v(0), w(0) {}
	TexCoord3(float u, float v, float w)
		: u(u), v(v), w(w) {}

	float u, v, w;
};

// TBD: Is int the best format for this?  I'd rather use normalized coordinates (i.e. float).
//      Otherwise how do we know how to represent the full scale in a device-independant manner?
class Color {
public:
	Color()
		: r(0), g(0), b(0), a(0) {} // TBD: a should get normalized 1.0
	Color(int r, int g, int b, int a)
		: r(r), g(g), b(b), a(a) {}
	Color(unsigned char rgba[4])
		: r(rgba[0]), g(rgba[1]), b(rgba[2]), a(rgba[3]) {}

	int r, g, b, a;
};

// TBD: Design a mechanism that uses a flexible vertex format.
// Motivation:  Should all windings waste storage space for the
//              maximal vertex format captured here?  coord + uv + rgb + norm
//
// Although game engines in the past have assumed a texture and a lightmap,
// this is no longer going to be acceptable for making good use of newer hardware.

class Vertex {
public:
	Vertex() {}
	Vertex(vector3 _position)
		: position(_position) {}
	Vertex(vector3 _position, TexCoord2 _texture)
		: position(_position), texture(_texture) {}
	Vertex(vector3 _position, TexCoord2 _texture, Color _color)
		: position(_position), texture(_texture), color(_color) {}
	Vertex(vector3 _position, TexCoord2 _texture, Color _color, vector3 _normal)
		: position(_position), texture(_texture), color(_color), normal(_normal) {}

	vector3   position;
	TexCoord2 texture;
	TexCoord2 lightmap;
	vector3   normal;
	Color     color;
};

typedef std::vector<Vertex> Winding;

inline float areaOf(const Winding& w)
{
	n_assert(w.size() > 2);

	float area(0);
	for (Winding::size_type i = 1; i < w.size() - 1; ++i) {
		vector3 a(w[i].position - w[0].position);
		vector3 b(w[i + 1].position - w[0].position);
		vector3 c(a * b);
		area += float(0.5) * c.len();
	}

	return area;
}

inline bool isDegenerate(const Winding& w)
{
	if (w.empty()) {
		return true;
	}

	int goodEdges(0);

	for (Winding::const_iterator i = w.begin(); i != w.end(); ++i) {
		Winding::const_iterator j = i;
		++j;
		if (j == w.end()) {
			j = w.begin();
		}

		if (vector3(j->position - i->position).len() > DIST_EPSILON) {
			if (++goodEdges == 3) {
				return false;
			}
		}
	}

	return true;
}

#endif // WINDING_H_0F23211B_E8D4_4e99_9062_1E0628438684
