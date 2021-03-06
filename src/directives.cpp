/**
* Copyright 2013 Marcelo Millani
*	This file is part of hephasmos.
*
* hephasmos is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* hephasmos is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with hephasmos.  If not, see <http://www.gnu.org/licenses/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <list>
#include <stack>

#include "directives.hpp"
#include "stringer.hpp"
#include "numbers.hpp"
#include "labels.hpp"
#include "memory.hpp"
#include "defs.hpp"
#include "expression.hpp"
#include "multiExpression.hpp"

#include "debug.hpp"

using namespace std;

/**
* executa a diretiva, retornando em qual byte a montagem deve continuar
*/
unsigned int Directives::execute(string directive, string operands, Labels labels, stack<t_pendency> *pendencies, Memory *memory,unsigned int currentByte,struct s_status *status)
{
	Expression opExp("a");
	list<Expression> expressions;
	expressions.push_back(opExp);
	MultiExpression opRepeat(expressions, "[n]a");
	MultiExpression opPlain(expressions, "a");

	if(!Directives::isDirective(directive))
		throw eUnknownMnemonic;
	Number n;
	unsigned int size = 0;
	list<string> ops = stringSplitCharProtected(operands," ,\t","['\"","]'\"",'\\');
	// org just changes assembling position
	if(stringCaselessCompare(directive,"org")==0)
	{
		// only accepts a number
		list<t_match > matches = opPlain.findAllSub(operands);
		t_match m = *matches.begin();
		t_operand op;
		op.name = m.element;
		op.operation = m.operation;
		op.aritOperand = m.operand;
		op.aritOperandType = TYPE_NONE;
		op.type = TYPE_LABEL;
		op.value = Number::toBin(op.name);
		// if there is an operation
		if(op.operation.compare("") != 0)
		{
			Operands::solveOperation(&op, labels, status);
			return n.toInt(op.value);
		}
		return n.toInt(operands);
	}
	else if(stringCaselessCompare(directive,"db")==0)
	{
		size = 1;
	}
	else if(stringCaselessCompare(directive,"dw")==0)
	{
		size = 2;
	}
	else if(stringCaselessCompare(directive,"dab")==0)
	{
		size = 1;
	}
	else if(stringCaselessCompare(directive,"daw")==0)
	{
		size = 2;
	}
	//array de valores
	list<string>::iterator ot;

	for(ot=ops.begin() ; ot!=ops.end() ; ot++)
	{
		unsigned int value,repeats;
		bool repeat = false;
		bool pendency = false;
		// checks if it's a plain value
		list<t_match > matches;
		try
		{
			matches = opPlain.findAllSub(*ot);
		}
		// if it isn't, it might be a repetition of values
		catch (e_exception e)
		{
			matches = opRepeat.findAllSub(*ot); //throws an exception on failure
			repeat = true;
		}

		if( !repeat)
		{
			// solves the value
			t_match m = *matches.begin();
			t_operand op;
			try
			{
				Operands::solveMatch(m, &op, labels, status);
				memory->writeNumber(op.value, currentByte, size);
				currentByte += size;
			}
			catch( e_exception e )
			{
				pendency = true;
			}

		}
		// the value has to be repeated a certain amount of times
		else
		{
			list<t_match>::iterator it = matches.begin();
			t_match mRepeat = *(it++);
			t_match mValue = *it;

			t_operand opRepeat;
			Operands::solveMatch(mRepeat, &opRepeat, labels, status);
			repeats = Number::toInt(opRepeat.value);
			try
			{
				Operands::solveMatch(mValue, &opRepeat, labels, status);
			}
			catch( e_exception e )
			{
				pendency = true;
			}

			if( ! pendency)
			{
				for( ; repeats>0 ; repeats--, currentByte += size)
					memory->writeNumber(opRepeat.value, currentByte, size);
			}
		}


		//se existir uma label com esse nome, usa seu valor
		/*if(labels.exists(*ot))
			memory->writeNumber(Number::toBin(labels.value(*ot)),currentByte,size);
		else if(Number::exists(*ot))
			memory->writeNumber(Number::toBin(*ot),currentByte,size);
		else if(Number::isString(*ot))
			currentByte += memory->writeString(*ot,currentByte,size,-1) - size;
		else if(Directives::isRepeat(*ot,&repeats,&value))
		{
			//cria o array com os valores certos
			unsigned char array[size];

			unsigned int i;
			unsigned int v = value;
			for(i=0 ; i<size ; i++)
			{
				//copia o byte menos significativo do valor
				array[i] = v&255;
				v >>= 8;
			}
			memory->writeArrayRepeat(array,size,currentByte,repeats);
			currentByte += (repeats-1)*size;
		}*/
		//se nao for nada conhecido, adiciona uma pendencia
		if(pendency)
		{
			t_pendency p;
			p.byte = currentByte;
			t_operand op;
			op.name = *ot;
			op.type = TYPE_LABEL;
			op.relative = false;
			p.operands.push_back(op);
			p.binFormat = "a";
			p.size = size*8;
			p.status = (t_status*)malloc(sizeof(*status));
			memcpy(p.status,status,sizeof(*status));

			size_t size = strlen(status->label);
			p.status->label = (char *)malloc(size+1);
			memcpy(p.status->label,status->label,size+1);

			size = strlen(status->mnemonic);
			p.status->mnemonic = (char *)malloc(size+1);
			memcpy(p.status->mnemonic,status->mnemonic,size+1);

			size = strlen(status->operand);
			p.status->operand = (char *)malloc(size+1);
			memcpy(p.status->operand,status->operand,size+1);

			pendencies->push(p);
		}
		currentByte += size;
	}

	return currentByte;
}

/**
* verifica se a string passada corresponde a uma diretiva
*/
bool Directives::isDirective(string directive)
{

	if(stringCaselessCompare(directive,"org")==0)
		return true;
	else if(stringCaselessCompare(directive,"db")==0)
		return true;
	else if(stringCaselessCompare(directive,"dw")==0)
		return true;
	else if(stringCaselessCompare(directive,"dab")==0)
		return true;
	else if(stringCaselessCompare(directive,"daw")==0)
		return true;
	return false;
}

/**
  * determina se a string passada corresponde a repeticao de um valor
  * ou seja, se esta no formato: "[valor]"
  * se amount e value forem diferentes de NULL, escreve a quantidade de repeticoes e o valor a ser repetido
  */
bool Directives::isRepeat(string op, unsigned int *amount, unsigned int *value)
{

	//verifica se esta entre []
	if(op[0] != '[' || op[op.size()-1] != ']')
		return false;

	//remove espacos em branco antes e depois
	unsigned int i,e;
	for(i=1 ; i<op.size() ; i++)
		if(!ISWHITESPACE(op[i]))
			break;
	for(e=op.size()-2 ; e>0 ; e--)
		if(!ISWHITESPACE(op[e]))
			break;
	string num = op.substr(i,e-i+1);
	if(Number::exists(num))
	{
		if(amount != NULL)
			*amount = Number::toInt(num);
		if(value != NULL)
			*value = 0;
		return true;
	}

	return false;

}
