Code Philosophy
===============
This is not gospel. These are ideas and approaches
that work for me, and seem to work for code I respect.

There's a couple of ways to write code: We regard it as simply the input to a compiler
or interpreter, and hence it's written to take advantage of language features, to maximize
the efficiency of object code and to minimize keystrokes and working space requirements.
_We do not do this_.

Instead we expect each line to be read and modified by unknown other people many times. They'll be
unfamiliar with the code and working under pressure with many other responsibilities. So we write
code to clearly express our intent, to lead the reader through the task at hand, to educate them
about the problem space and our understanding of it. We might even strive to write code so that
reading it is a pleasurable and invigorating experience.

When we sit down at the keyboard we expect what we write to be published in some journal, with
no prior opportunity for revision, so we want to be proud of it. Obviously we don't all write code
the same way. Our experiences and individual predelictions shape what we write, whether it be code
or prose.

We're careful when working with code that's collaborative, because the clear vision laid down
by the initial author is blurred with each incremental change. Sometimes that code's clarity
declines to the point that it must be rewritten, not because it doesn't work but because it's
no longer clearly intelligible, and thus ceases to be comprehensible for use or as a basis
for extension.


Guidelines
==========
Some development communities have rigid rules about low-level formatting and high-level
organization. I think dogmatic rules are neither necessary nor sufficient for intelligent and
experienced professionals. But these are the guidelines I use for myself.


Kindness
--------
Code is communication from your present state of mind to other people and to your future self.
Be kind to them.


Whitespace
----------
Different text editing environments use different fonts, styles, tab stops and
space widths. The only line-to-line alignment that you can count on is whitespace at the start of
a line. Creating a regular, intelligible and generally pleasing appearance by using embedded tabs
or spaces will create code that looks like its been through a shredder on most other people's
machines. Find some other way to make your code readable.

The rules of the language dictate that there are some things that
are going to be dense and ugly. Ameliorate that ugliness with whitespace, setting it off from
the rest of the source. An empty line before and after is usually all that's needed. Even clean
code should have a chunked feel to it, a handful of lines that address a particular sub-task,
then another handful that have a slightly different focus. Break them up with whitespace, just
as prose is broken into paragraphs. Within expressions use spaces to help the reader, making
space around operators so they're clearly visible, and breaking long expressions over
multiple lines, with appropriate indentation.


Have the courage of your convictions
------------------------------------
If you need to change some code then don't just tweak it, _change it_ ...

_Dive in and immerse yourself_. Make it lively and effective, stamped with
your personality and perspective. Sure, use preprocessor macros as scaffolding to let you switch
quickly amongst alternatives. But when you have confidence in a result, then _remove_ the old code.
People don't really want to see "The Making of Star Wars" in preference to "Star Wars" itself. They
don't want to read a magazine article or a book littered with change bars. They deserve the most
distilled expression of which the writer or director is capable. If you're called upon to direct
"Hamlet" having only ever done story boards for "South Park", then reach out to others who will
provide support and guidance.


Lexical scoping is your friend
------------------------------
We're working with C++. We're able to declare variables in
places that C never allowed. So do so. Declare variables as close to their point of first usage as
possible. A variable that is declared too early may inadvertantly be used without having been
given an appropriate value (doing so with simple types will trigger a warning, complex types won't.)
Use different names in different scopes if the variables have different intents, but don't be scared
to reuse a name in different scopes if that's what makes the code more readable.


Comments
--------
Generally the more powerful is a language feature the less often is it used.
I would claim that in any language comments are the most powerful feature of all; comments can be
placed anywhere, can contain anything and can express things formal code cannot. Use a comment to
make clear to a reader only what _cannot_ be made clear in code. Remember that comments
can express falsehoods no compiler will ever catch.


Question your first solution
----------------------------
Minsky said _"You don't understand anything until you learn it more than one way."_
When you write code to achieve some objective don't stop when it's working.
Find another solution, and another, and another until you have a suite of possibilities amongst
which to choose. Then you'll have created the best of which you're capable, rather than the least
with which you can get away.


The Most Powerful Debugger
--------------------------
The most powerful debugger is between your ears. A source level debugger is no substitute for a
deep understanding of the problem space and the code that tackles that problem space. Use the
debugger to confirm or deny hypotheses you've already formulated based on your brainwork. And
remember the second law of thermodynamics -- a random change is going to make things worse.

