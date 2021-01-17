#ifndef UMLGEN_CLANGASTVISITOR_H
#define UMLGEN_CLANGASTVISITOR_H

#include <iostream>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/Specifiers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

namespace umlgen
{
namespace generator
{

class ClangASTVisitor
  : public clang::RecursiveASTVisitor<ClangASTVisitor> 
{
public:
  explicit ClangASTVisitor(clang::ASTContext* ctx_)
    : _ctx(ctx_) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl* dcl_) 
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
    return true;
    }

  bool VisitCXXMethodDecl(clang::CXXMethodDecl* dcl_) 
  {
    clang::CXXRecordDecl* cppClass = dcl_->getParent();
    std::cout << "Found CXXMethod!" << std::endl;

    if(dcl_->isVirtual())
    {
      std::cout << "Virtual method!" << std::endl;
    }

    std::cout << "Parent class: "
              << cppClass->getNameAsString() 
              << std::endl; 

    std::cout << "Name: "
              << dcl_->getNameInfo().getAsString()
              << std::endl;

    std::cout << "Return type: "
              << dcl_->getReturnType().getAsString()
              << std::endl;
    
    std::cout << std::endl;
    return true;
  }


private:
  clang::ASTContext* _ctx;
};

} //generator
} //umlgen

#endif
