#include <memory>

#include <Wt/Dbo/backend/Postgres.h>
#include <Wt/Dbo/Dbo.h>

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

/**
  * This function initializes a database session, creates the database schema
  * if needed.
  * @param dbname_ The name of the database.
*/
bool startDbSession(const std::string& dbname_)
{
  std::unique_ptr<dbo::backend::Postgres> postgres{new dbo::backend::Postgres(dbname_)};
  dbo::Session session;
  session.setConnection(std::move(postgres));

  session.mapClass<model::CppNamespace>("cppnamespace");
  session.mapClass<model::CppRecord>("cpprecord");
  session.mapClass<model::CppAttribute>("cppattribute");
  session.mapClass<model::CppMethod>("cppmethod");
  session.mapClass<model::CppMethodParam>("cppmethodparam");

  // Tries to create tables, if they already exists, it fails.
  session.createTables();

  return true;
}

} //generator
} // umlgen
