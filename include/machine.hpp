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

#ifndef MACHINE_HPP
#define MACHINE_HPP

#include <string>

#include "memory.hpp"

using namespace std;

class	Machine
{

	public:

	Machine();

	/**
	*	carrega as caracteristicas da maquina que estao definidas na string config
	*/
	void load(string config);

	/**
	*	retorna o numero de bits do PC
	*/
	unsigned int getPCSize();

	/**
	*	escreve uma palavra na memoria, respeitando a endianess da maquina
	*	a palavra de entrada deve usar a notacao little-endian
	*/
	void writeWord(unsigned char *word,unsigned int wordSize,Memory *memory,unsigned int position);

	/**
	*	escreve o valor na memoria, respeitando a endianess da maquina
	*/
	void writeValue(int value,unsigned int wordSize,Memory *memory,unsigned int position);

	bool bigEndian;
	string name;
	private:
	unsigned int pcBits;
};

#endif // MACHINE_HPP
