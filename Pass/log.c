// log.c
#include <stdio.h>
#include <stdint.h>

// Старые — оставляем на месте, чтобы ничего не ломать
void funcStartLogger(char *funcName) {
  printf("[LOG] Start function '%s'\n", funcName);
}
void callLogger(char *callerName, char *calleeName, long int valID) {
  printf("[LOG] CALL '%s' -> '%s' {%ld}\n", callerName, calleeName, valID);
}
void resIntLogger(long int res, long int valID) {
  printf("[LOG] Result %ld {%ld}\n", res, valID);
}
void funcEndLogger(char *funcName, long int valID) {
  printf("[LOG] End function '%s' {%ld}\n", funcName, valID);
}
void binOptLogger(int val, int arg0, int arg1, char *opName, char *funcName, long int valID) {
  printf("[LOG] In function '%s': %d = %d %s %d {%ld}\n", funcName, val, arg0, opName, arg1, valID);
}

// Новые — для трассы исполнения и трассы использования
void instExecLogger(char *funcName, char *bbName, char *opName, long int valID) {
  // пример: [I] main :: entry :: add {140735123456}
  printf("[I] %s :: %s :: %s {%ld}\n", funcName, bbName, opName, valID);
}

void useLogger(long int userID, long int operandID, char *userOpName) {
  // пример: [U] 140735111 <- 140735222 (add)
  printf("[U] %ld <- %ld (%s)\n", userID, operandID, userOpName);
}
