#include "axle/cg/material.h"
#include <string.h>

namespace ax {
const char *Material::s_unnamed = "<Unnamed Material>";
const MaterialPtr Material::kNullMatl = 
    MaterialPtr(new Material("<Null Material>"));
} // ax
