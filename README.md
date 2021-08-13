# PERSIMPLEX COMPILER
Persimplex is a compiler which compiles `APLOS`(.apl) programming language to C programming language.

## SYNTAX
The syntax of `APLOS` language consists of `12 TOKENS`.They are:
- `LET` 
  - For initialising and assigning values to variables.
  - LET a = 0
- `PRINT`
  - Prints a string succeding it.
  - PRINT "HELLO WORLD!"
- `INPUT`
  - Takes input from the user and puts it in the variable following it.
  - INPUT a
- `LABEL`
  - A label. Which can be gotoed from anywhere in the code
  - LABEL loop
- `GOTO`
  - This will change the code flow to the point where the label is declared.
  - GOTO loop
- `IF`
- `THEN`
- `ENDIF`
- `WHILE`
- `REPEAT`
- `ENDWHILE`
- `NEWLINE`
  - Every statement must end with a newline (`'\n'`)
