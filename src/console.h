#pragma once

#include <atomic>
#include <array>
#include <thread>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <utility>
#include <print>
#include <source_location>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <optional>

namespace BlackTek::Console
{
    enum class MessageType : uint8_t
    {
        Print,
        Log,
        LogAndPrint,
        StyledPrint,
        LogAndStyledPrint,
        DebugPrint,
        DebugLog,
        DebugLogAndPrint
    };

    enum class PriorityType : uint8_t
    {
        None,
        Info,
        Warning,
        Error
    };

    struct Message
    {
        using Style = fmt::text_style;

        std::string text;
        std::optional<Style> secondary_style{};
        Style primary_style{};
        MessageType msg_type{MessageType::Print};
        PriorityType priority{PriorityType::None};

        bool styled { false };
        bool linebreak { true };

        Message() = default;
        Message(std::string_view msg_data, MessageType message_type, bool l_break = true, bool style = false) : text(msg_data), msg_type(message_type), linebreak(l_break), styled(style) {}
        Message(std::string_view msg_data, MessageType message_type, Style main_style, bool l_break = true) : text(msg_data), msg_type(message_type), primary_style(main_style), linebreak(l_break) {}
        Message(std::string_view msg_data, MessageType message_type, Style main_style, std::optional<Style> other_style, bool l_break = true,  bool style = true) : text(msg_data), msg_type(message_type), primary_style(main_style), secondary_style(other_style), linebreak(l_break), styled(style) {}

        Message(std::string msg_data, MessageType message_type, bool l_break = true, bool style = false) : text(msg_data), msg_type(message_type), linebreak(l_break), styled(style) {}
        Message(std::string msg_data, MessageType message_type, Style main_style, bool l_break = true) : text(msg_data), msg_type(message_type), primary_style(main_style), linebreak(l_break) {}
        Message(std::string msg_data, MessageType message_type, Style main_style, std::optional<Style> other_style, bool l_break = true, bool style = true) : text(msg_data), msg_type(message_type), primary_style(main_style), secondary_style(other_style), linebreak(l_break), styled(style) {}
    };


    inline constexpr size_t BUFFER_SIZE = 4096;
    inline constexpr size_t BUFFER_MASK = BUFFER_SIZE - 1;

    inline std::jthread worker;
    inline std::atomic<bool> running = false;
    inline std::atomic<size_t> notifyCounter{0};
    inline std::ofstream logfile;

    // We make this a configuration option
    // The purpose is that we can log everything appropriately,
    // and then the user can decide which things also print to their
    // console when being logged, based on priority level
    inline PriorityType print_level = PriorityType::None;

    class RingBuffer
    {
    private:
        struct Slot
        {
            std::atomic<size_t> sequence;
            Message message;
        };

    public:
        RingBuffer()
        {
            for (size_t i = 0; i < BUFFER_SIZE; ++i)
                buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }

