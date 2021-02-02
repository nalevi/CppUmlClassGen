#ifndef UMLGEN_GENERATOR_DBSESSION_H
#define UMLGEN_GENERATOR_DBSESSION_H

#include <Wt/Dbo/Dbo.h>

#include <string>

namespace umlgen
{
namespace generator
{

/**
  * This function initializes a database session, creates the database schema
  * if needed.
  * @param dbname_ The name of the database.
  * @param session_ the actual db session.
*/
bool startDbSession(const std::string& dbname_, std::shared_ptr<Wt::Dbo::Session> session_);

} // generator
} // umlgen
#endif
