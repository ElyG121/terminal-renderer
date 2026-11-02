#include "TerminalOutput.hpp"

TerminalOutput::TerminalOutput(std::ostream &out) : out(out)
{
}

void TerminalOutput::begin()
{
    out << "\x1b[2J"; // Clear the whole screen once at startup
}

void TerminalOutput::present(const Framebuffer &fb)
{
    out << "\x1b[H"; // Move cursor to start so the new frame overwrites the old one

    // Write to screen line-by-line
    const std::vector<char> &chars = fb.chars();
    for (int y = 0; y < fb.height(); y++)
    {
        out.write(&chars[y * fb.width()], fb.width());
        out << '\n';
    }

    out.flush(); // Make the whole frame appear at once
}
