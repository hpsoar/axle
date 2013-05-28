#include "trimesh_glmmodel.h"
#include <map>

namespace ax {
void TriMeshGLM::LoadFromGLMmodel(GLMmodelPtr model, ax::Options opts) {
 /* GLMmodelPtr model = GLMmodelPtr(glmReadOBJ(filename.c_str()), glmDelete);
  RET(model != NULL);*/

  if (model->numnormals == 0 || opts.Contain(ax::kGenNormal)) {
    glmFacetNormals(model.get());
    glmVertexNormals(model.get(), 180);
  }

  bool expand = (model->numvertices != model->numnormals && model->numnormals > 0) ||
                (model->numvertices != model->numtexcoords && model->numtexcoords > 0);
  
  if (expand) {
    this->Expand(model);
  }
  else {
    this->SetNormals(model->normals + 3, model->numnormals);
    this->SetVertices(model->vertices + 3, model->numvertices);
    this->SetTcoords(model->texcoords + 2, model->numtexcoords);
    this->ReserveTriangles(model->numtriangles);
    for (uint32 ti = 0; ti < model->numtriangles; ++ti) {
      for (uint32 vi = 0; vi < 3; ++vi) {
        this->AddIndex(model->triangles[ti].vindices[vi] - 1);
      }
    }
  } 

  if (opts.Contain(ax::kNeedAdjacency)) this->GenAdjacency(model);
}

void TriMeshGLM::Expand(GLMmodelPtr model) {
  this->ReserveNormals(model->numnormals);
  this->ReserveVertices(model->numvertices);
  this->ReserveTcoords(model->numtexcoords);
  this->ReserveTriangles(model->numtriangles);

  typedef std::map<ax::VertIdx, uint32> PointMap;
  PointMap expanded_pts;
  for (uint32 ti = 0; ti < model->numtriangles; ++ti) {
    for (int vi = 0; vi < 3; ++vi) {
      VertIdx idx;
      idx.pidx = model->triangles[ti].vindices[vi];
      if (model->numnormals > 0) idx.nidx = model->triangles[ti].nindices[vi];
      if (model->numtexcoords > 0) idx.tidx = model->triangles[ti].tindices[vi];
      auto it = expanded_pts.find(idx);
      if (it == expanded_pts.end()) {
        this->AddIndex(expanded_pts.size());        
        this->AddVertex(model->vertices + model->triangles[ti].vindices[vi] * 3);
        if (model->numnormals > 0) {
          this->AddNormal(model->normals + model->triangles[ti].nindices[vi] * 3);
        }
        if (model->numtexcoords > 0) {
          this->AddTcoord(model->texcoords + model->triangles[ti].tindices[vi] * 2);
        }      
        expanded_pts[idx] = expanded_pts.size();
      }
      else {
        this->AddIndex(it->second);
      }
    }
  }
}

void TriMeshGLM::GenAdjacency(GLMmodelPtr model) {  
  if (this->adj_indices_.size() > 0) {
    ax::Logger::Log("TriangleMesh::GenAdjacencyIndices:adjacency indices already generated");
    return;
  }

  typedef std::vector<uint32> TriangleList;
  typedef std::map<Edge, TriangleList> EdgeTriangles;
  EdgeTriangles edge_triangles;
  for (uint32 ti = 0; ti < model->numtriangles; ++ti) {    
    for (uint32 vi = 0; vi < 3; ++vi) {
      // use original vertices for neighborhood computation, expanded vertices may loose the information
      Edge e(model->triangles[ti].vindices[vi], model->triangles[ti].vindices[(vi+1) % 3]);
      edge_triangles[e].push_back(ti);   
    }
  }

  this->ReserveAdjacency();
  // NOTE: should fetch vertex index in this, because, the model may have been expanded,
  //       thus indices also changed to point to new vertices
  for (uint32 ti = 0; ti < model->numtriangles; ++ti) {
    const uint32 *idxs = this->tri_indices(ti);
    for (int vi = 0; vi < 3; ++vi) {      
      Edge e(model->triangles[ti].vindices[vi], model->triangles[ti].vindices[(vi+1) % 3]);

      auto triangles = edge_triangles[e];
      auto it = triangles.begin();
      while (it != triangles.end() && *it == ti) ++it; // find the triangle share the edge

      uint32 adj_vert = idxs[vi];
      if (it != triangles.end()) {
        const uint32 *neighbor_idxs = this->tri_indices(*it);
        adj_vert = neighbor_idxs[0];
        for (int nvi = 0; nvi < 3; ++nvi) {
          if (!e.Contains(model->triangles[*it].vindices[nvi])) {
            adj_vert = neighbor_idxs[nvi];
            break;
          }
        }
      }
      this->adj_indices_.push_back(idxs[vi]);
      this->adj_indices_.push_back(adj_vert);
    }
  }
}
}