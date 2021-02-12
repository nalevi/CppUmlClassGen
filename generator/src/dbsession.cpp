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
  postgres->setProperty("show-queries","true");
  session_->setConnection(std::move(postgres));

  session_->mapClass<model::CppNamespace>("cppnamespace");
  session_->mapClass<model::CppRecord>("cpprecord");
  session_->mapClass<model::CppAttribute>("cppattribute");
  session_->mapClass<model::CppMethod>("cppmethod");
  session_->mapClass<model::CppMethodParam>("cppmethodparam");


  // Tries to create tables, if they already exists, it fails.
  try
  {
    std::cout << "Creating database scheme..." << std::endl;
    session_->createTables(); 
  }
  catch(dbo::Exception& ex)
  {
    std::cerr << "SQLERROR code: " << ex.code() << std::endl;
    return false;
  }
  catch (...)
  {
    std::cout << "Skipping creating database scheme: already exists!" << std::endl;
  }

  return true;
}

template<class T>
dbo::ptr<T> sharedPtrToDboPtr(std::shared_ptr<T> ptr_)
{
  dbo::ptr<T> dboptr = ptr_.get();
  return dboptr;
}

} //generator
} // umlgen
