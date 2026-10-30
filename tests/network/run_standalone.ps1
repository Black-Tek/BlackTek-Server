param(
    [Parameter(Mandatory = $true)][string]$Compiler,
    [string]$OutputDirectory = (Join-Path $env:TEMP 'blacktek-network-tests'),
    [string]$BaselineRef = ''
)

# Runs the checked-in tests against production code without the server's third-party
# dependencies. Unrelated types and Google Test assertions are stubbed. Protocol
# framing uses a no-op XTEA boundary to inspect the exact decrypted span; it does
# not test the encryption algorithm.
$ErrorActionPreference = 'Stop'
$repository = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null

function Read-Source([string]$relative)
{
    if ($BaselineRef)
    {
        $result = & git -C $repository show "${BaselineRef}:$relative"
        if ($LASTEXITCODE -ne 0) { throw "Cannot read $relative at $BaselineRef" }
        return $result -join "`n"
    }
    return [IO.File]::ReadAllText((Join-Path $repository $relative))
}

function Get-Function([string]$source, [string]$signature)
{
    $start = $source.IndexOf($signature)
    if ($start -lt 0) { throw "Missing function $signature" }
    $opening = $source.IndexOf('{', $start)
    $depth = 1
    $end = $opening + 1
    while ($depth -gt 0 -and $end -lt $source.Length)
    {
        if ($source[$end] -eq '{') { $depth++ }
        if ($source[$end] -eq '}') { $depth-- }
        $end++
    }
    return $source.Substring($start, $end - $start)
}

$preamble = @'
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
static int Failures = 0;
static std::vector<void(*)()> Tests;
#define TEST(suite, name) void suite##_##name(); static bool suite##_##name##_registered = (Tests.push_back(suite##_##name), true); void suite##_##name()
#define EXPECT_EQ(a, b) do { if ((a) != (b)) ++Failures; } while (false)
#define EXPECT_TRUE(a) do { if (not (a)) ++Failures; } while (false)
#define EXPECT_FALSE(a) do { if (a) ++Failures; } while (false)
#define ASSERT_TRUE(a) do { if (not (a)) { ++Failures; return; } } while (false)
struct Position;
class Item;
using ItemConstPtr = std::shared_ptr<const Item>;
'@
$constant = [regex]::Match((Read-Source 'src/const.h'), 'static constexpr int32_t NETWORKMESSAGE_MAXSIZE = \d+;').Value
$networkHeader = [regex]::Replace((Read-Source 'src/networkmessage.h'), '(?m)^#include.*$', '')
$getString = Get-Function (Read-Source 'src/networkmessage.cpp') 'std::string_view NetworkMessage::getString('
$networkTests = [regex]::Replace([IO.File]::ReadAllText((Join-Path $PSScriptRoot 'networkmessage_test.cpp')), '(?m)^#include.*$', '')
$source = $preamble + "`n" + $constant + "`n" + $networkHeader + "`n" + $getString + "`n" + $networkTests
$source += @'

namespace xtea
{
    using round_keys = std::array<uint32_t, 64>;
    static size_t DecryptedBytes = 0;
    void decrypt(uint8_t*, size_t length, const round_keys&) { DecryptedBytes = length; }
}
'@
$source += "`n" + (Get-Function (Read-Source 'src/protocol.cpp') 'bool XTEA_decrypt(')
$source += @'

