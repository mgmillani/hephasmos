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

#ifndef DIRECTIVES_HPP
#define DIRECTIVES_HPP

#include <string>
#include <list>
#include <stack>

#include "directives.hpp"
#include "stringer.hpp"
#include "numbers.hpp"
#include "labels.hpp"
#include "memory.hpp"
#include "defs.hpp"

using namespace std;

class Directives
{

	public:

	/**
	  * executa a diretiva, retornando em qual byte a montagem deve continuar
	  */
	static unsigned int execute(string directive,string operands,Labels labels,stack<t_pendency> *pendencies,Memory *memory,unsigned int currentByte,struct s_status *status);

	/**
	  * verifica se a string passada corresponde a uma diretiva
	  */
	static bool isDirective(string name);

	/**
	  * determina se a string passada corresponde a repeticao de um valor
	  * se amount e value forem diferentes de NULL, escreve a quantidade de repeticoes e o valor a ser repetido
	  */
	static bool isRepeat(string op, unsigned int *amount=NULL, unsigned int *value=NULL);

	private:

};

#endif // DIRECTIVES_HPP

