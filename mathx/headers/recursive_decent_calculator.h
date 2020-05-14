#pragma once

// This is the most simple parser which calculate mathamatical expressions
// only INTEGERS *, +, (, ) symbols are allowed

#include <stack>
#include <map>
#include "../headers/simple_lexer.h"

class SimpleParser {
	SimpleLexer* lexer;

	int factor(int inherited, bool condition_status = true );
	int factor_(int inherited, bool condition_status = true);
	int term(int inherited, bool condition_status = true);
	int term_(int inherited, bool condition_status = true);
	int expr(bool condition_status = true);
	int expr_(int inherited, bool condition_status = true);
	int power(int inherited, bool condition_status = true);

	void program();
	void decls();
	void decl();
	void ids();

	void if_block(bool condition_status = true);

	void stmts(bool condition_status = true );
	void stmt(bool condition_status = true );

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