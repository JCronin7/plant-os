#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

#include <avr/pgmspace.h>

#define html_header         "HTTP/1.1"
#define button(href, text)  ("<button onclick=\"location.href='" href "'\" type=\"button\">" text "</button>")

const char *pagedata[] PROGMEM = {
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    "HTTP/1.1 200 OK",
    "Content-type:text/html",
    "",
    "<html>",
    "<body>",
    "<h1>SEX</h1>",
    // create the buttons
    button("/LBON", "led on"),
    button("/LBOFF", "led off"),
    "</body>",
    "</html>",
    // The HTTP response ends with another blank line:
    "",
};
const uint32_t pagesize = sizeof(pagedata) / sizeof(char *);

#endif /* _WEBPAGE_H_ */