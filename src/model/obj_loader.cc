#include "triangle_mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include "../core/types.h"

namespace ax {

void FirstPass(FILE* fd, uint32 *vertex_count,  uint32 *normal_count, 
               uint32 *texcoord_count, uint32 *face_count);

void ParseIndices(char *line, uint32 *vertex_idx,  
                  uint32 *texcoord_idx, uint32 *normal_idx);

//TriMeshPtr LoadObj(const char *filename) {
//  FILE *fd = fopen(filename, "rb");
//  if (NULL == fd) {
//    Logger::Log("Load, can't open '%s'", filename);
//    return TriMeshPtr();
//  }
//
//  uint32 nv, nn, nt, nf;
//  FirstPass(fd, &nv, &nn, &nt, &nf);
//
//  float* texcoords = NULL;
//  if (nt > 0) { texcoords = new float[nt * 2]; }
//
//  Point *points = new Point[nv];
//  Normal *normals = new Normal[nv];
//  uint32 *vertex_indices = new uint32[3 * nf];
//
//  /*float* normals = NULL;
//  if (nn > 0) { normals = new float[ 3 * nn ]; }*/
//
//  char line[kMaxLine];
//  int pi = 0, fi = 0, ti = 0, ni = 0;
//  while (fgets(line, kMaxLine, fd) != 0) {
//    if (line[0] == 'v') {
//      if (line[1] == 'n') {
//        /*float x, y, z;
//        sscanf(&line[2], "%f %f %f\n", &x, &y, &z);
//        normals[ni++] = Normal(x, y, z);*/
//      } else if (line[1] == 't') {       
//        float u, v;
//        sscanf(&line[2], "%f %f\n", &u, &v);
//        texcoords[ti++] = u;
//        texcoords[ti++] = v;
//      } else {
//        float x, y, z;
//        sscanf(&line[1], "%f %f %f\n", &x, &y, &z);
//        points[pi++] = Point(x, y, z);
//      }
//    } else if (line[0] == 'f') {
//      char s1[32], s2[32], s3[32];
//      sscanf(&line[1], "%s %s %s\n", s1, s2, s3);
//      
//      uint32 v1, t1, n1;					
//      ParseIndices(s1, &v1, &t1, &n1);
//
//      uint32 v2, t2, n2;
//      ParseIndices(s2, &v2, &t2, &n2);
//
//      uint32 v3, t3, n3;
//      ParseIndices(s3, &v3, &t3, &n3);
//
//      //indices start from 1 in obj files
//      v1 -= 1; v2 -= 1; v3 -= 1;
//      vertex_indices[fi++] = v1;
//      vertex_indices[fi++] = v2;
//      vertex_indices[fi++] = v3;      
//    }
//  }
//  if (ni == 0) FinalizeModel(points, nv, vertex_indices, &nf, normals);
//
//  return TriangleMesh::Create(points, normals, texcoords, vertex_indices,
//                              nv, nf);
//}

void FirstPass(FILE* fd, uint32 *vertex_count, uint32 *normal_count, 
               uint32 *texcoord_count, uint32 *face_count) {
  *vertex_count = 0;
  *face_count = 0;
  *normal_count = 0; 
  *texcoord_count = 0;
  char line[kMaxLine];
  while (fgets(line, kMaxLine, fd) != 0) {
    if (line[0] == 'v') {
      if (line[1] == 'n') {
        ++(*normal_count);
      } else if (line[1] == 't') {
        ++(*texcoord_count);
      } else {
        ++(*vertex_count);
      }
    } else if (line[0] == 'f') {
      ++(*face_count);
    }
  }
  fseek(fd, 0, 0);
}	

void ParseIndices(char *line, uint32 *vertex_idx, 
                  uint32 *texcoord_idx,  uint32 *normal_idx) {	
  //the following null used instead of NULL, in case of atoi's undefined
  //behavior on NULL
  char null[] = " ";
  char *ptr, *tp = null, *np = null;

  // replace slashes with null characters and cause tp and np to point
  // to character immediately following the first or second slash
  int count = 0;
  for (ptr = line; *ptr != '\0'; ++ptr) {
    if (*ptr == '/') {
      if (tp == null) {
        tp = ptr + 1;
      } else {
        np = ptr + 1;
      }
      *ptr = '\0';
    }
    ++count;
  }
  *vertex_idx = atoi(line); 
  *normal_idx = atoi(np); 
  *texcoord_idx = atoi(tp);			
}

void FinalizeModel(const Point *vertices, uint32 n_vertices, 
                   uint32 *indices, uint32 *face_count, 
                   Normal *normals) {
  uint32 *faces_on_vertex = new uint32[n_vertices];
  memset(faces_on_vertex, 0, sizeof(int) * n_vertices);
  int total_face_count = *face_count;
  int valid_face_count = 0;
  for (int i=0, fi=0, v_fi=0; i < total_face_count; ++i) {
    int a_i = indices[fi++];
    int b_i = indices[fi++];
    int c_i = indices[fi++];
    
    const Point &a = vertices[a_i];
    const Point &b = vertices[b_i];
    const Point &c = vertices[c_i];

    if (a == b || a == c || b == c) continue;
    indices[v_fi++] = a_i;
    indices[v_fi++] = b_i;
    indices[v_fi++] = c_i;
    ++valid_face_count;

    Vector3 ab = b - a;
    Vector3 ac = c - a;
    
    Normal n = Normal(Cross(ab, ac));
    float length = glm::length(n);    
    if (length > 0) n /= length;

    normals[a_i] += n;
    normals[b_i] += n;
    normals[c_i] += n;

    ++faces_on_vertex[a_i];
    ++faces_on_vertex[b_i];
    ++faces_on_vertex[c_i];
  }
  *face_count = valid_face_count;
  for (uint32 i = 0; i < n_vertices; ++i) {
    if (faces_on_vertex[i] > 0) {
      normals[i] /= static_cast<float>(faces_on_vertex[i]);
    }
    else {
      printf("ioslated vertex\n");
    }
  }
  delete[] faces_on_vertex;  
}

} // ax
