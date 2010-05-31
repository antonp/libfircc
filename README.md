# Introduction
firc is an IRC client library written in C\+\+.

# Goals
To create a library that is
- Working (stable)
- Flexible
- Easy to understand
- Easy to use
- Easy to expand

And personally I would also like to
- Learn more about open source.
- Learn how to write portable code.

# Current status
Nothing has been released yet.
Using the library is not recommended yet as things might still
change dramatically. This may be frustrating. You are welcome
to try though.

## Name
The project does not have a very good name, firc is temporary.

## Current features
- Connect to multiple servers/networks.
- Anti-flood implemented with an outgoing message queue.
- Event system (IRC messages, new connections, etc).
- Cache/database that keeps track of channels and users.
- Simple plugin manager.

## Todo
- There is some work to be done on the network cache/database.
- Open up the network interface some more.
- Documentation.
- Improve SConstruct to allow for some command line arguments.
- Verify status of release build.

## Future features
- Let worker threads handle eventdispatching.
- DCC ?
- Windows port.

## Contact
I would be happy to receive feedback (good or bad) and
suggestions. You can reach me by email.

Anton Petersson <anton-vcs@anp.nu>

