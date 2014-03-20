#ifndef TYPES_HPP
#define TYPES_HPP

typedef enum {TYPE_REGISTER,TYPE_ADDRESS,TYPE_LABEL,TYPE_NUMBER,TYPE_ANYTHING,TYPE_ADDRESSING,TYPE_NONE,TYPE_TOTAL} e_type;

/**
  * determina se A eh um subtipo de B
  */
bool isSubtype(e_type A, e_type B);

/**
  * A, if A <= B
  * B, if B < A
  * TYPE_NONE, otherwise
  */
e_type mostStrictType(e_type A, e_type B);

#endif // TYPES_HPP