TEST(ProtocolFraming, ChecksumlessEncryptedPayloadUsesCurrentOffset)
{
    NetworkMessage message;
    auto body = message.getBodyBuffer();
    body[0] = 1; body[1] = 0; body[2] = 0x42;
    message.setLength(10);
    ASSERT_TRUE(XTEA_decrypt(message, {}));
    EXPECT_EQ(xtea::DecryptedBytes, 8U);
    EXPECT_EQ(message.getByte(), 0x42);
    EXPECT_EQ(message.getByte(), 0);
    EXPECT_TRUE(message.isOverrun());
}
TEST(ProtocolFraming, ChecksummedEncryptedPayloadUsesExactInnerLength)
{
    NetworkMessage message;
    message.getBodyBuffer(); message.skipBytes(4);
    auto body = message.getBuffer() + message.getBufferPosition();
    body[0] = 1; body[1] = 0; body[2] = 0x42;
    message.setLength(14);
    ASSERT_TRUE(XTEA_decrypt(message, {}));
    EXPECT_EQ(xtea::DecryptedBytes, 8U);
    EXPECT_EQ(message.getByte(), 0x42);
    EXPECT_EQ(message.getByte(), 0);
    EXPECT_TRUE(message.isOverrun());
}
TEST(ProtocolFraming, EmptyEncryptedBodyIsRejectedBeforeDecrypt)
{
    NetworkMessage message;
    message.getBodyBuffer(); message.skipBytes(4);
    message.setLength(6); xtea::DecryptedBytes = 99;
    EXPECT_FALSE(XTEA_decrypt(message, {}));
    EXPECT_EQ(xtea::DecryptedBytes, 99U);
}
TEST(ProtocolFraming, OversizedInnerLengthIsRejected)
{
    NetworkMessage message;
    auto body = message.getBodyBuffer();
    body[0] = 7; body[1] = 0;
    message.setLength(10);
    EXPECT_FALSE(XTEA_decrypt(message, {}));
}
'@

if (-not $BaselineRef)
{
    $source += "`n" + (Read-Source 'src/loginqueue.h')
}
else
{
    # Adapt the original production queue to the same tests. This preserves its
    # actual container, iterator, expiry and insertion implementation.
    $source += @'

static int64_t CurrentTime;
static int64_t OTSYS_TIME() { return CurrentTime; }
static constexpr int PlayerFlag_CanAlwaysLogin = 1;
static constexpr int ACCOUNT_TYPE_GAMEMASTER = 4;
struct Player
{
    uint32_t id; bool premium;
    bool hasFlag(int) const { return false; }
    int getAccountType() const { return 0; }
    uint32_t getGUID() const { return id; }
    bool isPremium() const { return premium; }
};
using PlayerConstPtr = std::shared_ptr<const Player>;
struct ConfigManager { enum { MAX_PLAYERS }; uint32_t maximum; uint32_t GetNumber(int) const { return maximum; } };
static ConfigManager g_config;
struct Game { uint32_t online; uint32_t getPlayersOnline() const { return online; } };
static Game g_game;
'@
    $game = Read-Source 'src/protocolgame.cpp'
    $start = [regex]::Match($game, '(?m)^namespace\s*\{').Index
    $end = $game.IndexOf('void ProtocolGame::release()', $start)
    $source += "`n" + $game.Substring($start, $end - $start)
    $source += @'

namespace BlackTek::Net
{
    class LoginQueue
    {
    public:
        LoginQueue() { waitList.clear(); priorityEnd = waitList.end(); }
        std::size_t Login(uint32_t guid, bool premium, uint32_t online, uint32_t maximum, int64_t now)
        {
            CurrentTime = now; g_game.online = online; g_config.maximum = maximum;
            return clientLogin(std::make_shared<Player>(Player{guid, premium}));
        }
    };
}
'@
}
$source += "`n" + [regex]::Replace([IO.File]::ReadAllText((Join-Path $PSScriptRoot 'loginqueue_test.cpp')), '(?m)^#include.*$', '')
$source += "`nint main(int argc, char** argv) { if (argc != 2) return 255; Tests.at(std::stoul(argv[1]))(); return Failures; }`n"
$testNames = [regex]::Matches($source, 'TEST\((\w+),\s*(\w+)\)') | ForEach-Object { $_.Groups[1].Value + '.' + $_.Groups[2].Value }
$testNames = @($testNames | Where-Object { $_ -ne 'suite.name' })
$fixture = Join-Path $OutputDirectory 'network_fixture.cpp'
$binary = Join-Path $OutputDirectory 'network_tests.exe'
[IO.File]::WriteAllText($fixture, $source)
& $Compiler c++ -std=c++20 -O0 -g $fixture -o $binary
if ($LASTEXITCODE -ne 0) { throw "Network fixture compilation failed: $LASTEXITCODE" }
$failures = 0
for ($index = 0; $index -lt $testNames.Count; ++$index)
{
    & $binary $index
    $result = $LASTEXITCODE
    if ($result -ne 0) { $failures++ }
    Write-Output "$($testNames[$index]): exit $result"
}
Write-Output "Network production-source tests: $($testNames.Count), failures: $failures (baseline=$BaselineRef)"
exit $failures
