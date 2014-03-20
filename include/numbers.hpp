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

#ifndef NUMBERS_HPP
#define NUMBERS_HPP

#include <string>

using namespace std;

typedef enum {BINARY,DECIMAL,HEXADECIMAL,PRE_HEXADECIMAL,INVALID} e_numType;

class Number
{
	public:
	Number();
	/**
	  * inicializa o numero a partir de uma string binaria, decimal ou hexadecimal
	  */
	Number(string n);
	Number(unsigned int n);
	Number(int n);

	~Number();

	/**
	  * aplica a operacao dada usando n como o segundo operando
	  * escreve o resultado neste objeto
	  */
	void operate(char operation,Number &n);

	/**
	  *	converts a number to an int
	  *	if it is too big, it will be truncated, returning only the least-significative bits
	  * the type is given by the last character
	  * b/B - binary
	  * d/D - decimal
	  * h/H - hexadecimal
	  * nothing/algarism - decimal
	  * TODO: If compatibilty-mode is active, if the first character is h/H, the number is regarded as hexadecimal
	  */
	static int toInt(string n);

	/**
	  * converte a string do numero dado para uma string binaria (terminada com 'b')
	  */
	static string toBin(string n);
	static string toBin(unsigned int n);

	/**
	  * converte o numero do objeto para uma string binaria (terminada por 'b')
	  */
	string toBin();

	/**
	  * converte uma string de caracteres para um numero binario (terminado por 'b')
	  * cada carcatere sera convertido para 8 bits na ordem em que aparecer
	  */
	static string stringToBin(string str);

	/**
	  * escreve o valor binario de um digito hexadecimal em dest
	  */
	static void writeHexaDigitAsBin(char digit, char *dest);

	/**
	  * retorna um string contendo apenas os digitos do numero, sem 0s a esquerda
	  * e sem indicativos de sua base
	  */
	static string getMinDigits(string n);

	/**
	  * faz o complemento de 2 do numero, ou seja, troca o sinal
	  */
	void baseComplement();

	/**
	  *	converte o numero para um array de bytes com notacao little-endian
	  *	o ultimo caractere determina o tipo do numero:
	  *	b/B - binario
	  *	h/H - hexadecimal
	  *	nada/algarismo - binario
	  *	escreve o numero de bytes do numero em size
	  * nao suporta numeros decimais
	  */
	static unsigned char *toByteArray(string n, unsigned int *size);

	/**
	  * determina o tipo do numero (decimal, binario ou hexadecimal),retornando-o
	  * retorna INVALID se o numero nao estiver no formato adequado
	  */
	static e_numType numberType(string n);

	/**
	  * converte os caracteres do numero para seus respectivos valores
	  * o vetor values deve ser grande o suficiente
	  * o numero n deve ser valido
	  */
	static void convertDigits(string n, unsigned char *values,e_numType type);

	/**
	  * verifica se o dado numero eh valido
	  */
	static bool exists(string number);

	/**
	  * verifica se o elemento passado eh uma string valida
	  */
	static bool isString(string element);

	unsigned int numDigits;
	unsigned char *digits; //os digitos do numero, em complemento de 256, notacao little-endian

};

#endif // NUMBERS_HPP

