#include <memory>

#include <Wt/Dbo/backend/Postgres.h>
#include <Wt/Dbo/Dbo.h>

#include <generator/dbsession.h> 


namespace dbo = Wt::Dbo;

namespace umlgen
{
namespace generator
{

bool startDbSession(const std::string& dbname)
{
  std::unique_ptr<dbo::backend::Postgres> postgres{new dbo::backend::Postgres(dbname)};
  dbo::Session session;
  session.setConnection(std::move(postgres));


  return true;
}

} //generator
} // umlgen
