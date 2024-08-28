# Important files for internals

See: https://clangd.llvm.org/design/code#features

Main is under ClangDMain which gets either a JSONTransport or XPCTransport layer and runs the LSP Server. This calls into the infinite while loop for handling requests under ClangDLSPServer, which then uses Protocol for MSFT's specification on the protocol.

LSPBinder binds the methods to the name of the method like "document/OnOpen" which binds to the OnOpen method specified.

There are also some really neat optimizations like the Preamble for caching AST's of headers(as PCH's) which allow our used-to micro second completions.
