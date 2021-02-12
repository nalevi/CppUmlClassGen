#ifndef UMLGEN_MODEL_CPPRECORD_H
#define UMLGEN_MODEL_CPPRECORD_H

#include <Wt/Dbo/Dbo.h>

#include <string>

namespace dbo = Wt::Dbo;

namespace umlgen
{
namespace model
{

class CppRecord;

/*
  * This class represents a NamespaceDecl in the database.
  * attributes: - name: namespace identifier
*/
class CppNamespace
{
public:

  dbo::collection< dbo::ptr<CppRecord> > cppRecords;

  std::string name;
  
  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, name, "name");

    dbo::hasMany(a_, cppRecords, dbo::ManyToOne);
  } 

  
};

typedef std::shared_ptr<CppNamespace> CppNamespacePtr;
typedef dbo::ptr<CppNamespace> DboCppNamespacePtr;

enum class RecType
{
  CLASS = 0,
  STRUCT = 1,
  UNION = 2,
  INTERFACE = 3,
  ENUM = 4
};

class CppAttribute;
class CppMethod;

/*
  * This class represents a CppRecordDecl in the database.
  * attributes: - name: name of the record
  *             - type: type of the record (Class, Union, Struct)
*/
class CppRecord
{
public:

  dbo::ptr<CppNamespace> nsp;
  dbo::collection< dbo::ptr<CppAttribute> > attrs;
  dbo::collection< dbo::ptr<CppMethod> > methods;
 
  std::string name;
  RecType type;

  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, name, "name");
    dbo::field(a_, type, "type");

    dbo::belongsTo(a_, nsp, "cppnamespace");

    dbo::hasMany(a_, attrs, dbo::ManyToOne);
    dbo::hasMany(a_, methods, dbo::ManyToOne);
  }

  model::CppNamespacePtr nsptr;
};

typedef std::shared_ptr<CppRecord> CppRecordPtr;
typedef dbo::ptr<CppRecord> DboCppRecordPtr;

} //model
} //umlgen
#endif
