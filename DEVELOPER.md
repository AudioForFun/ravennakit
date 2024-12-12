# Notes for developers

## Conventions

### Include order

Include headers in the following order:

- Local project headers
- External dependency headers
- Standard headers

Each block separated by a space (to not have the include order changed by clang-format).

### Exception policy

This library adopts exceptions as the primary mechanism for error handling. Exceptions provide a clean, structured way
to manage error conditions, allowing the separation of normal code paths from error-handling logic. By utilizing
exceptions, the library aims to improve code readability and maintainability, while ensuring robust error management.

All exceptions are used to signal error conditions, and the library encourages developers to handle exceptions at
appropriate levels of abstraction. This ensures that resources are properly released and errors are dealt with in a way
that minimizes disruption to the overall application.

Key principles for using exceptions:

Use exceptions for error conditions: Exceptions should be thrown when an operation fails and cannot continue normally.
This allows the caller to handle the error without cluttering the code with manual error checks.

Avoid using exceptions for control flow: Exceptions should represent truly exceptional circumstances, not be used for
routine operations or logic control. They should be reserved for scenarios where normal execution cannot proceed.

Ensure exception safety: All code must be exception-safe. Resources must be managed using RAII (Resource Acquisition Is
Initialization) or other techniques that guarantee proper cleanup, even in the presence of exceptions. This ensures that
no resources are leaked during error conditions.

Document thrown exceptions: Functions and methods should clearly document which exceptions may be thrown and under what
conditions, allowing users of the library to understand potential error scenarios and handle them appropriately.

Catch exceptions where it makes sense: Exceptions should be caught and handled at a level where meaningful recovery can
occur. If recovery is impossible, exceptions should propagate up to higher layers, where the application can decide how
to respond.

In some cases, the library uses return values for error handling. This approach is appropriate when failures are
expected and common, or when using exceptions would cause issues with stack unwinding. By returning error codes or
status values, the library can handle scenarios where predictable failures occur without triggering the overhead of
exception handling.

## Quick commands

### Send audio as RTP stream example command

    ffmpeg -re -stream_loop -1 -f s16le -ar 48000 -ac 2 -i Sin420Hz@0dB16bit48kHzS.wav -c:a pcm_s16be -f rtp -payload_type 10 rtp://127.0.0.1:5004

## Resources

### RTSP

RTSP Test tool (also has a very good explanation of RTSP):  
[RTSPTest](https://github.com/rayryeng/RTSPTest)

### SDP

sdpoker test tool:  
[sdpoker](https://github.com/AMWA-TV/sdpoker)

### C++ web servers

https://github.com/oatpp/oatpp
https://github.com/civetweb/civetweb  
https://github.com/drogonframework/drogon  
https://github.com/uNetworking/uWebSockets  
https://github.com/Stiffstream/restinio  
https://github.com/CrowCpp/Crow  
https://github.com/pistacheio/pistache  (only Linux and macOS)

### Sample rate converter libraries

Audio Resampler:  
https://github.com/dbry/audio-resampler  
Offers dynamic ratio for asrc purposes and provides methods for getting the internal state (phase) for precise
resampling.

Secret Rabbit Code (libsamplerate):  
https://github.com/libsndfile/libsamplerate  
Offers dynamic ratio for asrc purposes but does not provide methods for getting the internal state (phase) for precise
resampling.
