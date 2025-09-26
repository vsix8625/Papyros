#include "prs_utils.h"
#include "prs_core.h"

void PRS_Trim(char *s)
{
    char *end;
    while (isspace((uint8_t) *s))
    {
        s++;
    }

    if (*s == 0)
    {
        return;
    }
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char) *end))
    {
        *end-- = '\0';
    }
    memmove(s, s, strlen(s) + 1);
}
