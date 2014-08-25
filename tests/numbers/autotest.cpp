#include <stdio.h>
#include <string.h>

#include "numbers.hpp"

#include "debug.hpp"

int convertDigits()
{
	string in[] =  {"00", "h00", "00h", "00d"};
	int size = 2;
	unsigned char out[][size] = {{0,0}, {0,0} , {0,0} , {0,0}};
	int tests = sizeof(in) / sizeof(*in);
	int fail = 0;

	for(int i=0 ; i<tests ; i++)
	{
		unsigned char result[size] = {1,1};
		Number::convertDigits(in[i], result, Number::numberType(in[i]));
		if(memcmp(out,result, size) != 0)
		{
			ERR("convertDigits(%s):\n",in[i].c_str());
			ERR("\tGave: ");
			for(int j=0 ; j<size ; j++)
			{
				ERR("%d ",result[j]);
			}
			ERR("\n\tExpected: ");
			for(int j=0 ; j<size ; j++)
			{
				ERR("%d ",out[i][j]);
			}
			fail = 1;
		}
	}

	return fail;
}

int getMinDigits()
{
	string in[] =  {"00", "h00", "00h", "00d"};
	string out[] = {"0", "0" , "0" , "0" };
	int tests = sizeof(in) / sizeof(*in);
	int fail = 0;

	for(int i=0 ; i<tests ; i++)
	{
		//Number n = Number(in[i]);
		string result = Number::getMinDigits(in[i]);
		if(result.compare(out[i]) != 0)
		{
			ERR("getMinDigits(%s):\n\tGave: %s\n\tExpected:%s\n",in[i].c_str(),result.c_str(), out[i].c_str());
			fail = 1;
		}
	}

	return fail;
}


int toBin()
{
	string in[] =  {"00", "h00", "00h", "00d"};
	string out[] = {"0", "0" , "0" , "0" };
	int tests = sizeof(in) / sizeof(*in);

	int fail = 0;

	for(int i=0 ; i<tests ; i++)
	{
		Number n = Number(in[i]);
		string result = n.toBin();
		if(Number::numberType(result) != BINARY)
		{
			ERR("%s.toBin():\n\tGave: %s\n\twhich is not a binary number\n",in[i].c_str(),result.c_str());
			fail = 1;
		}
		else if(Number::getMinDigits(result).compare(out[i]) != 0)
		{
			ERR("%s.toBin():\n\tGave: %s\n\tExpected:%s\n",in[i].c_str(),result.c_str(), out[i].c_str());
			fail = 1;
		}
	}

	return fail;
}

int main()
{

	int result,fail=0;

	result = convertDigits();
	if(result != 0)
		fail=1;

	result = toBin();
	if(result != 0)
		fail=1;

	result = getMinDigits();
	if(result != 0)
		fail=1;

	if(fail)
	{
		ERR("Some tests failed\n");
		return 1;
	}
	else
	{
		ERR("All tests passed\n");
		return 0;
	}


}
