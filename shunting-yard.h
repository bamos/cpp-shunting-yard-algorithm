#ifndef SHUNTING_YARD_H_
#define SHUNTING_YARD_H_

#include <map>
#include <stack>
#include <string>
#include <queue>
#include <list>

enum tokType { NONE, OP, VAR, NUM, STR, MAP };

struct TokenBase {
  tokType type;
  virtual ~TokenBase() {}
  virtual TokenBase* clone() const = 0;
};

template<class T> class Token : public TokenBase {
 public:
  T val;
  Token(T t, tokType type) : val(t) { this->type = type; }
  virtual TokenBase* clone() const {
    return new Token(static_cast<const Token&>(*this));
  }
};

class packToken;
typedef std::queue<TokenBase*> TokenQueue_t;
typedef std::map<std::string, packToken> TokenMap_t;
typedef std::map<std::string, int> OppMap_t;

#include "packToken.h"

struct Scope {
  typedef std::list<TokenMap_t*> Scope_t;
  mutable Scope_t scope;

  Scope() {}
  Scope(TokenMap_t* vars);

  TokenBase* find(std::string key) const;

  void push(TokenMap_t* vars);
  void push(Scope vars);
  void pop();
  void pop(unsigned N);

  void clean();
  unsigned size() const;
};

class calculator {
 private:
  static OppMap_t _opPrecedence;
  static OppMap_t buildOpPrecedence();
  static Scope empty_scope;

 public:
  static packToken calculate(const char* expr, const Scope& scope = empty_scope);

 private:
  static packToken calculate(TokenQueue_t RPN,
                             const Scope* global, const Scope* local);
  static void cleanRPN(TokenQueue_t* rpn);
  static TokenQueue_t toRPN(const char* expr,
                            const Scope* global, const Scope* local,
                            OppMap_t opPrecedence = _opPrecedence);

  static bool handle_unary(const std::string& str,
                           TokenQueue_t* rpnQueue, bool lastTokenWasOp,
                           OppMap_t opPrecedence);
  static void handle_op(const std::string& str,
                        TokenQueue_t* rpnQueue,
                        std::stack<std::string>* operatorStack,
                        OppMap_t opPrecedence);

 private:
  TokenQueue_t RPN;

 public:
  Scope scope;

  ~calculator();
  calculator() {}
  calculator(const calculator& calc);
  calculator(const char* expr, const Scope& scope = empty_scope,
             OppMap_t opPrecedence = _opPrecedence);
  void compile(const char* expr,
               OppMap_t opPrecedence = _opPrecedence);
  void compile(const char* expr,
               const Scope& local = empty_scope,
               OppMap_t opPrecedence = _opPrecedence);
  packToken eval(const Scope& local = empty_scope);

  // Serialization:
  std::string str();

  // Operators:
  calculator& operator = (const calculator& calc);
};

#endif  // SHUNTING_YARD_H_
