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

#define INTSIZE	100	/* max number of digits per integer value */
#define LINELEN	102	/* maximum length of any input line */
#define NVARS	26	/* number of different variables */

#define CH_BLANK ' '    /* character blank */
#define CH_ZERO  '0'    /* character zero */
#define CH_A     'a'    /* character 'a', first variable name */

#define ERROR	(-1)	/* error return value from some functions */

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define ALLOPS  "?=+"   /* list of all valid operators */
#define SGNCHRS "+-"    /* the two sign characters */
#define NUMCHRS "0123456789"
			/* list of characters legal within numbers */



typedef int longint_t;
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

/****************************************************************/

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

/* process the input line to extract the RHS argument, which
 * should start at the pointer that is passed
 */
int
get_second_value(longint_t vars[], char *rhsarg,
			longint_t *second_value) {
	char *p;
	int varnum2;
	if (strchr(NUMCHRS, *rhsarg) != NULL ||
				strchr(SGNCHRS, *rhsarg) != NULL) {
		/* first character is a digit or a sign, so RHS 
		 * should be a number
		 */
		p = rhsarg+1;
		while (*p) {
			if (strchr(NUMCHRS, *p) == NULL) {
				/* nope, found an illegal character */
				return ERROR;
			}
			p++;
		}
		*second_value = atoi(rhsarg);
		return !ERROR;
	} else {
		/* argument is not a number, so might be a variable */
		varnum2 = to_varnum(*rhsarg);
		if (varnum2==ERROR || strlen(rhsarg)!=1) {
			/* nope, not a variable either */
			return ERROR;
		}
		/* is a variable, so can use its value to assign to
		 * second_value
		 */
		*second_value = vars[varnum2];
		return !ERROR;
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
	printf("%d\n", *var);
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

/* update the indicated variable var1 by doing an addition
 * using var2 to compute var1 = var1 + var2
 */
void
do_plus(longint_t *var1, longint_t *var2) {
	*var1 += *var2;
}

/****************************************************************/

/* set the vars array to all zero values
 */
void
zero_vars(longint_t vars[]) {
	int i;
	longint_t zero = 0;
	for (i=0; i<NVARS; i++) {
		do_assign(vars+i, &zero);
	}
	return;
}
