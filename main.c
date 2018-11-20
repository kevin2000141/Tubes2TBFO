#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "mesintoken.h"
#include "stack.h"
#include "pda.h"
#include "stackchar.h"
#include "boolean.h"
#include "complex.h"

/* Kumpulan variabel yang digunakan untuk menghitung ekspresi */
Token CToken[10005];
int NToken;
Stackt tok;
boolean IsValid;

/* Kumpulan fungsi yang digunakan untuk menghitung ekspresi */
/* fungsi-fungsi di bawah ini juga merupakan implementasi dari recursive descent parser algorithm yang menggunakan CFG */
double complex parse_item();
double complex parse_factor();
double complex parse_term();
double complex parse_expression();

/*fungsi dibawah ini berfungsi untuk mengecek apakah suatu operasi matematika tersebut valid atau tidak */
/* contoh : 2 + 3 * 5 -> valid 
            9 / (9 - 3*3) -> tidak valid
            0 ^ 0 -> tidak valid 
*/
boolean cekvalid(double complex bil1, char opr, double complex bil2){
    if ((opr == '/') && (creal(bil2) == 0)){
        return false;
    }
    else if ((creal(bil1) == 0) && (opr == '^') && (creal(bil2) == 0)){
        return false;
    }
    else {
        return true;
    }
}

/*implementasi dari parse_item */
double complex parse_item(){
    Infotype t, s;
    Popst(&tok, &t);
    if (t.tkn == 'b') return t.val;
    double complex expr = parse_expression();
    Popst(&tok,&s);
    return expr;
}

/*implementasi dari parse_item */
double complex parse_factor(){
    Infotype t;
    Popst(&tok, &t);
    Pushst(&tok, t);    
    double complex sign, test;
    if (t.tkn == '-')
        sign = -1;
    else sign = 1;
    // printf("%.1f", creal(sign));

    if (t.tkn == '+' || creal(sign) < 0) 
        Popst(&tok, &t);
    double complex result = parse_item();
    Popst(&tok, &t);
    Pushst(&tok, t);
    while (t.tkn == '^'){
        Popst(&tok, &t);
        if (t.tkn != '^') {
            Pushst(&tok, t);
            break;
        }
        double complex rhs = parse_factor();
        if (cekvalid(result, '^', rhs))
        {
            result = cpow(creal(result), creal(rhs));
        }
        else
        {
            IsValid = false;
            return 0;
        }
        Popst(&tok, &t);
        Pushst(&tok, t);
    } 
    return result * creal(sign);
}

/*implementasi dari parse_term */
double complex parse_term(){
    double complex result = parse_factor();
    Infotype t;
    Popst(&tok, &t);
    if (t.tkn != '*' && t.tkn != '/')
        Pushst(&tok, t);
    while (t.tkn == '*' || t.tkn == '/'){
        double complex rhs = parse_factor();
        if (cekvalid(result, t.tkn, rhs)) {
            if (t.tkn == '/')
                result /= rhs;
            else
                result *= rhs;
        }
        else {
            IsValid = false;
            return 0;
        }
        Popst(&tok, &t);
        if (t.tkn != '*' && t.tkn != '/'){
            Pushst(&tok, t);
        }
    }
    return result;
}

/*implementasi dari parse_expression */
double complex parse_expression(){
    double complex result = parse_term();
    Infotype t;
    Popst(&tok, &t);
    if (t.tkn != '+' && t.tkn != '-') 
        Pushst(&tok, t);
    while (t.tkn == '+' || t.tkn == '-'){
        double complex rhs = parse_term();
        if (t.tkn == '+')
            result += rhs;
        else 
            result -= rhs;
        Popst(&tok, &t);
        if (t.tkn != '+' && t.tkn != '-'){
            Pushst(&tok, t);
        }
    }
    return result;
}

/*mendapatkan input dari user */
void getInput(){
    masukan inputuser;
    gets(inputuser);
    if (validasi(inputuser)) {    
        SplitToken(&inputuser, &CToken, &NToken);
        CreateEmptyst(&tok);
    }else{
        printf("Syntax error!\n");
        exit(-1);
    }
}


/* main program */
int main(){
    getInput();
    IsValid = true;
    for (int i = 0; i < NToken; i++){
        Pushst(&tok, CToken[i]);
    }
    tok = Reverse(tok);
    double complex ans = parse_expression();
    if (IsValid){
        if (cimag(ans) == 0)
            printf("%.6f\n", creal(ans));
        else
            printf("%.6f + %.6fi\n", creal(ans),cimag(ans));
    }
    else 
        printf("Math error!\n");
    return 0;
}