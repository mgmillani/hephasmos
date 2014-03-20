#include <stdio.h>

#include "types.hpp"
#include "debug.hpp"
/**
  * determina se A eh um subtipo de B
  */
bool isSubtype(e_type A, e_type B)
{

	if(B == TYPE_ANYTHING)
		return true;
	if(B == TYPE_ADDRESS)
		return A==TYPE_LABEL || A==TYPE_NUMBER || A==TYPE_ADDRESS;
	return A==B;

}

e_type mostStrictType(e_type A, e_type B)
{
	if(isSubtype(A,B))
		return A;
	else if(isSubtype(B,A))
		return B;
	else
		return TYPE_NONE;
}
