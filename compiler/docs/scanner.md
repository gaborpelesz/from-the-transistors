# The scanner of the compiler

## .lang files and regex
This is an example of the language syntax description files for the compiler's scanner. 
The syntax was mostly inspired by the lex style syntax description files, however, it is very much simplified and far not as powerful.

```lang
# This is a comment

# This is a TOKEN definition with a regex
DIGIT : [0-9]

# The TOKEN definitions are evaluated in order
# Higher definitions gets more priority
REGISTER : r[0-9]+
IDENTIFIER : [a-zA-Z][a-zA-Z_0-9]*
```

Definition of the regular expression language:
- concatenation: ab
- alteration: a|b
- closure (0 or any no.): a*
- parentheses: (a|b)(a|c)*
- positive closure: a+ == aa*
- empty string special character: \e
- zero or one occurance: a? == (\e | a)
- wildcard (matches any char in alphabet): a.b == a(a|b|c|...)b
- shorthands:
  - `[0-9] == (0|1|2|3|4|5|6|7|8|9)`
  - `[a-z] == (a|b|...|z)`
  - `[A-Z] == (A|B|...|Z)`
  - `[a-Z] == [a-z]|[A-Z] == (a|b|...|z|A|B|...|Z)`
  - arbitrary range: `[c-L][4-8]`
- match any whitespace character (\t, \r, \n, spaces): a\sb -> \s
- special character arithmetic: (a|b)* == a*b*
- precedence: parentheses ( (a|b)* ), closure (a*), concatenation (ab), alteration (a|b)
- special characters: ", \\, |, (, ), *, +, ?, ., [, ], \s, \t, \r, \n, \e
- special character handling:
  - place things inside quotes: "something with | (special characters)" will match exactly what's inside
  - to have a quotation mark, use "\\"" or \\"
