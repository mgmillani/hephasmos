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

#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <stdio.h>

#include <string>
#include <stack>
#include <list>
#include <map>

#include "addressings.hpp"
#include "messenger.hpp"
#include "registers.hpp"
#include "operands.hpp"
#include "labels.hpp"
#include "defs.hpp"
#include "memory.hpp"

using namespace std;

typedef struct s_instruction
{
	unsigned int size;
	string mnemonic;
	string operandExpression;
	list<string> addressingNames;
	list<string> regs;
	string binFormat;

}t_instruction;

class Instructions
{

	public:

	Instructions();

	/**
	*	carrega as instrucoes que estao definidas na string config
	*/
	void load(string config);

	/**
	*	determina se o dado mnemonico corresponde a uma instrucao ou nao
	*/
	bool isInstruction(string mnemonic);

	/**
  *	gera o codigo binario de uma instrucao, escrevendo-o na memoria
  * retorna o numero de bytes escritos na memoria
  */
	unsigned int assemble(string mnemonic, string operandsStr,Memory *mem,unsigned int pos,stack<t_pendency> *pendencies,Addressings addressings,Labels labels, Registers registers,t_status *status);

	/**
	  * escreve as caracteristicas do conjunto de instrucoes em stream
	  */
	void print(FILE *stream);

	list<t_instruction> getInstructions(string mnemonic);

	private:

	/**
	  * escreve os atributos da estrutura em stream
	  */
	void printInstruction(t_instruction *i,FILE *stream);

	map<string,list<t_instruction> > insts;	//relaciona o mnemonico com a estrutura
};

/**
  * substitui os operandos, escrevendo seu valor binario na string
  * em format:
  * r[n] indica o n-esimo registrador. Se n for omitido, segue a ordem em que aparecem
  * a[n](m) indica o n-esimo endereco. Se n for omitido, segue a ordem em que aparecem. m indica o tamanho, em bits
  * m[n] indica o n-esimo modo de enderecamento. Se n for omitido, segue a ordem em que aparecem
  * 1 e 0 indicam os proprios algarismos
  * qualquer outro caractere sera ignorado
  * size indica quantos bits o resultado deve ter
  * pos determina a posicao atual do Program Counter
  * a string retornanda contera somente 0s e 1s e sera terminada por um 'b'
  */
string replaceOperands(string format,list<t_operand> operands,unsigned int size, unsigned int pos);

#endif // INSTRUCTIONS_HPP
