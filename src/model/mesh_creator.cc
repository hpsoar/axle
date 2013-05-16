#include "../model/triangle_mesh.h"
#include "../core/math.h"

namespace ax {
/*        (x2, y2)
 *  |
 *  |
 *  |
 *  *-------
 *(x1, y1) 
 */
void FillFace(int z_axis, float x1, float y1, float x2, float y2, float z, 
              Point *vertices) {
  int x_axis = (z_axis + 1) % 3;
  int y_axis = (z_axis + 2) % 3;
  vertices[0][x_axis] = x1;
  vertices[0][y_axis] = y1;
  vertices[0][z_axis] = z;
  
  vertices[1][x_axis] = x2;
  vertices[1][y_axis] = y1;
  vertices[1][z_axis] = z;
  
  vertices[2][x_axis] = x2;
  vertices[2][y_axis] = y2;
  vertices[2][z_axis] = z;

  vertices[3][x_axis] = x1;
  vertices[3][y_axis] = y2;
  vertices[3][z_axis] = z;
}

TriMeshPtr CreateBoxMesh(const Point &pos1, const Point &pos2) {
  uint32 fn = 12, vn = 24;
  Point *points = new Point[vn];
  uint32 *indices = new uint32[fn * 3];
  
  Point *vptr = points;
  ax::FillFace(2, pos1.x, pos1.y, pos2.x, pos2.y, pos1.z, vptr); // cw
  std::swap(vptr[1], vptr[3]);                                   //ccw
  vptr += 4;
  ax::FillFace(2, pos1.x, pos1.y, pos2.x, pos2.y, pos2.z, vptr); // ccw
  vptr += 4;
  ax::FillFace(1, pos1.z, pos1.x, pos2.z, pos2.x, pos1.y, vptr); // cw
  std::swap(vptr[1], vptr[3]);                                   //ccw
  vptr += 4;  
  ax::FillFace(1, pos1.z, pos1.x, pos2.z, pos2.x, pos2.y, vptr); // ccw
  vptr += 4;                                                     
  ax::FillFace(0, pos1.y, pos1.z, pos2.y, pos2.z, pos1.x, vptr); // cw
  std::swap(vptr[1], vptr[3]);                                   //ccw
  vptr += 4;
  ax::FillFace(0, pos1.y, pos1.z, pos2.y, pos2.z, pos2.x, vptr); // ccw   
  
  uint32 *iptr = indices;
  int ioffset = 0;
  for (int i = 0, ioffset = 0; i < 6; ++i, ioffset += 4, iptr += 6) {
    iptr[0] = ioffset; iptr[1] = ioffset + 1; iptr[2] = ioffset + 2;
    iptr[3] = ioffset; iptr[4] = ioffset + 2; iptr[5] = ioffset + 3;
  }

  float *tcoords = new float[vn * 2];
  float *tptr = tcoords;
  for (int i = 0; i < 6; ++i) {
    tptr[0] = 0.f; tptr[1] = 0.f;
    tptr[2] = 1.f; tptr[3] = 0.f;
    tptr[4] = 1.f; tptr[5] = 1.f;
    tptr[6] = 0.f; tptr[7] = 1.f;
    tptr += 8;
  }
  
  Normal *normals = new Normal[vn];
  FinalizeModel(points, vn, indices, &fn, normals);
  return TriangleMesh::Create(points, normals, tcoords, indices, vn, fn);
}

enum { kCacheSize = 240 };
inline void ProcessQuad(const int slices, const uint32 n_idx, 
                        uint32 &vert_idx, uint32 *indices, uint32 &idx) {
  indices[idx++] = vert_idx;  
  indices[idx++] = vert_idx + slices;
  indices[idx++] = n_idx;
  indices[idx++] = n_idx;
  indices[idx++] = vert_idx + slices;
  indices[idx++] = n_idx + slices;  
  ++vert_idx;
}

TriMeshPtr CreateSphere(float radius, uint32 slices, uint32 stacks) {
  if (slices >= kCacheSize) slices = kCacheSize - 1;
  if (stacks >= kCacheSize) stacks = kCacheSize - 1;
  if (slices < 2 || stacks < 1 || radius < 0.0) {
    Logger::Log("invalid arguments");
    return TriMeshPtr();
  }
  
  float sin_cache_2a[kCacheSize];
  float cos_cache_2a[kCacheSize];
  float sin_cache_2b[kCacheSize];
  float cos_cache_2b[kCacheSize];
  
  float deg_step = 2 * ax::kPi / slices;
  float deg = 0.f;
  for (uint32 j = 0; j < slices; ++j, deg += deg_step) {        
    // normal
    sin_cache_2a[j] = sin(deg);
    cos_cache_2a[j] = cos(deg);  
  }
  sin_cache_2a[slices] = sin_cache_2a[0];
  cos_cache_2a[slices] = cos_cache_2a[0];

  deg_step = ax::kPi / stacks;
  deg = 0.f;
  for (uint32 i = 0; i <= stacks; ++i, deg += deg_step) {    
    // normal
    sin_cache_2b[i] = sin(deg);
    cos_cache_2b[i] = cos(deg);  
  }
  /* Make sure it comes to a point */
  sin_cache_2b[0] = 0;
  sin_cache_2b[stacks] = 0;
  const uint32 n_vertices = 2 + (stacks - 1) * slices;
  const uint32 n_triangles = (stacks - 1) * slices * 2;
  Point *vertices = new Point[n_vertices];
  Normal *normals = new Normal[n_vertices];
  uint32 *indices = new uint32[n_triangles * 3];
  
  vertices[0].x = 0; 
  vertices[0].y = 0; 
  vertices[0].z = cos_cache_2b[0] * radius;
  normals[0].x = 0;
  normals[0].y = 0;
  normals[0].z = cos_cache_2b[0];
  uint32 vert_idx = 1;
  for (uint32 i = 1; i < stacks; ++i) {
    for (uint32 j = 0; j < slices; ++j, ++vert_idx) {
      normals[vert_idx].x = cos_cache_2a[j] * sin_cache_2b[i];
      normals[vert_idx].y = sin_cache_2a[j] * sin_cache_2b[i];
      normals[vert_idx].z = cos_cache_2b[i];

      vertices[vert_idx].x = radius * normals[vert_idx].x;
      vertices[vert_idx].y = radius * normals[vert_idx].y;
      vertices[vert_idx].z = radius * normals[vert_idx].z;
    }
  }
  vertices[vert_idx].x = 0; 
  vertices[vert_idx].y = 0;
  vertices[vert_idx].z = cos_cache_2b[stacks] * radius;
  normals[vert_idx].x = 0;
  normals[vert_idx].y = 0;
  normals[vert_idx].z = cos_cache_2b[stacks];

  //[0 1 2], [0 2 3], [0 3 4]
  uint32 idx = 0;  
  for (vert_idx = 1; vert_idx < slices; ) {
    indices[idx++] = 0;
    indices[idx++] = vert_idx;
    indices[idx++] = ++vert_idx;
  }  
  indices[idx++] = 0; indices[idx++] = vert_idx; indices[idx++] = 1;

  vert_idx = 1;
  for (uint32 i = 1; i < stacks - 1; ++i) {
    for (uint32 j = 1; j < slices; ++j) {
      uint32 n_idx = vert_idx + 1;
      ProcessQuad(slices, n_idx, vert_idx, indices, idx);
    }
    int n_idx = vert_idx + 1 - slices;
    ProcessQuad(slices, n_idx, vert_idx, indices, idx);
  } 
  
  uint32 n_idx = vert_idx + slices;
  for (uint32 j = 1; j < slices; ++j) {
    indices[idx++] = vert_idx;
    indices[idx++] = n_idx;
    indices[idx++] = ++vert_idx;
  }
  indices[idx++] = vert_idx;
  indices[idx++] = n_idx;
  indices[idx++] = n_idx - slices;
  return TriangleMesh::Create(vertices, normals, NULL, indices,
                              n_vertices, idx/3);
}

TriMeshPtr CreateCylinder(float base_radius, float top_radius, 
                          float height, uint32 slices, uint32 stacks) {
  if (slices >= kCacheSize) slices = kCacheSize - 1;
  if (stacks >= kCacheSize) stacks = kCacheSize - 1;
  if (slices < 2 || stacks < 1 || top_radius < 0.0 || base_radius < 0.0) {
    Logger::Log("invalid arguments");
    return TriMeshPtr();
  }

  /* Compute length (needed for normal calculations) */
  float delta_radius = base_radius - top_radius;
  float length = sqrtf(delta_radius*delta_radius + height*height);
  if (length == 0.0) {
    Logger::Log("invalid arguments");
    return TriMeshPtr();
  }

  float sin_cache1[kCacheSize];
  float cos_cache1[kCacheSize];
  float sin_cache2[kCacheSize];
  float cos_cache2[kCacheSize];

  if (slices >= kCacheSize) slices = kCacheSize - 1;  
  float z_normal = delta_radius / length;
  float xy_normal_ratio = height / length;

  const float deg_step = 2 * ax::kPi / slices;
  float deg = 0.f;
  for (uint32 i = 0; i < slices; ++i, deg += deg_step) {                
    sin_cache1[i] = sin(deg);
    cos_cache1[i] = cos(deg);
    // for normal
    sin_cache2[i] = xy_normal_ratio * sin_cache1[i];
    cos_cache2[i] = xy_normal_ratio * cos_cache1[i];
  }  

  sin_cache1[slices] = sin_cache1[0];
  cos_cache1[slices] = cos_cache1[0];  
  sin_cache2[slices] = sin_cache2[0];
  cos_cache2[slices] = cos_cache2[0];

  const uint32 n_vertices = (stacks + 1) * slices;
  const uint32 n_triangles = stacks * 2 * slices;
  Point *vertices = new Point[n_vertices];
  Normal *normals = new Normal[n_vertices];
  uint32 *indices = new uint32[n_triangles * 3];

  const float z_step = height / stacks;
  float z_low = 0;
  const float radius_step = -delta_radius / stacks;
  float radius_low = base_radius;
  uint32 vert_idx = 0;
  for (uint32 j = 0; j <= stacks; ++j) {
    for (uint32 i = 0; i < slices; ++i, ++vert_idx) {
      vertices[vert_idx].x = radius_low * sin_cache1[i];
      vertices[vert_idx].y = radius_low * cos_cache1[i];
      vertices[vert_idx].z = z_low;

      normals[vert_idx].x = sin_cache2[i];
      normals[vert_idx].y = cos_cache2[i];
      normals[vert_idx].z = z_normal;
    }
    z_low += z_step;
    radius_low += radius_step;
  }
  uint32 idx = 0;
  vert_idx = 0;
  for (uint32 j = 0; j < stacks; ++j) {
    for (uint32 i = 0; i < slices - 1; ++i) {
      uint32 n_idx = vert_idx + 1;
      ProcessQuad(slices, n_idx, vert_idx, indices, idx);
    }
    uint32 n_idx = vert_idx + 1 - slices;
    ProcessQuad(slices, n_idx, vert_idx, indices, idx);
  }

  return TriangleMesh::Create(vertices, normals, NULL, indices, 
                              n_vertices, n_triangles);
}


//switch (qobj->drawStyle) {
//case GLU_FILL:
//  /* Note:
//  ** An argument could be made for using a TRIANGLE_FAN for the end
//  ** of the cylinder of either radii is 0.0 (a cone).  However, a
//  ** TRIANGLE_FAN would not work in smooth shading mode (the common
//  ** case) because the normal for the apex is different for every
//  ** triangle (and TRIANGLE_FAN doesn't let me respecify that normal).
//  ** Now, my choice is GL_TRIANGLES, or leave the GL_QUAD_STRIP and
//  ** just let the GL trivially reject one of the two triangles of the
//  ** QUAD.  GL_QUAD_STRIP is probably faster, so I will leave this code
//  ** alone.
//  */
//  for (j = 0; j < stacks; j++) {
//    zLow = j * height / stacks;
//    zHigh = (j + 1) * height / stacks;
//    radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
//    radiusHigh = baseRadius - deltaRadius * ((float) (j + 1) / stacks);
//
//    glBegin(GL_QUAD_STRIP);
//    for (i = 0; i <= slices; i++) {
//      switch(qobj->normals) {
//      case GLU_FLAT:
//        glNormal3f(sinCache3[i], cosCache3[i], zNormal);
//        break;
//      case GLU_SMOOTH:
//        glNormal3f(sinCache2[i], cosCache2[i], zNormal);
//        break;
//      case GLU_NONE:
//      default:
//        break;
//      }
//      if (qobj->orientation == GLU_OUTSIDE) {
//        if (qobj->textureCoords) {
//          glTexCoord2f(1 - (float) i / slices,
//            (float) j / stacks);
//        }
//        glVertex3f(radiusLow * sinCache[i],
//          radiusLow * cosCache[i], zLow);
//        if (qobj->textureCoords) {
//          glTexCoord2f(1 - (float) i / slices,
//            (float) (j+1) / stacks);
//        }
//        glVertex3f(radiusHigh * sinCache[i],
//          radiusHigh * cosCache[i], zHigh);
//      } else {
//        if (qobj->textureCoords) {
//          glTexCoord2f(1 - (float) i / slices,
//            (float) (j+1) / stacks);
//        }
//        glVertex3f(radiusHigh * sinCache[i],
//          radiusHigh * cosCache[i], zHigh);
//        if (qobj->textureCoords) {
//          glTexCoord2f(1 - (float) i / slices,
//            (float) j / stacks);
//        }
//        glVertex3f(radiusLow * sinCache[i],
//          radiusLow * cosCache[i], zLow);
//      }
//    }
//    glEnd();
//  }
//  break;
//case GLU_POINT:
//  glBegin(GL_POINTS);
//  for (i = 0; i < slices; i++) {
//    switch(qobj->normals) {
//    case GLU_FLAT:
//    case GLU_SMOOTH:
//      glNormal3f(sinCache2[i], cosCache2[i], zNormal);
//      break;
//    case GLU_NONE:
//    default:
//      break;
//    }
//    sintemp = sinCache[i];
//    costemp = cosCache[i];
//    for (j = 0; j <= stacks; j++) {
//      zLow = j * height / stacks;
//      radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
//
//      if (qobj->textureCoords) {
//        glTexCoord2f(1 - (float) i / slices,
//          (float) j / stacks);
//      }
//      glVertex3f(radiusLow * sintemp,
//        radiusLow * costemp, zLow);
//    }
//  }
//  glEnd();
//  break;
//case GLU_LINE:
//  for (j = 1; j < stacks; j++) {
//    zLow = j * height / stacks;
//    radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
//
//    glBegin(GL_LINE_STRIP);
//    for (i = 0; i <= slices; i++) {
//      switch(qobj->normals) {
//      case GLU_FLAT:
//        glNormal3f(sinCache3[i], cosCache3[i], zNormal);
//        break;
//      case GLU_SMOOTH:
//        glNormal3f(sinCache2[i], cosCache2[i], zNormal);
//        break;
//      case GLU_NONE:
//      default:
//        break;
//      }
//      if (qobj->textureCoords) {
//        glTexCoord2f(1 - (float) i / slices,
//          (float) j / stacks);
//      }
//      glVertex3f(radiusLow * sinCache[i],
//        radiusLow * cosCache[i], zLow);
//    }
//    glEnd();
//  }
//  /* Intentionally fall through here... */
//case GLU_SILHOUETTE:
//  for (j = 0; j <= stacks; j += stacks) {
//    zLow = j * height / stacks;
//    radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
//
//    glBegin(GL_LINE_STRIP);
//    for (i = 0; i <= slices; i++) {
//      switch(qobj->normals) {
//      case GLU_FLAT:
//        glNormal3f(sinCache3[i], cosCache3[i], zNormal);
//        break;
//      case GLU_SMOOTH:
//        glNormal3f(sinCache2[i], cosCache2[i], zNormal);
//        break;
//      case GLU_NONE:
//      default:
//        break;
//      }
//      if (qobj->textureCoords) {
//        glTexCoord2f(1 - (float) i / slices,
//          (float) j / stacks);
//      }
//      glVertex3f(radiusLow * sinCache[i], radiusLow * cosCache[i],
//        zLow);
//    }
//    glEnd();
//  }
//  for (i = 0; i < slices; i++) {
//    switch(qobj->normals) {
//    case GLU_FLAT:
//    case GLU_SMOOTH:
//      glNormal3f(sinCache2[i], cosCache2[i], 0.0);
//      break;
//    case GLU_NONE:
//    default:
//      break;
//    }
//    sintemp = sinCache[i];
//    costemp = cosCache[i];
//    glBegin(GL_LINE_STRIP);
//    for (j = 0; j <= stacks; j++) {
//      zLow = j * height / stacks;
//      radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
//
//      if (qobj->textureCoords) {
//        glTexCoord2f(1 - (float) i / slices,
//          (float) j / stacks);
//      }
//      glVertex3f(radiusLow * sintemp,
//        radiusLow * costemp, zLow);
//    }
//    glEnd();
//  }
//  break;
//default:
//  break;
//}

} // ax
