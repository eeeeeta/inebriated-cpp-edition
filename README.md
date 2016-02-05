inebriated, C++ edition (βeta)
==============================
*everybody's favourite drunkard!*

![Logo](http://i.imgur.com/afdr6cr.png)

wat?
---

The inebriated project line is a series of implementations of the same thing: a drunk AI that takes
input sentences and uses them to form stuff that sort-of makes sense and is sometimes funny.

Links to the other inebriateds:

- [the inebriated project spotlight](http://theta.eu.org/inebriated-family.html)
- [inebriated: the original edition](https://github.com/eeeeeta/inebriated-genesis)
- [inebriated, C edition](https://github.com/eeeeeta/inebriated-c-edition)

Compile it!
-----------

This thing needs the venerable [CMake](https://cmake.org/) installed, and pretty much nothing else
except your bog-standard C++ environment. To play around with it:

    cmake -DCMAKE_BUILD_TYPE=Release .
    make
    
    ./markov
    
If you want to compile it with the verbose debug stuff:

    cmake -DCMAKE_BUILD_TYPE=Debug .
    make

port status
-----------

I'm new with this C++ stuff, be gentle! This code so far isn't as advanced as the C implementation,
but is completely memory-safe (for now) and is much more concise (thank you, C++). It has multiple failures,
including the addition of duplicates, that must be corrected, and many things I'd still like to add.

Things that work
----------------

- Sentence input, tokenisation & storage
- *Decent* weighting!
- Sentence generation
- Basic saving & loading support

Things to be done
-----------------

- Recursion checking. This is a small big problem.
- Corpus control - control of input and output sources & finer-grained saving and loading
- Logging control. Right now, it's a firehose of debug info.
- A nice UI maybe? And/or CLI?
- Perhaps something to say "no, I don't like this sentence" or a hunk deletion interface
