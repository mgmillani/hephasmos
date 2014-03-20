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
#include <string.h>
#include <math.h>

#include <string>

#include <boost/algorithm/string.hpp>

#include "sha1.hpp"

#include "instructions.hpp"
#include "addressings.hpp"
#include "assembler.hpp"
#include "registers.hpp"
#include "numbers.hpp"
#include "messenger.hpp"
#include "stringer.hpp"
#include "machine.hpp"
#include "memory.hpp"

#include "defs.hpp"
#define TRACE_OFF
//#define ERR_OFF
#include "debug.hpp"

using namespace std;

/**
*	cria um montador, especificando quais sao as propriedades da maquina/arquiterua para qual os codigos serao montados
*/
//Assemlber::Assembler(Instructions *inst, Registers *reg, Machine *machine, Adressing *adr);

void Assembler::init(FILE *fl,Messenger messenger)
{
	if(fl == NULL)
		throw eFileNotFound;
	else
	{
		this->messenger = messenger;
		int size;
		fseek(fl,0,SEEK_END);
		size = ftell(fl);
		char *data = (char *)malloc(size+1);
		fseek(fl,0,SEEK_SET);
		size = fread(data,1,size,fl);
		data[size] = '\0';

		string text (data);

		list<string> lines = stringSplitChar(text,"\n\r");
		list<string>::iterator it;

		e_category category = CAT_NONE;
		for(it=lines.begin() ; it!=lines.end() ; it++)
		{
			string line = *it;
			for(unsigned int i=0 ; i<line.size() ; i++)
			{
				char c = line[i];

				//ignora caracteres em branco
				if(c==' ' || c=='\t')
					continue;

				//comentario
				if(c=='#')
					break;

				//se for um '[', eh a definicao de uma categoria
				if(c=='[')
				{
					//busca o ']'
					i++;
					int start = i;
					while(i<line.size() && line[i]!=']')
						i++;
					if(line[i]!=']')
						throw eInvalidFormat;

					string catName = stringTrim(string(line,start,i-start)," \t");
					if(stringCaselessCompare(catName,"instructions")==0)
						category = CAT_INST;
					else if(stringCaselessCompare(catName,"addressings")==0)
						category = CAT_ADDR;
					else if(stringCaselessCompare(catName,"machine")==0)
						category = CAT_MACH;
					else if(stringCaselessCompare(catName,"registers")==0)
						category = CAT_REGI;
					else
						category = CAT_NONE;
				}
				else if(category == CAT_INST)
				{
					this->inst.load(line);
					break;
				}
				else if(category == CAT_ADDR)
				{
					this->addr.load(line);
					break;
				}
				else if(category == CAT_REGI)
				{
					this->regs.load(line);
					break;
				}
				else if(category == CAT_MACH)
				{
					this->mach.load(line);
					break;
				}
			}
		}
		free(data);
	}
}

/**
*	le as caracteristicas da arquitetura que estao no arquivo dado
*/
Assembler::Assembler(const char *filename,Messenger messenger)
{
	FILE *fl = fopen(filename,"rb");
	this->init(fl,messenger);
	fclose(fl);
}
Assembler::Assembler(FILE *file,Messenger messenger)
{
	this->init(file,messenger);
}

