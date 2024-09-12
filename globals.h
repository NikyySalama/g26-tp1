#ifndef GLOBALS_H
#define GLOBALS_H

#define SLAVE_QTY               5

#define MD5_SIZE                32
#define MAX_FILENAME            30
#define MAX_FILES               256
#define MARGIN                  5

#define RESPONSE_SIZE           (MD5_SIZE+MAX_FILENAME+MARGIN)
#define BUFFER_SIZE             (RESPONSE_SIZE * 2) // TODO cambiar por la cantidad inicial del slave

#define DELIMITER               ","


#endif