#include "../headers/recursive_decent_calculator.h"

#include <cmath>

// TODO urgently add understandable comments : consider using CFG for corresponding function in the comment as well
// TODO this is the branch check.

void SimpleParser::getNextToken()
{
	if (!backtrack_stack.empty())
	{
		next_token = backtrack_stack.top();
		backtrack_stack.pop();
	}
	else if( !lexer->isEOF)
		next_token = lexer->getNextToken();
}


static bool is_relation_operator(Token* current_token)
{
	if (current_token->token_type >= MIN_RELATION_OPS && current_token->token_type <= MAX_RELATION_OPS)
		return true;

	else return false;
}

static bool evaluate_relation_operator(int l_value, int r_value, int ops)
{
	if ( ops == EE )			// == operator
	{
		if (l_value == r_value)
			return true;
	}
	else if (ops == LE)			// <= opertor
	{
		if (l_value <= r_value)
			return true;

	}
	else if (ops == GE)			// >= operator
	{
		if (l_value >= r_value)
			return true;
	}
	else if (ops == GREATER)	// > operator
	{
		if (l_value > r_value)
			return true;
	}
	else						// < operator
	{
		if (l_value < r_value)
			return true;
	}

	return false;
}

void SimpleParser::program()
{
	getNextToken();
	if (next_token->token_type == START)
	{
		decls();
		stmts();
	}
	else
	{
		std::cout << "Program should begin with START keyword \n";
		return;
	}

	getNextToken();
	if (next_token->token_type == END)
	{
		std::cout << "Parse completed successfully \n";
	}
	else
	{
		std::cout << "Program should terminate with END keyword \n";
		return;
	}
}


void SimpleParser::decls()
{
	getNextToken();
	backtrack_stack.push(next_token);

	if (next_token->token_type != AUTO)
	{
		return;
	}

	decl();
	decls();
}


void SimpleParser::decl()
{
	getNextToken();
	if (next_token->token_type == AUTO)
	{
		getNextToken();
		if (next_token->token_type == ID)
		{
			backtrack_stack.push(next_token);
			ids();

			getNextToken();
			if (next_token->token_type != SEMI_COLON)
			{
				backtrack_stack.push(next_token);
				std::cout << "Expected ; at the end of the declaration \n";
			}
		}
		else
		{
			std::cout << "Expected identifier after AUTO in declarations \n";
		}
	}
}

void SimpleParser::ids()
{
	if (next_token->token_type == ID)
	{
		getNextToken();
		symbol_table.insert(std::pair<std::string, double>(next_token->value, 0.0));

		getNextToken();
		if (next_token->token_type == COMMA)
		{
			getNextToken();
			backtrack_stack.push(next_token);
			ids();
		}

		backtrack_stack.push(next_token);
	}
}


void SimpleParser::stmts(bool condition_status )
{
	getNextToken();
	backtrack_stack.push(next_token);

	//here the condition check should be END && EOF
	if (next_token->token_type != END && !lexer->isEOF && next_token->token_type != E_BRACES)
	{
		stmt(condition_status);
		stmts(condition_status);
	}
	else
	{
		return;
	}
}


void SimpleParser::stmt(bool condition_status )
{
	getNextToken();

	if (next_token->token_type == ID)
	{
		if (symbol_table.find(next_token->value) != symbol_table.end())
		{
			std::string temp = next_token->value;
			getNextToken();

			if (next_token->token_type == EQUAL)
			{
				double result = expr(condition_status);
				getNextToken();

				if (next_token->token_type != SEMI_COLON)
				{
					std::cout << "Error : expect ; after expression \n";
				}

				if (condition_status == true)
				{
					auto it = symbol_table.find(temp);
					it->second = result;
				}
			}
		}
		else
		{
			std::cout << "Error : found use of undeclared variable : " << next_token->value << " \n";
		}
	}
	else if (next_token->token_type == OUT)
	{
		getNextToken();
		if (next_token->token_type == O_PARAM)
		{
			double result = expr(condition_status);

			getNextToken();
			if (next_token->token_type != E_PARAM)
			{
				std::cout << "Error : expected ) at the end of OUT expression \n";
			}

			std::cout << "Value of \""<< out_id_name << "\" : " << result << " \n";
		}
		else
		{
			std::cout << "Error : expected ( after keyword OUT \n";
		}
	}
	else if (next_token->token_type == IF)
	{
		getNextToken();
		if (next_token->token_type == O_PARAM)
		{
			double result1 = expr(condition_status);

			getNextToken();
			if (is_relation_operator(next_token))
			{
				// store the token type, as below call to expr will change the tocken
				int relation_op = next_token->token_type;

				// right side of the relation expression.
				double result2 = expr(condition_status);

				getNextToken();

				// end paranthasis of IF condition when both E1 and E2 are present
				if (next_token->token_type == E_PARAM)
				{
					// compare the return values of the expression here
					// if the comparison success then parse the statement block of the IF statement.
					if ( evaluate_relation_operator(result1, result2, relation_op) )
					{
						block(true && condition_status);
					}
					else
					{
						block(false && condition_status);
					}
				}	
			}
			else if(next_token->token_type == E_PARAM) // end paranthasis of IF condition if only one E is present
			{
				// only one expression present in the if condition
				// In this case if the expression return value is > 0 then if condition passes;
				if (result1 > 0)
				{
					block(true && condition_status);
				}
				else
				{
					block(false && condition_status);
				}
			}
			else
			{
				std::cout << "Error : expected relation operator in the IF statement \n";
			}
		}
		else
		{
			std::cout << "Error : expected ( after keyword IF \n";
		}
	} // end IF statement
	else if (next_token->token_type == WHILE)
	{	
		//handling while with current implementation is pretty hard. 
		getNextToken();
		if (next_token->token_type == O_PARAM)
		{
			double result1 = expr(condition_status);

			getNextToken();
			if (is_relation_operator(next_token))
			{
				// store the token type, as below call to expr will change the tocken
				int relation_op = next_token->token_type;

				// right side of the relation expression.
				double result2 = expr(condition_status);

				getNextToken();

				// end paranthasis of IF condition when both E1 and E2 are present
				if (next_token->token_type == E_PARAM)
				{
					// compare the return values of the expression here
					// if the comparison success then parse the statement block of the IF statement.
					if (evaluate_relation_operator(result1, result2, relation_op))
					{
						block(true && condition_status);
					}
					else
					{
						block(false && condition_status);
					}
				}
			}
			else if (next_token->token_type == E_PARAM) // end paranthasis of IF condition if only one E is present
			{
				// only one expression present in the if condition
				// In this case if the expression return value is > 0 then if condition passes;
				if (result1 > 0)
				{
					block(true && condition_status);
				}
				else
				{
					block(false && condition_status);
				}
			}
			else
			{
				std::cout << "Error : expected relation operator in the IF statement \n";
			}
		}
		else
		{
			std::cout << "Error : expected ( after keyword IF \n";
		}
	} // end WHILE statement
	else
	{
		backtrack_stack.push(next_token);
		expr(condition_status);
		getNextToken();
		if (next_token->token_type != SEMI_COLON)
		{
			std::cout << "Error : expect ; after expression \n";
		}
	}
}

