#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "object.h"
#include "strobj.h"
#include "integer.h"
#include "canmsg.h"
#include "ast.h"

#include <map>
#include <string>
#include <set>

extern std::set<std::string> global_var_keys;

enum TableType_t
{
  routine_table,
  test_table,
  generic_table,
};


class SymbolTable
{
public:
  SymbolTable(Node * root = 0);
  virtual ~SymbolTable() { 
    symbols.clear(); 
    if (root)
      delete root; 
  };
  void setRetval(Object * o);
  void setObject(std::string key, Object * o);
  bool insert(std::string key, Object * o);
  bool setGlobal(std::string key, Object * o);
  
  obj_t objectType(std::string key) { return getObject(key)->type(); };

  Object * getObject(std::string key);
  Node * getRoot() { return root; };

  virtual TableType_t type() {return generic_table;};
  friend class Perterpreter;

protected:
  std::map<std::string, Object *> symbols;
  SymbolTable * globals;

  Node * root;

private:
  void initGlobals() 
  {  
    symbols = {
                {"SERIAL_LOG_FILE", new String()},
                {"RETVAL", new Object()},
                {"LOG_FILE", new String()},
                {"VERBOSE", new Integer(0)},
                {"SERIAL_DEVICE", new String()},
                {"GPIO_DEVICE", new String()}
              };
  }
  void setReadOnlyVar(std::string key, Object * o);
};



class Routine : public SymbolTable
{
public:
  Routine(Node * root = 0, SymbolTable * globes = 0);
  virtual ~Routine() { };
  virtual TableType_t type() {return routine_table;};
};



class Test : public SymbolTable
{ 
public:
  Test(Node * root = 0, SymbolTable * globes = 0);
  virtual ~Test() { };
  virtual TableType_t type() {return test_table;};
  bool testPassed() {return passed;};
  void passTest() {passed = true;};
  void failTest() {passed = false;};

private:
  bool passed = false;

};





class Routines
{
public:
  Routines() {};
  virtual ~Routines() { routines.clear(); };

  Routine * getRoutine(std::string key, int lineno = 0);
  Routine * addRoutine(Node * node, SymbolTable * global);
private:
  std::map<std::string, Routine *> routines;
};



class Tests
{
public:
  Tests() {};
  virtual ~Tests() { tests.clear(); };

  Test * addTest(Node * node, SymbolTable * global);
  Test * getTest(std::string key, int lineno = 0);
  bool hasTest(std::string key);

private:
  std::map<std::string, Test *> tests;
};


#endif