/**
*	monta o codigo assembly passado
* retorna a memoria gerada
*/
Memory Assembler::assembleCode(string code)
{
	//quebra as linhas
	list<string> lines = stringGetLines(code);

	//aloca espaco suficiente para a memoria
	//int size = pow(2,this->mach.getPCSize());
	int size = 2<<(this->mach.getPCSize()-1);
	Memory memory(size,this->mach.bigEndian);

	unsigned int pos = 0;

	list<string>::iterator it;
	unsigned int line=1;
	//monta cada linha
	t_status status;
	for(it=lines.begin() ; it!=lines.end() ; it++,line++)
	{
		pos = this->assembleLine(*it,&memory,pos,line,&status);
	}

	//resolve as pendencias
	while(!this->pendecies.empty())
	{
		t_pendency pend = this->pendecies.top();
		this->pendecies.pop();

		//recalcula os valores dos operandos
		try
		{
			list<t_operand> operands = this->recalculateOperands(pend.operands,pend.status,pend.byte);
			string binFormat = pend.binFormat;
			unsigned int size = pend.size;
			unsigned int pos = pend.byte;

			string result = replaceOperands(binFormat, operands, size,pend.byte);
			memory.writeNumber(result,pos,-1);

		}
		catch(e_exception e)
		{
			this->messenger.generateMessage(Messenger::exceptionToMessage(e),pend.status);
		}
		//free(pend.status->label);
		free(pend.status->mnemonic);
		free(pend.status->operand);
		free(pend.status);
	}
	return memory;
}


/**
  *	monta uma linha, escrevendo seu codigo binario a partir de memory[byte]
  * line eh a linha a ser montada
  * se houver alguma label que ainda nao foi definida, reserva espaco e adiciona a pendencia na pilha
  * se for encontrada a definicao de uma label, acrescenta-a as Labels conhecidas
  * retorna a posicao da memoria em que a proxima linha deve comecar
  */
unsigned int Assembler::assembleLine(string line, Memory *memory,unsigned int byte,unsigned int lineNumber,t_status *status)
{
	status->value = 0;
	status->lastOrgLine = 0;
	status->position = 0;
	status->foundOperands = 0;
	status->operandSize = 0;
	status->line = lineNumber;
	status->position = byte;
	string defLabel;
	string mnemonic;
	string operands;
	this->parseLine(line,&defLabel,&mnemonic,&operands);
	//ERR("parseLine: '%s' '%s'\n",mnemonic.c_str(),operands.c_str());
	boost::to_upper(mnemonic);
	status->firstDefinition = 0;
	status->label = (char *)defLabel.c_str();
	status->mnemonic = (char *)mnemonic.c_str();
	status->operand = (char *)operands.c_str();
	status->operandFormat = "";

	//define a label
	if(defLabel!="")
	{
		try
		{
			this->labels.define(defLabel,status->position,status->line);
		}
		catch(e_exception e)
		{
			if(e == eRedefinedLabel)
			{
				status->firstDefinition = this->labels.line(defLabel);
				this->messenger.generateMessage(mRedefinedLabel,status);
			}
		}
	}
	//se for uma instrucao, monta-a
	//string a = mnemonic;
	if(mnemonic == "")
		return byte;
	if(this->inst.isInstruction(mnemonic))
	{
		TRACE("Instruction: %s(%s)\n",mnemonic.c_str(),operands.c_str());
		try
		{
			byte+=this->inst.assemble(mnemonic,operands,memory,byte,&this->pendecies,this->addr,this->labels,this->regs,status);
		}
		catch(e_exception e)
		{
			list<t_instruction> insts = this->inst.getInstructions(mnemonic);
			list<t_instruction>::iterator it;
			for(it=insts.begin() ; it!=insts.end() ; it++)
			{
				status->operandFormat = (char *)it->operandExpression.c_str();
				this->messenger.generateMessage(Messenger::exceptionToMessage(e),status);
			}
		}
	}
	//executa a diretiva
	else
	{
		TRACE("Directive: %s\n",mnemonic.c_str());
		try
		{
			byte = this->directives.execute(mnemonic,operands,this->labels,&this->pendecies,memory,byte,status);
		}
		catch(e_exception e)
		{
			this->messenger.generateMessage(Messenger::exceptionToMessage(e),status);
		}
	}

	return byte;
}

