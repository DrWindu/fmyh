%{
	#include "logic_language.tab.hpp"

	#include <string.h>

	void dialogParseError(const char* error);
%}

%%

"@Pick"        { return PPICK; }

"("            { return OPENPAR;  }
")"            { return CLOSEPAR; }

"NOT"          { return NOT; }
"AND"          { return AND; }
"OR"           { return OR;  }

"!"            { return BANG; }
"<"            { return LESS; }
">"            { return MORE; }
"="            { return EQ;   }

"["            { return OPENVAL;  }
"]"            { return CLOSEVAL; }

[A-Za-z_]*     { yylval.id = strdup(yytext);
                 return ID; }

[0-9]*         { yylval.val = atoi(yytext);
                 return VAL; }

[ \t\n]        { }

.              { dialogParseError("Unexpected token"); }

%%

