# Markdown

A simple Elixir Markdown to HTML conversion library.

Implemented entirely as a [NIF][1] binding to the [Hoedown][2] library.

## Usage

```iex
iex> Markdown.to_html "# Hello World"
"<h1>Hello World</h1>\n"
iex> Markdown.to_html "http://elixir-lang.org/", autolink: true
"<p><a href=\"http://elixir-lang.org/\">http://elixir-lang.org/</a></p>\n"
```

### Options

* `:tables` - Enables Markdown Extra style tables (default: `false`)
* `:fenced_code` - Enables fenced code blocks (default: `false`)
* `:autolink` - Automatically turn URLs into links (default: `false`)

## TODO

* Add more options to the NIF that are [available in Hoedown][3]
* ANSI renderer

[1]: http://www.erlang.org/doc/tutorial/nif.html
[2]: https://github.com/hoedown/hoedown
[3]: https://github.com/craigbarnes/hoedown/blob/09cfaed19d037e82739f8336f8a7ceb5cbf1ce35/examples/hoedown.c#L17
