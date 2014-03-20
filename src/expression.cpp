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
#include <list>

#include <boost/regex.hpp>

#include "expression.hpp"
#include "stringer.hpp"
#include "types.hpp"
#include "defs.hpp"

using namespace std;

Expression::Expression()
{
	this->vars = NULL;
}

Expression::~Expression()
{
	//if(this->vars != NULL)
	//	free(this->vars);
}

/**
  * converte a expressao passada para uma expressao regular no formato Perl
  */
Expression::Expression(string expression)
{
	this->init(expression);
}


/**
  * se a frase satisfizer a expressao, faz o match entre as variaveis da frase com as da expressao
  * se nenhuma expressao for passado, usa a do construtor
  * retorna uma lista de pares onde o primeiro elemento eh a variavel e o segundo, seu tipo
  * se a frase nao satisfizer, throws eUnmatchedExpression
  */
list<pair<string,char> > Expression::findAll(string phrase,string exp)
{
	if(exp!="")
		this->init(exp);
	//usa a regex
	boost::match_results<const char*> what;
	boost::regex_match(phrase.c_str(),what,this->regexp);
	if(what[0].matched==0)
		throw(eUnmatchedExpression);

	unsigned int i;
	list<pair<string,char> > vars;
	for(i=1 ; i<what.size() ; i++)
	{
		vars.push_back(pair<string,char>(what[i],this->vars[i-1]));
	}

	return vars;
}

/**
  * construtor
  */
void Expression::init(string expression)
{
	this->exp = expression;
	//conta o numero de variaveis na expressao
	bool escape = false;
	unsigned int i;
	unsigned int amount = 0;
	for(i=0 ; i<expression.size() ; i++)
	{
		char c = expression[i];
		if(escape)
			escape = false;
		else if(isReserved(c))
			amount++;
		else if(c=='\\')
			escape=true;
	}

	this->vars = (char *)malloc(amount);
	/*
	 * converte a expressao em uma regex
	 * substitui cada variavel por qualquer sequencia de a-zA-Z0-9 e _
	 * escapa todos os .[{}()\*+?|^$
	 * permite qualquer quantidade de espaco entre os caracteres
	 */

	string regexp = "[[:blank:]]*";
	escape = false;
	amount = 0;
	for(i=0 ; i<expression.size() ; i++)
	{
		char c= expression[i];
		if(c=='\\')
		{
			escape = true;
			continue;
		}
		else if(stringIn(c,".[{}()\\*+?|^$"))
		{
			regexp += '\\';
			regexp += c;
		}
		//as variaveis sao qualquer sequencia de a-zA-Z0-9 ou _
		else if(isReserved(c) && !escape)
		{
			//expressao regular que agrupa qualquer caractere alfanumerico ou _
			string element = "((?:[[:alnum:]]|_)+|(?:\".*\")+|(?:'.*')+)";
			//expressao para agrupar a aplicacao de um operador aritmetico
			string operation = "(?:[[:blank:]]*(?:\\+|\\*|-|/|\\^))";
			regexp += element + "(" + operation + element+ ")?";
			//guarda o tipo da variavel
			this->vars[amount++] = c;
		}
		else
		{
			regexp += c;
		}
		// any amount of spaces
		// TODO: allow commas only if in compatibility mode
		regexp += "(?:[[:blank:]]|,*)";
		escape = false;
	}

	this->regexStr = regexp;
	this->regexp = boost::regex(regexp);
}

string Expression::expression()
{
	return this->exp;
}

/**
  * retorna a string da expressao regular utilizada
  */
string Expression::regexpression()
{
	return this->regexStr;
}

//retorna o primeiro tipo encontrado na subexpressao
e_type Expression::getExpressionType(string exp)
{
	unsigned int i;
	unsigned int max = exp.size();
	bool escape = false;
	for(i=0 ; i<max ; i++)
	{
		if(!escape)
		{
			if(exp[i] == '\\')
				escape = true;
			else if(isReserved(exp[i]))
				return varToNum(exp[i]);
		}
		else
			escape = false;
	}
	return TYPE_NONE;
}

/**
  * dado o caractere de uma variavel, retorna seu tipo
  */
e_type varToNum(char c)
{
	switch(c)
	{
		case 'r': return TYPE_REGISTER;
		case 'a': return TYPE_ADDRESS;
		case 'l': return TYPE_LABEL;
		case 'o': return TYPE_ANYTHING;
		case 'n': return TYPE_NUMBER;
	}

	throw(eInvalidExpressionVariable);
}

/**
  * verifica se o caractere passado pode ser parte do nome de uma variavel
  */
bool isVarChar(char c)
{
	if('A' <= c && c <= 'Z')
		return true;
	if('a' <= c && c <= 'z')
		return true;
	if('0' <= c && c<='9')
		return true;
	if(c=='_')
		return true;
	return false;
}

/**
  * verifica se o caractere eh reservado
  */
bool isReserved(char c)
{
	switch(c)
	{
		case 'r':
		case 'a':
		case 'l':
		case 'o':
		case 'n':
			return true;
		default:
			return false;
	}
}


