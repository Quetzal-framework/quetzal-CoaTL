# Newick tree parser

Newick tree format is a way of representing graph-theoretical trees with edge
lengths using parentheses and commas.

Yet not very efficient, Newick is a simple format for representing trees, and provides
an useful abstraction in coalescence theory to represent the shared history of populations
(species trees, population trees) or gene genealogies that coalesce into these
trees.

Quetzal-CoaTL provides some utilities to parse and generate such format.

## Grammar

Whitespace here refer to any of the following: spaces, tabs, carriage returns, and linefeeds.

- Whitespace  within number is prohibited.
- Whitespace outside of node names is ignored.
- Grammar characters (semicolon, parentheses, comma, and colon) are prohibited
- Comments are enclosed in square brackets.

## Complete example

### Input

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=../../test/tutorials_test/newick_parser.cpp) -->
<!-- MARKDOWN-AUTO-DOCS:END -->

### Output

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=../../build/bin/newick_parser.output) -->
<!-- MARKDOWN-AUTO-DOCS:END -->
