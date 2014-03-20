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

#include <string>
#include <list>

#include "stringer.hpp"
#include "file.hpp"

using namespace std;

/**
  *	le todas as linhas do arquivo, criando uma lista com cada uma delas
  */
list<string> fileReadLines(const char *filename)
{
	int size;
	char *raw = fileRead(filename,&size,1);
	list<string> result = fileReadLines(raw,size);
	free(raw);
	return result;
}

/**
  *	le todas as linhas do arquivo, criando uma lista com cada uma delas
  */
list<string> fileReadLines(FILE *file)
{
	int size;
	char *raw = fileRead(file,&size,1);
	list<string> result = fileReadLines(raw,size);
	free(raw);
	return result;
}

/**
  *	le todas as linhas do arquivo, criando uma lista com cada uma delas
  */
list<string> fileReadLines(char *raw, int size)
{
	list<string> list;
	if(raw == NULL)
		return list;
	raw[size] = '\0';
	string text (raw);
	list = stringSplitChar(text,"\n\r");

	return list;
}

/**
  *	le todos os bytes do arquivo, escrevendo seu tamanho em size
  * extra indica quantos bytes a mais serao deixados no final do arquivo (nao eh somado a size)
  */
char *fileRead(const char *filename,int *size,int extra)
{
	FILE *fl = fopen(filename,"rb");
	char *data = fileRead(fl,size,extra);
	fclose(fl);
	return data;
}

/**
  *	le todos os bytes do arquivo, escrevendo seu tamanho em size
  * extra indica quantos bytes a mais serao deixados no final do arquivo (nao eh somado a size)
  */
char *fileRead(FILE *fl,int *size,int extra)
{
	if(fl == NULL)
	{
		*size = 0;
		return NULL;
	}

	fseek(fl,0,SEEK_END);
	*size = ftell(fl);
	fseek(fl,0,SEEK_SET);

	char *data = (char *)malloc(*size+extra);
	*size = fread(data,1,*size,fl);
	return data;

}
