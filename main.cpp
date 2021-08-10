/*==================================== PERSIMPLEX COMPILER ================================================
AUTHOR : Jonayed Mohiuddin
EMAIL  : jonayedmohiuddin@gmail.com
GITHUB : https://github.com/JonayedMohiuddin
DETAILS : 
=PERSIMPLEX Compiler : This is a very very basic Compiler written in C++ which converts "ANETA" language to
C language.
=ANETA LANGUAGE : A language created by me for this compiler demonstration purpos. 
+Extension-(.ane)

FlowChart:
SOURCE CODE(ANETA) => LEXER => PARSER => EMITTER => COMPILED CODE(C) 
===========================================================================================================*/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Enum containing all TOKENS of SIMPOWER language.
enum class TokenType
{
    _EOF = -1,
    NEWLINE = 0,
    NUMBER = 1,
    IDENT = 2,
    STRING = 3,
    // Keywords.
    LABEL = 101,
    GOTO = 102,
    PRINT = 103,
    INPUT = 104,
    LET = 105,
    IF = 106,
    THEN = 107,
    ENDIF = 108,
    WHILE = 109,
    REPEAT = 110,
    ENDWHILE = 111,
    // Operators.
    EQ = 201,
    PLUS = 202,
    MINUS = 203,
    ASTERISK = 204,
    SLASH = 205,
    EQEQ = 206,
    NOTEQ = 207,
    LT = 208,
    LTEQ = 209,
    GT = 210,
    GTEQ = 211
};

// Token contains the original text (String, Number, etc) and the type of token.
class Token
{
public:
    string text;
    TokenType kind;

    Token() {}
    // Token text can be either char or string so two constructor
    Token(char tokenText, TokenType tokenKind)
    {
        text = tokenText; // The token's actual text. Used for identifiers, strings, and numbers.
        kind = tokenKind; // The TokenType that this token is classified as.
    }
    Token(string tokenText, TokenType tokenKind)
    {
        text = tokenText; // The token's actual text. Used for identifiers, strings, and numbers.
        kind = tokenKind; // The TokenType that this token is classified as.
    }

    // Checks if a given string is a keyword.
    static int checkIfKeyword(string tokenText)
    {
        if (tokenText == "LABEL")
            return 101;
        else if (tokenText == "GOTO")
            return 102;
        else if (tokenText == "PRINT")
            return 103;
        else if (tokenText == "INPUT")
            return 104;
        else if (tokenText == "LET")
            return 105;
        else if (tokenText == "IF")
            return 106;
        else if (tokenText == "THEN")
            return 107;
        else if (tokenText == "ENDIF")
            return 108;
        else if (tokenText == "WHILE")
            return 109;
        else if (tokenText == "REPEAT")
            return 110;
        else if (tokenText == "ENDWHILE")
            return 111;
        else
            return -1;
    }
};

// Tokenizer 
class Lexer
{
public:
    string filename;
    string source;
    char curChar;
    int curPos;

    char lastChar;
    int startPos;
    int lastPos;
    string tokenText;

    Lexer()
    {
        // Current character in the string.
        curChar = ' ';
        // Current position in the string.
        curPos = -1;
        nextChar();
    }

    Lexer(string filename)
    {
        // Source code to lex as a string. Append a newline to simplify lexing/parsing the last token/statement.
        ifstream sourceFile;
        sourceFile.open(filename);
        if(sourceFile)
        {
           getline(sourceFile, source, '\0');
        }
        else
        {
            cout << "~ ! No such file ("+filename+") in Codes directory. Please add one first! " << endl;
            cout << "~ ! Exiting now !" << endl;
            exit(EXIT_FAILURE);
        }
        sourceFile.close();
        source += '\n';

        // Current character in the string.
        curChar = ' ';
        // Current position in the string.
        curPos = -1;
        nextChar();
    }

    // Return the next charecter
    void nextChar()
    {
        curPos++;

        if (curPos >= source.length())
            curChar = '\0'; // EOF
        else
            curChar = source[curPos];
    }

    // Return the lookahead character.
    char peek()
    {
        if ((curPos + 1) >= source.length())
            return '\0';

        return source[curPos + 1];
    }

    // Invalid token found, print error message and exit.
    void abort(string message, char expected = ' ')
    {
        cout << "~ ! Lexing ERROR, " << message << expected << endl;

        exit(EXIT_FAILURE); // Exits the program.
    }

    // Skip whitespace except newlines, which we will use to indicate the end of a statement.
    void skipWhitespace()
    {
        while (curChar == ' ' || curChar == '\t' || curChar == '\r')
        {
            nextChar();
        }
    }

