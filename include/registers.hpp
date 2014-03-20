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

#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <stdio.h>

#include <string>
#include <map>

using namespace std;

typedef struct s_register
{
	string name;
	string index;
}t_register;

class	Registers
{

	public:

	Registers();

	/**
	  *	carrega os registradores que estao definidas na string config
	  */
	void load(string config);

	/**
    * garante que todos os registradores possuam o numero certo de digitos
    */
	void adjustRegistersIndexes();

	/**
	  *	retorna o numero do registrador caso ele exista,
	  * string vazia caso nao exista
	  */
	string number(string regName);

	/**
	  * verifica se um registrador existe ou nao
	  */
	bool exists(string regName);

	/**
	  * escreve os dados do objeto em stream
	  */
	void print(FILE *stream);

	private:

	map<string,t_register> regs;

};

#endif // REGISTERS_HPP
