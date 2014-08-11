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

#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>
#include <stack>
#include <list>

#include "instructions.hpp"
#include "addressings.hpp"
#include "directives.hpp"
#include "registers.hpp"
#include "messenger.hpp"
#include "machine.hpp"
#include "labels.hpp"

using namespace std;

typedef enum {CAT_NONE,CAT_INST,CAT_ADDR,CAT_REGI,CAT_MACH} e_category;

class Assembler
{

	public:
	/**
	  *	cria um montador, especificando quais sao as propriedades da maquina/arquiterua para qual os codigos serao montados
	  */
	Assembler(Instructions inst, Registers reg, Machine machine, Addressings adr);

	void init(FILE *filename,Messenger messenger);

	/**
	  *	le as caracteristicas da arquitetura que estao no arquivo dado
	  */
	Assembler(const char *filename,Messenger messenger);
	Assembler(FILE *file,Messenger messenger);

	/**
	  *	monta o codigo assembly passado
	  * retorna a memoria gerada
	  */
	Memory assembleCode(string code);

	/**
	  *	cria o arquivo binario para a memoria
	  * o arquivo tera o seguinte formato:
	  * primeiro byte: versao (0, no caso)
	  * nome da maquina, terminado por um '\0'
	  * dump da memoria (size bytes)
	  * SHA1 do resto do arquivo (20 bytes)
	  */
	void createBinaryV0(string filename,Memory *memory);
	void createBinaryV0(FILE *fl,Memory *memory);

	/**
	  * escreve todos os simbolos declarados no codigo montado
	  */
	void createSymTable(FILE *fl);

	/**
	  * cria o arquivo binario no formato do Daedalus
	  * o formato eh o seguinte:
	  * primeiro byte: 3
	  * segundo  byte: 0
	  * proximos 3 bytes: nome da maquina, em maiusculas
	  * resto: dump da memoria, sendo que cada valor eh um inteiro de 16 bit em notacao little-endian
	  */
	void createBinaryV3(FILE *fl,Memory *memory);

	/**
	  * se ocorreu algum erro durante o processo de montagem
	  */
	bool hasErrors();

	/**
	  * escreve a situacao atual dos atributos do objeto
	  */
	void print(FILE *stream);

	private:

	stack<t_pendency> pendecies; //labels pendentes
	Labels labels; //labels definidas
	Instructions inst;
	Registers regs;
	Addressings addr;
	Machine mach;
	Messenger messenger;
	Directives directives;

	/**
	  *	monta uma linha, escrevendo seu codigo binario a partir de memory[byte]
	  * line eh a linha a ser montada
	  * se houver alguma label que ainda nao foi definida, reserva espaco e adiciona a pendencia na pilha
	  * se for encontrada a definicao de uma label, acrescenta-a as Labels conhecidas
	  * retorna a posicao da memoria em que a proxima linha deve comecar
	  */
	unsigned int assembleLine(string line, Memory *memory,unsigned int byte,unsigned int lineNumber,t_status *status);

	/**
	  * faz o parsing de uma linha, escrevendo a label definida em defLabel,
	  * o mnemonico da linha em mnemonic e os operandos em operands
	  */
	void parseLine(string line,string *defLabel, string *mnemonic, string *operands);

	/**
	  * recalcula o valor das labels, retornando uma nova lista de operandos
	  */
	list<t_operand> recalculateOperands(list<t_operand> operands,t_status *status, unsigned int pos);

};

#endif // ASSEMBLER_HPP