    // Skip comments in the code.
    void skipComment()
    {
        if (curChar == '#')
            while (curChar != '\n')
                nextChar();
    }

    // Return the next token.
    Token getToken()
    {
        skipWhitespace();
        skipComment();

        Token token;
        // Check the first character of this token to see if we can decide what it is.
        // If it is a multiple character operator (e.g., !=), number, identifier, or keyword then we will process the rest.
        if (curChar == '+')
        {
            token = Token(curChar, TokenType::PLUS);
        }

        else if (curChar == '-')
        {
            token = Token(curChar, TokenType::MINUS);
        }

        else if (curChar == '*')
        {
            token = Token(curChar, TokenType::ASTERISK);
        }

        else if (curChar == '/')
        {
            token = Token(curChar, TokenType::SLASH);
        }

        // Check whether this token is = or ==
        else if (curChar == '=')
        {
            if (peek() == '=')
            {
                lastChar = curChar;
                nextChar();
                tokenText = "";
                tokenText += lastChar;
                tokenText += curChar;
                token = Token(tokenText, TokenType::EQEQ);
            }
            else
                token = Token(curChar, TokenType::EQ);
        }

        // Check whether this token is > or >=
        else if (curChar == '>')
        {
            if (peek() == '=')
            {
                lastChar = curChar;
                nextChar();
                tokenText = "";
                tokenText += lastChar;
                tokenText += curChar;
                token = Token(tokenText, TokenType::GTEQ);
            }
            else
                token = Token(curChar, TokenType::GT);
        }

        // Check whether this token is < or <=
        else if (curChar == '<')
        {
            if (peek() == '=')
            {
                lastChar = curChar;
                nextChar();
                tokenText = "";
                tokenText += lastChar;
                tokenText += curChar;
                token = Token(tokenText, TokenType::LTEQ);
            }
            else
                token = Token(curChar, TokenType::LT);
        }

        // Check if it is !=
        else if (curChar == '!')
        {
            if (peek() == '=')
            {
                lastChar = curChar;
                nextChar();
                tokenText = "";
                tokenText += lastChar;
                tokenText += curChar;
                token = Token(tokenText, TokenType::NOTEQ);
            }
            else
                abort("~ ! Expected != got !", peek());
        }

        // Check for strings
        else if (curChar == '\"')
        {
            // Get characters between quotations.
            nextChar();
            startPos = curPos;

            while (curChar != '\"')
            {
                // Don't allow special characters in the string. No escape characters, newlines, tabs, or %.
                // We will be using C's printf on this string.
                if (curChar == '\r' || curChar == '\n' || curChar == '\t' || curChar == '\\' || curChar == '%')
                    abort("~ ! Illegal character in string.");

                nextChar();
                tokenText = source.substr(startPos, curPos - startPos); // Get the substring.
                token = Token(tokenText, TokenType::STRING);
            }
        }

        // Check if it is a number. ( .n , n. not allowed n.n allowed)
        else if (isdigit(curChar))
        {
            // Leading character is a digit, so this must be a number.
            // Get all consecutive digits and decimal if there is one.
            startPos = curPos; // save the starting position

            while (isdigit(peek()))
                nextChar();

            if (peek() == '.') //Decimal!
            {
                nextChar();
                // Must have at least one digit after decimal.
                if (!isdigit(peek())) // if not digit
                    abort("~ ! Illegal character in number.");
            }

            while (isdigit(peek()))
                nextChar();

            tokenText = source.substr(startPos, curPos - startPos + 1); // Get the substring.
            token = Token(tokenText, TokenType::NUMBER);
        }

        // Check for new-line charecter
        else if (curChar == '\n')
        {
            token = Token(curChar, TokenType::NEWLINE);
        }

        // Check for End-Of-File charecter
        else if (curChar == '\0')
        {
            token = Token(curChar, TokenType::_EOF);
        }

        // Check if it is a identifier
        else if (isalpha(curChar)) // First letter is always alphabet of a identifier
        {
            // Leading character is a letter, so this must be an identifier or a keyword.
            // Get all consecutive alpha numeric characters.
            startPos = curPos;

            while (isalnum(peek()))
                nextChar();

            tokenText = source.substr(startPos, curPos - startPos + 1); // Get the substring.

            // Check if the token is in the list of keywords.
            int keyword = token.checkIfKeyword(tokenText);

            if (keyword == -1) // Identifier
                token = Token(tokenText, TokenType::IDENT);
            else
                token = Token(tokenText, (TokenType)keyword);
        }

        else
            abort("~ ! Unknown token : ", curChar);

        nextChar();

        return token;
    }
}; // lexer Class

