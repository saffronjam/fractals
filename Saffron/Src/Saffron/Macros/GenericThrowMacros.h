#pragma once

//#define THROW(ExceptionClass, msg, format...)          \
//    {                                                  \
//        char buf[200];                                 \
//        sprintf(buf, msg, format);                     \
//        throw ExceptionClass(__LINE__, __FILE__, buf); \
//    }

#define LogWhat log_warn("\n%s", e.what())
#define LogNoDetails log_warn("\n%s", "No details available");

#ifdef LOG_EXCEPTION
#define LogOnly                           \
    catch (const IException &e)           \
    {                                     \
        log_warn("\n%s", e.what());       \
    }                                     \
    catch (const std::exception &e)       \
    {                                     \
        log_warn("\n%s", e.what());       \
    }                                     \
    catch (...)                           \
    {                                     \
        log_warn("No details available"); \
    }
#else
#define LogOnly                     \
    catch (const IException &e)     \
    {                               \
    }                               \
    catch (const std::exception &e) \
    {                               \
    }                               \
    catch (...)                     \
    {                               \
    }
#endif