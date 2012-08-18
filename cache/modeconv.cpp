/*

Copyright (c) 2012, Anton Petersson < anton (at) anp.nu >
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdexcept>
#include "inc/modeconv.h"

namespace anp
{
namespace irc
{

bool modeconv_validMode(char mchar)
{
    switch ( mchar )
    {
    // Modes affecting user relations
    case 'v': // voice
    case 'h': // half-op
    case 'o': // op
    case 'a': // protected
    case 'q': // founder
    case 'y': // IRCoper
        return true;
    default:
        return false;
    }
}

char modeconv_parseNick(std::string &nick)
{
    char mode = ' ';
    switch ( nick[0] ) {
    case '+':// voice
        mode = 'v';
        nick.erase(0, 1);
        break;
    case '%':// half-op
        mode = 'h';
        nick.erase(0, 1);
        break;
    case '@':// op
        mode = 'o';
        nick.erase(0, 1);
        break;
    case '&':// protected
        mode = 'a';
        nick.erase(0, 1);
        break;
    case '~':// founder
        mode = 'q';
        nick.erase(0, 1);
        break;
    case '!':// IRCoper
        mode = 'y';
        nick.erase(0, 1);
        break;
    default:
        break; // No error; user has no mode
    }

    return mode;
}

} // namespace irc
} // namespace anp

