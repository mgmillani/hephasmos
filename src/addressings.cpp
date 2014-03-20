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

#include "addressings.hpp"
#include "defs.hpp"

#include "debug.hpp"

using namespace std;

Addressings::Addressings()
{

}

/**
*	carrega os modos de enderecamento que estao definidos na string config
*/
void Addressings::load(string config)
{
	typedef enum {STATE_INI,STATE_NAME,STATE_RELATIVE,STATE_SKIP,STATE_CODE,STATE_EXP,STATE_END} e_state;

	e_state state = STATE_INI;
	e_state nextState = STATE_INI;
	unsigned int i;
	unsigned int b=0;
	t_addressing addr;
	string name;
	bool comment = false;
	for(i=0 ; i<config.size() && !comment ; i++)
	{
		char c = config[i];
		switch(state)
		{
			//le espacos a esquerda
			case STATE_INI:
				//comentario
				if(c==SYMB_COMMENT)
					comment = true;
				else if(!ISWHITESPACE(c))
				{
					b = i;
					state = STATE_NAME;
				}
				break;
			//le o nome interno do modo de enderecamento
			case STATE_NAME:

				if(ISWHITESPACE(c))
				{
					name = config.substr(b,i-b);
					state = STATE_RELATIVE;
				}
				break;
			//determina se eh relativo ao pc ou nao
			case STATE_RELATIVE:
				if(c == '=')
				{
					addr.relative = false;
					state = STATE_SKIP;
					nextState = STATE_CODE;
				}
				else if(c=='-')
				{
					addr.relative = true;
					state = STATE_SKIP;
					nextState = STATE_CODE;
				}
				else if(!ISWHITESPACE(c))
				{
					addr.relative = false;
					b = i;
					state = STATE_CODE;
				}
				break;
			//pula caracteres em branco
			case STATE_SKIP:
				if(!ISWHITESPACE(c))
				{
					state = nextState;
					b = i;
				}
				break;
			//le o codigo binario do modo de enderecamento
			case STATE_CODE:
				if(ISWHITESPACE(c))
				{
					addr.code = config.substr(b,i-b);
					state = STATE_SKIP;
					nextState = STATE_EXP;
				}
				break;
			//le a expressao que identifica o modo de enderecamento
			case STATE_EXP:
				if(ISWHITESPACE(c))
				{
					addr.expStr = config.substr(b,i-b);
					addr.type = Expression::getExpressionType(addr.expStr);
					state = STATE_END;
				}
				break;
			case STATE_END:
				if(c==SYMB_COMMENT)
					comment = true;
				else if(!ISWHITESPACE(c))
					throw(eInvalidFormat);
				break;
		}//end switch
	}//end for

	if(state != STATE_END && !comment)
	{
		addr.expStr = config.substr(b,i-b);
		addr.type = Expression::getExpressionType(addr.expStr);
		addr.expression = Expression(addr.expStr);
	}

	this->addrs[name] = addr;
}

/**
	* escreve os dados da estrutura em stream
	*/
void Addressings::print(FILE *stream)
{

	map<string,t_addressing>::iterator it;
	for(it=this->addrs.begin() ; it!=this->addrs.end() ; it++)
	{
		t_addressing a = it->second;
		//escreve o nome do modo de enderecamento
		fprintf(stream,"%s mode:\n",it->first.c_str());

		this->printAddressing(&a,stream);
	}

}

void Addressings::printAddressing(t_addressing *a, FILE *stream)
{

	//expressao
	fprintf(stream,"Expression: %s\n",a->expStr.c_str());
	//codigo
	fprintf(stream,"Code: %s\n",a->code.c_str());
	//se eh relativo ao pc ou nao
	fprintf(stream,"Relative: %d\n",(int)a->relative);

}

/**
*	retorna a estrutura do modo de enderecamento com o nome dado
*/
t_addressing Addressings::getAddressing(string name)
{
	return this->addrs.find(name)->second;
}

/**
  * retorna uma lista com todos os modos de enderecamento
  */
list<t_addressing> Addressings::getAllAddressings()
{
	list<t_addressing> addrsList;

	map<string,t_addressing>::iterator it;

	for(it=this->addrs.begin() ; it!=this->addrs.end() ; it++)
		addrsList.push_back(it->second);

	return addrsList;

}

/**
  * dada uma lista de enderecamentos e um tipo, determina qual deles possui o tipo mais restrito que sirva para o tipo dado
  */
t_addressing findBestAddressing(list<t_addressing> lst, e_type t)
{
	bool found = false;
	t_addressing best;
	list<t_addressing>::iterator at;
	for(at=lst.begin() ; at!=lst.end() ; at++)
	{
		if(at->type == t)
			return *at;
		//se ja encontrou algum que sirva
		if(found)
		{
			if(isSubtype(t,at->type) && !isSubtype(best.type,at->type))
				best = *at;
		}
		else
			if(isSubtype(t,at->type))
			{
				best = *at;
				found = true;
			}

	}
	if(!found)
	{
		throw(eAddressingNotFound);
	}
	else
		return best;
}





