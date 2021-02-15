#ifndef UMLGEN_MODEL_CPPATTRIBUTE_H
#define UMLGEN_MODEL_CPPATTRIBUTE_H

#include <Wt/Dbo/Dbo.h>

#include <string>

#include "cpprecord.h"
#include "visibility.h"

namespace dbo = Wt::Dbo;

namespace umlgen
{
namespace model
{

typedef std::shared_ptr<CppAttribute> CppAttributePtr;
typedef dbo::ptr<CppAttribute> DboCppAttributePtr;

/*
  * This class represents a C++ record's attributes.
  * Fields: - name: identifier
  *         - visibility: private, protected, public
  *         - type: the tpye of the attribute (e.g. int, string...)
*/
class CppAttribute
{
public:
  // CppAttribute(const std::string& n_,
  //              const Visibility& v_,
  //              const std::string& t_): name(n_), visibility(v_), type(t_) {}

  dbo::ptr<CppRecord> cpprec;

  std::string name;
  Visibility visibility;
  std::string type;
  
  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, name, "name");
    dbo::field(a_, visibility, "visibility");
    dbo::field(a_, type, "type");

    dbo::belongsTo(a_, cpprec, "cpprecord");
  }

  model::CppRecordPtr recptr;
};

} // model
} // umlgen
#endif
