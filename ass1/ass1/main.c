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
#define FALSE 0

#define PRINT	'?'	/* the print operator */
#define ASSIGN	'='	/* the assignment operator */
#define PLUS	'+'	/* the addition operator */
#define MINUS   '-'
#define ALLOPS  "?=+-"   /* list of all valid operators */
#define SGNCHRS "+-"    /* the two sign characters */
#define NUMCHRS "0123456789,"
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
int numcmp(char *s1, char*s2);
//void do_minus(longint_t *var1, longint_t *var2);
void back_zero_removal(char *string);
int ispositve(longint_t *var);
void difference(char *s1, char *s2, char *result, int result_size);
void negative_plus(longint_t *var1, longint_t *var2);
void num_plus (longint_t *var1, longint_t *var2);
void removecommas(char *string);
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


/* prints the prompt indicating ready for input
 */
void
print_prompt(void) {
	printf("> ");
}

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


/* process a command by parsing the input line into parts
 */
void
process_line(longint_t vars[], char *line) {
	int varnum, optype, status;
	longint_t second_value;
    initstruct(&second_value);
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
		num_plus(vars+varnum, &second_value);
        do_print(vars+varnum);
	}
	return;
}

/* check if the sign of a longint_t is positive */
int
ispositve(longint_t *var) {
    if (var->sign == PLUS)
        return TRUE;
    return FALSE;
}

/* summation operation for negative numbers */
void
negative_plus(longint_t *var1, longint_t *var2) {
    if (!ispositve(var1) && !ispositve(var2)) {
        do_plus(var1, var2);
        var1->sign = MINUS;
        return;
    }
    
    char diff[INTSIZE + 1];
    difference(var1->value, var2->value, diff, sizeof(diff));
    storevalue(var1, diff);
    
    if (!ispositve(var1) && ispositve(var2)) {
        if (numcmp(var1->value, var2->value)) {
            var1->sign = MINUS;
        } else {
            var1->sign = PLUS;
        }
    } else if (ispositve(var1) && !ispositve(var2)) {
        if (numcmp(var1->value, var2->value)) {
            var1->sign = PLUS;
        } else {
            var1->sign = MINUS;
        }
    }
}

/* does the summation for negative numbers, var1 = var1 + var2 */
void
num_plus (longint_t *var1, longint_t *var2) {
    if (ispositve(var1) && ispositve(var2)) {
        do_plus(var1, var2);
    } else if (!ispositve(var1) || !ispositve(var2)) {
        negative_plus(var1, var2);
    } 
}


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


/* store an integer (+ve or -ve) string into longint_t longvalue */
void
storevalue(longint_t *longvalue, char *string){
    int i = 0; // a counter for longvalue.value
    initstruct(longvalue);
    
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

/* check that each comma and the final null terminator in a string is 3 characters apart */
int
comma_pattern_check(char *string) {
    int previous = (int)strlen(string), i = (int)strlen(string) - 1;
    while (i >= 0) {
        if (string[i] == ',') {
            if ((previous - i) != 3)
                return FALSE;
            previous = i;
        }
        i--;
    }
    return TRUE;
}


/* check that the string is a proper number string */
int
num_check(char *string){
    if (strchr(NUMCHRS, *string) == NULL && strchr(SGNCHRS, *string) == NULL)
        return FALSE;
    
    
    while (*(++string)) {
        if (strchr(NUMCHRS, *string) == NULL) {
            return FALSE;
        }
    }
    
    if (!comma_pattern_check(string)) {
        return FALSE;
    }
    
    return TRUE;
}

/* remove all the commas in a given string */
void
removecommas(char *string) {
    int s = 0, t = 0;
    char scopy[strlen(string)], temp;
    while ((temp = string[s])) {
        if (temp != ',')
            scopy[t++] = temp;
        s++;
    }
    scopy[t] = '\0';
    strcpy(string, scopy);
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
        removecommas(rhsarg);
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
}


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


/* and back again to a digit */
char
to_digit(int number) {
	return number + CH_ZERO;
}



/* print out a longint value
 */
void
do_print(longint_t *var) {
    if (var->value[0] == CH_ZERO) {
        printf("0\n");
    } else if (!ispositve(var)) {
        printf("-%s\n", var->value);
    } else {
        printf("%s\n", var->value);
    }
	return;
}


/* update the indicated variable var1 by doing an assignment
 * using var2
 */
void
do_assign(longint_t *var1, longint_t *var2) {
	/* hey, this is trivial in this representation
	 */
	*var1 = *var2;
}


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


/* accept two positive number strings, and evaluate if s2 is smaller than s1 */
int
numcmp(char *s1, char*s2){
    if (strlen(s1) > strlen(s2)) {
        return TRUE;
    } else if (strlen(s1) == strlen(s2)){
        for (int i = FALSE; i < strlen(s1); i++) {
            if (s1[i] > s2[i])
                return TRUE;
            else if (s1[i] < s2[i])
                return FALSE;
        }
        return TRUE;
    } else {
        return FALSE;
    }
}
    
/* update the indicated variable var1 by doing an addition, using var2 to compute var1 = var1 + var2, assuming both are positive numbers */
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
    return;
}

/* remove the zero characters at the back of a number string, except the first zero */
void
back_zero_removal(char *string) {
    while ((strlen(string)-1) && (string[strlen(string) -1] == CH_ZERO))
        string[strlen(string) -1] = '\0';
}


/* calculates the difference between two positve numbers, stores that in the result. s1 and s2 are unchanged in the process */
void
difference(char *s1, char *s2, char *result, int result_size) {
    char *bignum, *smallnum, tempresult[result_size];
    
    if (numcmp(s1, s2)) {
        bignum = s1;
        smallnum = s2;
    } else {
        bignum = s2;
        smallnum = s1;
    }
    *tempresult = 0;
    
    int u1 = (int)strlen(bignum), u2 = (int)strlen(smallnum), i = 0;
    
    assert(numcmp(bignum, smallnum));
    // stores the difference between bignum and smallnum in an tempresult in a reverse order
    for (; u1 > 0 && i < INTSIZE; u1--, u2--, i++) {
        int diff;
        
        int v1 = to_int(bignum[u1 - 1]);
        int v2;
        if (u2 > 0) {
            v2 = to_int(smallnum[u2 - 1]);
        } else {
            v2 = 0;
        }
        
        if ((v1 + tempresult[i])>= v2) {
            diff = v1 + tempresult[i] - v2;
            tempresult[i+1] = 0;
        } else {
            diff = v1 + tempresult[i] + 10 - v2;
            tempresult[i+1] = -1;
        }
        tempresult[i] = to_digit(diff);
    }
    tempresult[i] = '\0';
    
    back_zero_removal(tempresult);
    reversecp(result, tempresult);
}


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
