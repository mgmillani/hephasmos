#include <stdio.h>

#include <list>

#include "operands.hpp"
#include "labels.hpp"
#include "numbers.hpp"
#include "expression.hpp"

#include "debug.hpp"
#include "defs.hpp"

using namespace std;

/**
  * resolve a operacao aritmetica presente no operando, escrevendo o valor em o->value
  */
void Operands::solveOperation(t_operand *op,Labels labels,t_status *status)
{
	if(op->operation.compare("") == 0)
		return;

	bool opPotentialLabel = false;
	Number *operand = NULL;
	if(status != NULL)
			status->label = (char *)op->aritOperand.c_str();
	if(labels.exists(op->aritOperand))
	{
		operand = new Number(Number::toBin(labels.value(op->aritOperand)));
	}
	else
	{
		try
		{
			operand = new Number(op->aritOperand);
		}
		catch(e_exception e)
		{
			opPotentialLabel = true;
		}
	}

	if(!opPotentialLabel)
	{
		Number result(op->value);
		result.operate(op->operation[0],*operand);
		op->value = result.toBin();
		delete operand;
	}
	else
	{
		op->aritOperandType = TYPE_LABEL;
		throw(eUndefinedLabel);
	}

}


Operands::Operands(list<t_operand> ops)
{
	this->operands = ops;
	this->itMode = this->operands.begin();
	this->itReg = this->operands.begin();
	this->itAddr = this->operands.begin();
}

/**
	* retorna o proximo operando do tipo dado
	*/
t_operand Operands::getNextOperand(e_type type)
{
	switch(type)
	{
		case TYPE_ADDRESSING:
			while(this->itMode != this->operands.end())
			{
				t_operand r = *this->itMode;
				this->itMode++;
				return r;
			}
			break;
		case TYPE_ADDRESS:
			while(this->itAddr != this->operands.end())
			{
				// ERR("this->itAddr->type: %d\n",this->itAddr->type);
				if(this->itAddr->type == TYPE_ADDRESS || this->itAddr->type == TYPE_LABEL)
				{
					t_operand r = *this->itAddr;
					this->itAddr++;
					return r;
				}
				this->itAddr++;
			}

			for(this->itAddr=this->operands.begin() ; this->itAddr!=this->operands.end() ; this->itAddr++)
			{
				e_type typeFound = this->itAddr->type;
				if(isSubtype(typeFound,type))
					return *this->itAddr;
			}
			break;
		case TYPE_REGISTER:
			while(this->itReg != this->operands.end())
			{
				if(this->itReg->type == type)
				{
					t_operand r = *this->itReg;
					this->itReg++;
					return r;
				}

				this->itReg++;
			}
			break;
		default:
			break;
	}

	TRACE("Not found: %d\n",type);
	throw(eOperandNotFound);
	return this->operands.front();

}

void Operands::solveMatch(struct s_match m, struct s_operand *op, Labels labels, struct s_status *status)
{
	op->name = m.element;
	op->operation = m.operation;
	op->aritOperand = m.operand;
	op->aritOperandType = TYPE_NONE;
	op->type = TYPE_LABEL;

	// if the operand is a label
	if((m.subtype[TYPE_ADDRESS] || m.subtype[TYPE_LABEL]) && labels.exists(op->name))
		op->value = Number::toBin(labels.value(op->name));
	// if not, it might be a number
	else if (Number::exists(op->name))
		op->value == Number::toBin(op->name);
	else
		throw(eUndefinedLabel);

	// checks if there is an operation
	if(op->operation.compare("") != 0)
		Operands::solveOperation(op, labels, status);
}

/**
	* retorna o n-esimo operando do tipo dado
	*/
t_operand Operands::getOperandIndex(e_type type, unsigned int index)
{
	unsigned int i;
	switch(type)
	{
		case TYPE_ADDRESSING:
			this->itMode = this->operands.begin();

			//se nao existirem operandos o suficiente
			if(this->operands.size() <= index)
				throw(eOperandNotFound);

			for(i=0 ; i<index && this->itMode!=this->operands.end() ; this->itMode++,i++)
				;

			return *this->itMode;
			break;
		case TYPE_ADDRESS:
			this->itAddr = this->operands.begin();

			for(i=0 ; i<=index && this->itAddr!=this->operands.end() ; this->itAddr++)
			{
				if(this->itAddr->type == type)
					i++;
			}
			if(i==index)
				throw(eOperandNotFound);
			return *this->itAddr;
			break;
		case TYPE_REGISTER:
			this->itReg = this->operands.begin();

			for(i=0 ; i<=index && this->itReg!=this->operands.end() ; this->itReg++)
			{
				if(this->itReg->type == type)
					i++;
			}
			if(i==index)
				throw(eOperandNotFound);
			return *this->itReg;
			break;
		default:
			break;
	}
	return this->operands.front();
}
