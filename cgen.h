/****************************************************/
/* File: cgen.h                                     */
/* The code generator interface                     */
/* for the C- compiler                              */
/* Adapted from:                                    */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

#define nlabel_size 3
#define ntemp_size 3

typedef enum {  opADD, opSUB, opMULT, opDIV, opLT, opLEQUAL, opGT, opGREQUAL, opAND, opOR, opASSIGN, opALLOC, opIMMED, opLOAD, opSTORE,
                opVEC, opGOTO, opIFF, opRET, opFUN, opEND, opPARAM, opCALL, opARG, opLAB, opHLT } OpKind;
typedef enum {  Empty, IntConst, String } AddrKind;

typedef struct {
  AddrKind kind;
  union {
    int val;
    struct {
      char * name;
      char * scope;
    } var;
  } contents;
} Address;

typedef struct {
  OpKind op;
  Address addr1, addr2, addr3;
} Quad;

typedef struct QuadListRec {
  int location;
  Quad quad;
  struct QuadListRec * next;
} * QuadList;

/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile);

QuadList getIntermediate();

#endif
