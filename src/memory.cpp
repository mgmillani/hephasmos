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

#include "numbers.hpp"
#include "memory.hpp"

#include "debug.hpp"

//using namespace std;

/**
  *	inicializa uma memoria com suporte de, pelo menos, size bytes
  */
Memory::Memory(unsigned int size,bool bigEndian)
{
	this->bigEndian = bigEndian;
	this->area = (unsigned char*)calloc(size,1);
	this->size = size;
}

Memory::~Memory()
{
	if(this->size>0)
		free(this->area);
}

/**
  *	escreve um valor numa determinada posicao da memoria
  */
void Memory::writeValue(unsigned char value,unsigned int pos)
{
	this->area[pos] = value;
}

/**
  *	escreve todos os valores do array a partir da posicao startPos na memoria
  */
void Memory::writeArray(unsigned char *array, unsigned int arraySize, unsigned int startPos)
{
	memcpy(this->area+startPos,array,arraySize);
}

/**
  * repete o valor passado diversas vezes
  * usa a endianess adequada para escrever cada repeticao
  */
void Memory::writeArrayRepeat(unsigned char *array, unsigned int arraySize, unsigned int pos, unsigned int repeat)
{

	unsigned int begin=0,end=arraySize-1;
	int step = 1;
	if(this->bigEndian)
	{
		begin = end;
		end = 0;
		step = -1;
	}

	unsigned int i;
	for(i=0 ; i<repeat ; i++)
	{
		unsigned int j,p;
		for(j=0,p=begin ; j<arraySize ; j++,p+=step)
		{
			this->area[pos++] = array[p];
		}
	}

}

/**
	* escreve uma string a partir da posicao especificada. Cada caractere tera width bytes
	* escreve os caracteres em ordem, mas cada um respeitando a endianess
	* retorna o numero de bytes escritos, sendo que sera escrito no maximo max bytes
	*/
unsigned int Memory::writeString(string str, unsigned int pos, unsigned int width, unsigned int max)
{
	typedef enum {State_Ini,State_Escape} e_state;

	unsigned int i;
	unsigned int w=0,r=0;
	unsigned char num[width];
	e_state state = State_Ini;

	for(i=0 ; i<width ; i++)
		num[i] = 0;

	for(i=1 ; i+1<str.size() && w<max ; i++)
	{
		char c = str[i];
		switch(state)
		{
			case State_Ini:
				if(c == '\\')
					state = State_Escape;
				else
				{
					if(this->bigEndian)
						num[width-1] = c;
					else
						num[0] = c;
					r=0;
				}

				while(r<width && w<max)
					this->area[pos + w++] = num[r++];

				break;
			case State_Escape:
				if(this->bigEndian)
					num[width-1] = c;
				else
					num[0] = c;
				r=0;
				while(r<width && w<max)
					this->area[pos + w++] = num[r++];
				state = State_Ini;
			break;
		}
	}

	return w;
}

/**
  * escreve um numero a partir de uma determinada posicao
  * usa a notacao adequada
  * retorna o numero de bytes escritos
  */
unsigned int Memory::writeNumber(string number,unsigned int startPos,unsigned int maxSize)
{
	unsigned int size;
	unsigned char *array = Number::toByteArray(number,&size);
	if(size > maxSize)
		size = maxSize;

	if(this->bigEndian)
	{
		unsigned int i;
		for(i=0 ; i<size ; i++)
		{
			this->area[startPos+i] = array[size-i-1];
		}
	}
	else
		memcpy(this->area+startPos,array,size);
	free(array);
	return size;
}

/**
  *	le um o valor que esta na posicao dada da memoria
  */
unsigned char Memory::readValue(unsigned int pos)
{
	return this->area[pos];
}

/**
  *	le amount posicoes da memoria a partir da posicao dada, escrevendo os valores em array
  */
void Memory::readArray(unsigned char *array, unsigned int amount, unsigned int pos)
{
	memcpy(array,this->area+pos,amount);
}

/**
  *	retorna uma forma compactada da memoria
  *	o vetor retornado nao deve ser desalocado
  * (por enquanto, eh apenas o dump da memoria, sem nenhuma compactacao)
  */
unsigned char *Memory::pack(unsigned int *size)
{
	*size = this->size;
	return this->area;
}

/**
	* escreve todos os valores da memoria na stream dada
	*/
void Memory::print(FILE *stream)
{

	unsigned int i;
	for(i=0 ; i<this->size ; i++)
	{
		fprintf(stream,"%u:\t%d\t%s\n",i,(int)this->area[i],Number::toBin(this->area[i]).c_str());
	}

}
