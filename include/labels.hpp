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

#ifndef LABELS_HPP
#define LABELS_HPP

#include <string>
#include <map>
#include <set>

using namespace std;

typedef struct s_label
{
	string name;
	unsigned int pos,line;
}t_label;

class Labels
{

	public:

	/**
	  *	adiciona a definicao de uma label
	  */
	void define(string name,unsigned int pos,unsigned int line);

	/**
	  *	retorna a posicao em que a label foi definida
	  *	conta como uma referencia a label
	  */
	unsigned int value(string name);

	/**
	  * retorna a linha em que a label foi definida
	  * nao conta como uma referencia
	  */
	unsigned int line(string name);

	/**
	  * verifica se existe uma label definida com o nome dado
	  */
	bool exists(string name);

	/**
	  * retorna uma lista com todas as labels e seus valores
	  */
	list<t_label> copy();

	private:

	map<string,unsigned int> defs;	//labels definidas
	map<string,unsigned int> lines;	//linhas em que as labels foram definidas
	set<string> unrefs;	//labels que ainda nao foram referenciadas

};
#endif // LABELS_HPP
