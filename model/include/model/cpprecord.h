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
class Namespace
{
public:
  Namespace(const std::string& n_): name(n_) {}

  dbo::collection< dbo::ptr<CppRecord> > cppRecords;

  std::string name;
  
  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, name, "name");

    dbo::hasMany(a_, cppRecords, dbo::ManyToOne, "name");
  } 

  
};

enum class RecType
{
  CLASS = 0,
  STRUCT = 1,
  UNION = 2
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
  CppRecord(const RecType& t_,
         const std::string& n_): type(t_), name(n_) {}
  ~CppRecord();

  dbo::ptr<Namespace> nsp;
  dbo::collection< dbo::ptr<CppAttribute> > attrs;
  dbo::collection< dbo::ptr<CppMethod> > methods;
 
  std::string name;
  RecType type;

  template<class Action>
  void persist(Action& a_)
  {
    dbo::field(a_, name, "name");
    dbo::field(a_, type, "type");

    dbo::belongsTo(a_, nsp, "name");

    dbo::hasMany(a_, attrs, "name");
    dbo::hasMany(a_, methods, "name");
  }
};


} //model
} //umlgen
#endif
