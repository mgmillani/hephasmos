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

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <string>

using namespace std;

class Memory
{

	public:

	/**
	  *	inicializa uma memoria com suporte de, pelo menos, size bytes
	  */
	Memory(unsigned int size,bool bigEndian);
	~Memory();

	/**
	  *	escreve um valor numa determinada posicao da memoria
	  */
	void writeValue(unsigned char value,unsigned int pos);

	/**
	  *	escreve todos os valores do array a partir da posicao startPos na memoria
	  * ignora a endianess
	  */
	void writeArray(unsigned char *array, unsigned int arraySize, unsigned int startPos);

	/**
	  * repete o valor passado diversas vezes
	  * usa a endianess adequada para escrever cada repeticao
	  */
	void writeArrayRepeat(unsigned char *array, unsigned int arraySize, unsigned int pos, unsigned int repeat);

	/**
	  * escreve uma string a partir da posicao especificada. Cada caractere tera width bytes
	  * escreve os caracteres em ordem, mas cada um respeitando a endianess
	  * retorna o numero de bytes escritos, sendo que sera escrito no maximo max bytes
	  */
	unsigned int writeString(string str, unsigned int pos, unsigned int width, unsigned int max);

	/**
    * escreve um numero a partir de uma determinada posicao
    * usa a notacao adequada
    * retorna o numero de bytes escritos
    */
	unsigned int writeNumber(string number,unsigned int startPos,unsigned int maxSize);

	/**
	  *	le um o valor que esta na posicao dada da memoria
	  */
	unsigned char readValue(unsigned int pos);

	/**
	  *	le amount posicoes da memoria a partir da posicao dada, escrevendo os valores em array
	  */
	void readArray(unsigned char *array, unsigned int amount, unsigned int pos);

	/**
	  *	retorna uma forma compactada da memoria
	  *	o vetor retornado nao deve ser desalocado
	  * (por enquanto, eh apenas o dump da memoria, sem nenhuma compactacao)
	  */
	unsigned char *pack(unsigned int *size);

	void print(FILE *stream);

	private:

	bool bigEndian;
	unsigned char *area;
	unsigned int size;

};

#endif // MEMORY_HPP

