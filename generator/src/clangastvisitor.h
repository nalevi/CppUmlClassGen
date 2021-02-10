#ifndef UMLGEN_CLANGASTVISITOR_H
#define UMLGEN_CLANGASTVISITOR_H

#include "model/visibility.h"
#include <iostream>
#include <vector>
#include <stack>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Specifiers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

#include <model/cpprecord.h>
#include <model/cppmethod.h>
#include <model/cppattribute.h>

#include <generator/dbsession.h>

namespace umlgen
{
namespace generator
{

class ClangASTVisitor
  : public clang::RecursiveASTVisitor<ClangASTVisitor> 
{
public:
  explicit ClangASTVisitor(
    clang::ASTContext* ctx_, 
    std::shared_ptr<Wt::Dbo::Session> dbsession_)
    : _ctx(ctx_), _dbsession(dbsession_) {}

  ~ClangASTVisitor()
  {
    dbo::Transaction transaction{*_dbsession.get()};

    // committing namesapces
    for(auto it: _namespaces)
    {
      model::DboCppNamespacePtr nsptr = _dbsession
        ->add(std::unique_ptr<model::CppNamespace>{new model::CppNamespace()}); 

      nsptr.modify()->name = it->name;
    }

    // committing cpprecords
    for(auto it: _types)
    {
      model::DboCppRecordPtr record = _dbsession
        ->add(std::unique_ptr<model::CppRecord>{new model::CppRecord()});
      record.modify()->name = it->name;

      model::DboCppNamespacePtr nsp = _dbsession->find<model::CppNamespace>()
        .where("name = ?").bind(it->nsptr->name);
      record.modify()->type = it->type;
      record.modify()->nsp = nsp; 

    }

    for(auto it: _methods)
    {
      model::DboCppMethodPtr method = _dbsession
        ->add(std::unique_ptr<model::CppMethod>{new model::CppMethod()});

      method.modify()->name = it->name;
      method.modify()->isVirtual = it->isVirtual;
      method.modify()->returnType = it->returnType;
      method.modify()->visibility = it->visibility;
     
      model::DboCppRecordPtr rec = _dbsession->find<model::CppRecord>()
        .where("name = ?").bind(it->cpprecptr->name);
      method.modify()->cpprec = rec;
    }
  }

  bool TraverseNamespaceDecl(clang::NamespaceDecl* dcl_)
  {
    _namespacesStack.push(std::make_shared<model::CppNamespace>());

    bool b = Base::TraverseNamespaceDecl(dcl_);
    
    if(_namespacesStack.top()->name.empty())
    {
      _namespaces.push_back(_namespacesStack.top());
    }

    _namespacesStack.pop();

    return b;
  }

  bool TraverseCXXRecordDecl(clang::CXXRecordDecl* dcl_)
  {
    _typesStack.push(std::make_shared<model::CppRecord>());

    bool b = Base::TraverseCXXRecordDecl(dcl_);

    if(!_typesStack.top()->name.empty())
    {
      _types.push_back(_typesStack.top());
    }
    _typesStack.pop(); 
    
    return b;
  }

  bool TraverseCXXMethodDecl(clang::CXXMethodDecl* dcl_)
  {
    _methodstack.push(std::make_shared<model::CppMethod>());

    bool b = Base::TraverseCXXMethodDecl(dcl_);

    if(!_methodstack.top()->name.empty())
    {
      _methods.push_back(_methodstack.top());
    }
    _methodstack.pop();

    return b;
  }

  bool VisitNamespaceDecl(clang::NamespaceDecl* dcl_)
  {
    model::CppNamespacePtr nsptr = _namespacesStack.top();
 
    nsptr->name = dcl_->getNameAsString();
    
    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl* dcl_) 
  { 
    //getting the current namespace node
    model::CppNamespacePtr nsptr = _namespacesStack.top();

    //getting the actual cpprecord
    model::CppRecordPtr recptr = _typesStack.top();

    clang::TagTypeKind tagKind = dcl_->getTagKind();

    recptr->name = dcl_->getNameAsString();
    recptr->nsptr = nsptr; 
    recptr->type = tagKind == clang::TagTypeKind::TTK_Class ?
      model::RecType::CLASS : 
        (tagKind == clang::TagTypeKind::TTK_Struct ?
          model::RecType::STRUCT : (tagKind == clang::TagTypeKind::TTK_Union ?
            model::RecType::UNION : (tagKind == clang::TagTypeKind::TTK_Enum ?
              model::RecType::ENUM : model::RecType::INTERFACE))); 
    
    return true;
  }

  bool VisitCXXMethodDecl(clang::CXXMethodDecl* dcl_) 
  {
    // the currentyl iterated cpprecord node
    model::CppRecordPtr rec = _typesStack.top();

    // the current cxxmethoddecl 
    model::CppMethodPtr method = _methodstack.top();
    clang::AccessSpecifier vis = dcl_->getAccess();

    method->name = dcl_->getNameAsString();
    method->isVirtual = dcl_->isVirtual();
    method->returnType = dcl_->getReturnType().getAsString();
    method->visibility = vis == clang::AccessSpecifier::AS_private ?
      model::Visibility::PRIVATE : ( vis == clang::AccessSpecifier::AS_protected 
        ? model::Visibility::PROTECTED : model::Visibility::PUBLIC);
    std::cout << "** SETTING cpprecptr **" << std::endl;
    method->cpprecptr = rec;
    
    return true;
  }


private:
  using Base = clang::RecursiveASTVisitor<ClangASTVisitor>;
  clang::ASTContext* _ctx;

  std::shared_ptr<Wt::Dbo::Session> _dbsession; 
  
  std::vector<model::CppRecordPtr> _types;
  std::stack<model::CppRecordPtr> _typesStack;

  std::vector<model::CppMethodPtr> _methods;
  std::stack<model::CppMethodPtr> _methodstack;
  std::vector<model::CppMethodParamPtr> _methodparams;

  std::vector<model::CppAttributePtr> _attributes;

  std::stack<model::CppNamespacePtr> _namespacesStack;
  std::vector<model::CppNamespacePtr> _namespaces;
};

} //generator
} //umlgen

#endif
