# Compiler Journal

## Resources / References
1. [Engineering a Compiler](http://www.r-5.org/files/books/computers/compilers/writing/Keith_Cooper_Linda_Torczon-Engineering_a_Compiler-EN.pdf), by Keith Copper, Linda Torczon
1. [The elements of computing systems: building a modern computer from first principles](http://f.javier.io/rep/books/The%20Elements%20of%20Computing%20Systems.pdf), by Noam Nisan and Shimon Schocken
1. [Crafting Interpreters](https://craftinginterpreters.com/contents.html), by Robert Nystrom

## Progress
Use this list to include any change in development and to keep track of progress.

PINNED:
- output of the scanner: triplets/quadruples -> (token, line, col[, lexeme])
  - col = index of first character in the line
  - when lexeme is optional either we leave it out or the token and the lexeme will have the same values.
  - scanner's skeleton should implement line counter and col counter
----
- **25/11/21**:
    - **Currently**:
        - small progress on guide
    - **Next**:
        - follow guide of *24/11/21**
- **24/11/21**:
    - **Currently**:
        - scanner had some issues and was not working correctly. Fixing those issues,
        - Experimenting further with lex to get an idea of how the scanner works from a user perspective.
        - Scanner is ready to be generated at this point.
    - **Next**:
        1. start to build scanner generation framework and algorithms
            1. comeup with lex like language to store the language's micro-syntax
            1. build generator
            1. build regex parser
                - regex syntax like lex's!
            1. test
        1. defining C language
            1. gather with the tokens I'll need at the C language
                - this I can just try to copy
            1. come up with the regular expression for the C language micro-syntax
                - this I have to do because I'm controlling my regex language
        1. start working on parser
- **17/11/21**:
    - **Currently**:
        - scanner works with basic test
    - **Next**:
        - test scanner generator lex and see how our output should look like, how are we dealing with edge cases
- **16/11/21**:
    - **Currently**:
        - make scanner.c a cmake program (make a main.c/test.c program to test it)
        - refactoring cutils/arrayi and cutils/string
    - **Next**:
        - make scanner work with basic test
- **04/11/21**:
    - **Currently**:
        - recents days did do most of the goals of *25/10/21**
            - unittests and implementation for cstring.h
            - include array in cutils
        - Quickly create the basic functionality of arrayi that is needed inside the scanner. Any additional functionality will be implemented on need.
    - **Next**:
        - Follow the plan of *22/10/21* and *21/10/21*
- **26/10/21**:
    - **Currently**:
        - created cmocka framework for memory leak
        - started unittesting cstring.h
    - **Next**:
        - **25/10/21**
- **25/10/21**:
    - **Currently**:
        - created cmake and cmocka unittest framework for my cutils
    - **Next**:
        - write unittests for cstring.h
        - test and solve issues if any
        - include arrayi in cutils somehow
        - write unittests for arrayi.h
        - Follow the plan of *22/10/21* and *21/10/21*
- **24/10/21**:
    - **Currently**:
        - chosen build system: cmake
        - ~~chosen unit test framework: [Check](https://github.com/libcheck/check)~~
            - didn't see a sign of extensive usage in the industry
            - ~~using [Google Test framework](https://github.com/google/googletest/) instead because I might benefit more from using it. (leaving Check here for reference)~~
                - extensive use of C++ scared me away...
                - let's see **[cmocka](cmocka.org)**
        - tdd for the basic data structures
    - **Next**:
        - play around with cmocka and cmake
        - create unit tests to test cstring and arrayi structures
        - test scanner skeleton
- **23/10/21**:
    - **Currently**:
        - Dropped the generic array for now, I'm implementing specific array and string classes. Later on I can switch it to using a generic array.
        - Implemented the string and arrayi structs (almost)
    - **Next**:
        - finish implementation of string and arrayi.
        - test these classes!
        - Follow the plan of *22/10/21* and *21/10/21*
- **22/10/21**:
    - **Currently**:
        - small modifications to the data structures and the scanner generator function
    - **Next**:
        - I should consider using the generic array type for the stack instead of the stack_short struct
        - fill out the dummy data structures with actual code
            - test the data structure (write unit tests for them -> lets investigate how they are doing tests in C)
        - implement the tables for the very simple register name analyzer
            - test the scanner and modify until it works
        - try other test-cases
- **21/10/21**:
    - **Currently**:
        - Started implementing the scanner skeleton algorithm which receives a table and classifiers.
        - I've implemented the original algorithm outline from source \[1\].
        - I've used dummy data structures and objects, and will be creating my own from everything. The reason for that is in the future I need to build the standard library in a later stage. Also this makes sure that the usage of the C language will be limited to what I've created in the compiler. (lot of unexpected hard bugs can be eliminated this way)
        - the algorithm can be easily optimized in a lot of ways but first I wanted to make sure that everything is implemented correctly, and it works in theoretical level.
    - **Next**:
        - This should be the expected roadmap:
            1. implement dummy scanner skeleton algorithm
            1. fill in the dummy details
            1. test the algorithm
            1. write test-cases
            1. try to use dummy tables and hand-generated table to test the behavior
            1. start to build scanner generation framework and algorithms
        - And after I feel finished with the scanner generator
            1. come up with the tokens I'll need at the C language
            1. come up with the regular expression for the C language micro-syntax
- **20/10/21**:
    - **Currently**:
        - I still don't understand one thing: How lexical analyzers deal with whitespaces?
            - Is it hard-coded into the skeleton that runs the table?
                - is easy to understand and implement
                - don't see the caveats yet
            - Is it coded into the table as a separate regex was created for them in the languages micro-syntax?
                - I saw people talking about that some scanners contain them as regex-es.
                - In this case at the first pass the scanner goes through the whole text and backtracts to the first lexeme??
                    - this would be nuts...
                    - however!!! --> in C, ";" is a clear separator. Maybe we could split at that symbol...?!
                        - ";" can't be hardcoded into the scanner generator. It would make the generator uncompatible with any other language that does not use this style.
            - **Answering the previous question. All of them.**
                - The moral of the story: **Repeate what you've read! Read it more carefully and you may find your answers.**
                - The key: The scanner process stops when the DFA recognizes a word --> IT DOES NOT SCAN EVERYTHING TO FIND OUT IT IS STUCK
                    - this occurs when the current state has no outbound transition! (Engineering a compiler Section 2.5, page 60)
        - **I decided on the approach**.
            - generated table-scanner, no hardcoded whitespace or ";" separator thing, with appropriate REs the scanner will eliminate these.
            - input of the scanner: text
            - output of the scanner: triplets/quadruples -> (token, line, col[, lexeme])
                - col = index of first character in the line
                - when lexeme is optional either we leave it out or the token and the lexeme will have the same values.
                - scanner's skeleton should implement line counter and col counter
    - **Next**:
        - Design the implementation of the scanner.
        - Create end-to-end tests and tests for each part
        - Parts of the implementation:
            - scanner skeleton
            - scanner table generator
                - regex -> NFA
                    - ...
                - NFA -> DFA
                    - ...
                - (DFA minimization, after tests proved that DFA works)
                - minimized DFA -> generated code
        - Identify what each implementation parts need, e.g.:
            - regex will need a regex parser
            - FA will need a small graph library
            - ...
- **18/10/21**:
    - **Currently**:
        - *Scanner algorithm*: Understood the whole scanner and why backtracking is needed (unfortunately I missed that point until now...). Wrote the algorithm outline for scanner generation and scanner "inference".
    - **Next**:
        - *Decide on implementation*: Going to read through the different scanner implementation/generation strategies and will decide on which one to use

- **08/10/21**:
    - **Currently**:
        - I’ve read first few sections of the Parser
        - I understood the input output relation of the scanner
    - **Next**:
        - Decide which type of scanner should I do (generated, hand-coded…). For this I need to read some scanner sections again
        - Identify the key components that need to be done in order to make the scanner. Build a tree of component in MindMap (Scanner —> RE —> RE engine —> Precedence graph builder —> Graph library)

- **< 08/10/21**:
    - **Currently**:
        - I’ve read the scanning section, twice. Still don’t really understand why and for what we need Scanners for.
    - **Next**:
        - READ the Parser chapter: In order to implement scanner I need to write a parser. For this I should read the chapter first.
        - UNDERSTAND the scanner: I don’t understand why a scanner is needed. I should have a deeper understanding before the implementation.
