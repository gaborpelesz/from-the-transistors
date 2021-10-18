# Compiler Journal

## Resources / References
[Engineering a Compiler](http://www.r-5.org/files/books/computers/compilers/writing/Keith_Cooper_Linda_Torczon-Engineering_a_Compiler-EN.pdf), by Keith Copper, Linda Torczon  
[The elements of computing systems: building a modern computer from first principles](http://f.javier.io/rep/books/The%20Elements%20of%20Computing%20Systems.pdf), by Noam Nisan and Shimon Schocken  
[Crafting Interpreters](https://craftinginterpreters.com/contents.html), by Robert Nystrom

## Progress
Use this list to include any change in development and to keep track of progress.

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
