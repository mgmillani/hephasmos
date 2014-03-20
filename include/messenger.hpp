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

typedef struct s_status t_status;

#ifndef MESSENGER_HPP
#define MESSENGER_HPP

#include <string>
#include <list>
#include <map>

#include "instructions.hpp"
#include "addressings.hpp"
#include "registers.hpp"
#include "machine.hpp"
#include "labels.hpp"

#include "defs.hpp"

using namespace std;

//OBS: A ordem dos elementos eh MUITO importante e deve corresponder a do arquivo que possui as mensagens
typedef enum
{
	mBaseError,
	mBaseWarning,
	mUnknownInstruction,
	mIncorrectOperands,
	mLargeRelativeAddress,
	mUnkownAddressing,
	mRedefinedLabel,
	mUndefinedLabel,
	mOverwritenRegion,
	mUnusedLabel

}e_messages;

typedef struct s_message
{
	string message;
	bool error;
}t_message;

class Messenger
{

	public:

	/**
	  *	inicializa sem nenhuma mensagem
	  */
	Messenger(FILE *warningStream = stderr, FILE *errorStream = stderr);
	/**
	  * carrega as mensagens e inicializa
	  */
	Messenger(const char *filename,FILE *warningStream = stderr, FILE *errorStream = stderr);
	Messenger(FILE *filename,FILE *warningStream = stderr, FILE *errorStream = stderr);

	void init(FILE *fl,FILE *warningStream, FILE *errorStream);

	~Messenger();

	/**
	  *	carrega as mensagens de erro e avisos de um arquivo
	  */
	void load(const char *filename);
	void load(FILE *filename);

	/**
	  *	gera a mensagem com o codigo dado, escrevendo-a na stream adequada
	  *	usa as informacoes de status para gerar a mensagem
	  */
	void generateMessage(unsigned int code,t_status *status);

	/**
	  *	retorna o numero de erros que ocorreram
	  */
	unsigned int numberErrors();

	/**
    * converte uma excecao para seu respectivo codigo
    */
	static e_messages exceptionToMessage(e_exception e);

	private:

	/**
	  *	atualiza o valor de todas as variaveis utilizadas que estao em this->variables
	  */
	void updateVariables(t_status *status);

	map<unsigned int, t_message> msgs;

	map<string,string> variables;	//associa cada variavel a seu valor

	unsigned int errors;
	unsigned int warnings;

	FILE *errorStream;
	FILE *warningStream;

};

#endif // MESSENGER_HPP
