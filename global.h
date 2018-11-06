#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>

#ifdef __linux__

#include <sys/prctl.h>

#endif

#include <netinet/in.h>

#include <msgpack.h>

#include "sglib.h"
#include "util.h"

