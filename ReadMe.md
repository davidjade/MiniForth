
# MiniForth -  a minimal Forth Interpreter

I created this minimal Forth Interpreter while exploring ideas about creating a new type of 
Forth-like language. This is just the baseline benchmark I created for the simplest yet 
pretty efficient Forth interpreter I could create in C. It is written in C rather than assembly 
because the goal was to just create a baseline that I could easily change as needed for tests.

This is not a complete Forth system though - it is just a minimal Inner Interpreter that can
run one small statically defined Forth program, a rather (intentionally) poor sorting algorithm.
That program written in Forth, is in the sort4th.txt file (for reference only). 

Because it is so stripped down it highlights just how simple a Forth language implementation
can be and that to me, makes it interesting and perhaps even educational.

I have made minimal effort to clean this up but decided to put it our there as-is
because news.ycombinator.com was recently having a bit of a "Forth week" of Forth topics.

---

## License

- **[MIT license](http://opensource.org/licenses/mit-license.php)**
- Copyright 2021 © David Jade.
