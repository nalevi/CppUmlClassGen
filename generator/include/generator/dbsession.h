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

// This one doesn't work i think.
/**
  * This function converts std::shared_ptr into Wt::Dbo::ptr. This is neccessary
  * because during the AST traversal, we put shared pointers in stacks (to keep
  * connection information between different node types ), but we need Dbo 
  * pointers to persist the AST nodes to the databse at the end.
  * @param ptr_ the shared pointer which will be converted.
*/
template<class T>
Wt::Dbo::ptr<T> sharedPtrToDboPtr(std::shared_ptr<T> ptr_);

} // generator
} // umlgen
#endif
