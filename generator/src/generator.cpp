#include <clang/Frontend/FrontendAction.h>
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
  explicit ClangASTConsumer(
    clang::ASTContext* ctx_,
    Wt::Dbo::Session& dbsession_)
    : _visitor(ctx_, dbsession_), _dbsession(dbsession_) {}

  virtual void HandleTranslationUnit(clang::ASTContext& ctx_)
  {
    _visitor.TraverseDecl(ctx_.getTranslationUnitDecl());
  }
private:
  umlgen::generator::ClangASTVisitor _visitor;

  Wt::Dbo::Session _dbsession;
};

class GeneratorActionFactory : public clang::tooling::FrontendActionFactory 
{
public:
  GeneratorActionFactory(Wt::Dbo::Session& dbsession_)
  {
    _dbsession = dbsession_;
  }

  std::unique_ptr<clang::FrontendAction> create() override
  {
    return std::make_unique<MyGenFrontendAction>(_dbsession);
  }

private:
  class MyGenFrontendAction : public clang::ASTFrontendAction
  {
    friend class GeneratorActionFactory;
  public:
    MyGenFrontendAction(Wt::Dbo::Session& dbsession_): _dbsession(dbsession_) {} 

    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance& Compiler, llvm::StringRef InFile)
    {
      return std::unique_ptr<clang::ASTConsumer>(
          new ClangASTConsumer(&Compiler.getASTContext(), _dbsession));
    }

  private:
    Wt::Dbo::Session _dbsession;
  };

  Wt::Dbo::Session _dbsession;

};

static llvm::cl::OptionCategory GenToolCategory("generator options");
static llvm::cl::extrahelp CommonHelp(
                           clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");


int main(int argc, const char** argv)
{
  Wt::Dbo::Session session;
  // TODO: get the connection string from commandline argument
  bool dbSession = umlgen::generator::startDbSession(
    "host=127.0.0.1 user=test password=1234 port=5432 dbname=umlgen",
    session);
 
  GeneratorActionFactory factory(session); 

  clang::tooling::CommonOptionsParser OptionParser(argc, argv, GenToolCategory);
  clang::tooling::ClangTool genTool(OptionParser.getCompilations(),
                                    OptionParser.getSourcePathList());
  return genTool.run(&factory);
         //clang::tooling::newFrontendActionFactoryctionFactory<GeneratorActionFactory>()
         //              .get());
}
