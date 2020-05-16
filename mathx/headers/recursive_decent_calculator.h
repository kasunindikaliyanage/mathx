#pragma once

// This is the most simple parser which calculate mathamatical expressions
// But it is getting grow day by day.

// TODO 1. Change the type of the variables to double and handle calculations as doubles for more precision.	- DONE
// TODO 2. Add usfull functions add, sub, pow, root, log etc.													- 
// TODO 3. Addition of multi-dimention arrays, printing of multi-dimension arrays.								-
// TODO 4. Add while loops.																						- NOT FEASIBLE WITH CURRENT IMPLEMENTATION
// TODO 5. Addition of vec and mat types and matrix related calculations.										-
// TODO 6. Make parser can evaluate and report all errors, with error recovery mechanism.						-
// TODO 7. Parallel implementation of matrix calculations.														-
// TODO 8. Add &&, || operations.
// TODO 9. Division by zero exception.

#include <stack>
#include <map>
#include "../headers/simple_lexer.h"

class SimpleParser {
	SimpleLexer* lexer;

	double factor(double inherited, bool condition_status = true );
	double factor_(double inherited, bool condition_status = true);
	double term(double inherited, bool condition_status = true);
	double term_(double inherited, bool condition_status = true);
	double expr(bool condition_status = true);
	double expr_(double inherited, bool condition_status = true);
	double power(double inherited, bool condition_status = true);

	void program();
	void decls();
	void decl();
	void ids();

	void block(bool condition_status = true);

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