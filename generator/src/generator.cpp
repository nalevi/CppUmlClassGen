#include <iostream>
#include <sstream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <clang/AST/ASTConsumer.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/Support/CommandLine.h>

#include "clangastvisitor.h" 

//namespace fs = boost::filesystem;

class ClangASTConsumer : public clang::ASTConsumer 
{
public:
  explicit ClangASTConsumer(clang::ASTContext* ctx_)
    : _visitor(ctx_) {}

  virtual void HandleTranslationUnit(clang::ASTContext& ctx_)
  {
    _visitor.TraverseDecl(ctx_.getTranslationUnitDecl());
  }
private:
  umlgen::generator::ClangASTVisitor _visitor;
};

class GeneratorFrontendAction : public clang::ASTFrontendAction
{
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance& Compiler, llvm::StringRef InFile)
  {
    return std::unique_ptr<clang::ASTConsumer>(
        new ClangASTConsumer(&Compiler.getASTContext()));
  }
};

static llvm::cl::OptionCategory GenToolCategory("generator options");
static llvm::cl::extrahelp CommonHelp(
                           clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char** argv)
{

  clang::tooling::CommonOptionsParser OptionParser(argc, argv, GenToolCategory);
  clang::tooling::ClangTool genTool(OptionParser.getCompilations(),
                                    OptionParser.getSourcePathList());
  return genTool.run(
         clang::tooling::newFrontendActionFactory<GeneratorFrontendAction>()
                        .get());
}
