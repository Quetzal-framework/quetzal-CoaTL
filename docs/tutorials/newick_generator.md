# Newick tree generator

Yet not very efficient, Newick is a simple format for representing trees, what is
a useful abstraction in coalescence to represent the shared history of populations
(species trees, population trees) or gene genealogies that coalesce into these
trees.

Here we provide some utilities to parse and generate such format.

## Grammar

Whitespace here refer to any of the following: spaces, tabs, carriage returns, and linefeeds.

- Whitespace  within number is prohibited.
- Whitespace elsewhere is ignored.
- Grammar characters (semicolon, parentheses, comma, and colon) are prohibited
- Unquoted string may not contain blanks, parentheses, square brackets, single_quotes, colons, semicolons, or commas. --- Underscore characters in unquoted strings are converted to blanks.
- A string may also be quoted by enclosing it in single quotes.
- Single quotes in the original string are represented as two consecutive single quote characters.
- Whitespace may appear anywhere except within an unquoted string or a Length
- Newlines may appear anywhere except within a string or a Length.
- Comments are enclosed in square brackets. They can appear anywhere newlines are allowed.[3]
- Comments starting with & - are generally computer-generated for additional data.

## Parser
