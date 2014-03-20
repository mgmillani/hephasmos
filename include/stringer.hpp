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

#ifndef STRINGER_HPP
#define STRINGER_HPP

#include <map>
#include <list>
#include <string>

using namespace std;

/**
  * quebra a string em linhas
  * funciona tanto com \r, \n e \n\r
  */
list<string> stringGetLines(string text);

/**
	*	quebra a string nos divisores passados
	* nao acrescenta elementos vazios a lista
	*/
list<string> stringSplitChar(string text, string dividers);

/**
	*	quebra a string nos divisores passados
	* tudo que estiver entre dois protectors sera tratado como um elemento unico
	* um protector so pode ser fechado pelo respectivo closer (mesmo indice)
	* nao acrescenta elementos vazios a lista
	*/
list<string> stringSplitCharProtected(string text, string dividers, string protectors, string closers, char escape);

/**
* le todas as palavras de uma string, as quais podem estar separadas por '\t' ou ' '
* strings sao identificadas por qualquer caractere em delimiters, e sao fechadas pelo mesmo caractere
* caracteres precedidos por escape sao escapados
* ignora tudo o que estiver depois de um caractere comment
* retorna uma lista com todas as palavras
* caso uma string termine em aberto, throws eOpenString
*/
list<string> stringReadWords(string text, string delimiters, char escape, char comment);

/**
	*	remove todos os dividers que estiverem nos cantos de s, retornando a nova string
	*/
string stringTrim(string s,string dividers);

/**
	* verifica se c esta em s
	*/
bool stringIn(char c, string s);

/**
*	substitui cada ocorrencia da primeira string do par pela segunda, para cada par de elements
* retorna a nova string
*/
string stringReplaceAll(string s,list<pair<string,string> > elements);

string stringReplaceAll(string s,map<string,string> elements);

/**
* substitui a primeira ocorrencia de oldS em s por newS
* retorna a nova string
*/
string stringReplaceFirst(string s, string oldS, string newS);
/**
*	compara duas strings ignorando maiusculas e minusculas
*	retorna 0 se forem iguais, >0 se a > b, <0 se a < b
*/
int stringCaselessCompare(string a, string b);
int stringCaselessCompare(const char **a, const char **b);
int stringCaselessCompare(const char* a, const char* b);

#endif // STRINGER_HPP

