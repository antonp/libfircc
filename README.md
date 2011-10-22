# Introduction
fircc is an IRC client library written in C\+\+.

# Goals
To create a library that is

-	Something I can use to quickly develop IRC bots
	or clients
-   Working (stable)
-   Flexible
-   Easy to understand
-   Easy to use
-   Easy to expand

## Name
The project does not have a very good name, fircc is temporary.

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
-   Bindings to other languages. (working on bindings for Pawn and Python).

## Contact
I would be happy to receive feedback and
suggestions. You can reach me by email.

Anton Petersson <anton at anp dot nu>

