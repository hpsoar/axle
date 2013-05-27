#include "../cg/glmesh.h"
#include "../core/container.hpp"
#include "../cg/scene.h"
#include "glm_avl.h"

namespace ax {
uint32 *CreateIndexArray(const GLMmodel *model) {
  if (model->numtriangles == 0) return NULL;

  uint32 *indices = new uint32[model->numtriangles * 3];
  for (size_t i = 0; i < model->numtriangles; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      indices[3 * i + j] = model->triangles[i].vindices[j] - 1;      
    }
  }
  return indices;
}

Point *CreateVertexArray(const GLMmodel *model) {
  if (model->numvertices == 0) return NULL;

  Point *vertices = new Point[model->numvertices];
  memcpy(vertices, model->vertices + 3, model->numvertices * sizeof(Point));  
  return vertices;
}

Normal *CreateNormalArray(const GLMmodel *model) {
  if (model->numnormals == 0) return NULL;

  Normal *normals =  new Normal[model->numnormals];
  memcpy(normals, model->normals + 3, model->numnormals * sizeof(Normal));
  return normals;
}

float *CreateTexCoordArray(const GLMmodel *model) {
  if (model->numtexcoords == 0) return NULL;

  float *tcoords =  new float[model->numtexcoords * 2];
  memcpy(tcoords, model->texcoords + 2, 
         model->numtexcoords * 2 * sizeof(float));
  return tcoords;
}

typedef List<size_t> Idxs;
typedef Array<Idxs> VertAttribsVec;
//!!!don't use vector, it'll cause problem in Unregister

typedef size_t TriVertAttribIdx(const GLMmodel *model, size_t tri, size_t vidx);

inline size_t GetNormalIdx(const GLMmodel *model, size_t tri, size_t vidx) {
  return model->triangles[tri].nindices[vidx] - 1;
}

inline size_t GetVertexIdx(const GLMmodel *model, size_t tri, size_t vidx) {
  return model->triangles[tri].vindices[vidx] - 1;
}

inline size_t GetTexcoordIdx(const GLMmodel *model, size_t tri, size_t vidx) {
  return model->triangles[tri].tindices[vidx] - 1;
}

void VertAttribStastics(const GLMmodel *model, VertAttribsVec *vert_attribs,
                        TriVertAttribIdx attrib) {
  for (size_t i = 0; i < model->numtriangles; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      size_t vidx = model->triangles[i].vindices[j] - 1;
      size_t nidx = model->triangles[i].nindices[j] - 1;
      (*vert_attribs)[GetVertexIdx(model, i, j)].insert(attrib(model, i, j));      
    }
  }
}

size_t VOffset(const Idxs &nidxs, size_t nidx) {
  size_t i = 0;
  Idxs::const_iterator it = nidxs.begin();
  for (; it != nidxs.end(); ++it, ++i) {
    if (*it == nidx) return i;
  }
  return -1;
}

size_t CountExpandedVerts(const VertAttribsVec &attribs) {
  size_t n = 0;
  for (size_t i = 0; i < attribs.size(); ++i) n += attribs[i].size();
  return n;
}

size_t CountExpandedVerts(const VertAttribsVec &attribs1, 
                          const VertAttribsVec &attribs2) {
  assert(attribs1.size() == attribs2.size());
  size_t n = 0;
  for (size_t i = 0; i < attribs1.size(); ++i) 
    n += attribs1[i].size() * attribs2[i].size();
  return n;
}

