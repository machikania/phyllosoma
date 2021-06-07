/*
   This program is provided under the LGPL license ver 2.1
   KM-BASIC for ARM, written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
   https://github.com/kmorimatsu
*/

#include "./compiler.h"

int get_simple_integer(void){
	int i;
	skip_blank();
	if ('+'==source[0]) {
		source++;
	} else if ('-'==source[0]){
		source++;
		i=get_simple_integer();
		if (i) return i;
		check_object(1);
		(object++)[0]=0x4240; // negs	r0, r0
		return 0;
	}
	if ('$'==source[0] || '0'==source[0] && 'X'==source[1]) {
		// Hex value
		source+=('$'==source[0]) ? 1:2;
		i=0;
		while(1){
			if ('0'<=source[0] && source[0]<='9') {
				i=(i<<4)|(source[0]-'0');
				source++;
			} else if ('A'<=source[0] && source[0]<='F') {
				i=(i<<4)|(source[0]-'A'+10);
				source++;
			} else {
				break;
			}
		}
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<'9') {
		// Decimal value
		i=0;
		while(1){
			if ('0'<=source[0] && source[0]<='9') {
				i=i*10+(source[0]-'0');
				source++;
			} else {
				break;
			}
		}
		return set_value_in_register(0,i);
	} else if ('0'<=source[0] && source[0]<'9' || '_'==source[0]) {
		// Variable or function
		// TODO: here
		return ERROR_UNKNOWN;
	}
	return 0;
}

/*
100003bc:	1808      	adds	r0, r1, r0 // +
100003bc:	1a08      	subs	r0, r1, r0 // -
100003bc:	4348      	muls	r0, r1     // *

100003bc:	4008      	ands	r0, r1     // AND
100003bc:	4308      	orrs	r0, r1     // OR
100003bc:	4048      	eors	r0, r1     // XOR

// =
100003bc:	1a08      	subs	r0, r1, r0
100003be:	4243      	negs	r3, r0
100003c0:	4158      	adcs	r0, r3

// !=
100003bc:	1a08      	subs	r0, r1, r0
100003be:	1e43      	subs	r3, r0, #1
100003c0:	4198      	sbcs	r0, r3

//>=
100003bc:	0003      	movs	r3, r0
100003be:	17c8      	asrs	r0, r1, #31
100003c0:	0fda      	lsrs	r2, r3, #31
100003c2:	4299      	cmp	r1, r3
100003c4:	4150      	adcs	r0, r2

//<=
100003bc:	0003      	movs	r3, r0
100003be:	0fc8      	lsrs	r0, r1, #31
100003c0:	17da      	asrs	r2, r3, #31
100003c2:	428b      	cmp	r3, r1
100003c4:	4150      	adcs	r0, r2


*/
int get_integer(void){
	skip_blank();
	return get_simple_integer();
	return 0;
}
