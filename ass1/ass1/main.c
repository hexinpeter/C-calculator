/*
 Extended precision integer calculation program
 Skeleton program written by Alistair Moffat, August 2013
 (Add your name and login name)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define INTSIZE	100	/* max number of digits per integer value */
#define LINELEN	102	/* maximum length of any input line */
#define NVARS	26	/* number of different variables */

#define CH_BLANK ' '    /* character blank */
#define CH_ZERO  '0'    /* character zero */
#define CH_A     'a'    /* character 'a', first variable name */

#define ERROR	(-1)	/* error return value from some functions */

#define TRUE 1

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define ALLOPS  "?=+"   /* list of all valid operators */
#define SGNCHRS "+-"    /* the two sign characters */
#define NUMCHRS "0123456789"
/* list of characters legal within numbers */



typedef struct{
    char value[INTSIZE+1]; //stores the value and '\0' at the end
    int length;
    int sign;
} longint_t;
/* one extended-precision "variable" */

/****************************************************************/

/* function prototypes */

void print_prompt(void);
int read_line(char *line, int maxlen);
void process_line(longint_t vars[], char *line);
int to_varnum(char ident);
int get_second_value(longint_t vars[], char *rhsarg,
                     longint_t *second_value);
int to_int(char digit);
char to_digit(int number);
void do_print(longint_t *var);
void do_assign(longint_t *var1, longint_t *var2);
void do_plus(longint_t *var1, longint_t *var2);
void zero_vars(longint_t vars[]);
void initstruct(longint_t *a);
void storevalue(longint_t *longvalue, char *string);
int num_check(char *string);
void reversecp(char *s1, char *s2);
/****************************************************************/


/* inintialise the longint_t struct */
void
initstruct(longint_t *a){
    a->value[0] = '\0';
    a->length = 0;
    a->sign = PLUS;
}



/* main program controls all the action
 */
int
main(int argc, char **argv) {
	char line[LINELEN+1];
	longint_t vars[NVARS]; // vars: a list of variables in the form of pointers
    
	zero_vars(vars);
	print_prompt();
	while (read_line(line, LINELEN)) {
		if (strlen(line)>0) {
			/* non empty line, so process it */
			process_line(vars, line);
		}
		print_prompt();
	}
    
	/* all done, so pack up and go home */
	printf("\n");
	return 0;
}

/****************************************************************/

/* prints the prompt indicating ready for input
 */
void
print_prompt(void) {
	printf("> ");
}

/****************************************************************/

/* read a line of input into the array passed as argument
 * returns false if there is no input available
 * all whitespace characters are removed
 * all input are stored in the string 'line'
 */
int
read_line(char *line, int maxlen) {
	int i=0, c;
	while (((c=getchar())!=EOF) && (c!='\n')) {
		if (i<maxlen && !isspace(c)) {
			line[i++] = c;
		}
	}
	line[i] = '\0';
	return ((i>0) || (c!=EOF));
}




/****************************************************************/

/* process a command by parsing the input line into parts
 */
void
process_line(longint_t vars[], char *line) {
	int varnum, optype, status;
	longint_t second_value;
    
	/* determine the LHS variable, it
	 * must be first character in line
	 */
	varnum = to_varnum(line[0]);
	if (varnum==ERROR) {
		printf("Invalid LHS variable\n");
		return;
	}
    
	/* more testing for validity
	 */
	if (strlen(line)<2) {
		printf("No operator supplied\n");
		return;
	}
    
	/* determine the operation to be performed, it
	 * must be second character in line
	 */
	optype = line[1];
	if (strchr(ALLOPS, optype) == NULL) {
		printf("Unknown operator\n");
		return;
	}
    
	/* determine the RHS argument (if one is required),
	 * it must start in third character of line
	 */
	if (optype != PRINT) {
		if (strlen(line)<3) {
			printf("No RHS supplied\n");
			return;
		}
		status = get_second_value(vars, line+2, &second_value);
        // status is only used to check the validity of the rhs value
        // the value is assigned to the second_value in the get_second_value function
		if (status==ERROR) {
			printf("RHS argument is invalid\n");
			return;
		}
	}
    
	/* finally, do the actual operation
	 */
	if (optype == PRINT) {
		do_print(vars+varnum);
	} else if (optype == ASSIGN) {
		do_assign(vars+varnum, &second_value);
	} else if (optype == PLUS) {
		do_plus(vars+varnum, &second_value);
	}
	return;
}

/****************************************************************/

/* convert a character variable identifier to a variable number
 */