size_t ExpandNormalAttrib(const GLMmodel *model, Point **vertices, 
                          Normal **normals, uint32 **indices) {
  VertAttribsVec vert_nidxs(model->numvertices);
  ax::VertAttribStastics(model, &vert_nidxs, GetNormalIdx);

  size_t n_vertices = ax::max(CountExpandedVerts(vert_nidxs), 
                              model->numvertices);
  ax::Logger::Log("n_vertices = %d, %d", n_vertices, model->numvertices);

  *vertices = new Point[n_vertices];
  *normals = new Normal[n_vertices];
      
  std::vector<int> vidx_begins(model->numvertices, 0);
  for (size_t i = 0, vi = 0; i < model->numvertices; ++i) {
    if (i > 0) {
      vidx_begins[i] = vidx_begins[i-1] + vert_nidxs[i-1].size();
    }

    const float *vptr = model->vertices + i*3 + 3;
    Idxs::const_iterator it = vert_nidxs[i].begin();
    for ( ; it != vert_nidxs[i].end(); ++it) {
      const float *nptr = model->normals + (*it + 1) * 3;
      for (int k = 0; k < 3; ++k) {
        (*vertices)[vi][k] = vptr[k];
        (*normals)[vi][k] = nptr[k];
      }
      ++vi;
    }
  }

  *indices = new uint32[model->numtriangles * 3];
  for (size_t i = 0; i < model->numtriangles; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      size_t vidx = model->triangles[i].vindices[j] - 1;
      size_t nidx = model->triangles[i].nindices[j] - 1;
      (*indices)[3 * i + j] = vidx_begins[vidx] + 
                            VOffset(vert_nidxs[vidx], nidx);
    }
  }
  return n_vertices;
}

size_t ExpandTexcoordAttrib(const GLMmodel *model, Point **vertices, 
                            float **tcoords, uint32 **indices) {
  VertAttribsVec vert_tidxs(model->numvertices);
  ax::VertAttribStastics(model, &vert_tidxs, GetTexcoordIdx);

  size_t n_vertices = ax::max(CountExpandedVerts(vert_tidxs), 
                              model->numvertices);
  ax::Logger::Log("n_vertices = %d, %d", n_vertices, model->numvertices);

  *vertices = new Point[n_vertices];
  *tcoords = new float[2*n_vertices];
      
  std::vector<int> vidx_begins(model->numvertices, 0);
  for (size_t i = 0, vi = 0; i < model->numvertices; ++i) {
    if (i > 0) {
      vidx_begins[i] = vidx_begins[i-1] + vert_tidxs[i-1].size();
    }

    const float *vptr = model->vertices + i*3 + 3;
    Idxs::const_iterator it = vert_tidxs[i].begin();
    for ( ; it != vert_tidxs[i].end(); ++it) {
      const float *tptr = model->texcoords + (*it + 1) * 2;
      for (int k = 0; k < 3; ++k) {
        (*vertices)[vi][k] = vptr[k];        
      }
      (*tcoords)[2*vi] = tptr[0];
      (*tcoords)[2*vi + 1] = tptr[1];
      ++vi;
    }
  }

  *indices = new uint32[model->numtriangles * 3];
  for (size_t i = 0; i < model->numtriangles; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      size_t vidx = model->triangles[i].vindices[j] - 1;
      size_t nidx = model->triangles[i].nindices[j] - 1;
      (*indices)[3 * i + j] = vidx_begins[vidx] + 
                            VOffset(vert_tidxs[vidx], nidx);
    }
  }
  return n_vertices;
}