        bool Push(Message&& msg) noexcept
        {
            size_t pos = head_.load(std::memory_order_relaxed);

            for (;;)
            {
                Slot& slot = buffer_[pos & BUFFER_MASK];
                size_t seq = slot.sequence.load(std::memory_order_acquire);

                intptr_t diff = (intptr_t)seq - (intptr_t)pos;

                if (diff == 0)
                {
                    if (head_.compare_exchange_weak(
                            pos, pos + 1,
                            std::memory_order_relaxed))
                    {
                        slot.message = std::move(msg);
                        slot.sequence.store(pos + 1, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    return false; // full
                }
                else
                {
                    pos = head_.load(std::memory_order_relaxed);
                }
            }
        }

        bool Pop(Message& out) noexcept
        {
            size_t pos = tail_.load(std::memory_order_relaxed);
            Slot& slot = buffer_[pos & BUFFER_MASK];

            size_t seq = slot.sequence.load(std::memory_order_acquire);
            intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

            if (diff == 0)
            {
                tail_.store(pos + 1, std::memory_order_relaxed);

                out = std::move(slot.message);
                slot.sequence.store(
                    pos + BUFFER_SIZE,
                    std::memory_order_release);

                return true;
            }

            return false;
        }

    private:
        std::array<Slot, BUFFER_SIZE> buffer_;
        alignas(64) std::atomic<size_t> head_{0};
        alignas(64) std::atomic<size_t> tail_{0};
    };


    inline RingBuffer queue;


    inline void Worker(std::stop_token stop)
    {
        Message msg;
        size_t observed = notifyCounter.load(std::memory_order_acquire);

        const auto printStyled = [&]()
        {
            if (msg.secondary_style.has_value())
                fmt::print(msg.primary_style, "{}", fmt::styled(msg.text, msg.secondary_style.value()));
            else
                fmt::print(msg.primary_style, "{}", msg.text);

            if (msg.linebreak)
                std::println();
        };

        const auto printPlain = [&]()
        {
            std::println("{}", msg.text);
        };

        const auto logToFile = [&]()
        {
            if (logfile.is_open())
                logfile << msg.text << '\n';
        };

        while (not stop.stop_requested())
        {
            while (queue.Pop(msg))
            {
                switch (msg.msg_type)
                {
                    case MessageType::Print:
                        msg.styled ? printStyled() : printPlain();
                        break;

                    case MessageType::Log:
                        logToFile();
                        break;

                    case MessageType::LogAndPrint:
                        msg.styled ? printStyled() : printPlain();
                        logToFile();
                        break;

                    case MessageType::StyledPrint:
                        printStyled();
                        break;

                    case MessageType::LogAndStyledPrint:
                        printStyled();
                        logToFile();
                        break;

                    case MessageType::DebugPrint:
                        msg.styled ? printStyled() : printPlain();
                        break;

                    case MessageType::DebugLog:
                        logToFile();
                        break;

                    case MessageType::DebugLogAndPrint:
                        msg.styled ? printStyled() : printPlain();
                        logToFile();
                        break;
                }
            }

            notifyCounter.wait(observed, std::memory_order_relaxed);
            observed = notifyCounter.load(std::memory_order_acquire);
        }

        while (queue.Pop(msg))
        {
            switch (msg.msg_type)
            {
                case MessageType::Print:
                    msg.styled ? printStyled() : printPlain();
                    break;

                case MessageType::Log:
                    logToFile();
                    break;

                case MessageType::LogAndPrint:
                    msg.styled ? printStyled() : printPlain();
                    logToFile();
                    break;

                case MessageType::StyledPrint:
                    printStyled();
                    break;

                case MessageType::LogAndStyledPrint:
                    printStyled();
                    logToFile();
                    break;

                case MessageType::DebugPrint:
                    msg.styled ? printStyled() : printPlain();
                    break;

                case MessageType::DebugLog:
                    logToFile();
                    break;

                case MessageType::DebugLogAndPrint:
                    msg.styled ? printStyled() : printPlain();
                    logToFile();
                    break;
            }
        }
    }

    inline void Initialize(const std::string& logFileName = "blacktek.log")
    {
        if (running.exchange(true))
            return;

        logfile.open(logFileName, std::ios::out | std::ios::app);
        worker = std::jthread(Worker);
    }

    inline void Shutdown()
    {
        if (not running.exchange(false))
            return;

        worker.request_stop();
        notifyCounter.fetch_add(1, std::memory_order_release);
        notifyCounter.notify_one();
        worker.join();

        if (logfile.is_open())
            logfile.close();
    }

    inline void PushAndNotify(Message&& msg)
    {
        while (not queue.Push(std::move(msg)))
            std::this_thread::yield();

        notifyCounter.fetch_add(1, std::memory_order_release);
        notifyCounter.notify_one();
    }


    inline void Print(std::string_view text)
    {
        PushAndNotify({text, MessageType::Print});
    }

    template <typename... Args>
    inline void Print(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        PushAndNotify(Message{
            fmt::format(fmtStr, std::forward<Args>(args)...),
            MessageType::Print
        });
    }

    inline void Log(std::string_view text)
    {
        PushAndNotify(Message{std::move(text), MessageType::Log});
    }

    inline void LogAndPrint(std::string_view text)
    {
        PushAndNotify(Message{text, MessageType::LogAndPrint});
    }

    inline void StyledPrint(std::string_view text, fmt::text_style primaryStyle, bool line_break = true)
    {
        PushAndNotify(Message{
            text,
            MessageType::StyledPrint,
            primaryStyle,
            std::nullopt,
            line_break
        });
    }

    template <typename... Args>
    inline void StyledPrint(bool line_break, fmt::text_style style, fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        PushAndNotify(Message{
            fmt::format(fmtStr, std::forward<Args>(args)...),
            MessageType::StyledPrint,
            style,
            std::nullopt,
            line_break,
            true
        });
    }

    inline void StyledPrint(std::string_view text, fmt::text_style primaryStyle, fmt::text_style secondaryStyle, bool line_break = true)
    {
        PushAndNotify(Message{
            text,
            MessageType::StyledPrint,
            primaryStyle,
            std::optional<fmt::text_style>(secondaryStyle),
            line_break,
            true
        });
    }

    template <typename... Args>
    inline void StyledPrint(bool line_break, fmt::text_style primaryStyle, fmt::text_style secondaryStyle, fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        PushAndNotify(Message{
            fmt::format(fmtStr, std::forward<Args>(args)...),
            MessageType::StyledPrint,
            primaryStyle,
            secondaryStyle,
            line_break,
            true
        });
    }

    inline void LogAndStyledPrint(std::string_view text, fmt::text_style primaryStyle, bool line_break = true)
    {
        PushAndNotify(Message{
            text,
            MessageType::LogAndStyledPrint,
            primaryStyle,
            std::nullopt,
            line_break,
            true
        });
    }

    inline void DebugPrint(std::string_view text)
    {
        StyledPrint(text, fmt::fg(fmt::color::cyan) | fmt::emphasis::bold);
    }

    inline void DebugLog(std::string_view text)
    {
        PushAndNotify(Message{
            text,
            MessageType::DebugLog,
            false
        });
    }

    inline void DebugLogAndPrint(std::string_view text)
    {
        LogAndStyledPrint(text, fmt::fg(fmt::color::cyan) | fmt::emphasis::bold);
    }

    // These are the intended "Debugger" methods to be used in the codebase.
    template <typename... Args>
    inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        std::string text = fmt::format(fmtStr, std::forward<Args>(args)...);
        if (print_level != PriorityType::None and print_level >= PriorityType::Info)
        {
            LogAndStyledPrint(text, fmt::fg(fmt::color::cyan) | fmt::emphasis::bold);
        }
        else
        {
            Log(text);
        }
    }

    template <typename... Args>
    inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        std::string text = fmt::format(fmtStr, std::forward<Args>(args)...);
        if (print_level != PriorityType::None and print_level >= PriorityType::Warning)
        {
            LogAndStyledPrint(text, fmt::fg(fmt::color::orange) | fmt::emphasis::bold);
        }
        else
        {
            Log(text);
        }
    }

    template <typename... Args>
    inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        std::string text = fmt::format(fmtStr, std::forward<Args>(args)...);
        if (print_level != PriorityType::None and print_level >= PriorityType::Error)
        {
            LogAndStyledPrint(text, fmt::fg(fmt::color::red) | fmt::emphasis::bold);
        }
        else
        {
            Log(text);
        }
    }
}
