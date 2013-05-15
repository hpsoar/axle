#include "axle/cg/object-factory.h"
#include <glm/gtc/matrix_transform.hpp>
#include "axle/cg/scene.h"
#include "axle/cg/group.h"
#include "axle/cg/glmesh.h"
#include "axle/cg/parser.h"

namespace ax {
glm::mat4 ExtractMatrix(const char *str) {
  LineToken token;
  const char *ptr = token.Extract(str);
  if (token.Equals("identity")) {
    return glm::mat4(1.0);
  }
  else if (token.Equals("scale")) {    
    return glm::scale(glm::mat4(1.0), ax::ExtractVec3(ptr));
  }
  else if (token.Equals("translate")) {
    return glm::translate(glm::mat4(1.0), ax::ExtractVec3(ptr));
  }
  else if (token.Equals("rotate")) {
    glm::mat4 m(1.0);
    glm::vec3 v = ax::ExtractVec3(ptr);
    m = glm::rotate(m, v.x, glm::vec3(1, 0, 0));
    m = glm::rotate(m, v.y, glm::vec3(0, 1, 0));
    m = glm::rotate(m, v.z, glm::vec3(0, 0, 1));
    return m;
  }
  else {
    ax::Unsupported("transformation type", token.ptr());
    return glm::mat4(1.f);
  }  
}

ObjectPtr ObjectFactory::CreateGroup(FILE *fp, Scene *s) {  
  ax::GroupPtr g = ax::ObjectFactory::CreateGroup();

  ax::Matrix4x4 transform(1.0);

  char buf[512];
  LineToken token;
  while (fgets(buf, 512, fp) != NULL) {
    const char *ptr = ax::StripLeading(buf);
    if (ax::IsBlankOrComment(ptr)) continue;

    ptr = token.Extract(ptr);
    if (token.Equals("end")) break;

    if (token.Equals("object")) {
      token.Extract(ptr);

      ObjectPtr obj = s->named_object(token.ptr()); // case: object <name>

      if (obj == NULL) obj = s->LoadObject(ptr, fp);
      g->Add(obj);
    }
    else if (token.Equals("matrix")) {
      transform *= ax::ExtractMatrix(ptr);
    }
    else if (token.Equals("material")) {
      g->set_material(ObjectFactory::LoadMaterial(ptr, s, fp));
    }
    else {
      ax::UnknownKeyword(token.ptr(), "Group");
      ax::EatToEndKeyword(fp, token); // the end correspond to the keyword
    }
  }
  g->set_transform(transform);

  return g;
}

/*
ObjectPtr ObjectFactory::CreateGLMesh(FILE *fp, Scene *s) {
  ax::GLMeshPtr mesh;
  
  return mesh;
}
*/

ObjectPtr ObjectFactory::LoadWavefront(FILE *fp, Scene *s, Options opts) {
  ax::ObjectPtr mesh;
  ax::MaterialPtr matl;
  ax::Matrix4x4 transform(1.0);

  char buf[512];
  LineToken token;
  while (fgets(buf, 512, fp)) {
    const char *ptr = ax::StripLeading(buf);
    if (ax::IsBlankOrComment(ptr)) continue;

    ptr = token.Extract(ptr);
    if (token.Equals("end")) break;

    if (token.Equals("file")) { 
      ptr = token.Extract(ptr);
      if (mesh == NULL) {
        mesh = ax::LoadGLMesh(
            s, ax::UserOptions::GetFullModelPath(token.ptr()), opts);
      }
      else {
        ax::Logger::Log("Duplicated mesh file '%s' ignored!", token.ptr());
      }
    }
    else if (token.Equals("matrix")) {
      transform *= ax::ExtractMatrix(ptr);
    }
    else if (token.Equals("material")) {
      if (matl == NULL) matl = ObjectFactory::LoadMaterial(ptr, s, fp);
      else ax::Logger::Log("Duplicated material ignored!");
    }
    else if (token.Equals("uvscale")) {
      /*ptr = token.Extract(ptr);
      float uscale = atoi(token.ptr());
      ptr = token.Extract(ptr);
      float vscale = atoi(token.ptr());*/
      //mesh->ScaleUV(uscale, vscale);
    }
    else {
      ax::UnknownCommand(token.ptr(), "Wavefront object");
    }
  }
  if (mesh != NULL) {
    mesh->set_material(matl);
    mesh->set_transform(transform);
  }
  return mesh;
}

GroupPtr ObjectFactory::CreateGroup() { return GroupPtr(new Group()); }

/*
 * @ptr: material <@ptr: matl_name/matl_type [matl_name] >
 */
MaterialPtr ObjectFactory::LoadMaterial(const char *ptr, Scene *s, FILE *fp) {
  LineToken token;
  token.Extract(ptr);
  ax::MaterialPtr m = s->material(token.ptr());

  if (m == NULL) return s->LoadMaterial(ptr, fp);
  return m;
}

} // ax

