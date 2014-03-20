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
#include <ctype.h>

#include <list>
#include <string>
#include <utility>

#include "stringer.hpp"
#include "defs.hpp"

#include "debug.hpp"

using namespace std;

/**
  * quebra a string em linhas
  * funciona tanto com \r, \n e \r\n
  */
list<string> stringGetLines(string text)
{
	typedef enum {State_Ini, State_N, State_R} e_state;
	e_state state = State_Ini;
	list<string> result;

	unsigned int i;
	unsigned int b=0;
	for(i=0 ; i<text.size() ; i++)
	{
		char c = text[i];
		switch(state)
		{
			case State_Ini:
				if(c == '\n')
				{
					state = State_N;
					result.push_back(text.substr(b,i-b));
				}
				else if(c == '\r')
				{
					state = State_R;
					result.push_back(text.substr(b,i-b));
				}
				break;
			case State_N:
				if(c == '\n')
				{
					result.push_back("");
				}
				else if(c=='\r')
				{
					result.push_back("");
					state = State_R;
				}
				else
				{
					b=i;
					state = State_Ini;
				}

				break;
			case State_R:
				if(c == '\n')
				{
					state = State_N;
				}
				else if(c == '\r')
				{
					result.push_back("");
				}
				else
				{
					b=i;
					state = State_Ini;
				}

				break;
		}
	}

	if(state==State_Ini)
	{
		result.push_back(text.substr(b,i-b));
	}

	return result;
}
/**
	*	quebra a string nos divisores passados
	* nao acrescenta elementos vazios a lista
	*/
list<string> stringSplitChar(string text, string dividers)
{
	typedef enum {STATE_READ,STATE_SPLIT} e_state;
	list<string> sections;

	e_state state = STATE_READ;
	unsigned int i=0,b = 0;
	for(i=0 ; i<text.size() ; i++)
	{
		char c = text[i];
		switch(state)
		{
			case STATE_READ:
				//se for encontrado um divisor
				if(dividers.find(c)!=string::npos)
				{
					sections.push_back(text.substr(b,i-b));
					state = STATE_SPLIT;
				}
				break;
			case STATE_SPLIT:
				//se nao for um divider
				if(dividers.find(c)==string::npos)
				{
					state = STATE_READ;
					b = i;
				}
				break;
		}

	}

	if(state == STATE_READ)
		sections.push_back(text.substr(b,i-b));

	return sections;

}

/**
	*	quebra a string nos divisores passados
	* tudo que estiver entre dois protectors sera tratado como um elemento unico
	* um protector so pode ser fechado pelo respectivo closer (mesmo indice)
	* nao acrescenta elementos vazios a lista
	*/
list<string> stringSplitCharProtected(string text, string dividers, string protectors, string closers, char escape)
{
	typedef enum {STATE_READ,STATE_SPLIT,STATE_ESCAPE,STATE_PROTECTED} e_state;
	list<string> sections;

	e_state state = STATE_SPLIT;
	e_state nextState = STATE_SPLIT;
	unsigned int i=0,b = 0;
	size_t index;
	char protectClose = '\0';
	for(i=0 ; i<text.size() ; i++)
	{
		char c = text[i];
		switch(state)
		{
			case STATE_ESCAPE:

				state = nextState;
				break;
			case STATE_PROTECTED:
				if(c == escape)
				{
					state = STATE_ESCAPE;
					nextState = STATE_PROTECTED;
				}
				else if(c == protectClose)
				{
					sections.push_back(text.substr(b,i-b+1));
					state = STATE_SPLIT;
				}
				break;
			case STATE_READ:
				//se for encontrado um divisor
				if(dividers.find(c)!=string::npos)
				{
					sections.push_back(text.substr(b,i-b));
					state = STATE_SPLIT;
				}
				//elemento protegido
				else if((index=protectors.find(c))!=string::npos)
				{
					protectClose = closers[index];
					state = STATE_PROTECTED;
				}
				//escape
				else if(c == escape)
				{
					state = STATE_ESCAPE;
				}
				break;
			case STATE_SPLIT:
				//se for o inicio de um elemento protegido
				if((index=protectors.find(c)) != string::npos)
				{
					state = STATE_PROTECTED;
					protectClose = closers[index];
					b = i;
				}
				else if(c == escape)
				{
					state = STATE_ESCAPE;
					b = i;
				}
				//se nao for um divider
				else if(dividers.find(c)==string::npos)
				{
					state = STATE_READ;
					b = i;
				}
				break;
		}

	}

	if(state == STATE_READ)
		sections.push_back(text.substr(b,i-b));
	else if(state == STATE_PROTECTED)
		throw(eOpenString);

	return sections;
}

