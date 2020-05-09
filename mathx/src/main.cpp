#include <iostream>

#include "../headers/simple_lexer.h"
#include "../headers/recursive_decent_calculator.h"

int main()
{
	SimpleLexer my_lexer("C:\\Projects\\mathx\\mathx\\tests\\recursive_decent_calculator.txt");;
	SimpleParser parser(&my_lexer);

	parser.parse();

	return 0;
}