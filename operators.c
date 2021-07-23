/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

/*
Operators: (upper ones have higher priority
()
* / %
+ -
<< >>
< <= > >=
= !=
XOR
AND
OR
*/

const unsigned char g_priority[]={
	0, // OP_VOID:
	1, // OP_OR:
	2, // OP_AND:
	3, // OP_XOR:
	4,4, // OP_EQ:
	5,5,5,5, // OP_LT:
	6,6, // OP_SHL:
	7,7, // OP_ADD:
	8,8,8 // OP_MUL:
};

int get_integer_operator(void){
	skip_blank();
	switch((source++)[0]){
		case '%': return OP_REM;
		case '/': return OP_DIV;
		case '*': return OP_MUL;
		case '-': return OP_SUB;
		case '+': return OP_ADD;
		case '>':
			if (source[0]=='>') {
				source++;
				return OP_SHR;
			} else if ('='==source[0]) {
				source++;
				return OP_MTE;
			} else {
				return OP_MT;
			}
		case '<':
			if (source[0]=='<') {
				source++;
				return OP_SHL;
			} else if ('='==source[0]) {
				source++;
				return OP_LTE;
			} else {
				return OP_LT;
			}
		case '!':
			if (source[0]!='=') break;
			source++;
			return OP_NEQ;
		case '=':
			if ('='==source[0]) source++;
			return OP_EQ;
		case 'X':
			if (source[0]!='O') break;
			if (source[1]!='R') break;
			source+=2;
			return OP_XOR;
			break;
		case 'O':
			if (source[0]!='R') break;
			source++;
			return OP_OR;
		case 'A':
			if (source[0]!='N') break;
			if (source[1]!='D') break;
			source+=2;
			return OP_AND;
		default:
			break;
	}
	source--;
	return ERROR_SYNTAX;
}

int get_float_operator(void){
	int e;
	e=get_integer_operator();
	if (e<0) return e;
	switch(e){
		// Following operators cannot be used for float values.
		case OP_XOR:
			source-=3;
			return ERROR_SYNTAX;
		case OP_REM:
			source--;
			return ERROR_SYNTAX;
		case OP_SHR:
		case OP_SHL:
			source-=2;
			return ERROR_SYNTAX;
		default:
			return e;
	}
}

int integer_calculation(int op){
	switch(op){
		case OP_OR:
			check_object(1);
			(object++)[0]=0x4308; // orrs	r0, r1
			return 0;
		case OP_AND:
			check_object(1);
			(object++)[0]=0x4008; // ands	r0, r1
			return 0;
		case OP_XOR:
			check_object(1);
			(object++)[0]=0x4048; // eors	r0, r1
			return 0;
		case OP_EQ:
			check_object(3);
			(object++)[0]=0x1a08; // subs	r0, r1, r0
			(object++)[0]=0x4243; // negs	r3, r0
			(object++)[0]=0x4158; // adcs	r0, r3
			return 0;
		case OP_NEQ:
			check_object(3);
			(object++)[0]=0x1a08; // subs	r0, r1, r0
			(object++)[0]=0x1e43; // subs	r3, r0, #1
			(object++)[0]=0x4198; // sbcs	r0, r3
			return 0;
		case OP_LT:
			check_object(7);
			(object++)[0]=0x0003; // movs	r3, r0
			(object++)[0]=0x17c8; // asrs	r0, r1, #31
			(object++)[0]=0x0fda; // lsrs	r2, r3, #31
			(object++)[0]=0x4299; // cmp	r1, r3
			(object++)[0]=0x4150; // adcs	r0, r2
			(object++)[0]=0x2301; // movs	r3, #1
			(object++)[0]=0x1a18; // subs	r0, r3, r0
			return 0;
		case OP_LTE:
			check_object(5);
			(object++)[0]=0x0003; // movs	r3, r0
			(object++)[0]=0x0fc8; // lsrs	r0, r1, #31
			(object++)[0]=0x17da; // asrs	r2, r3, #31
			(object++)[0]=0x428b; // cmp	r3, r1
			(object++)[0]=0x4150; // adcs	r0, r2
			return 0;
		case OP_MT:
			check_object(7);
			(object++)[0]=0x0003; // movs	r3, r0
			(object++)[0]=0x0fc8; // lsrs	r0, r1, #31
			(object++)[0]=0x17da; // asrs	r2, r3, #31
			(object++)[0]=0x428b; // cmp	r3, r1
			(object++)[0]=0x4150; // adcs	r0, r2
			(object++)[0]=0x2301; // movs	r3, #1
			(object++)[0]=0x1a18; // subs	r0, r3, r0
			return 0;
		case OP_MTE:
			check_object(5);
			(object++)[0]=0x0003; // movs	r3, r0
			(object++)[0]=0x17c8; // asrs	r0, r1, #31
			(object++)[0]=0x0fda; // lsrs	r2, r3, #31
			(object++)[0]=0x4299; // cmp	r1, r3
			(object++)[0]=0x4150; // adcs	r0, r2
			return 0;
		case OP_SHL:
			check_object(2);
			(object++)[0]=0x4081; // lsls	r1, r0
			(object++)[0]=0x0008; // movs	r0, r1
			return 0;
		case OP_SHR:
			check_object(2);
			(object++)[0]=0x40c1; // lsrs	r1, r0
			(object++)[0]=0x0008; // movs	r0, r1
			return 0;
		case OP_ADD:
			check_object(1);
			(object++)[0]=0x1808; // adds	r0, r1, r0
			return 0;
		case OP_SUB:
			check_object(1);
			(object++)[0]=0x1a08; // subs	r0, r1, r0
			return 0;
		case OP_MUL:
			check_object(1);
			(object++)[0]=0x4348; // muls	r0, r1
			return 0;
		case OP_DIV:
		case OP_REM:
			set_value_in_register(2,op);
			return call_lib_code(LIB_CALC);
		case OP_VOID:
		default:
			return ERROR_UNKNOWN;
	}
}

int float_calculation(int op){
	switch(op){
		case OP_EQ:
		case OP_NEQ:
		case OP_LT:
		case OP_LTE:
		case OP_MT:
		case OP_MTE:
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_OR:
		case OP_AND:
			set_value_in_register(2,op);
			return call_lib_code(LIB_CALC_FLOAT);
		case OP_XOR:
		case OP_REM:
		case OP_SHR:
		case OP_SHL:
		case OP_VOID:
		default:
			return ERROR_UNKNOWN;
	}
}

int get_operator(int vmode){
	switch(vmode){
		case VAR_MODE_INTEGER:
			return get_integer_operator();
		case VAR_MODE_FLOAT:
			return get_float_operator();
		default:
			return ERROR_UNKNOWN;
	}
}

int calculation(int op, int vmode){
	switch(vmode){
		case VAR_MODE_INTEGER:
			return integer_calculation(op);
		case VAR_MODE_FLOAT:
			return float_calculation(op);
		default:
			return ERROR_UNKNOWN;
	}
}