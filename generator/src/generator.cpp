#include <boost/program_options/options_description.hpp>
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

namespace fs = boost::filesystem;
namespace po = boost::program_options;

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


po::options_description commandLineArgs()
{
  po::options_description desc("Generator options");

  desc.add_options()
    ("help,h",
      "Prints help message.")
    ("object,c", po::value<std::string>()->required(),
      "The name of the object to generate uml class diagramm for.")
    ("workspace,w", po::value<std::string>()->required(),
      "The path to the project root library.")
    ("fileformat,f",
      "The output file format: SVG,TXT, DOT")
    ("database,d",po::value<std::string>()->required(),
      "The database used to store the AST informations,"
      "valid options are PostgreSQL or SQLite. For PostgreSQl"
      "a connection string is needed, for example: "
      "'host=127.0.0.1 user=test password=1234 port5432 dbname=umlgen_test'");

    return desc;
}


/**
 * This function checks the existence of the workspace and project directory
 * based on the given command line arguments.
 * @return Whether the project directory exists or not.
 */
bool checkProjectDir(const po::variables_map& vm_)
{
  const std::string projDir
    = vm_["workspace"].as<std::string>() + '/';

  return fs::is_directory(projDir);
}
  

int main(int argc, const char** argv)
{

  // Proecessing command line arguments
  po::options_description desc = commandLineArgs();
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
    .options(desc).allow_unregistered().run(), vm);

  std::string CONNECTION_STRING("");
  std::string UML_CLASS("");

  if(vm.count("workspace"))
  {
    if(!checkProjectDir(vm))
    {
      std::cerr << "Error: workspace doesn't exist!" << std::endl;
    }
  }

  if(vm.count("database"))
  {
    CONNECTION_STRING = vm["database"].as<std::string>(); 
  }

  if(vm.count("object"))
  {
    UML_CLASS = vm["object"].as<std::string>();
  }


  std::shared_ptr<Wt::Dbo::Session> session_ptr(new Wt::Dbo::Session);

  bool dbSession = umlgen::generator::startDbSession(
    //"host=127.0.0.1 user=test password=1234 port=5432 dbname=umlgen_test",
    //CONNECTION_STRING,
    "umlgen.db",
    session_ptr);
 
  if(dbSession)
  {

    GeneratorActionFactory factory(session_ptr); 

    clang::tooling::CommonOptionsParser OptionParser(argc, argv, GenToolCategory);
    clang::tooling::ClangTool genTool(OptionParser.getCompilations(),
                                    OptionParser.getSourcePathList());
    bool res = genTool.run(&factory);

    return res;
  }
  else {
    return 1;
  }
}
