# vfmd

[![Build Status](https://travis-ci.org/vfmd/vfmd-src.svg?branch=master)](https://travis-ci.org/vfmd/vfmd-src)

vfmd is a Markdown parsing engine that adheres to the [vfmd
specification]. vfmd handles all syntax constructs in the [original
Markdown], and is designed to be cleanly extended to support additional
syntaxes without affecting the handling of the core syntax constructs.
It can therefore be used as the parsing engine for different variants of
Markdown.

For more information on vfmd, please see <http://vfmd.github.io>

[vfmd specification]: http://vfmd.github.io/vfmd-spec/specification/
[original Markdown]: http://daringfireball.net/projects/markdown/syntax

## Unique features

### Unicode-aware

vfmd expects input in UTF-8 encoding and inherently handles multi-byte
UTF-8 code points.

  1. Tab expansion is done based on the number of Unicode code points
     before the tab character in the line (most Markdown parsers count
     the number of *bytes* rather than *code points*).
  2. Detection of word boundaries (e.g. for identifying emphasis and
     automatic links) is done using Unicode Character Properties, to
     enable the parser to work well for non-English text too.
  3. Any bytes in the input that are invalid in UTF-8 (or insecure, like
     overlong sequences) are assumed to be a byte in ISO-8859-1 encoding
     [as per the vfmd specification][vfmd-utf8]. As a consequence, vfmd
     always outputs valid and secure UTF-8, even if the input contains
     bad UTF-8 bytes.

### HTML-aware

This implementation of vfmd uses a HTML parser to correctly handle
any inline HTML, including HTML5 elements.

  1. HTML tags and HTML comments can appear anywhere in a line, not
     necessarily at the start of the line
  2. HTML tags and HTML comments can span multiple lines
  3. Raw-HTML elements can contain Markdown-enabled blocks by separating
     the raw-HTML part and the Markdown part with a blank line, [as
     described in the syntax guide][vfmd-syntax-verbatim-html]
  4. vfmd looks out for well-formed `pre`, `style` and `script` elements
     and treats them as a single block, even if they contain blank lines
     within. (This is not done if they are not well-formed - i.e. open
     tag without a matching close tag or vice versa.) This is done
     because these elements can potentially contain blank lines within
     that should not be treated as Markdown-block-separating blank
     lines.
  5. On encountering a HTML tag without a matching opening or closing
     tag in a text block, vfmd just gets out of the way by not wrapping
     the text in `p` tags, thereby allowing for the tag to get matched
     elsewhere, possibly in another text block.
  6. When HTML is mixed with Markdown text, vfmd recognizes a span-level
     Markdown syntax only if it is valid for it to contain any contained
     raw HTML. For example, the asterisks in `*A <u>B</u> C*` are
     recognized as emphasis tags, but those in `*A <p>B</p> C*` are not
     (because `p` tags cannot be contained in `em` tags).

For information on using HTML with Markdown in vfmd, see [Mixing HTML
with vfmd].

For information on how vfmd's handling of inline HTML is different from
the original Markdown, see [Differences: Including raw
HTML][vfmd-html-difference].

### Parse-tree

vfmd first converts the input document into a parse-tree in memory.  The
parse-tree is then rendered into HTML output. After the document has
been parsed, the parse-tree can be obtained and traversed to observe or
analyze the document.

The basic structure of the parse tree can be seen by using the `-t`
option to the vfmd command.

### Extendable

Given that the vfmd specification allows for [extending the
syntax][vfmd-spec-extending], this implementation of the spec is
designed to be able to easily add support for additional syntax
constructs.

Base classes are defined in C++ that describe the interface for
recognizing block-level and span-level syntaxes. To add a new syntax
construct, one has to inherit these base classes, implement the code for
the new syntaxes and register an instance of the new class with a
registry of syntax constructs to enable them to be used during parsing.
The core syntaxes themselves are implemented using the same interface.

[vfmd-utf8]: http://vfmd.github.io/vfmd-spec/specification/#document
[vfmd-html-difference]: http://vfmd.github.io/differences/#including-raw-html
[Mixing HTML with vfmd]: http://vfmd.github.io/vfmd-spec/syntax/#mixing-html-with-vfmd
[vfmd-syntax-verbatim-html]: http://vfmd.github.io/vfmd-spec/syntax/#verbatim-html
[vfmd-spec-extending]: http://vfmd.github.io/vfmd-spec/specification/#extending-the-syntax

## Building

vfmd uses [waf] as the build system. It compiles in Linux and Mac OS X -
I have not tried it on Windows yet (please [let me know] if you manage
to).

To build a debug version of the vfmd command:

    $ cd vfmd-src/
    $ ./waf configure
    $ ./waf build_debug

Then you can run vfmd like:

    $ echo "Hello *World*" | ./build/debug/vfmd     # HTML output
    <p>Hello <em>World</em></p>

    $ echo "Hello *World*" | ./build/debug/vfmd -t  # Parse-tree structure
    +- block (paragraph)
       |
       +- span (text-span)
       |  'Hello '
       +- span (emphasis)
          [*]
          |
          +- span (text-span)
             'World'

    $ ./build/debug/vfmd input.md > output.html

vfmd is tested against the [vfmd-test] testsuite for compliance with
the [vfmd specification][vfmd-spec].

[waf]: https://code.google.com/p/waf/
[let me know]: mailto:roop@roopc.net
[vfmd-test]: https://github.com/vfmd/vfmd-test
[vfmd-spec]: http://vfmd.github.io/vfmd-spec/specification/

## Feedback

Bug reports and pull requests are welcome. You can also contact me
at <roop@roopc.net> or
at [@roopeshchander](http://twitter.com/roopeshchander).

