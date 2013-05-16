#include "../geom/vector3.h"

#include "../geom/point.h"
#include "../geom/normal.h"

namespace ax {
  Vector3::Vector3(const Normal& n) : x(n.x), y(n.y), z(n.z) { }
  Vector3::Vector3(const Point& p) : x(p.x), y(p.y), z(p.z) { }
	const Vector3 Vector3::kUnitZ( 0.f, 0.f, 1.f );
	const Vector3 Vector3::kUnitY( 0.f, 1.f, 0.f );
	const Vector3 Vector3::kUnitX( 1.f, 0.f, 0.f );
	const Vector3 Vector3::kZero( 0.f, 0.f, 0.f );	
} // ax
