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

  bool TraverseCXXRecordDecl(clang::CXXRecordDecl* dcl_)
  {
    _classesStack.push(dcl_->getNameAsString());

    bool b = Base::TraverseCXXRecordDecl(dcl_);

    if(!_classesStack.top().empty())
      _classes.push_back(_classesStack.top());
    _classesStack.pop(); 

    for(auto it: _classes)
    {
      std::cout << it << ", ";
    }
    std::cout << std::endl;
    return b;
  }

  bool TraverseCXXMethodDecl(clang::CXXMethodDecl* dcl_)
  {
    _functionStack.push(dcl_->getNameAsString());

    bool b = Base::TraverseCXXMethodDecl(dcl_);

    if(!_functionStack.top().empty())
      _functions.push_back(_functionStack.top());
    _functionStack.pop();

    for(auto it: _functions)
    {
      std::cout << it << ", ";
    } 
    std::cout << std::endl;
    return b;
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
  
  std::vector<std::string> _classes;
  std::stack<std::string> _classesStack;

  std::vector<std::string> _functions;
  std::stack<std::string> _functionStack;
};

} //generator
} //umlgen

#endif
