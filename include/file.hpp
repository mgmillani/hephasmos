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

#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <list>

using namespace std;

/**
*	le todas as linhas do arquivo, criando uma lista com cada uma delas
*/
list<string> fileReadLines(FILE *file);
list<string> fileReadLines(const char *filename);
list<string> fileReadLines(char *raw, int size);

/**
*	le todos os bytes do arquivo, escrevendo seu tamanho em size
* extra indica quantos bytes a mais serao deixados no final do arquivo (nao eh somado a size)
*/
char *fileRead(const char *filename,int *size,int extra);
char *fileRead(FILE *filename,int *size,int extra);

#endif // FILE_HPP

