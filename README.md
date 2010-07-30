# Introduction
firc is an IRC client library written in C\+\+.

# Goals
To create a library that is

-	Something I can use to quickly develop IRC bots
	or clients quickly
-   Working (stable)
-   Flexible
-   Easy to understand
-   Easy to use
-   Easy to expand

And personally I would also like to

-   Learn more about open source projects.
-   Become more familiar with writing portable code.

# Current status
Nothing has been released yet.
Using the library is not recommended yet as APIs might still
change. This may be frustrating. You are welcome to try though.

## Name
The project does not have a very good name, firc is temporary.

## Current features

-   Connect to multiple servers/networks.
-   Anti-flood implemented with an outgoing message queue.
-   Event system (IRC messages, new connections, etc).
-   Cache/database that keeps track of channels and users.
-   Simple plugin manager.
-	Thread safety (at least aiming for it)

## Future features

-   Let worker threads handle eventdispatching.
-   DCC ?
-   Windows compatibility
-   Bindings to other languages.

## Contact
I would be happy to receive feedback (good or bad) and
suggestions. You can reach me by email.

Anton Petersson <anton-vcs at anp dot nu>

