#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <clang/AST/ASTConsumer.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

#include <llvm/Support/CommandLine.h>

#include <generator/dbsession.h>

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
  // TODO: get the connection string from commandline argument
  bool dbSession = umlgen::generator::startDbSession(
    "host=127.0.0.1 user=test password=1234 port=5432 dbname=umlgen");
 
  clang::tooling::CommonOptionsParser OptionParser(argc, argv, GenToolCategory);
  clang::tooling::ClangTool genTool(OptionParser.getCompilations(),
                                    OptionParser.getSourcePathList());
  return genTool.run(
         clang::tooling::newFrontendActionFactory<GeneratorFrontendAction>()
                        .get());
}