// Emits compiled code in a C file
class Emitter
{
public:
    string filename;
    string header;
    string code;

    Emitter()
    {
        header = "";
        code = "";
    }
    Emitter(string filename)
    {
        this->filename = filename;
        header = "";
        code = "";
    }

    void emit(string code)
    {
        this->code += code;
    }

    void emitLine(string code)
    {
        this->code += code + '\n';
    }

    void headerLine(string code)
    {
        header += code + '\n';
    }

    void writeFile()
    {
        // Create and open a c file
        fstream file;
        file.open(filename, fstream::out | ofstream::trunc);
        if (!file)
            cout << "~ ! Error in creating file!" << endl;

        file << header;
        file << code;
        file.close();
    }
}; // Emitter Class

// Checks grammar of tokens using lexer
class Parser
{
public:
    Lexer lexer;
    Emitter emitter;

    Token curToken;
    Token peekToken;

    vector<string> symbols;        // Variables declared so far.
    vector<string> labelsDeclared; // Labels declared so far.
    vector<string> labelsGotoed;   // Labels goto'ed so far.

    Parser(Lexer lexer, Emitter emitter)
    {
        this->lexer = lexer;
        this->emitter = emitter;

        nextToken();
        nextToken(); // Called this twice to initialize current and peek token.
    }

    // Return true if the current token matches passed token.
    bool checkToken(TokenType tokenKind)
    {
        return (tokenKind == curToken.kind) ? true : false;
    }

    // Return true if the next token matches.
    bool checkPeek(TokenType tokenKind)
    {
        return (tokenKind == peekToken.kind) ? true : false;
    }

    // Try to match current token. If not, error. Advances the current token.
    void match(TokenType tokenKind)
    {
        if (!checkToken(tokenKind)) // if current token doesnt matches expected token
        {
            abort("~ ! Expected " + to_string((int)tokenKind) + " got " + to_string((int)curToken.kind));
        }

        nextToken();
    }

    // Advances the current token.
    void nextToken()
    {
        curToken = peekToken;
        peekToken = lexer.getToken();
        // No need to worry about passing the EOF, lexer handles that.
    }

    void abort(string message)
    {
        cout << "~ ! Parsing Error, " << message << endl;
        exit(EXIT_FAILURE);
    }

    // Production rules.

    // program ::= {statement}
    void program()
    {
        cout << "--------------" << endl;
        cout << "PARSING SOURCE:" << endl;
        cout << "--------------" << endl;
        cout << "~ > PROGRAM" << endl;
        emitter.headerLine("#include <stdio.h>");
        emitter.headerLine("");
        emitter.headerLine("int main(void) {");

        // Skip the newlines on the top of program
        // Since some newlines are required in our grammar, need to skip the excess.
        while (checkToken(TokenType::NEWLINE))
            nextToken();

        // Parse all the statements in the program.
        while (!checkToken(TokenType::_EOF))
            statement();

        // Wrap things up. (Write the last lines)
        emitter.emitLine("return 0;");
        emitter.emitLine("}");

        // Check if LABEL of GOTO is declared. If not ERROR so abort.
        for (vector<string>::iterator gotoed = labelsGotoed.begin(); gotoed != labelsGotoed.end(); ++gotoed)
        {
            for (vector<string>::iterator declared = labelsDeclared.begin(); declared != labelsDeclared.end(); ++declared)
            {
                if (*gotoed == *declared)
                    break;
                else
                    abort("~ ! Attempting to GOTO undeclared LABEL");
            }
        }

        cout << "---------------------" << endl;
        cout << "- Parsing complete. -" << endl;
        cout << "---------------------" << endl;
        emitter.writeFile();
    }

