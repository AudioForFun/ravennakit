# Notes for developers

## Conventions

### Include order

Include headers in the following order:

- Local project headers
- External dependency headers
- Standard headers

Each block separated by a space (to not have the include order changed by clang-format).

### Exception policy

This library uses return values for error handling, and exceptions are only thrown for critical failures that are equivalent to program termination. Exceptions should never be used for control flow and are reserved for situations where recovery is either impossible or highly unlikely.

All code must be exception-safe, meaning that resources must be managed using RAII (Resource Acquisition Is Initialization) to ensure proper cleanup in the event of an exception. While exceptions are considered severe and may justify terminating the program, there is flexibility to handle them in specific cases where recovery might be feasible.

To summarize:

- Exceptions are accepted, but only for critical, unrecoverable errors.
- Throwing an exception is considered equivalent to aborting the program, but there is some room for handling in exceptional cases.
- Never use exceptions to control the program's flow.
- The aim is to never have any exceptions raised in the codebase.
