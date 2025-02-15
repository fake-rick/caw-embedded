#include "log.h"
#include "../protocols/code.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static log_t global_log_object;

const char* log_templates_colorful[] = {
    "\033[1;36m[DEBUG] <%s> | <%d> > %s\033[m\r\n",
    "\033[1;32m[INFO] <%s> | <%d> > %s\033[m\r\n",
    "\033[1;33m[WARN] <%s> | <%d> > %s\033[m\r\n",
    "\033[1;31m[ERROR] <%s> | <%d> > %s\033[m\r\n"};

const char* log_templates[] = {"[D] %s # %s", "[I] %s # %s", "[W] %s # %s",
                               "[E] %s # %s"};

int log_init(device_t* dev) { global_log_object.dev = dev; }

void log_write(const char* fmt, log_level_e level, const char* file, int line,
               const char* func, ...) {
  log_protocol_t log_protocol;
  char tmp[100];
  va_list args;
  va_start(args, fmt);
  vsprintf(tmp, (char*)fmt, args);
  va_end(args);

#ifdef LOG_COLORFUL
  snprintf(global_log_protocol.buf, sizeof(global_log_protocol.buf),
           log_templates_colorful[level], file, line, tmp);
#else
  snprintf(log_protocol.buf, sizeof(log_protocol.buf), log_templates[level],
           func, tmp);
#endif
  protocol_header_init(&log_protocol, main_code_system, sub_code_system_log, 0,
                       strlen(log_protocol.buf) + 1);
  device_write_buffer(global_log_object.dev, &log_protocol,
                      strlen(log_protocol.buf));
}

void _printf(const char* fmt, ...) {
  log_protocol_t log_protocol;
  va_list args;
  va_start(args, fmt);
  vsprintf(log_protocol.buf, (char*)fmt, args);
  va_end(args);
  protocol_header_init(&(log_protocol.header), main_code_system,
                       sub_code_system_log, 0, strlen(log_protocol.buf));
  device_write_buffer(global_log_object.dev, &log_protocol,
                      sizeof(protocol_header_t) + strlen(log_protocol.buf));
}