    // One of the following statements...
    void statement()
    {
        // Check the first token to see what kind of statement this is.
        // "PRINT" (expression | string)
        if (checkToken(TokenType::PRINT))
        {
            cout << "~ > STATEMENT-PRINT" << endl;
            nextToken();

            if (checkToken(TokenType::STRING)) // Simple string.
            {
                // Print the strint in file
                emitter.emitLine("printf(\"" + curToken.text + "\\n\");");
                nextToken();
            }
            else // Expect an expression.
            {
                // Expect an expression and print the result as float in file.
                emitter.emit("printf(\"%.2f\\n\", (float)("); // printf("%.2f \n", (float)( *expression will print a num here* ));

                expression();

                emitter.emitLine("));");
            }
        }

        // "IF" comparison "THEN" {statement} "ENDIF"
        else if (checkToken(TokenType::IF))
        {
            cout << "~ > STATEMENT-IF" << endl;
            nextToken();
            emitter.emit("if(");

            comparison();

            match(TokenType::THEN); // Match token also advances to the next token
            nl();
            emitter.emitLine(") {");

            // Zero or more statements in the body until ENDIF.
            while (!checkToken(TokenType::ENDIF))
                statement();

            match(TokenType::ENDIF);
            cout << "~ > ENDIF" << endl;
            emitter.emitLine("}");
        }

        // "WHILE" comparison "REPEAT" {statement} "ENDWHILE"
        else if (checkToken(TokenType::WHILE))
        {
            cout << "~ > STATEMENT-WHILE" << endl;
            nextToken();
            emitter.emit("while(");

            comparison();

            match(TokenType::REPEAT); // Match token also advances to the next token
            nl();
            emitter.emitLine(") {");

            // Zero or more statements in the body until ENDIF.
            while (!checkToken(TokenType::ENDWHILE))
                statement();

            match(TokenType::ENDWHILE);
            cout << "~ > ENDWHILE" << endl;
            emitter.emitLine("}");
        }

        // "LABEL" ident
        else if (checkToken(TokenType::LABEL))
        {
            cout << "~ > STATEMENT-LABEL" << endl;
            nextToken();

            // make sure this label doesnt already exist
            for (vector<string>::iterator it = labelsDeclared.begin(); it != labelsDeclared.end(); ++it)
                if (*it == curToken.text)
                    abort("~ ! Label already exists!");

            labelsDeclared.push_back(curToken.text); // add this label in array

            emitter.emitLine(curToken.text + ":");
            match(TokenType::IDENT);
        }

        // "GOTO" ident
        else if (checkToken(TokenType::GOTO))
        {
            cout << "~ > STATEMENT-GOTO" << endl;
            nextToken();
            labelsGotoed.push_back(curToken.text);

            emitter.emitLine("goto " + curToken.text + ";");
            match(TokenType::IDENT);
        }

        // "LET" ident "=" expression
        else if (checkToken(TokenType::LET))
        {
            cout << "~ > STATEMENT-LET" << endl;
            nextToken();

            // Check if IDENT exists in symbol table.
            bool declared = false;
            for (vector<string>::iterator symbol = symbols.begin(); symbol != symbols.end(); ++symbol)
            {
                if (*symbol == curToken.text)
                {
                    declared = true;
                    break;
                }
            }
            // If not declare it.
            if (!declared)
            {
                symbols.push_back(curToken.text);
                emitter.headerLine("float " + curToken.text + ";"); // Add var declaration to headerv of c (before main())
            }

            emitter.emit(curToken.text + "=");
            match(TokenType::IDENT);
            match(TokenType::EQ);

            expression();
            emitter.emitLine(";");
        }

        // "INPUT" ident
        else if (checkToken(TokenType::INPUT))
        {
            cout << "~ > STATEMENT-INPUT" << endl;
            nextToken();

            // Check if IDENT exists in symbol table.
            bool declared = false;
            for (vector<string>::iterator symbol = symbols.begin(); symbol != symbols.end(); ++symbol)
            {
                if (*symbol == curToken.text)
                {
                    declared = true;
                    break;
                }
            }
            // If not declare it.
            if (!declared)
            {
                symbols.push_back(curToken.text);
                emitter.headerLine("float " + curToken.text + ";"); // Add var declaration to headerv of c (before main())
            }

            // Emit scanf but also validate the input. If invalid, set the variable to 0 and clear the input;
            emitter.emitLine("if(0 == scanf(\"%f\", &" + curToken.text + ")) {");
            emitter.emitLine(curToken.text + " = 0;");
            emitter.emit("scanf(\"%");
            emitter.emitLine("*s\");");
            emitter.emitLine("}");
            emitter.emitLine("");

            match(TokenType::IDENT);
        }

        // This is not a valid statement. Error!
        else
            abort("~ ! Invalid statement at " + curToken.text + " (" + to_string((int)curToken.kind) + ")");

        // Newline. expect a new line after every statement
        nl();
    }

