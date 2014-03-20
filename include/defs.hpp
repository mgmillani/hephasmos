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

#ifndef DEFS_HPP
#define DEFS_HPP

#include <list>

#define LOG210 1.301029995663981195
#define LOG102 0.30102999566398119801746702250966
#define LOG2(n) (log10((double)n)/LOG102)

#define ISWHITESPACE(c) ((c)==' ' || (c)=='\t')
#define ISEOL(c) ((c)=='\r' || (c)=='\n')

#define ISADDRESS(t) ((t)==TYPE_ADDRESS || (t)==TYPE_NUMBER || (t)==TYPE_LABEL)

#define SYMB_COMMENT ';'

#define REGISTER   'r'
#define OPERAND    'o'
#define ADDRESSING 'm'
#define ADDRESS    'a'
#define NUMBER     'n'
#define LABEL      'l'

using namespace std;

typedef struct s_status
{
	unsigned int line;
	unsigned int lastOrgLine;	//a linha do codigo fonte em que ocorreu o ultimo ORG
	unsigned int position;	//a posicao do proximo byte a ser escrito
	unsigned int foundOperands;
	//unsigned int expectedOperands;
	unsigned int operandSize;	//numero de bits do operando
	unsigned int firstDefinition; //numero da linha da primeira definicao da label
	int value;	//valor do operando, sem truncar
	char *operandFormat; //expressao para os operandos
	char *operand;	//o operando junto com seu modo de enderecamento
	char *label;	//a ultima label lida (referencia ou definicao)
	char *mnemonic;	//mnemonico da ultima instrucao ou diretiva lida
}t_status;

#include "operands.hpp"

typedef struct s_pendency
{
	unsigned int byte;  //posicao onde comeca a instrucao
	struct s_status *status;

	list<t_operand> operands;
	string binFormat;   //formato binario da instrucao a ser montada
	unsigned int size;  //tamanho da instrucao + operandos
}t_pendency;

typedef enum Exceptions
{
	eFileNotFound,
	eUnknownMnemonic,
	eInvalidFormat,
	eInvalidOperation,
	eIncorrectOperands,
	eUnmatchedExpression,
	eOpenString,
	eRedefinedLabel,
	eUndefinedLabel,
	eInvalidExpressionVariable,
	eAddressingNotFound,
	eOperandNotFound,
	eInvalidMachine

} e_exception;

#endif // DEFS_HPP

