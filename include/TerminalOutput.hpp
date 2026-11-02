#pragma once
#include <ostream>
#include "Framebuffer.hpp"

// Writes framebuffer contents to a terminal using ANSI escape codes.
// The stream is injected so tests can capture output in a std::ostringstream.
// RAII: if begin() changed the terminal, the destructor puts it back.
class TerminalOutput
{
public:
    explicit TerminalOutput(std::ostream &out);
    ~TerminalOutput(); // show the cursor again if begin() hid it

    // Copying would restore the terminal twice, so it is not allowed
    TerminalOutput(const TerminalOutput &) = delete;
    TerminalOutput &operator=(const TerminalOutput &) = delete;

    void begin();                        // hide the cursor and clear the screen once
    void present(const Framebuffer &fb); // "\x1b[H", then each row + '\n'
private:
    std::ostream &out;
    bool started = false; // whether begin() has changed the terminal
};