    // comparison ::= expression (("==" | "!=" | ">" | ">=" | "<" | "<=") expression)+
    void comparison()
    {
        cout << "~ > COMPARISON" << endl;
        expression();

        // Must be at least one comparison operator and another expression.
        if (isComparisonOperator())
        {
            emitter.emit(curToken.text);
            nextToken();
            expression();
        }
        else
            abort("~ ! Expected comparison operator at ");

        // Can have 0 or more comparison operator and expressions.
        while (isComparisonOperator())
        {
            emitter.emit(curToken.text);
            nextToken();
            expression();
        }
    }

    bool isComparisonOperator()
    {
        return checkToken(TokenType::EQEQ) || checkToken(TokenType::NOTEQ) || checkToken(TokenType::GT) || checkToken(TokenType::GTEQ) || checkToken(TokenType::LT) || checkToken(TokenType::LTEQ);
    }

    // expression ::= term {( "-" | "+" ) term}
    void expression()
    {
        cout << "~ > EXPRESSION" << endl;
        term();

        // Can have 0 or more +/- and expressions.
        while (checkToken(TokenType::PLUS) || checkToken(TokenType::MINUS))
        {
            emitter.emit(curToken.text);
            nextToken(); // Skip the PLUS-MINUS
            term();      // Expect a term.
        }
    }

    // term ::= unary {( "/" | "*" ) unary}
    void term()
    {
        cout << "~ > ^   TERM" << endl;
        unary();

        // Can have 0 or more * or / and expressions.
        while (checkToken(TokenType::ASTERISK) || checkToken(TokenType::SLASH))
        {
            emitter.emit(curToken.text);
            nextToken(); // Skip the ASTERISK-SLASH
            unary();
        }
    }

    // unary ::= ["+" | "-"] primary
    void unary()
    {
        cout << "~ > ^^  UNARY" << endl;

        // Optional unary +/-
        if (checkToken(TokenType::PLUS) || checkToken(TokenType::MINUS))
        {
            emitter.emit(curToken.text);
            nextToken(); // Skip the PLUS-MINUS
        }
        primary();
    }

    // primary ::= number | ident
    void primary()
    {
        cout << "~ > ^^^ PRIMARY (" << curToken.text << ")" << endl;

        if (checkToken(TokenType::NUMBER))
        {
            emitter.emit(curToken.text);
            nextToken();
        }
        else if (checkToken(TokenType::IDENT))
        {
            // Check if IDENT exists in symbol table.
            bool declared = false;
            for (vector<string>::iterator symbol = symbols.begin(); symbol != symbols.end(); ++symbol)
            {
                if (*symbol == curToken.text)
                {
                    declared = true;
                    break;
                }
            }
            // If not ERROR so abort.
            if (!declared)
                abort("~ ! Referencing variable before assignment: " + curToken.text);

            emitter.emit(curToken.text);
            nextToken();
        }
        else
            abort("~ ! Unexpected token at primary"); //Error!
    }

    // nl ::= '\n'+
    void nl()
    {
        // Require at least one newline.
        match(TokenType::NEWLINE);

        cout << "~ > NEWLINE" << endl;

        //But we will allow extra newlines too, of course.
        while (checkToken(TokenType::NEWLINE))
            nextToken();
    }
}; // Parser Class

int main(void)
{
    cout << "==================================================================" << endl;
    cout << "=                     PERSIMPLEX COMPILER                        =" << endl;
    cout << "==================================================================" << endl;

    string sourceFilename = "Codes/";
    string emitFilename = "Codes/";
    string input = "";

    cout << "~ $ Input the filename to compile (Codes/): ";
    cin >> input;
    sourceFilename += input;

    // Just removes any extension from the source code and adds .c
    int i = 0;
    for(;;)
    {
        if(input[i]=='.' || input[i] == '\0')
            break;
        
        emitFilename += input[i];
        i++;
    }
    emitFilename += ".c";

    Lexer lexer(sourceFilename);
    cout << "-----------\nSOURCE-CODE: \n-----------\n" << lexer.source << endl;
    Emitter emitter(emitFilename);
    Parser parser(lexer, emitter);
    parser.program();

    // Print compiled code
    string emittededCode = "";
    ifstream emittededFile;
    emittededFile.open(emitFilename);
    if(emittededFile.is_open())
        getline(emittededFile, emittededCode, '\0');
    emittededFile.close();
    emittededCode += '\n';

    cout << "-------------\nCOMPILED-CODE: \n-------------\n" <<emittededCode << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "~ $ Compiled C code can be found at " + emitFilename << endl;
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

    // Repeats everything again if you want to.....
    char repeat = 'y';
    cout << "Want to compile another file (Y/n)? :";
    cin >> repeat;
    if('Y' == toupper(repeat))
      main();
    cout << endl;
    
    return 0;
}
