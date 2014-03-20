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

#ifndef ADDRESSINGS_HPP
#define ADDRESSINGS_HPP

#include <stdio.h>

#include <string>
#include <map>

#include "expression.hpp"
#include "types.hpp"

using namespace std;

typedef struct s_addressing
{
	string expStr;
	Expression expression;
	string code;
	e_type type;
	bool relative;
}t_addressing;

class Addressings
{

	public:

	Addressings();

	/**
	  *	carrega os modos de enderecamento que estao definidos na string config
	  */
	void load(string config);

	/**
	  *	retorna a estrutura do modo de enderecamento com o nome dado
	  */
	t_addressing getAddressing(string name);

	/**
	  * retorna uma lista com todos os modos de enderecamento
	  */
	list<t_addressing> getAllAddressings();

	/**
	  * escreve os dados da estrutura em stream
	  */
	void print(FILE *stream);

	void printAddressing(t_addressing *a, FILE *stream);

	private:

	map<string,t_addressing> addrs;	//associa o nome do modo de enderecamento com seus atributos

};

/**
  * dada uma lista de enderecamentos e um tipo, determina qual deles possui o tipo mais restrito que sirva para o tipo dado
  */
t_addressing findBestAddressing(list<t_addressing> lst, e_type);

#endif // ADDRESSINGS_HPP
