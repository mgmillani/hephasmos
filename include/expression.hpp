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

#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <list>

#include <boost/regex.hpp>

#include "types.hpp"

using namespace std;

typedef struct s_match
{
	unsigned char type;	//tipo da variavel na expressao
	unsigned char subtype[TYPE_TOTAL]; //tipo da variavel nas subexpressoes
	string element;	//a variavel encontrada
	string operation;	//o operador, se houver
	string operand;	//o operando, se houver operador
	string subCode[TYPE_TOTAL]; //codigo da subexpressao
	list<unsigned int> indexes; //os indices das subexpressoes matched
}t_match;

class Expression
{
	public:

	//retorna o primeiro tipo encontrado na subexpressao
	static e_type getExpressionType(string exp);

	Expression();
	~Expression();

	/**
    * converte a expressao passada para uma expressao regular no formato Perl
    */
	Expression(string expression);

	/**
	  * se a frase satisfizer a expressao, faz o match entre as variaveis da frase com as da expressao
	  * se nenhuma expressao for passado, usa a do construtor
	  * retorna uma lista de pares onde o primeiro elemento eh a variavel e o segundo, seu tipo
	  * se a frase nao satisfizer, throws eUnmatchedExpression
	  */
	list<pair<string,char> > findAll(string phrase,string expression = "");

	/**
	  * retorna a string da expressao
	  */
	string expression();

	/**
	  * retorna a string da expressao regular utilizada
	  */
	string regexpression();

	char *vars;

	private:

  /**
    * faz o mesmo que o construtor
    */
	void init(string exp);

	string exp;
	string regexStr;
	boost::regex regexp;


};

/**
  * dado o caractere de uma variavel, retorna seu indice
  */
e_type varToNum(char c);

/**
  * verifica se o caractere passado pode ser parte do nome de uma variavel
  */
bool isVarChar(char c);

/**
  * verifica se o caractere eh reservado
  */
bool isReserved(char c);

#endif // EXPRESSION_HPP

