#pragma once

#include <atomic>
#include <array>
#include <thread>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <utility>
#include <print>
#include <source_location>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <optional>
#include <toml++/toml.h>

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

    enum class LogLevel : uint8_t
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    // Combat is a reserved placeholder: the registry slot exists so the future
    // combat metrics system can wire it up without renumbering other channels,
    // but it has no sub-namespace yet and stays disabled by default.
    enum class ChannelType : uint8_t
    {
        System,
        Network,
        Database,
        Combat,
        Script,
        Map,
        Player,
        Admin,
        Security,
        COUNT
    };

    inline constexpr size_t CHANNEL_COUNT = static_cast<size_t>(ChannelType::COUNT);

    struct ChannelMeta
    {
        ChannelType type;
        std::string_view key;   // lowercase identifier used for the per-channel log directory and config table
        std::string_view label; // padded uppercase label used in structured log lines
    };

    inline constexpr std::array<ChannelMeta, CHANNEL_COUNT> CHANNEL_TABLE
    {{
        { ChannelType::System,   "system",   "SYSTEM " },
        { ChannelType::Network,  "network",  "NETWORK" },
        { ChannelType::Database, "database", "DATABASE" },
        { ChannelType::Combat,   "combat",   "COMBAT " },
        { ChannelType::Script,   "script",   "SCRIPT " },
        { ChannelType::Map,      "map",      "MAP    " },
        { ChannelType::Player,   "player",   "PLAYER " },
        { ChannelType::Admin,    "admin",    "ADMIN  " },
        { ChannelType::Security, "security", "SECURITY" },
    }};

    struct Message
    {
        using Style = fmt::text_style;

        std::string text;
        std::optional<Style> secondary_style{};
        Style primary_style{};
        MessageType msg_type{MessageType::Print};
        ChannelType channel{ChannelType::System};
        LogLevel level{LogLevel::Info};

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

    struct FloodGuard
    {
        std::string lastMessage;
        uint32_t repeatCount { 0 };
        uint32_t suppressedCount { 0 };
        std::chrono::steady_clock::time_point windowStart { std::chrono::steady_clock::now() };
        uint32_t maxPerWindow { 100 };
    };

    struct LogChannel
    {
        std::string name;
        std::string dir;
        std::ofstream file;
        LogLevel minLevel { LogLevel::Info };
        LogLevel printLevel { LogLevel::Warning };
        bool enabled { true };
        FloodGuard flood;
        std::atomic<uint64_t> messagesWritten { 0 };
        std::atomic<uint64_t> messagesDropped { 0 };

        LogChannel() = default;

        // Channels live permanently in a fixed-size array for the program's lifetime;
        // they are never copied or moved, so those operations are simply not provided.
        LogChannel(const LogChannel&) = delete;
        LogChannel& operator=(const LogChannel&) = delete;
        LogChannel(LogChannel&&) = delete;
        LogChannel& operator=(LogChannel&&) = delete;

        void Open(const std::string& logRoot, const std::string& dateSuffix)
        {
            if (not enabled)
                return;

            std::filesystem::create_directories(logRoot + "/" + dir);
            file.open(logRoot + "/" + dir + "/" + dir + "_" + dateSuffix + ".log", std::ios::out | std::ios::app);

            if (file.is_open())
                file << "==== Session started " << dateSuffix << " ====" << '\n';
        }

        void Close()
        {
            if (file.is_open())
            {
                file << "==== Session ended ====" << '\n';
                file.close();
            }
        }

        void Cleanup(const std::string& logRoot, uint32_t retentionDays) const
        {
            namespace fs = std::filesystem;

            std::error_code ec;
            fs::path channelDir = fs::path(logRoot) / dir;
            if (not fs::exists(channelDir, ec))
                return;

            auto cutoff = fs::file_time_type::clock::now() - std::chrono::hours(24LL * retentionDays);

            for (const auto& entry : fs::directory_iterator(channelDir, ec))
            {
                if (not entry.is_regular_file())
                    continue;

                if (entry.last_write_time() < cutoff)
                    fs::remove(entry, ec);
            }
        }

        void Rotate(const std::string& logRoot, const std::string& dateSuffix, uint32_t retentionDays)
        {
            Close();
            Open(logRoot, dateSuffix);
            Cleanup(logRoot, retentionDays);
        }
    };

    inline constexpr size_t BUFFER_SIZE = 4096;
    inline constexpr size_t BUFFER_MASK = BUFFER_SIZE - 1;

    inline std::jthread worker;
    inline std::atomic<bool> running = false;
    inline std::atomic<size_t> notifyCounter{0};

    inline std::array<LogChannel, CHANNEL_COUNT> channels;

    inline std::string log_dir = "data/logs";
    inline std::string timestamp_format = "%Y-%m-%d %H:%M:%S";
    inline uint32_t retention_days = 7;
    inline std::chrono::milliseconds dedup_window { 1000 };
    inline std::string current_date;

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

    // ------------------------------------------------------------------------
    // Formatting helpers
    // ------------------------------------------------------------------------

    inline std::string_view LevelLabel(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Trace:   return "TRACE";
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO ";
            case LogLevel::Warning: return "WARN ";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
        }
        return "?????";
    }

    inline fmt::text_style LevelColor(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Trace:   return fmt::fg(fmt::color::gray);
            case LogLevel::Debug:   return fmt::fg(fmt::color::cyan) | fmt::emphasis::bold;
            case LogLevel::Info:    return fmt::fg(fmt::color::cyan) | fmt::emphasis::bold;
            case LogLevel::Warning: return fmt::fg(fmt::color::orange) | fmt::emphasis::bold;
            case LogLevel::Error:   return fmt::fg(fmt::color::red) | fmt::emphasis::bold;
            case LogLevel::Fatal:   return fmt::fg(fmt::color::red) | fmt::emphasis::bold;
        }
        return fmt::text_style{};
    }

    inline std::string FormatLine(std::string_view channelLabel, LogLevel level, std::string_view text)
    {
        auto timeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        try
        {
            std::string pattern = "[{:" + timestamp_format + "}][{}][{}] {}";
            return fmt::format(fmt::runtime(pattern), *std::localtime(&timeT), channelLabel, LevelLabel(level), text);
        }
        catch (const fmt::format_error&)
        {
            return fmt::format("[{:%Y-%m-%d %H:%M:%S}][{}][{}] {}", *std::localtime(&timeT), channelLabel, LevelLabel(level), text);
        }
    }

    inline std::string CurrentDateString()
    {
        auto timeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return fmt::format("{:%Y-%m-%d}", *std::localtime(&timeT));
    }

    inline LogLevel ParseLevel(std::string_view text)
    {
        if (text == "trace")   return LogLevel::Trace;
        if (text == "debug")   return LogLevel::Debug;
        if (text == "warning") return LogLevel::Warning;
        if (text == "error")   return LogLevel::Error;
        if (text == "fatal")   return LogLevel::Fatal;
        return LogLevel::Info;
    }

    // ------------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------------

    struct ChannelDefaults
    {
        bool enabled;
        LogLevel minLevel;
        LogLevel printLevel;
    };

    inline constexpr std::array<ChannelDefaults, CHANNEL_COUNT> CHANNEL_DEFAULTS
    {{
        { true,  LogLevel::Info,    LogLevel::Info },    // System
        { true,  LogLevel::Warning, LogLevel::Error },   // Network
        { true,  LogLevel::Warning, LogLevel::Error },   // Database
        { false, LogLevel::Info,    LogLevel::Warning }, // Combat — placeholder, disabled until the metrics system wires it up
        { true,  LogLevel::Debug,   LogLevel::Error },   // Script
        { true,  LogLevel::Info,    LogLevel::Warning }, // Map
        { true,  LogLevel::Info,    LogLevel::Warning }, // Player
        { true,  LogLevel::Info,    LogLevel::Info },    // Admin
        { true,  LogLevel::Warning, LogLevel::Warning }, // Security
    }};

    inline void LoadLoggingConfig()
    {
        toml::table tbl;

        try
        {
            tbl = toml::parse_file("config/logging.toml");
        }
        catch (const toml::parse_error& err)
        {
            std::cerr << "[Warning - Console] Failed to parse config/logging.toml: " << err.description() << "\n";
        }

        log_dir          = tbl["logging"]["log_dir"].value_or<std::string>("data/logs");
        timestamp_format = tbl["logging"]["timestamp_format"].value_or<std::string>("%Y-%m-%d %H:%M:%S");
        retention_days   = static_cast<uint32_t>(tbl["logging"]["retention_days"].value_or(int64_t{7}));
        dedup_window     = std::chrono::milliseconds(tbl["logging"]["dedup_window_ms"].value_or(int64_t{1000}));

        for (size_t i = 0; i < CHANNEL_COUNT; ++i)
        {
            const auto& fallback = CHANNEL_DEFAULTS[i];
            auto channelTbl = tbl["logging"]["channels"][std::string(CHANNEL_TABLE[i].key)];

            channels[i].enabled = channelTbl["enabled"].value_or(fallback.enabled);

            if (auto val = channelTbl["min_level"].value<std::string>())
                channels[i].minLevel = ParseLevel(*val);
            else
                channels[i].minLevel = fallback.minLevel;

            if (auto val = channelTbl["print_level"].value<std::string>())
                channels[i].printLevel = ParseLevel(*val);
            else
                channels[i].printLevel = fallback.printLevel;
        }
    }

    // ------------------------------------------------------------------------
    // Dispatch — shared by the root API and every channel sub-namespace
    // ------------------------------------------------------------------------

    inline void PushAndNotify(Message&& msg)
    {
        while (not queue.Push(std::move(msg)))
            std::this_thread::yield();

        notifyCounter.fetch_add(1, std::memory_order_release);
        notifyCounter.notify_one();
    }

    inline void PushFormatted(ChannelType channel, LogLevel level, std::string text)
    {
        auto& ch = channels[static_cast<size_t>(channel)];
        bool shouldPrint = level >= ch.printLevel;

        Message msg{
            std::move(text),
            shouldPrint ? MessageType::LogAndStyledPrint : MessageType::Log,
            LevelColor(level),
            std::nullopt,
            true,
            true
        };
        msg.channel = channel;
        msg.level = level;

        PushAndNotify(std::move(msg));
    }

    template <typename... Args>
    inline void Dispatch(ChannelType channel, LogLevel level, fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        auto& ch = channels[static_cast<size_t>(channel)];
        if (not ch.enabled or level < ch.minLevel)
            return;

        PushFormatted(channel, level, fmt::format(fmtStr, std::forward<Args>(args)...));
    }

    // Bundles a compile-time-checked format string with the call site's source_location.
    // Args is deduced from the trailing args... pack at the call site; the literal then
    // converts into this wrapper, which is simpler and more portable than defaulting a
    // std::source_location parameter after a function parameter pack.
    template <typename... Args>
    struct SourceFormat
    {
        fmt::format_string<Args...> text;
        std::source_location loc;

        template <typename T>
        consteval SourceFormat(const T& fmtStr, std::source_location location = std::source_location::current())
            : text(fmtStr), loc(location)
        {
        }
    };

    template <typename... Args>
    inline void DebugDispatch(ChannelType channel, SourceFormat<Args...> fmtAndLoc, Args&&... args)
    {
        auto& ch = channels[static_cast<size_t>(channel)];
        if (not ch.enabled or LogLevel::Debug < ch.minLevel)
            return;

        PushFormatted(channel, LogLevel::Debug, fmt::format("[{}:{}] {}", fmtAndLoc.loc.file_name(), fmtAndLoc.loc.line(), fmt::format(fmtAndLoc.text, std::forward<Args>(args)...)));
    }

    template <typename... Args>
    inline void TraceDispatch(ChannelType channel, SourceFormat<Args...> fmtAndLoc, Args&&... args)
    {
        auto& ch = channels[static_cast<size_t>(channel)];
        if (not ch.enabled or LogLevel::Trace < ch.minLevel)
            return;

        PushFormatted(channel, LogLevel::Trace, fmt::format("[{}:{}] {}", fmtAndLoc.loc.file_name(), fmtAndLoc.loc.line(), fmt::format(fmtAndLoc.text, std::forward<Args>(args)...)));
    }

    inline void FatalWrite(ChannelType channel, const std::string& text)
    {
        auto& ch = channels[static_cast<size_t>(channel)];
        if (ch.file.is_open())
        {
            ch.file << FormatLine(ch.name, LogLevel::Fatal, text) << '\n';
            ch.file.flush();
        }
        fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "[FATAL] {}\n", text);
    }

    template <typename... Args>
    inline void FatalDispatch(ChannelType channel, fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        FatalWrite(channel, fmt::format(fmtStr, std::forward<Args>(args)...));
    }

    // ------------------------------------------------------------------------
    // Worker thread
    // ------------------------------------------------------------------------

    inline void WriteToChannel(LogChannel& channel, const Message& msg)
    {
        if (not channel.enabled or not channel.file.is_open())
        {
            channel.messagesDropped.fetch_add(1, std::memory_order_relaxed);
            return;
        }

        auto& flood = channel.flood;
        auto now = std::chrono::steady_clock::now();

        if (msg.text != flood.lastMessage or now - flood.windowStart > dedup_window)
        {
            if (flood.suppressedCount > 0)
            {
                channel.file << FormatLine(channel.name, LogLevel::Warning,
                    fmt::format("{} identical messages suppressed", flood.suppressedCount)) << '\n';
                flood.suppressedCount = 0;
            }

            flood.lastMessage = msg.text;
            flood.windowStart = now;
            flood.repeatCount = 1;
        }
        else
        {
            ++flood.repeatCount;
            if (flood.repeatCount > flood.maxPerWindow)
            {
                ++flood.suppressedCount;
                channel.messagesDropped.fetch_add(1, std::memory_order_relaxed);
                return;
            }
        }

        channel.file << FormatLine(channel.name, msg.level, msg.text) << '\n';
        channel.messagesWritten.fetch_add(1, std::memory_order_relaxed);
    }

    inline void CheckRotation()
    {
        std::string today = CurrentDateString();
        if (today == current_date)
            return;

        current_date = today;
        for (auto& channel : channels)
            channel.Rotate(log_dir, current_date, retention_days);
    }

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
            WriteToChannel(channels[static_cast<size_t>(msg.channel)], msg);
        };

        const auto processMessage = [&]()
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
        };

        while (not stop.stop_requested())
        {
            CheckRotation();

            while (queue.Pop(msg))
                processMessage();

            notifyCounter.wait(observed, std::memory_order_relaxed);
            observed = notifyCounter.load(std::memory_order_acquire);
        }

        while (queue.Pop(msg))
            processMessage();
    }

    // ------------------------------------------------------------------------
    // Lifecycle
    // ------------------------------------------------------------------------

    inline void Initialize()
    {
        if (running.exchange(true))
            return;

        for (size_t i = 0; i < CHANNEL_COUNT; ++i)
        {
            channels[i].dir  = std::string(CHANNEL_TABLE[i].key);
            channels[i].name = std::string(CHANNEL_TABLE[i].label);
        }

        LoadLoggingConfig();

        current_date = CurrentDateString();
        for (auto& channel : channels)
            channel.Open(log_dir, current_date);

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

        for (auto& channel : channels)
            channel.Close();
    }

    // ------------------------------------------------------------------------
    // Runtime introspection (used by the Lua `log` table)
    // ------------------------------------------------------------------------

    struct ChannelStats
    {
        uint64_t written;
        uint64_t dropped;
    };

    inline ChannelStats GetChannelStats(ChannelType channel)
    {
        const auto& ch = channels[static_cast<size_t>(channel)];
        return { ch.messagesWritten.load(std::memory_order_relaxed), ch.messagesDropped.load(std::memory_order_relaxed) };
    }

    inline std::optional<std::string> GetChannelLogPath(ChannelType channel)
    {
        const auto& ch = channels[static_cast<size_t>(channel)];
        if (not ch.enabled)
            return std::nullopt;

        return log_dir + "/" + ch.dir + "/" + ch.dir + "_" + current_date + ".log";
    }

    inline void LuaLog(ChannelType channel, LogLevel level, std::string_view text)
    {
        auto& ch = channels[static_cast<size_t>(channel)];
        if (not ch.enabled or level < ch.minLevel)
            return;

        PushFormatted(channel, level, std::string(text));
    }

    // ------------------------------------------------------------------------
    // Public API — unchanged signatures, all routed to the System channel
    // ------------------------------------------------------------------------

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
        Message msg{
            text,
            MessageType::DebugLog,
            false
        };
        msg.level = LogLevel::Debug;
        PushAndNotify(std::move(msg));
    }

    inline void DebugLogAndPrint(std::string_view text)
    {
        Message msg{
            text,
            MessageType::LogAndStyledPrint,
            fmt::fg(fmt::color::cyan) | fmt::emphasis::bold,
            std::nullopt,
            true,
            true
        };
        msg.level = LogLevel::Debug;
        PushAndNotify(std::move(msg));
    }

    // These are the intended "Debugger" methods to be used in the codebase.
    template <typename... Args>
    inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        Dispatch(ChannelType::System, LogLevel::Info, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        Dispatch(ChannelType::System, LogLevel::Warning, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        Dispatch(ChannelType::System, LogLevel::Error, fmtStr, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
    {
        FatalDispatch(ChannelType::System, fmtStr, std::forward<Args>(args)...);
    }

    // ------------------------------------------------------------------------
    // Channel sub-namespaces
    // ------------------------------------------------------------------------

    namespace Net
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Network, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Network, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Network, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Network, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Network, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Network, fmtStr, std::forward<Args>(args)...); }
    }

    namespace Database
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Database, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Database, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Database, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Database, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Database, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Database, fmtStr, std::forward<Args>(args)...); }
    }

    namespace Script
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Script, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Script, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Script, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Script, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Script, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Script, fmtStr, std::forward<Args>(args)...); }
    }

    namespace Map
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Map, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Map, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Map, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Map, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Map, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Map, fmtStr, std::forward<Args>(args)...); }
    }

    namespace Player
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Player, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Player, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Player, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Player, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Player, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Player, fmtStr, std::forward<Args>(args)...); }
    }

    namespace Admin
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Admin, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Admin, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Admin, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Admin, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Admin, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Admin, fmtStr, std::forward<Args>(args)...); }
    }

    namespace Security
    {
        template <typename... Args>
        inline void Info(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Security, LogLevel::Info, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Warn(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Security, LogLevel::Warning, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Error(fmt::format_string<Args...> fmtStr, Args&&... args)
        { Dispatch(ChannelType::Security, LogLevel::Error, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Debug(SourceFormat<Args...> fmtStr, Args&&... args)
        { DebugDispatch<Args...>(ChannelType::Security, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Trace(SourceFormat<Args...> fmtStr, Args&&... args)
        { TraceDispatch<Args...>(ChannelType::Security, fmtStr, std::forward<Args>(args)...); }

        template <typename... Args>
        inline void Fatal(fmt::format_string<Args...> fmtStr, Args&&... args)
        { FatalDispatch(ChannelType::Security, fmtStr, std::forward<Args>(args)...); }
    }
}
