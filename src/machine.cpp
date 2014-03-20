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

#include <string>
#include <iostream>

#include "stringer.hpp"
#include "machine.hpp"
#include "numbers.hpp"
#include "defs.hpp"

using namespace std;

Machine::Machine()
{

}

/**
*	carrega as caracteristicas da maquina que estao definidas na string config
*/
void Machine::load(string config)
{
	typedef enum {STATE_INI,STATE_ENDIAN,STATE_SKIP,STATE_PC,STATE_END,STATE_NAME} e_state;

	e_state state = STATE_INI;
	e_state nextState = STATE_INI;
	unsigned int i;
	unsigned int b = 0;
	for(i=0 ; i<config.size() ; i++)
	{
		char c = config[i];
		if(c == SYMB_COMMENT)
			break;
		switch(state)
		{
			case STATE_INI:
				if(!ISWHITESPACE(c))
				{
					state = STATE_ENDIAN;
					b = i;
				}
				break;
			//espera-se uma palavra que indique a endianess da maquina
			case STATE_ENDIAN:

				//a palavra terminou
				if(ISWHITESPACE(c))
				{
					string word = config.substr(b,i-b);
					if(stringCaselessCompare(word,"little-endian")==0)
						this->bigEndian = false;
					else if(stringCaselessCompare(word,"big-endian")==0)
						this->bigEndian = true;
					else
						throw(eInvalidFormat);

					state = STATE_SKIP;
					nextState = STATE_PC;
				}

				break;

				//endianess lida, pula espacos em branco
				case STATE_SKIP:
					if(!ISWHITESPACE(c))
					{
						state = nextState;
						b = i;
					}
					break;

				case STATE_PC:
					if(ISWHITESPACE(c))
					{
						//converte a string para um inteiro
						string number = config.substr(b,i-b);
						Number n;
						this->pcBits = n.toInt(number);
						state = STATE_SKIP;
						nextState = STATE_NAME;
					}
					break;
				//nome da maquina
				case STATE_NAME:
					if(ISWHITESPACE(c))
					{
						//converte a string para um inteiro
						this->name = config.substr(b,i-b);
						state = STATE_END;
					}
					break;
				//tudo ja foi lido
				case STATE_END:

					if(!ISWHITESPACE(c))
						throw(eInvalidFormat);
					break;
		}
	}

	if(state==STATE_NAME)
	{
		//le o nome da maquina
		this->name = config.substr(b,i-b);
	}

}

/**
*	retorna o numero de bits do PC
*/
unsigned int Machine::getPCSize()
{
	return this->pcBits;
}

/**
*	escreve uma palavra na memoria, respeitando a endianess da maquina
*	a palavra de entrada deve usar a notacao little-endian
*/
void Machine::writeWord(unsigned char *word,unsigned int wordSize,Memory *memory,unsigned int position)
{
	unsigned char buffer[wordSize];
	unsigned char *wat = buffer;
	//se for big-endian, inverte a ordem
	if(this->bigEndian)
	{
		int i,j;
		for(j=0,i=wordSize-1; i>=0 ;j++, i--)
			buffer[i] = word[j];
	}
	else
		wat = word;

	memory->writeArray(wat,wordSize,position);
}

/**
*	escreve o valor na memoria, respeitando a endianess da maquina
*/
void Machine::writeValue(int value,unsigned int wordSize,Memory *memory,unsigned int position)
{
	if(this->bigEndian)
	{
		int i;
		for(i=wordSize-1;i>=0;i--)
		{
			//escreve o byte menos significativo do valor
			memory->writeValue(value&0xFF,position+i);
			value>>=8;
		}
	}
	else
	{
		unsigned int i;
		for(i=0;i<wordSize;i++,position++)
		{
			//escreve o byte menos significativo do valor
			memory->writeValue(value&0xFF,position);
			value>>=8;
		}
	}
}
