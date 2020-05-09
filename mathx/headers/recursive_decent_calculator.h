#pragma once

// This is the most simple parser which calculate mathamatical expressions
// only INTEGERS *, +, (, ) symbols are allowed

#include <stack>
#include <map>
#include "../headers/simple_lexer.h"

class SimpleParser {
	SimpleLexer* lexer;

	int factor(int inherited);
	int factor_(int inherited);
	int term(int inherited);
	int term_(int inherited);
	int expr();
	int expr_(int inherited);
	int power(int inherited);

	void program();
	void decls();
	void decl();

	void stmts();
	void stmt();

	void getNextToken();

	Token* next_token = nullptr;

	std::stack<Token*> backtrack_stack;

	//store the identifier and current value of that identifier
	std::map<std::string, double> symbol_table;

	std::string out_id_name;

	bool eol = false;

public:

	SimpleParser(SimpleLexer* _lexer) : lexer(_lexer)
	{}

	void parse();
};