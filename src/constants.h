/** @file constants.h Common functionality + constants for all operations */

#pragma once

namespace genetic {

// Max number of threads per block to use with tournament and evaluation kernels
const int GENE_TPB = 256;

// Max size of stack used for AST evaluation
const int MAX_STACK_SIZE = 20;

}  // namespace genetic