/**
*	cria o arquivo binario para a memoria
* o arquivo tera o seguinte formato:
* primeiro byte: versao (0, no caso)
* nome da maquina, terminado por um '\0'
* dump da memoria (size bytes)
* SHA1 do resto do arquivo (20 bytes)
*/
void Assembler::createBinaryV0(string filename,Memory *memory)
{
	FILE *fl = fopen(filename.c_str(),"wb");
	this->createBinaryV0(fl,memory);
	fclose(fl);
}
void Assembler::createBinaryV0(FILE *fl,Memory *memory)
{
	string machineName = this->mach.name;
	//escreve a versão
	char version = 0;
	fwrite(&version,1,1,fl);
	//nome da maquina
	fwrite(machineName.c_str(),1,machineName.size(),fl);
	//termina com '\0'
	char zero = '\0';
	fwrite(&zero,1,1,fl);
	//calcula o SHA1 do arquivo
	//comeca concatenando o arquivo
	unsigned int size;
	unsigned char *memPack = memory->pack(&size);
	char *cat = (char *)malloc(1+machineName.size()+1+size);
	unsigned int pos=0;
	cat[pos++] = 0;
	memcpy(cat+pos,machineName.c_str(),machineName.size());
	pos+=machineName.size();
	cat[pos++]='\0';

	//copia a memoria compactada

	memcpy(cat+pos,memPack,size);
	pos+=size;

	//calcula o SHA1
	SHA1 *shaCalc = new SHA1();
	shaCalc->Input(cat,pos);
	unsigned int *sha = (unsigned int *)malloc(20);	//SHA1 = 160 bits = 20 bytes
	shaCalc->Result(sha);

	//faz um dump da memoria
	fwrite(memPack,1,size,fl);

	//escreve o SHA1
	fwrite(sha,1,20,fl);

	delete shaCalc;
	free(cat);
	free(sha);


}

/**
	* cria o arquivo binario no formato do Daedalus
	* o formato eh o seguinte:
	* primeiro byte: 3
	* proximos 3 bytes: nome da maquina, em maiusculas
	* resto: dump da memoria, sendo que cada valor eh um inteiro de 16 bit em notacao little-endian
	*/
void Assembler::createBinaryV3(FILE *fl,Memory *memory)
{
	const char *machineName = this->mach.name.c_str();
	const char *alias = NULL;
	if(strcmp("neander",machineName)==0)
		alias = "NEA";
	else if(strcmp("ahmes",machineName)==0)
		alias = "AHM";
	else if(strcmp("ramses",machineName)==0)
		alias = "RMS";
	else if(strcmp("cesar",machineName)==0)
		alias = "C16";
	else
		throw eInvalidMachine;

	if(fl == NULL)
		throw eFileNotFound;
	//escreve o 3
	unsigned char data[2];
	data[0] = 3;
	fwrite(data,1,1,fl);
	//escreve a sigla
	fwrite(alias,1,3,fl);

	data[1] = 0;
	unsigned int size;
	unsigned int i;
	unsigned char *mem = memory->pack(&size);
	for(i=0 ; i<size ; i++)
	{
		data[0] = mem[i];
		fwrite(data,1,2,fl);
	}
}

/**
  * escreve todos os simbolos declarados no codigo montado
  */
void Assembler::createSymTable(FILE *fl)
{
	//determina a largura da maior label (em caracteres)
	list<t_label> lst;
	list<t_label>::iterator it;
	unsigned int largest = 0;
	for(it=lst.begin() ; it!=lst.end() ; it++)
	{
		if(it->name.size() > largest)
			largest = it->name.size();
	}

	unsigned int margin = 8;

	for(it=lst.begin() ; it!=lst.end() ; it++)
	{
		fprintf(fl,"%s",it->name.c_str());
		fprintf(fl,"\t%u\n",it->pos);
	}

}