int
to_varnum(char ident) {
	int varnum;
	varnum = ident - CH_A;
	if (0<=varnum && varnum<NVARS) {
		return varnum;
	} else {
		return ERROR;
	}
}

/****************************************************************/


/* store an integer string into longint_t longvalue */
void
storevalue(longint_t *longvalue, char *string){
    initstruct(longvalue);
    int i = 0; // a counter for longvalue.value
    
    if(ERROR == num_check(string))
        exit(EXIT_FAILURE);
    
    if (strchr(SGNCHRS, *string)) {
        longvalue->sign = *string++;
        for (; *string != '\0' && i < INTSIZE; string++, i++) {
            longvalue->value[i] = *string;
            longvalue->length++;
        }
        longvalue->value[i] = '\0';
    }
    
    else {
        for (; *string != '\0' && i < INTSIZE; string++, i++) {
            longvalue->value[i] = *string;
            longvalue->length++;
        }
        longvalue->value[i] = '\0';
    }
}

/* check that the string is a proper number string */
int
num_check(char *string){
    if (strchr(NUMCHRS, *string) == NULL && strchr(SGNCHRS, *string) == NULL)
        return ERROR;
    
    
    while (*(++string)) {
        if (strchr(NUMCHRS, *string) == NULL) {
            return ERROR;
        }
    }
    return TRUE;
}

/* process the input line to extract the RHS argument, which
 * should start at the pointer that is passed
 */
int
get_second_value(longint_t vars[], char *rhsarg,
                 longint_t *second_value) {
	int varnum2;
    
    // rhsarg is a proper number string
	if (TRUE == num_check(rhsarg)) {
        storevalue(second_value, rhsarg);        
		return TRUE;
	}
    
    /* argument is not a number, so might be a variable */
    else {
		varnum2 = to_varnum(*rhsarg);
		if (varnum2==ERROR || strlen(rhsarg)!=1) {
			return ERROR;
		}
		*second_value = vars[varnum2];
		return TRUE;
	}
    
	return ERROR;
}

/****************************************************************/

/* convert a character digit to the int equivalent, but null bytes
 * stay as zero integers
 */
int
to_int(char digit) {
	if (digit != '\0') {
		return digit - CH_ZERO;
	} else {
		return 0;
	}
}

/****************************************************************/

/* and back again to a digit */
char
to_digit(int number) {
	return number + CH_ZERO;
}


/****************************************************************/

/* print out a longint value
 */
void
do_print(longint_t *var) {
	printf("%s\n", var->value);
	return;
}

/****************************************************************/

/* update the indicated variable var1 by doing an assignment
 * using var2
 */
void
do_assign(longint_t *var1, longint_t *var2) {
	/* hey, this is trivial in this representation
	 */
	*var1 = *var2;
}

/****************************************************************/

/* stores the string s2 in s1 in a reverse order,s1 is assumed to have enough storage space */
void
reversecp(char *s1, char *s2){
    int s2len = (int)strlen(s2);
    //assert(s1[s2len] = 1);
    int i;
    for (i =0; s2len > 0; i++, s2len--) {
        s1[i] = s2[s2len -1];
    }
    s1[i] = '\0';
}


/* update the indicated variable var1 by doing an addition
 * using var2 to compute var1 = var1 + var2
 */
void
do_plus(longint_t *var1, longint_t *var2) {
    char result[INTSIZE+1];
    
    *result = 0;
    
    int u1 = var1->length, u2 = var2->length, i = 0; // i: counter on result
	
    // stores the resulting number in an array in a reverse order
    for (; (u1 > 0 || u2 > 0) && i < INTSIZE; u1--, u2--, i++) {
        int sum;
        if (u1 <= 0) {
            sum = to_int(var2->value[u2 - 1]) + result[i];
        } else if(u2 <= 0){
            sum = to_int(var1->value[u1 - 1]) + result[i];
        } else {
        sum = to_int(var1->value[u1 - 1]) + to_int(var2->value[u2 - 1]) + result[i];
        }
        result[i] = to_digit(sum % 10);
        result[i+1] = '\0'; 
        if (sum > 9)
            result[i+1] = 1;
    }
    result[i+1] = '\0';
    
    char result2[INTSIZE+1];
    reversecp(result2, result);
    storevalue(var1, result2);
    do_print(var1);
    return;
}

/****************************************************************/

/* set the vars array to all zero values
 */
void
zero_vars(longint_t vars[]) {
	int i;
	longint_t zero;
    initstruct(&zero);
	for (i=0; i<NVARS; i++) {
		do_assign(vars+i, &zero);
	}
	return;
}