// evaluation of condition in the if statement is give as inherited attribute for this.
void SimpleParser::block( bool condition_status )
{
	// check the begining of the if block by open { curly braces
	getNextToken();
	if (next_token->token_type != O_BRACES)
	{
		std::cout << "Error : expect { at the begining of the IF block \n";
	}

	stmts( condition_status );

	// check the end of the if block by closing } curly braces
	getNextToken();
	if (next_token->token_type != E_BRACES)
	{
		std::cout << "Error : expect } at the end of the IF block \n";
	}
}

double SimpleParser::expr(bool condition_status )
{
	double t_return = term(1);
	double e_dash_return = expr_(t_return);
	return e_dash_return;
}

double SimpleParser::expr_(double inherited, bool condition_status )
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == PLUS)
		{
			double t_return = term(1);
			double e_dash_inherited = inherited + t_return;

			return expr_(e_dash_inherited);
		}
		else if (next_token->token_type == MINUS)
		{
			double t_return = term(1);
			double e_dash_inherited = inherited - t_return;

			return expr_(e_dash_inherited);
		}
		else if (next_token->token_type == EMPTY)
		{
			return inherited;
			eol = true;
		}
		else
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
	}
	return inherited;
}

double SimpleParser::term(double inherited, bool condition_status )
{
	double f_return = factor(inherited);
	double t_dash_return = term_(f_return);
	return t_dash_return;
}

double SimpleParser::term_(double inherited, bool condition_status )
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == MUL)
		{
			double f_return = factor(inherited);
			double t_dash_inherited = inherited * f_return;

			return term_(t_dash_inherited);
		}
		if (next_token->token_type == DIV)
		{
			double f_return = factor(inherited);
			double t_dash_inherited = inherited;

			if (f_return != 0)
				t_dash_inherited = inherited / f_return;

			return term_(t_dash_inherited);
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return inherited;
		}
		else
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
	}
	return inherited;
}

double SimpleParser::factor(double inherited, bool condition_status )
{
	double p_return = power(inherited);
	double t_dash_return = factor_(p_return);
	return t_dash_return;
}

double SimpleParser::factor_(double inherited, bool condition_status )
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == POW)
		{
			double p_return = power(inherited);
			double f_dash_inherited = std::pow(inherited, p_return);

			return factor_(f_dash_inherited);
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return inherited;
		}
		else
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
	}
	return inherited;
}

double SimpleParser::power(double inherited, bool condition_status )
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == NUM)
		{
			return atof(next_token->value.c_str());
		}
		else if (next_token->token_type == O_PARAM)
		{
			double result = expr(condition_status);
			getNextToken();
			if (!next_token->token_type == E_PARAM)
			{
				std::cout << "found an error : could not find the end paranthasis \n";
			}
			return result;
		}
		else if (next_token->token_type == ID)
		{
			auto it = symbol_table.find(next_token->value);
			if (it != symbol_table.end())
			{
				out_id_name = it->first;
				return it->second;
			}
			else
			{
				std::cout << "Error : found use of undeclared variable" << next_token->value << " \n";
			}
		}
		else if (next_token->token_type == END || next_token->token_type == SEMI_COLON)
		{
			backtrack_stack.push(next_token);
			return inherited;
		}
		else if (next_token->token_type == EMPTY)
		{
			eol = true;
			return 0;
		}
		else
		{
			std::cout << "found an error string : " << next_token->value << "\n";
			return 0;
		}
	}
}

void SimpleParser::parse()
{
	//int return_val = expr();
	program();

	//std::cout << "FINAL CALCULATED VALUE : " << return_val << std::endl;
}