/**
* faz o parsing de uma linha, escrevendo a label definida em defLabel,
* o mnemonico da linha em mnemonic e os operandos em operands
*/
void Assembler::parseLine(string line,string *defLabel, string *mnemonic, string *operands)
{
	typedef enum {STATE_INI,STATE_FIRST_WORD,STATE_FIRST_END,STATE_LABEL,STATE_INST,STATE_INST_END,STATE_OPERAND} e_states;

	e_states state = STATE_INI;

	unsigned int i;
	unsigned int b=0;
	bool read = false;	//indica se uma palavra esta sendo lida ou nao
	for(i=0 ; i<line.size() ; i++)
	{
		char c = line[i];
		if(c == ';')
			break;

		switch(state)
		{
			case STATE_INI:
				if(!ISWHITESPACE(c))
				{
					state = STATE_FIRST_WORD;
					b = i;
					read = true;
				}
				break;

			case STATE_FIRST_WORD:
				//a primeira palavra eh uma instrucao ou diretiva
				if(ISWHITESPACE(c))
				{
					state = STATE_FIRST_END;
					*mnemonic = line.substr(b,i-b);
					read = false;
				}
				//eh uma label
				else if(c==':')
				{
					state = STATE_LABEL;
					*defLabel = line.substr(b,i-b);
					read = false;
				}
				break;
			//le os espacos em branco ate encontrar outro caractere
			case STATE_FIRST_END:
				if(!ISWHITESPACE(c))
				{
					state = STATE_OPERAND;
					b = i;
					read = true;
				}
				break;
			//a definicao de uma label foi lida
			//a proxima palavra eh um instrucao ou diretiva
			//le os espacos em branco
			case STATE_LABEL:
				if(!ISWHITESPACE(c))
				{
					state = STATE_INST;
					b = i;
					read = true;
				}
				break;
			//inicio de uma instrucao ou diretiva
			case STATE_INST:
				if(ISWHITESPACE(c))
				{
					state = STATE_INST_END;
					*mnemonic = line.substr(b,i-b);
					read = false;
				}
				break;
			//le os espacos em branco apos uma instrucao
			case STATE_INST_END:
				//a proxima palavra eh um operando
				if(!ISWHITESPACE(c))
				{
					state = STATE_OPERAND;
					b = i;
					read = true;
				}
				break;
			case STATE_OPERAND:

				break;
		}
	}

	//if we still need to read a word
	if(read)
	{
		//removes whitespaces
		if(ISWHITESPACE(line[i-1]))
		{
			i--;
			while(ISWHITESPACE(line[i]))
				i--;
			i++;
		}

		if(*mnemonic == "")
			*mnemonic = line.substr(b,i-b);
		else
			*operands= line.substr(b,i-b);
	}
}

/**
  * recalcula o valor das labels, retornando uma nova lista de operandos
  */
list<t_operand> Assembler::recalculateOperands(list<t_operand> operands,t_status *status,unsigned int pos)
{
	list<t_operand>::iterator ot;
	list<t_operand> result;
	for(ot=operands.begin() ; ot!=operands.end() ; ot++)
	{
		t_operand o = *ot;
		if(o.type == TYPE_LABEL)
		{
			status->label = (char *)o.name.c_str();
			o.value = Number::toBin(this->labels.value(o.name));
			Operands::solveOperation(&o,this->labels,status);
		}
		else if(o.aritOperandType == TYPE_LABEL)
		{
			status->label = (char *)o.aritOperand.c_str();
			Operands::solveOperation(&o,this->labels,status);
		}
		result.push_back(o);
	}

	return result;
}

/**
	* se ocorreu algum erro durante o processo de montagem
	*/
bool Assembler::hasErrors()
{
	return this->messenger.numberErrors() > 0;
}

/**
	* escreve a situacao atual dos atributos do objeto
	*/
void Assembler::print(FILE *stream)
{

	//lista as instrucoes definidas
	const char instStr[] = "Instructions:\n";
	fwrite(instStr,1,sizeof(instStr),stream);
	this->inst.print(stream);

	//lista os modos de enderecamento
	const char addrStr[] = "\n\nAddressings:\n";
	fwrite(addrStr,1,sizeof(addrStr),stream);
	this->addr.print(stream);

	//lista os registradores
	const char regStr[] = "\n\nRegisters:\n";
	fwrite(regStr,1,sizeof(regStr),stream);
	this->regs.print(stream);

}






