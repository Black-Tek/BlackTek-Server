param(
    [Parameter(Mandatory = $true)][string]$Compiler,
    [string]$OutputDirectory = (Join-Path $env:TEMP 'blacktek-persistence-tests'),
    [string]$BaselineRef = ''
)

# The production DBTransaction class, result accessors, and the custom-data save
# tail are compiled verbatim. A deterministic database boundary simulates query
# failures and primary-key uniqueness; this is not a live MySQL integration test.
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
$header = Read-Source 'src/database.h'
$transactionStart = $header.IndexOf('class DBTransaction' + "`n")
if ($transactionStart -lt 0) { $transactionStart = $header.IndexOf("class DBTransaction`r`n") }
$transaction = $header.Substring($transactionStart)
$transaction = $transaction.Substring(0, $transaction.IndexOf('#endif'))
$number = Get-Function $header 'template<typename T>'
$string = Get-Function (Read-Source 'src/database.cpp') 'std::string_view DBResult::getString('
$login = Read-Source 'src/iologindata.cpp'
$save = Get-Function $login 'bool IOLoginData::savePlayer('
$tailStart = $save.IndexOf('if (!db.executeQuery(fmt::format("DELETE FROM `player_custom_skills`')
if ($tailStart -lt 0) { throw 'Custom save tail was not found' }
$tail = $save.Substring($tailStart)
$conditionLoad = Get-Function $login 'for (auto condition = Condition::createCondition(propStream);'
$preamble = @'
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
namespace BlackTek::Console::Database { template<class... T> void Error(T&&...) {} }
namespace pugi { template<class T> T cast(const char* value) { return static_cast<T>(std::stoll(value)); } }
namespace fmt { template<class... T> std::string format(const char* text, T&&...) { return text; } }
class Database
{
public:
    static Database& getInstance() { static Database instance; return instance; }
    bool begin_success = true;
    bool fail_skills = false;
    bool fail_stats = false;
    bool fail_delete = false;
    int begins = 0, rollbacks = 0, commits = 0;
    std::optional<int> skills, stats, saved_skills, saved_stats;
    bool beginTransaction() { ++begins; saved_skills = skills; saved_stats = stats; return begin_success; }
    bool rollback() { ++rollbacks; skills = saved_skills; stats = saved_stats; return true; }
    bool commit() { ++commits; return true; }
    bool executeQuery(const std::string& query)
    {
        if (query.find("player_custom_stats") != std::string::npos)
        {
            if (fail_delete) return false;
            stats.reset();
        }
        if (query.find("player_custom_skills") != std::string::npos) skills.reset();
        return true;
    }
};
class DBResult
{
public:
    std::map<std::string_view, std::size_t> listNames;
    char** row = nullptr;
    void* handle = nullptr;
    std::string_view getString(std::string_view column) const;
'@
$source = $preamble + "`n" + $number + "`n};`n"
$source += "unsigned long* mysql_fetch_lengths(void*) { static unsigned long size = 0; return &size; }`n" + $string + "`n" + $transaction
$source += @'

struct Player { int skills = 0, stats = 0; uint32_t getGUID() const { return 1; } };
struct PropWriteStream {};
struct DBInsert { explicit DBInsert(std::string) {} };
bool savePlayerCustomSkills(const std::shared_ptr<Player>& player, DBInsert&, PropWriteStream&)
{
    auto& db = Database::getInstance();
    if (db.fail_skills or db.skills.has_value()) return false;
    db.skills = player->skills;
    return true;
}
bool savePlayerCustomStats(const std::shared_ptr<Player>& player, DBInsert&, PropWriteStream&)
{
    auto& db = Database::getInstance();
    if (db.fail_stats or db.stats.has_value()) return false;
    db.stats = player->stats;
    return true;
}
bool SaveCustomData(const std::shared_ptr<Player>& player)
{
    auto& db = Database::getInstance();
    DBTransaction transaction;
    if (not transaction.begin()) return false;
'@
$source += "`n" + $tail
$source += @'

struct Condition
{
    static inline int remaining = 0;
    static std::shared_ptr<Condition> createCondition(PropWriteStream&)
    {
        return remaining-- > 0 ? std::make_shared<Condition>() : nullptr;
    }
    bool unserialize(PropWriteStream&) const { return true; }
};
using ConditionHandle = std::shared_ptr<Condition>;
struct OfflinePlayer { std::vector<ConditionHandle> conditions; };
void LoadConditions(const std::shared_ptr<OfflinePlayer>& player, std::vector<ConditionHandle>* outConditions)
{
    PropWriteStream propStream;
    Condition::remaining = 1;
'@
$source += "`n" + $conditionLoad + "`n}`n"
$source += @'

int main(int argc, char** argv)
{
    if (argc != 2) return 255;
    const int test = std::stoi(argv[1]);
    auto& db = Database::getInstance();
    int failures = 0;
    auto check = [&](bool value) { if (not value) ++failures; };
    if (test == 0)
    {
        db.begin_success = false;
        { DBTransaction transaction; check(not transaction.begin()); }
        check(db.rollbacks == 0);
    }
    else if (test == 1)
    {
        { DBTransaction transaction; check(transaction.begin()); check(not transaction.begin()); }
        check(db.begins == 1); check(db.rollbacks == 1);
    }
    else if (test == 2)
    {
        { DBTransaction transaction; check(transaction.begin()); }
        check(db.rollbacks == 1);
    }
    else if (test == 3)
    {
        { DBTransaction transaction; check(transaction.begin()); check(transaction.commit()); }
        check(db.rollbacks == 0); check(db.commits == 1);
    }
    else if (test == 4)
    {
        DBResult result;
        check(result.getNumber<int>("missing") == 0);
    }
    else if (test == 5)
    {
        DBResult result; result.listNames["value"] = 0;
        check(result.getNumber<int>("value") == 0);
    }
    else if (test == 6)
    {
        DBResult result; result.listNames["value"] = 0;
        check(result.getString("value").empty());
    }
    else if (test >= 11)
    {
        auto player = std::make_shared<OfflinePlayer>();
        std::vector<ConditionHandle> onlineConditions;
        LoadConditions(player, test == 12 ? &onlineConditions : nullptr);
        check(test == 12 ? player->conditions.empty() and onlineConditions.size() == 1 : player->conditions.size() == 1);
    }
    else
    {
        auto player = std::make_shared<Player>(); player->skills = 10; player->stats = 20;
        check(SaveCustomData(player));
        player->skills = 30; player->stats = 40;
        if (test == 7)
        {
            check(SaveCustomData(player)); check(db.skills == 30); check(db.stats == 40);
        }
        else
        {
            db.fail_skills = test == 8; db.fail_stats = test == 9; db.fail_delete = test == 10;
            check(not SaveCustomData(player)); check(db.skills == 10); check(db.stats == 20);
            check(db.rollbacks == 1);
        }
    }
    return failures;
}
'@
$fixture = Join-Path $OutputDirectory 'persistence_fixture.cpp'
$binary = Join-Path $OutputDirectory 'persistence_tests.exe'
[IO.File]::WriteAllText($fixture, $source)
& $Compiler c++ -std=c++20 -O0 -g $fixture -o $binary
if ($LASTEXITCODE -ne 0) { throw "Persistence fixture compilation failed: $LASTEXITCODE" }
$names = @('FailedBeginDoesNotRollback', 'RepeatedBeginIsRejected', 'AbandonedTransactionRollsBack', 'CommittedTransactionDoesNotRollback', 'MissingColumnIsSafe', 'ExhaustedNumericResultIsSafe', 'ExhaustedStringResultIsSafe', 'RepeatedSaveUpdatesStats', 'SkillFailureRollsBack', 'StatFailureRollsBack', 'StatDeleteFailureRollsBack', 'OfflineConditionsAreRetained', 'OnlineConditionsAreDeferred')
$failures = 0
for ($index = 0; $index -lt $names.Count; ++$index)
{
    & $binary $index
    $result = $LASTEXITCODE
    if ($result -ne 0) { $failures++ }
    Write-Output "$($names[$index]): exit $result"
}
Write-Output "Persistence production-source tests: $($names.Count), failures: $failures (baseline=$BaselineRef)"
exit $failures
