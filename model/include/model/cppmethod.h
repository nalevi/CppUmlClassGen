#ifndef UMLGEN_MODEL_CPPMETHOD_H
#define UMLGEN_MODEL_CPPMETHOD_H

#include <Wt/Dbo/Dbo.h>

#include <string>

#include "cpprecord.h"
#include "visibility.h"

namespace dbo = Wt::Dbo;

namespace umlgen
{
namespace model
{

class CppMethodParam
{
public:
  //CppMethodParam(const std::string& i_,
  //               const std::string& t_): identifier(i_), type(t_) {}

  std::string identifier;
  std::string type;

  dbo::ptr<CppMethod> method;

  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, identifier, "identifier");
    dbo::field(a_, type, "type");

    dbo::belongsTo(a_, method, "cppmethod");
  }
};

/*
  * This class represents a c++ method in the database.
  * Fields: - name: identifier
  *         - visibility: private, protected, public
  *         - returnType: the type of the return for the method
  *         - isVirtual: is the method a virtual method
*/
class CppMethod
{
public:
  //CppMethod(const std::string& n_,
  //          const Visibility& v_,
  //          const std::string& rt_,
  //          const bool& iv_): 
  //            name(n_), visibility(v_), returnType(rt_), isVirtual(iv_) {}
  //~CppMethod();

  dbo::ptr<CppRecord> cpprec;
  dbo::collection< dbo::ptr<CppMethodParam> > params;

  std::string name;
  Visibility visibility;
  std::string returnType;
  bool isVirtual;

  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, name, "name");
    dbo::field(a_, visibility, "visibility");
    dbo::field(a_, returnType, "returntype");
    dbo::field(a_, isVirtual, "isvirtual");

    dbo::belongsTo(a_, cpprec, "cpprecord");
    
    dbo::hasMany(a_, params, dbo::ManyToOne, "id");

  }
};


} // model
} // umlgen
#endif
