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

#include <string>
#include <map>
#include <set>

#include <boost/algorithm/string.hpp>

#include "labels.hpp"
#include "defs.hpp"

#include "debug.hpp"

using namespace std;

/**
*	adiciona a definicao de uma label
*/
void Labels::define(string name,unsigned int pos,unsigned int line)
{

	string str = boost::to_upper_copy(name);
	if(this->defs.find(name) == this->defs.end())
	{
		this->defs[str] = pos;
		this->lines[str] = line;
		//marca como nao referenciada
		this->unrefs.insert(str);
	}
	else
		throw eRedefinedLabel;
}

/**
*	retorna a posicao em que a label foi definida
*	conta como uma referencia a label
* se a label ainda nao foi definida, throws eUndefinedLabel
*/
unsigned int Labels::value(string name)
{
	string str = boost::to_upper_copy(name);
	map<string,unsigned int>::iterator i = this->defs.find(str);
	if(i == this->defs.end())
	{
		//label referenciada mas nao definida
		throw(eUndefinedLabel);
	}
	else
	{
		//marca a label como referenciada
		this->unrefs.erase(str);
		return i->second;
	}
}

/**
* retorna a linha em que a label foi definida
* nao conta como uma referencia
* se a label ainda nao foi definida, throws eUndefinedLabel
*/
unsigned int Labels::line(string name)
{
	string str = boost::to_upper_copy(name);
	map<string,unsigned int>::iterator i = this->lines.find(str);
	if(i == this->defs.end())
	{
		//label referenciada mas nao definida
		throw(eUndefinedLabel);
	}
	else
	{
		return i->second;
	}
}

/**
	* verifica se existe uma label definida com o nome dado
	*/
bool Labels::exists(string name)
{

	string str = boost::to_upper_copy(name);
	map<string,unsigned int>::iterator i = this->lines.find(str);
	if(i == this->lines.end())
		return false;
	else
		return true;

}


/**
  * a < b sse a.pos < b.pos
  */
bool labelSortPos(t_label a, t_label b)
{
	return a.pos < b.pos;
}

/**
  * retorna uma lista com todas as labels e seus valores
  */
list<t_label> Labels::copy()
{

	list<t_label> lst;

	map<string, unsigned int>::iterator it;
	for(it=this->defs.begin() ; it!=this->defs.end() ; it++)
	{
		t_label la;
		la.name = it->first;
		la.pos = it->second;

		la.line = this->lines.find(la.name)->second;
		lst.push_back(la);
	}

	lst.sort(labelSortPos);

	return lst;

}
