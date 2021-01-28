/* specified manually on Windows [automaticaly set on Unix] */
#ifndef PARSER_VERSION
#ifdef _WIN64
#define PARSER_VERSION "3.4.7b (compiled on amd64-pc-win64)"
#else
#define PARSER_VERSION "3.4.7b (compiled on i386-pc-win32)"
#endif
#endif
