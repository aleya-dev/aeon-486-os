#include "hexdump.h"
#include <drivers/display/display.h>
#include <aeon/ctype.h>

void khexdump(const void *src, const ksize_t len)
{
    ksize_t i = 0;
    ksize_t c = 0;

    const unsigned char *data = src;

    kprintf("--------|------------------------------------------------|----------------|\n");
    kprintf(" offset |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|\n");
    kprintf("--------|------------------------------------------------|----------------|\n");

    for (; i < len;)
    {
        ksize_t start;
        ksize_t written;

        start = i;
        kprintf("%08x|", (unsigned int)(i));
        for (c = 0; c < 16 && i < len;) // write 16 bytes per line
        {
            kprintf("%02x ", (int)(data[i]));
            ++i;
            ++c;
        }

        written = c;
        for (; c < 16; ++c) // finish off any incomplete bytes
            kprintf("   ");

        // write the text part
        kprintf("|");
        for (c = 0; c < written; ++c)
        {
            const unsigned char byte = data[start + c];
            if (isprint((int)(byte)))
                kprintf("%c", (char)(byte));
            else
                kprintf(".");
        }

        for (; c < 16; ++c)
            kprintf(" ");

        kprintf("|\n");
    }

    kprintf("---------------------------------------------------------------------------\n");
}
