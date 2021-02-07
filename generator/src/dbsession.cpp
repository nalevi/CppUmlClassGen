#include <memory>

#include <Wt/Dbo/backend/Postgres.h>
#include <Wt/Dbo/Exception.h>

#include <model/cppmethod.h>
#include <model/cpprecord.h>
#include <model/cppattribute.h>
#include <model/visibility.h>

#include <generator/dbsession.h> 

namespace dbo = Wt::Dbo;

namespace umlgen
{
namespace generator
{

bool startDbSession(const std::string& dbname_, std::shared_ptr<dbo::Session> session_)
{
  std::unique_ptr<dbo::backend::Postgres> postgres{
    new dbo::backend::Postgres(dbname_)};
  session_->setConnection(std::move(postgres));

  session_->mapClass<model::CppNamespace>("cppnamespace");
  session_->mapClass<model::CppRecord>("cpprecord");
  session_->mapClass<model::CppAttribute>("cppattribute");
  session_->mapClass<model::CppMethod>("cppmethod");
  session_->mapClass<model::CppMethodParam>("cppmethodparam");

  // Tries to create tables, if they already exists, it fails.
  try
  {
    session_->createTables(); 
  }
  catch (...)
  {
    std::cout << "Skipping creating database scheme: already exists!" << std::endl;
    //session.dropTables();
    //session.createTables();
  }

  return true;
}

template<class T>
Wt::Dbo::ptr<T> sharedPtrToDboPtr(std::shared_ptr<T> ptr_)
{
  Wt::Dbo::ptr<T> dboptr = ptr_.get();
  return dboptr;
}

} //generator
} // umlgen
