#include "Arduino.h"
#include "utils.h"

void splitString(String str, char delimiter, String *str_ptr)
{
    int startIndex = 0;                    // Start of the substring
    int endIndex = str.indexOf(delimiter); // Find the first delimiter
    int count = 0;

    while (endIndex != -1)
    {
        // Extract the substring
        String part = str.substring(startIndex, endIndex);
        str_ptr[count] = part;

        // Update startIndex and find the next delimiter
        startIndex = endIndex + 1;
        endIndex = str.indexOf(delimiter, startIndex);
        count++;
    }
}