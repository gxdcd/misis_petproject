module;

#pragma region Info
/*
    https://learn.microsoft.com/en-us/windows/win32/debug/using-an-exception-handler
    https://learn.microsoft.com/en-us/windows/win32/debug/getexceptioncode
    https://learn.microsoft.com/en-us/windows/win32/debug/getexceptioninformation
    https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record

    Effective Exception Handling in Visual C++
    https://www.codeproject.com/Articles/207464/Exception-Handling-in-Visual-Cplusplus

    How to generate stack trace from SEH exception
    https://stackoverflow.com/questions/26790042/how-to-generate-stack-trace-from-seh-exception

    Walking the callstack
    https://github.com/JochenKalmbach/StackWalker
    https://www.codeproject.com/Articles/11132/Walking-the-callstack-2
*/
#pragma endregion

#pragma region Header
#include <Windows.h>
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp.lib")
export module seh;
import logging;
#pragma endregion

constexpr int MAX_FRAMES = 10;
constexpr int MAX_SYMBOL_NAME_LENGTH = 255;

#define STATUS_POSSIBLE_DEADLOCK (0xC0000194L)

void log_error_description(DWORD code)
{
    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record
    // https://learn.microsoft.com/en-us/windows/win32/debug/getexceptioncode
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:
            logging::error(
                "EXCEPTION_ACCESS_VIOLATION"
                ": "
                "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.");
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            logging::error(
                "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"
                ": "
                "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.");
            break;
        case EXCEPTION_BREAKPOINT:
            logging::error(
                "EXCEPTION_BREAKPOINT"
                ": "
                "A breakpoint was encountered.");
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            logging::error(
                "EXCEPTION_DATATYPE_MISALIGNMENT"
                ": "
                "The thread tried to read or write data that is misaligned on hardware that does not provide alignment.");
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            logging::error(
                "EXCEPTION_FLT_DENORMAL_OPERAND"
                ": "
                "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.");
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            logging::error(
                "EXCEPTION_FLT_DIVIDE_BY_ZERO"
                ": "
                "The thread tried to divide a floating-point value by a floating-point divisor of zero.");
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            logging::error(
                "EXCEPTION_FLT_INEXACT_RESULT"
                ": "
                "The result of a floating-point operation cannot be represented exactly as a decimal fraction.");
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            logging::error(
                "EXCEPTION_FLT_INVALID_OPERATION"
                ": "
                "This exception represents any floating-point exception not included in this list.");
            break;
        case EXCEPTION_FLT_OVERFLOW:
            logging::error(
                "EXCEPTION_FLT_OVERFLOW"
                ": "
                "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.");
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            logging::error(
                "EXCEPTION_FLT_STACK_CHECK"
                ": "
                "The stack overflowed or underflowed as the result of a floating-point operation.");
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            logging::error(
                "EXCEPTION_FLT_UNDERFLOW"
                ": "
                "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.");
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            logging::error(
                "EXCEPTION_ILLEGAL_INSTRUCTION"
                ": "
                "The thread tried to execute an invalid instruction.");
            break;
        case EXCEPTION_IN_PAGE_ERROR:
            logging::error(
                "EXCEPTION_IN_PAGE_ERROR"
                ": "
                "The thread tried to access a page that was not present, and the system was unable to load the page.");
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            logging::error(
                "EXCEPTION_INT_DIVIDE_BY_ZERO"
                ": "
                "The thread tried to divide an integer value by an integer divisor of zero.");
            break;
        case EXCEPTION_INT_OVERFLOW:
            logging::error(
                "EXCEPTION_INT_OVERFLOW"
                ": "
                "The result of an integer operation caused a carry out of the most significant bit of the result.");
            break;
        case EXCEPTION_INVALID_DISPOSITION:
            logging::error(
                "EXCEPTION_INVALID_DISPOSITION"
                ": "
                "An exception handler returned an invalid disposition to the exception dispatcher.");
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            logging::error(
                "EXCEPTION_NONCONTINUABLE_EXCEPTION"
                ": "
                "The thread tried to continue execution after a noncontinuable exception occurred.");
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            logging::error(
                "EXCEPTION_PRIV_INSTRUCTION"
                ": "
                "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.");
            break;
        case EXCEPTION_SINGLE_STEP:
            logging::error(
                "EXCEPTION_SINGLE_STEP"
                ": "
                "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.");
            break;
        case EXCEPTION_STACK_OVERFLOW:
            logging::error(
                "EXCEPTION_STACK_OVERFLOW"
                ": "
                "The thread used up its stack.");
            break;
        case EXCEPTION_GUARD_PAGE:
            logging::error(
                "EXCEPTION_GUARD_PAGE"
                ": "
                "The thread accessed memory allocated with the PAGE_GUARD modifier. This value is defined as STATUS_GUARD_PAGE_VIOLATION.");
            break;
        case EXCEPTION_INVALID_HANDLE:
            logging::error(
                "EXCEPTION_INVALID_HANDLE"
                ": "
                "The thread used a handle to a kernel object that was invalid (probably because it had been closed.) This value is defined as STATUS_INVALID_HANDLE.");
            break;
        case EXCEPTION_POSSIBLE_DEADLOCK:
            logging::error(
                "EXCEPTION_POSSIBLE_DEADLOCK"
                ": "
                "See https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-entercriticalsection");
            break;
    }
}

void log_stacktrace(CONTEXT* context)
{
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    // Initialize the symbol handler
    SymInitialize(process, NULL, TRUE);

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

    DWORD machineType;
#ifdef _M_IX86
    machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context->Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    machineType = IMAGE_FILE_MACHINE_IA64;
    stackFrame.AddrPC.Offset = context->StIIP;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->IntSp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrBStore.Offset = context->RsBSP;
    stackFrame.AddrBStore.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->IntSp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#else
#    error "Platform not supported!"
#endif

    for (int frame = 0; frame < MAX_FRAMES; ++frame) {
        if (!StackWalk64(machineType, process, thread, &stackFrame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
            break;

        DWORD64 displacement = 0;
        char symbolBuffer[sizeof(SYMBOL_INFO) + (MAX_SYMBOL_NAME_LENGTH + 1) * sizeof(char)];
        SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
        symbol->MaxNameLen = MAX_SYMBOL_NAME_LENGTH;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        std::stringstream ss;
        if (SymFromAddr(process, (DWORD64)(stackFrame.AddrPC.Offset), &displacement, symbol)) {
            ss << frame << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address;
            IMAGEHLP_LINE64 line;
            DWORD displacement32 = 0;
            if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &displacement32, &line))
                ss << " (" << line.FileName << ":" << std::dec << line.LineNumber << ")";
        } else {
            ss << frame << ": [Unknown function] - 0x" << std::hex << stackFrame.AddrPC.Offset;
        }
        logging::error("{}", ss.str());
    }

    SymCleanup(process);
}

namespace seh {
    export int exception_handler(void* exception_pointers)
    {
        auto* ep = static_cast<EXCEPTION_POINTERS*>(exception_pointers);
        log_error_description(ep->ExceptionRecord->ExceptionCode);
        log_stacktrace(ep->ContextRecord);
        return EXCEPTION_EXECUTE_HANDLER;
    }
}