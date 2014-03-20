/**
* Copyright 2013, 2014 Marcelo Millani
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
#include <string.h>
#include <errno.h>

#include "messenger.hpp"
#include "assembler.hpp"
#include "file.hpp"

#include "debug.hpp"

using namespace std;

bool openFile(const char *fname, FILE **fl, const char *mode)
{
	*fl = fopen(fname,mode);
	if(fl == NULL)
	{
		ERR("Error: failed to open %s (%d)\n",fname,errno);
		return true;
	}
	return false;
}

int cmp(int *a, int *b)
{
	return *a-*b;
}

int main(int argc,char *argv[])
{
	bool help = false;
	bool failed = false;
	int exitValue = 0;

	FILE *source = stdin;
	FILE *output = stdout;
	FILE *errors = stderr;
	FILE *warnings = stderr;
	FILE *symbols = NULL;
	FILE *machine = NULL;
	FILE *messages = NULL;
	unsigned int version = 0;

	char *versionNames[] = {"daedalus","mecaria0"};
	int versionNumbers[] = {3,0};
	unsigned int numVersions = sizeof(versionNames) / sizeof(*versionNames);

	int i;
	//parses the command line arguments
	for(i=1;i<argc;i++)
	{
		char *opt = argv[i];
		char *left = strtok(opt,"=");
		char *right = strtok(NULL,"");
		bool opened = true;
		FILE *file = NULL;

		if(strcmp(left,"source") == 0)
			file = source = fopen(right,"rb");
		else if(strcmp(left,"output") == 0)
			file = output = fopen(right,"wt");
		else if(strcmp(left,"warnings") == 0)
			file = warnings = fopen(right,"wt");
		else if(strcmp(left,"errors") == 0)
			file = errors = fopen(right,"wt");
		else if(strcmp(left,"symbols") == 0)
			file = symbols = fopen(right,"wt");
		else if(strcmp(left,"machine") == 0)
			file = machine = fopen(right,"rb");
		else if(strcmp(left,"messages") == 0)
			file = messages = fopen(right,"rb");
		else if(strcmp(left,"version") == 0)
		{
			opened = false;
			//finds the version number
			unsigned int i;

			void *key = bsearch(&right, versionNames, numVersions , sizeof(*versionNames), (int (*)(const void*, const void*)) ( (int (*)(const char**, const char**))stringCaselessCompare));

			if(key == NULL)
			{
				ERR("'%s' is not a valid version name.",right);
				failed = true;
				help = true;
			}
			else
				version = versionNumbers[(char *)versionNames - (char *)key];
		}
		else
		{
			ERR("Invalid option: %s\n",left);
			help = true;
			opened = false;
			failed = true;
		}

		if(file == NULL && opened)
		{
			ERR("Failed to open:'%s' (%d)\n",right,errno);
			failed = true;
		}
	}

	//shows help message
	if(help)
	{
		ERR("\n\nusage: hidrassembler [OPTIONS...] machine=<machine> messages=<messages>\n");
		ERR("where:\n");
		ERR("machine=<machine>\tloads the specifications for the assembler from the file <machine>\n");
		ERR("messages=<messages>\tloads errors and warnings from <messages>\n");
		ERR("\navailable options:\n");
		ERR("source=<filename>\t source file <filename> will be used instead of stdin\n");
		ERR("output=<filename>\t generated binary will be written to <filename> instead of stdout\n");
		ERR("warnings=<filename>\t generated warnings will be written to <filename> instead of stderr\n");
		ERR("errors=<filename>\t generated errors will be written to <filename> instead of stderr\n");
		ERR("symbols=<filename>\t defined symbols will be written to <filename>\n");
		ERR("version=<name>\t creates a binary file for the <name> version. Available values: Daedalus, Mecamiria0\n");
	}
	// assembles the source code
	else if(machine != NULL && messages != NULL && !failed)
	{
		Messenger messenger(messages,warnings,errors);
		Assembler assembler(machine,messenger);

		// assembler.print(stderr);

		int size;
		char *codeP = fileRead(source,&size,1);
		codeP[size] = '\0';
		string code(codeP);

		Memory mem = assembler.assembleCode(code);

		if(!assembler.hasErrors())
		{
			if(version == 3)
				assembler.createBinaryV3(output,&mem);
			else if(version == 0)
				assembler.createBinaryV0(output,&mem);

			//creates symbol table
			if(symbols != NULL)
				assembler.createSymTable(symbols);
		}
		else
			exitValue = -1;

		free(codeP);
	}
	else
	{
		if(machine == NULL)
			ERR("Error: the machine was not specified\n");
		if(messages == NULL)
			ERR("Error: message file was not specified\n");

		return -1;
	}

	if(source != NULL)
		fclose(source);
	if(output != NULL)
		fclose(output);
	if(errors != NULL)
		fclose(errors);
	if(warnings != NULL)
		fclose(warnings);
	if(symbols != NULL)
		fclose(symbols);
	if(machine != NULL)
		fclose(machine);
	if(messages != NULL)
		fclose(messages);

	return exitValue;

}
