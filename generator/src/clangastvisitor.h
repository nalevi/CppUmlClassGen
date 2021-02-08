#ifndef UMLGEN_CLANGASTVISITOR_H
#define UMLGEN_CLANGASTVISITOR_H

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
    std::cout << "Found namespace: " << dcl_->getQualifiedNameAsString()
      << std::endl;

    model::CppNamespacePtr nsptr = _namespacesStack.top();
 
    nsptr->name = dcl_->getNameAsString();
    
    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl* dcl_) 
  { 
    clang::DeclContext* dcontext = dcl_->getParent();

    if(!dcontext->isStdNamespace())
    {
        std::cout << "Found class: "
                  << dcl_->getQualifiedNameAsString()
                  << std::endl;

        std::cout << "FieldDecls: "; 

        for(auto it = dcl_->field_begin(); it != dcl_->field_end(); ++it)
        {
           clang::AccessSpecifier visibility = it->getAccess();

           std::cout <<  it->getNameAsString()
                     << "(" << (visibility == clang::AS_private
                               ? "private " : visibility == clang::AS_protected
                                              ? "protected " : "public " )
                     << it->getType().getAsString()
                     << ")" << ", ";
        }
    
        std::cout << std::endl; 
    }

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
    if(!dcl_->getDeclContext()->isStdNamespace())
    {
        clang::CXXRecordDecl* cppClass = dcl_->getParent();
        std::cout << "Found CXXMethod!" << std::endl;

        if(dcl_->isVirtual())
        {
          std::cout << "Virtual method!" << std::endl;
        }

        std::cout << "Parent class: "
                  << cppClass->getQualifiedNameAsString() 
                  << std::endl; 

        std::cout << "Name: "
                  << dcl_->getNameInfo().getAsString()
                  << std::endl;

        std::cout << "Return type: "
                  << dcl_->getReturnType().getAsString()
              << std::endl;
     
        std::cout << std::endl;
    }
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
