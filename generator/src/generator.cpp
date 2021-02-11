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
    std::shared_ptr<Wt::Dbo::Session> dbsession_)
    : _visitor(ctx_, dbsession_), _dbsession(dbsession_) {}

  virtual void HandleTranslationUnit(clang::ASTContext& ctx_)
  {
    _visitor.TraverseDecl(ctx_.getTranslationUnitDecl());
  }
private:
  umlgen::generator::ClangASTVisitor _visitor;

  std::shared_ptr<Wt::Dbo::Session> _dbsession;
};

class GeneratorActionFactory : public clang::tooling::FrontendActionFactory 
{
public:
  GeneratorActionFactory(std::shared_ptr<Wt::Dbo::Session> dbsession_)
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
    MyGenFrontendAction(std::shared_ptr<Wt::Dbo::Session> dbsession_) 
    {
      _dbsession = dbsession_;
    } 

    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
      clang::CompilerInstance& Compiler, llvm::StringRef InFile)
    {
      return std::unique_ptr<clang::ASTConsumer>(
          new ClangASTConsumer(&Compiler.getASTContext(), _dbsession));
    }

  private:
    std::shared_ptr<Wt::Dbo::Session> _dbsession;
  };

  std::shared_ptr<Wt::Dbo::Session> _dbsession;

};

static llvm::cl::OptionCategory GenToolCategory("generator options");
static llvm::cl::extrahelp CommonHelp(
                           clang::tooling::CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMore help text...\n");


int main(int argc, const char** argv)
{
  std::shared_ptr<Wt::Dbo::Session> session_ptr(new Wt::Dbo::Session);

  // TODO: get the connection string from commandline argument
  bool dbSession = umlgen::generator::startDbSession(
    "host=127.0.0.1 user=test password=1234 port=5432 dbname=umlgen",
    session_ptr);
 
  GeneratorActionFactory factory(session_ptr); 

  clang::tooling::CommonOptionsParser OptionParser(argc, argv, GenToolCategory);
  clang::tooling::ClangTool genTool(OptionParser.getCompilations(),
                                    OptionParser.getSourcePathList());
  bool res = genTool.run(&factory);

  return res;
}