size_t ExpandMesh(const GLMmodel *model, Point **vertices, Normal **normals, 
                  float **tcoords, uint32 **indices) {
  VertAttribsVec vert_nidxs(model->numvertices);
  VertAttribsVec vert_tidxs(model->numvertices);
  ax::VertAttribStastics(model, &vert_nidxs, GetNormalIdx);
  ax::VertAttribStastics(model, &vert_tidxs, GetTexcoordIdx);
  size_t n_vertices = ax::max(CountExpandedVerts(vert_nidxs, vert_tidxs),
                              model->numvertices);
  *vertices = new Point[n_vertices];
  *normals = new Point[n_vertices];
  *tcoords = new float[2*n_vertices];
  std::vector<int> vidx_begins(model->numvertices, 0);
  for (size_t i = 0, vi = 0; i < model->numvertices; ++i) {
    if (i > 0) {
      vidx_begins[i] = vidx_begins[i-1] + 
                       vert_nidxs[i-1].size() * vert_tidxs[i-1].size();
    }
    //[vi ni0 ti0] .. [vi ni0 tiT][vi ni1 ti0]..[vi ni1 tiT]...[vi niN tiT]
    const float *vptr = model->vertices + i*3 + 3;
    Idxs::const_iterator nit = vert_nidxs[i].begin();

    for ( ; nit != vert_nidxs[i].end(); ++nit) {
      const float *nptr = model->normals + (*nit + 1) * 3;
      Idxs::const_iterator tit = vert_tidxs[i].begin();

      for ( ; tit != vert_tidxs[i].end(); ++tit) {
        const float *tptr = model->texcoords + (*tit + 1) * 2;
        for (int k = 0; k < 3; ++k) {
          (*vertices)[vi][k] = vptr[k];
          (*normals)[vi][k] = nptr[k];
        }
        (*tcoords)[2*vi] = tptr[0];
        (*tcoords)[2*vi + 1] = tptr[1];
        ++vi;
      }
    }
  }
  *indices = new uint32[model->numtriangles * 3];
  for (size_t i = 0; i < model->numtriangles; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      size_t vidx = model->triangles[i].vindices[j] - 1;
      size_t nidx = model->triangles[i].nindices[j] - 1;
      size_t tidx = model->triangles[i].tindices[j] - 1;
      (*indices)[3 * i + j] = vidx_begins[vidx] + 
          VOffset(vert_nidxs[vidx], nidx) * vert_tidxs[vidx].size() +
          VOffset(vert_tidxs[vidx], tidx);
    }
  }
  return n_vertices;
}

// NOTE: Unsafe!!!
ObjectPtr LoadGLMesh(Scene *s, const std::string &filename, Options opts) {
  GLMmodel *model = glmReadOBJ(filename.c_str());
  if (model == NULL) return ObjectPtr();

  if (model->numnormals == 0 || opts.Contain(kGenNormal)) {
    glmFacetNormals(model);
    glmVertexNormals(model, 180);
  }

  Point *vertices = NULL;
  Normal *normals = NULL;
  float *tcoords = NULL;
  uint32 *indices = NULL;

  size_t n_vertices = model->numvertices;
  bool expand_normal = model->numnormals > 0 && 
                       model->numnormals != model->numvertices;
  bool expand_tcoord = model->numtexcoords > 0 &&
                       model->numtexcoords != model->numvertices;
  bool expand_all = model->numnormals > 0 && model->numtexcoords > 0 &&
                    (model->numvertices != model->numnormals || 
                     model->numvertices != model->numtexcoords);
  if (expand_all) {
    n_vertices = ax::ExpandMesh(model, &vertices, &normals, &tcoords, &indices);    
  }  
  else if (expand_normal) {
    n_vertices = ax::ExpandNormalAttrib(model, &vertices, &normals, &indices);    
  }
  else if (expand_tcoord) {
    n_vertices = ax::ExpandTexcoordAttrib(model, &vertices, &tcoords, &indices);    
  }
  else {
    // no expand needed
    vertices = ax::CreateVertexArray(model);
    indices = ax::CreateIndexArray(model);
    if (model->numnormals > 0) normals = ax::CreateNormalArray(model);
    if (model->numtexcoords > 0) tcoords = ax::CreateTexCoordArray(model);
  }  

  GLMeshPtr ptr = GLMesh::Create(ax::TriangleMesh::Create(
      vertices, normals, tcoords, indices, n_vertices,
      model->numtriangles));
  
  if (opts.Contain(kIgnoreGroup)) {
    ptr->Add(GLGroup::Create(0, model->numtriangles));
  }
  else {
    assert(s != NULL);
    GLMgroup *g = model->groups;
    while (g != NULL) {
      if (g->numtriangles > 0) {
        GLGroupPtr obj(GLGroup::Create(g->triangles[0], g->numtriangles));
        obj->set_material(s->material(model->materials[g->material].name));
        ptr->Add(obj);
      }
      g = g->next;
    }
  }
  glmDelete(model);

  return ptr;
}
} // ax
