#ifndef UMLGEN_CLANGASTVISITOR_H
#define UMLGEN_CLANGASTVISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>
#include <iostream>

namespace umlgen
{
namespace generator
{

class ClangASTVisitor
  : public clang::RecursiveASTVisitor<ClangASTVisitor> {
public:
  explicit ClangASTVisitor(clang::ASTContext *Context)
    : Context(Context) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration) { 
    std::cout << "Found class: "
              << Declaration->getQualifiedNameAsString()
              << std::endl;

    std::cout << std::endl; 
    return true;
    }

  bool VisitCXXMethodDecl(clang::CXXMethodDecl *dcl_) {
    clang::CXXRecordDecl *cppClass = dcl_->getParent();
    std::cout << "Found CXXMethod!" << std::endl;

    if(dcl_->isVirtual()) {
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
  clang::ASTContext *Context;
};

} //generator
} //umlgen

#endif
