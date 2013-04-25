#include "axle/geom/normal.h"
#include "axle/geom/vector3.h"

namespace ax {
Normal::Normal(const Vector3& v) : x(v.x), y(v.y), z(v.z) { }
} // ax
