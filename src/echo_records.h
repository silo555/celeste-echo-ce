#pragma once
#include "echo_state.h"

extern echo::State echo_state;
void echo_records_load();
bool echo_records_save();
bool echo_records_save_failed();
