#ifndef OPERANDS_HPP
#define OPERANDS_HPP

using namespace std;

#include <string>

#include "types.hpp"

typedef struct s_operand
{
	string name;
	string operation; //uma operacao aritmetica ou \0
	string aritOperand; //o operando da operacao aritmetica
	string addressingCode;	//codigo binario do modo de enderecamento
	string value;
	e_type type;
	e_type aritOperandType; //tipo do operando da operacao aritmetica, se houver
	bool relative;
}t_operand;

#include "numbers.hpp"
#include "labels.hpp"
#include "defs.hpp"

class Operands
{
	public:

	/**
	  * resolve a operacao aritmetica presente no operando, escrevendo o valor em o->value
	  * retorna o resultado
	  */
	static void solveOperation(t_operand *o,Labels labels,t_status *status);

	Operands(list<t_operand>);

	/**
	  * retorna o proximo operando do tipo dado
	  */
	t_operand getNextOperand(e_type type);

	/**
	  * retorna o n-esimo operando do tipo dado
	  */
	t_operand getOperandIndex(e_type type, unsigned int index);

	/**
	  * resolves the matched operand, solving any operations and writing the results in op
	  */
	static void solveMatch(struct s_match m, t_operand *op, Labels labels, t_status *status);

	private:
	list<t_operand> operands;
	list<t_operand>::iterator itMode;
	list<t_operand>::iterator itReg;
	list<t_operand>::iterator itAddr;
};

#endif // OPERANDS_HPP

