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
#include <math.h>

#include <string>
#include <map>

#include "messenger.hpp"
#include "stringer.hpp"
#include "file.hpp"
#include "defs.hpp"

#include "debug.hpp"

//log10(2^s) = s*log10(2)
//+2 por causa do sinal e do \0
#define MAXDIGITS (unsigned int)(sizeof(int)*8*LOG102+2)


using namespace std;

/**
*	inicializa sem nenhuma mensagem
*/
Messenger::Messenger(FILE *warningStream, FILE *errorStream)
{
	this->errorStream = errorStream;
	this->warningStream = warningStream;
	this->msgs = map<unsigned int,t_message>();
	this->variables = map<string,string>();

	this->errors = 0;
	this->warnings = 0;
}

Messenger::~Messenger()
{

}

/**
*	inicializa e carrega as mensagens do arquivo
*/
Messenger::Messenger(FILE *fl,FILE *warningStream, FILE *errorStream)
{
	this->init(fl,warningStream,errorStream);
}

/**
*	inicializa e carrega as mensagens do arquivo
*/
Messenger::Messenger(const char *filename,FILE *warningStream, FILE *errorStream)
{
	FILE *fl = fopen(filename,"rb");
	this->init(fl,warningStream,errorStream);
	fclose(fl);

}

void Messenger::init(FILE *fl,FILE *warningStream, FILE *errorStream)
{
	this->errorStream = errorStream;
	this->warningStream = warningStream;

	this->msgs = map<unsigned int,t_message>();
	this->variables = map<string,string>();
	this->variables["\\n"] = "\n";
	this->variables["\\t"] = "\t";
	this->variables["\\r"] = "\r";
	this->errors = 0;
	this->warnings = 0;
	this->load(fl);
}

/**
*	carrega as mensagens de erro e avisos de um arquivo
*/
void Messenger::load(const char *filename)
{
	FILE *fl = fopen(filename,"rb");
	this->load(fl);
	fclose(fl);
}

/**
*	carrega as mensagens de erro e avisos de um arquivo
*/
void Messenger::load(FILE *filename)
{
	list<string> lines = fileReadLines(filename);

	list<string>::iterator it;

	for(it=lines.begin() ; it!=lines.end() ; it++)
	{
		string line = stringTrim(*it," \t");

		if(line[0] == '#')
			continue;
		else
		{
			bool error;
			unsigned int i;
			//determina o numero da mensagem
			for(i=0 ; i<line.size() && line[i]!=' ' && line[i]!='\t' ; i++)
				;
			//se a linha terminou
			if(i==line.size())
				continue;

			unsigned int num;
			sscanf(line.substr(0,i).c_str(),"%u",&num);

			//determina se eh um erro ou aviso
			while(i<line.size() && (line[i]==' ' || line[i]=='\t'))
				i++;
			//se a linha terminou
			if(i==line.size())
				continue;

			if(line[i]=='e')
				error = true;
			else if(line[i] == 'w')
				error = false;
			else
				continue;

			//determina o inicio da mensagem
			for(i++ ; i<line.size() && (line[i]==' ' || line[i]=='\t') ; i++)
				;
			//se a linha terminou
			if(i==line.size())
				continue;

			//o restante da linha eh a mensagem
			t_message msg;
			msg.message = line.substr(i,line.size()-i);
			msg.error = error;

			this->msgs.insert(pair<unsigned int,t_message>(num,msg));
		}
	}
}

/**
*	gera a mensagem com o codigo dado, escrevendo-a em stream
*	usa as informacoes de status para gerar a mensagem
*/
void Messenger::generateMessage(unsigned int code,t_status *status)
{
	this->updateVariables(status);

	map<unsigned int, t_message>::iterator it = this->msgs.find(code);
	string msg;
	if(it != this->msgs.end())
	{
		t_message message = it->second;
		string type;
		FILE *stream;
		//escreve se eh erro ou aviso
		if(message.error)
		{
			type = this->msgs.find(0)->second.message;
			this->errors++;
			stream = this->errorStream;
		}
		else
		{
			type = this->msgs.find(1)->second.message;
			this->warnings++;
			stream = this->warningStream;
		}

		msg = stringReplaceAll(type + message.message,this->variables) + '\n';

		fwrite(msg.c_str(),1,msg.size(),stream);
	}
	else
	{
		ERR("Unknown error!\n");
	}
}

/**
*	atualiza o valor de todas as variaveis utilizadas que esta em this->variables
*/
void Messenger::updateVariables(t_status *status)
{

	char buffer[MAXDIGITS];

	this->variables["$ADDRESSING_MODE"] = string(status->operand);

	sprintf(buffer,"%d",status->value);
	this->variables["$DISTANCE"] = string(buffer);

	//sprintf(buffer,"%d",status->operandFormat);
	this->variables["$OPERAND_FORMAT"] = string(status->operandFormat);

	sprintf(buffer,"%d",status->firstDefinition);
	this->variables["$FIRST_DEFINITION"] = string(buffer);

	//sprintf(buffer,"%d",status->foundOperands);
	this->variables["$FOUND_OPERANDS"] = string(status->operand);

	this->variables["$LABEL"] = string(status->label);

	sprintf(buffer,"%d",status->lastOrgLine);
	this->variables["$LAST_ORG_LINE"] = string(buffer);

	sprintf(buffer,"%d",status->line);
	this->variables["$LINE"] = string(buffer);

	this->variables["$MNEMONIC"] = string(status->mnemonic);

	int a = 2<<(status->operandSize-1);
	sprintf(buffer,"%d",a-1);
	this->variables["$S_OPERAND_MAX"] = string(buffer);

	sprintf(buffer,"-%d",a);
	this->variables["$S_OPERAND_MIN"] = string(buffer);
}

/**
*	retorna o numero de erros que ocorreram
*/
unsigned int Messenger::numberErrors()
{
	return this->errors;
}

/**
  * converte uma excecao para seu respectivo codigo
  */
e_messages Messenger::exceptionToMessage(e_exception e)
{
	switch(e)
	{
		case eUnknownMnemonic:
			return mUnknownInstruction;
		case eIncorrectOperands:
			return mIncorrectOperands;
		case eRedefinedLabel:
			return mRedefinedLabel;
		case eUndefinedLabel:
			return mUndefinedLabel;
		default:
			return mUnknownInstruction;
	}
}









