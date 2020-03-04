#pragma once

#include "VirtualMachine.h"

using addressmap = std::map<std::string, uint64_t>;

vmcode assemble(std::string assembly, addressmap addresses);