#include "../headers/recursive_decent_calculator.h"

#include <cmath>


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
			symbol_table.insert(std::pair<std::string, double>(next_token->value, 0.0));
			getNextToken();

			if (next_token->token_type != SEMI_COLON)
			{
				backtrack_stack.push(next_token);
				std::cout << "Expected ; after the declaration \n";
			}
		}
		else
		{
			std::cout << "Expected identifier after AUTO in declarations \n";
		}
	}
}


void SimpleParser::stmts()
{
	getNextToken();
	backtrack_stack.push(next_token);

	//here the condition check should be END && EOF
	if (next_token->token_type != END && !lexer->isEOF )
	{
		stmt();
		stmts();
	}
	else
	{
		return;
	}
}


void SimpleParser::stmt()
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
				double result = expr();
				getNextToken();

				if (next_token->token_type != SEMI_COLON)
				{
					std::cout << "Error : expect ; after expression \n";
				}

				auto it = symbol_table.find(temp);
				it->second = result;
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
			int result = expr();

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
	else
	{
		backtrack_stack.push(next_token);
		expr();
		getNextToken();
		if (next_token->token_type != SEMI_COLON)
		{
			std::cout << "Error : expect ; after expression \n";
		}
	}
}


int SimpleParser::expr()
{
	int t_return = term(1);
	int e_dash_return = expr_(t_return);
	return e_dash_return;
}

int SimpleParser::expr_(int inherited)
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == PLUS)
		{
			int t_return = term(1);
			int e_dash_inherited = inherited + t_return;

			return expr_(e_dash_inherited);
		}
		else if (next_token->token_type == MINUS)
		{
			int t_return = term(1);
			int e_dash_inherited = inherited - t_return;

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

int SimpleParser::term(int inherited)
{
	int f_return = factor(inherited);
	int t_dash_return = term_(f_return);
	return t_dash_return;
}

int SimpleParser::term_(int inherited)
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == MUL)
		{
			int f_return = factor(inherited);
			int t_dash_inherited = inherited * f_return;

			return term_(t_dash_inherited);
		}
		if (next_token->token_type == DIV)
		{
			int f_return = factor(inherited);
			int t_dash_inherited = inherited;

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

int SimpleParser::factor(int inherited)
{
	int p_return = power(inherited);
	int t_dash_return = factor_(p_return);
	return t_dash_return;
}

int SimpleParser::factor_(int inherited)
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == POW)
		{
			int p_return = power(inherited);
			int f_dash_inherited = std::pow(inherited, p_return);

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

int SimpleParser::power(int inherited)
{
	if (!eol)
	{
		getNextToken();

		if (next_token->token_type == NUM)
		{
			return atoi(next_token->value.c_str());
		}
		else if (next_token->token_type == O_PARAM)
		{
			int result = expr();
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