/**
* le todas as palavras de uma string, as quais podem estar separadas por '\t' ou ' '
* strings sao identificadas por qualquer caractere em delimiters, e sao fechadas pelo mesmo caractere
* caracteres precedidos por escape sao escapados
* ignora tudo o que estiver depois de um caractere comment
* retorna uma lista com todas as palavras
*/
list<string> stringReadWords(string text, string delimiters, char escape, char comment)
{

	typedef enum {STATE_SKIP,STATE_READ,STATE_STRING,STATE_ESCAPE,STATE_COMMENT} e_state;

	list<string> words;

	unsigned int i,b=0;

	e_state state = STATE_SKIP;
	e_state nextState;
	char close = '\'';

	for(i=0 ; i<text.size() ; i++)
	{
		char c = text[i];

		switch(state)
		{
			//pula caracteres em branco
			case STATE_SKIP:

				if(!ISWHITESPACE(c) && !ISEOL(c))
				{
					b = i;
					//se for o inicio de uma string
					if(delimiters.find(c)!=string::npos)
					{
						close = c;
						state = STATE_STRING;

					}
					else if(c == comment)
						state = STATE_COMMENT;
					//inicio de uma palavra
					else
					{
						state = STATE_READ;
					}
				}
				break;
			case STATE_READ:
				//final de uma palavra
				if(ISWHITESPACE(c) || ISEOL(c))
				{
					words.push_back(text.substr(b,i-b));
					state = STATE_SKIP;
				}
				//inicio de uma string
				else if(delimiters.find(c) != string::npos)
				{
					close = c;
					state = STATE_STRING;
				}
				else if(c==comment)
					state = STATE_COMMENT;

				break;
			case STATE_STRING:
				//final da string
				if(c == close)
					state = STATE_READ;
				else if(c==escape)
				{
					state = STATE_ESCAPE;
					nextState = STATE_STRING;
				}
				break;
			//pula o caractere
			case STATE_ESCAPE:
				state = nextState;
				break;
			case STATE_COMMENT:
				if(ISEOL(c))
					state = STATE_SKIP;
		}
	}

	if(state == STATE_READ)
		words.push_back(text.substr(b,i-b));
	else if(state == STATE_STRING)
		throw(eOpenString);

	return words;
}

/**
	*	remove todos os dividers que estiverem nos cantos de s, retornando a nova string
	*/
string stringTrim(string s,string dividers)
{
	int b=0,e = s.size();
	string::iterator begin = s.begin();
	string::reverse_iterator end = s.rbegin();

	//busca o primeiro caractere que nao eh divider
	while(stringIn(*begin,dividers)&&b<e)
	{
		b++;
		begin++;
	}

	//busca o ultimo caractere que nao eh divider
	while(stringIn(*end,dividers)&&b<e)
	{
		e--;
		end++;
	}

	if(b<e)
		return s.substr(b,e-b+1);
	else
		return "";

}

/**
	* verifica se c esta em s
	*/
bool stringIn(char c, string s)
{

	string::iterator it;
	bool found = false;
	for(it=s.begin();it!=s.end() && !found;it++)
	{
		if(c == *it)
			found = true;
	}

	return found;
}

/**
*	substitui cada ocorrencia da primeira string do par pela segunda, para cada par de elements
* retorna a nova string
*/
string stringReplaceAll(string s,list<pair<string,string> > elements)
{
	size_t pos;
	list<pair<string,string> >::iterator it;
	for(it=elements.begin() ; it!=elements.end() ; it++)
	{
		pos = s.find(it->first);

		//npos: valor de retorno de string::find quando nao eh encontrado
		while(pos != string::npos)
		{
			s = s.replace(s.begin()+pos,s.begin()+it->first.size()+pos,it->second);
			pos = s.find(it->first,pos+it->second.size());
		}
	}

	return s;
}

string stringReplaceAll(string s,map<string,string> elements)
{
	size_t pos;
	map<string,string >::iterator it;
	for(it=elements.begin() ; it!=elements.end() ; it++)
	{
		pos = s.find(it->first);

		//npos: valor de retorno de string::find quando nao eh encontrado
		while(pos != string::npos)
		{
			s = s.replace(s.begin()+pos,s.begin()+it->first.size()+pos,it->second);
			pos = s.find(it->first,pos+it->second.size());
		}
	}

	return s;
}

/**
* substitui a primeira ocorrencia de oldS em s por newS
* retorna a nova string
*/
string stringReplaceFirst(string s, string oldS, string newS)
{
	size_t pos = s.find(oldS);
	if(pos != string::npos)
		return s.replace(s.begin()+pos,s.begin()+oldS.size()+pos,newS);
	else
	 return s;
}

/**
*	compara duas strings ignorando maiusculas e minusculas
*	retorna 0 se forem iguais, >0 se a >b, <0 se a<b
*/
int stringCaselessCompare(string a, string b)
{
	return stringCaselessCompare(a.c_str(),b.c_str());
}

int stringCaselessCompare(const char **a, const char **b)
{
	return stringCaselessCompare(*a,*b);
}

int stringCaselessCompare(const char *a, const char *b)
{
	unsigned int i;
	for(i=0 ; a[i]!='\0' && b[i]!='\0' ; i++)
	{
		char d = tolower(a[i]);
		char e = tolower(b[i]);
		int dif = d-e;
		if(dif != 0)
			return dif;
	}
	if(b[i] != '\0')
		return 1;
	else if(a[i] != '\0')
		return -1;
	else
		return 0;
}








