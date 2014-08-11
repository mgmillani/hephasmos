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

#define ANY(a,b,c) (strcmp(a,b)==0 || strcmp(a,c)==0)

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
	bool nonhuman = false;
	bool help = false;
	bool showVersion = false;

	char *versionNames[] = {"daedalus","mecaria0"};
	int versionNumbers[] = {3,0};
	unsigned int numVersions = sizeof(versionNames) / sizeof(*versionNames);

	int i;
	//parses the command line arguments
	char *opt;
	for(i=1 ; i<argc-1 ; i++)
	{
		opt = argv[i];
		FILE *file = NULL;
		bool opened = true;

		if(ANY(opt,"--errors","-e"))
			file = errors = fopen(argv[++i],"wb");
		else if(ANY(opt,"--input","-i"))
			file = source = fopen(argv[++i],"rb");
		else if(ANY(opt,"--machine","-m"))
			file = machine = fopen(argv[++i],"rb");
		else if(ANY(opt,"--non-human","-n"))
		{
			nonhuman = true;
			opened = false;
		}
		else if(ANY(opt,"--messages","-g"))
			file = messages = fopen(argv[++i],"rb");
		else if(ANY(opt,"--output","-o"))
			file = output = fopen(argv[++i],"wb");
		else if(ANY(opt,"--symbols","-s"))
			file = symbols = fopen(argv[++i],"wb");
		else if(ANY(opt,"--type","-t"))
		{
			opened = false;
			//finds the version name
			char *value = argv[++i];
			void *key = bsearch(&value, versionNames, numVersions , sizeof(*versionNames), (int (*)(const void*, const void*)) ( (int (*)(const char**, const char**))stringCaselessCompare));

			if(key == NULL)
			{
				ERR("'%s' is not a valid output type.",value);
				failed = true;
			}
			else
				version = versionNumbers[ ((char *)key - (char *)versionNames) / sizeof(key)];
		}
		else if(ANY(opt,"--warnings","-w"))
			file = warnings = fopen(argv[++i],"wb");
		else if(ANY(opt,"--help","-h"))
		{
			help = true;
			opened = false;
		}
		else if(ANY(opt,"--version","-v"))
		{
			opened = false;
			showVersion = true;
		}
		else
		{
			ERR("Invalid option: %s\n",opt);
			opened = false;
			failed = true;
		}

		if(file == NULL && opened)
		{
			ERR("Failed to open:'%s' (%d)\n",argv[i],errno);
			failed = true;
		}
	}

	if( i<argc)
	{
		opt = argv[argc-1];
		bool needArg = false;
		// last option does not have an argument
		if(ANY(opt,"--errors","-e"))
			needArg = true;
		else if(ANY(opt,"--input","-i"))
			needArg = true;
		else if(ANY(opt,"--machine","-m"))
			needArg = true;
		else if(ANY(opt,"--non-human","-n"))
			nonhuman = true;
		else if(ANY(opt,"--messages","-g"))
			needArg = true;
		else if(ANY(opt,"--output","-o"))
			needArg = true;
		else if(ANY(opt,"--symbols","-s"))
			needArg = true;
		else if(ANY(opt,"--type","-t"))
			needArg = true;
		else if(ANY(opt,"--warnings","-w"))
			needArg = true;
		else if(ANY(opt,"--help","-h"))
			help = true;
		else if(ANY(opt,"--version","-v"))
		{
			needArg = false;
			showVersion = true;
		}
		else
		{
			ERR("Invalid option: %s\n",opt);
			failed = true;
		}

		if(needArg)
		{
			ERR("Option %s expects an argument, but none was given.\n",opt);
			failed = true;
		}
	}

	//shows help message
	if(help)
	{
		ERR("\nusage: hephasmos [OPTIONS...] --machine FILE --messages FILE\n");
		ERR("\noptions:\n");
		ERR("-h,--help          \tExplains how to use the program and exits\n");
		ERR("-v,--version       \tExplains how to use the program and exits\n");
		ERR("-e,--errors FILE   \tWhere will error messages be written to. (default: standard error)\n");
		ERR("-i, --input FILE   \tSource file (default: standard input)\n");
		ERR("-m, --machine FILE \tFile which describes the machine (required)\n");
		ERR("-g, --messages FILE\tFile containing error and warning messages (required)\n");
		ERR("-n, --non-human    \tIf set, error messages will be printed one perline, and will not be escaped\n");
		ERR("-o, --output FILE  \tOutput file, with the assembled source code (default: standard output)\n");
		ERR("-s, --symbols FILE \tSymbols file (default: none)\n");
		ERR("-t, --type NAME    \tType of output file. Possible values: ");
		for(int i=0 ; i<numVersions-1 ; i++)
			ERR("%s, ", versionNames[i]);
		ERR("%s\n", versionNames[numVersions-1]);
		ERR("-w, --warnings FILE\tWhere will warning messages be written to (default: standard error)\n");
	}
	// assembles the source code
	else if(machine != NULL && messages != NULL && !failed)
	{
		Messenger messenger(messages,warnings,errors,nonhuman);
		Assembler assembler(machine,messenger);

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
	else if(!showVersion)
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
