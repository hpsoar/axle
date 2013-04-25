#include "axle/cg/material-factory.h"
#include "axle/cg/glmaterial.h"
#include "axle/cg/glslmaterial.h"
#include "axle/cg/scene.h"
#include "axle/cg/parser.h"

namespace ax {
GLMaterialPtr MaterialFactory::CreateGLMaterial() {
  return GLMaterialPtr(new GLMaterial());
}

GLSLMaterialPtr MaterialFactory::CreateGLSLMaterial() {
  return GLSLMaterialPtr(new GLSLMaterial());
}

MaterialPtr MaterialFactory::CreateGLMaterial(
    const char *name, FILE *fp) {    
  GLMaterialPtr matl = MaterialFactory::CreateGLMaterial();  
  
  char buf[512];
  LineToken token;
  while (fgets(buf, 512, fp) != NULL) {
    const char *ptr = StripLeading(buf);
    if (ax::IsBlankOrComment(ptr)) continue;

    ptr = token.Extract(ptr);
    if (token.Equals("end")) break;

    if (token.Equals("ambient") || token.Equals("amb") || token.Equals("Ka"))
      matl->set_ambient(ax::ExtractColorf(ptr));
    else if (token.Equals("diffuse") || token.Equals("dif") || token.Equals("Kd"))
      matl->set_diffuse(ax::ExtractColorf(ptr));
    else if (token.Equals("specular") || token.Equals("spec") || token.Equals("Ks"))
      matl->set_specular(ax::ExtractColorf(ptr));
    else if (token.Equals("emission") || token.Equals("emit") || token.Equals("Ke"))
      matl->set_emission(ax::ExtractColorf(ptr));
    else if (token.Equals("shininess") || token.Equals("shiny") || token.Equals("Ns"))
      matl->set_shininess(ax::ParseNumber(ptr));
    else 
      ax::UnknownCommand(token.ptr(), "GLMaterial");
  }
  return matl;
}

MaterialPtr MaterialFactory::CreateGLSLMaterial(const char *name, FILE *fp, 
                                                Scene *s) {
  GLSLMaterialPtr matl = MaterialFactory::CreateGLSLMaterial();

  char buf[512];
  LineToken token;  
  while (fgets(buf, 512, fp) != NULL) {
    const char *ptr = ax::StripLeading(buf);
    if (ax::IsBlankOrComment(ptr)) continue;

    ptr = token.Extract(ptr);
    if (token.Equals("end")) break;
    
    /*if (token.Equals("vert")) {
      ptr = token.Extract(ptr);
      matl->shader()->AttachVertShader(token.ptr());
    }
    else if (token.Equals("frag")) {
      ptr = token.Extract(ptr);
      matl->shader()->AttachFragShader(token.ptr());
    }
    else if (token.Equals("geom")) {
      ptr = token.Extract(ptr);
      matl->shader()->AttachGeomShader(token.ptr());
    }
    else */
    if (token.Equals("bind")) {
      LineToken var_name;
      ptr = var_name.Extract(ptr);
      ptr = token.Extract(ptr);
      if (token.Equals("const4")) {
        matl->BindConstVec4(var_name.ptr(), ax::ExtractVec4(ptr));
      }
      else if (token.Equals("const3")) {
        matl->BindConstVec3(var_name.ptr(), ax::ExtractVec3(ptr));
      }
      else if (token.Equals("tex")) {
        ptr = token.Extract(ptr);
        matl->BindTexture(var_name.ptr(), s->LoadTexture(token.ptr()));
      }
      else if (token.Equals("float")) {
        matl->BindConst(var_name.ptr(), ax::ParseNumber(ptr));
      }
      else if (token.Equals("vsub")) {
        ptr = token.Extract(ptr);
        matl->BindVertSubroutine(token.ptr());
      }
      else if (token.Equals("fsub")) {
        ptr = token.Extract(ptr);
        matl->BindFragSubroutine(token.ptr());
      }
      else {
        ax::Unknown("bind type", token.ptr(), "GLSMMaterial");
      }
    }
    else {
      ax::UnknownCommand(token.ptr(), "GLSLMaterial");
    }
  }
  return matl;
}
} // ax


