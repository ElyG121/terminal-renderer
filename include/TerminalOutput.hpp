#pragma once
#include <ostream>
#include "Framebuffer.hpp"

// Writes framebuffer contents to a terminal using ANSI escape codes.
// The stream is injected so tests can capture output in a std::ostringstream.
class TerminalOutput
{
public:
    explicit TerminalOutput(std::ostream &out);
    void begin();                        // clear whole screen once: "\x1b[2J"
    void present(const Framebuffer &fb); // "\x1b[H", then each row + '\n'
private:
    std::ostream &out;
};
