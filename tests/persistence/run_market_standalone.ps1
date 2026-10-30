param(
    [Parameter(Mandatory = $true)][string]$Compiler,
    [string]$OutputDirectory = (Join-Path $env:TEMP 'blacktek-market-tests'),
    [string]$BaselineRef = ''
)

# Compiles actual Game market operations and IOMarket settlement functions.
# Inventory, SQL and scheduler boundaries are deterministic doubles. These tests
# cover ordering, recipient selection and database failure rollback, not crash
# recovery or transactional delivery to persistent offline inventories.
$ErrorActionPreference = 'Stop'
$repository = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
function Read-Source([string]$relative)
{
    if ($BaselineRef)
    {
        $result = & git -C $repository show "${BaselineRef}:$relative"
        if ($LASTEXITCODE -ne 0) { throw "Cannot read $relative" }
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
$market = Read-Source 'src/iomarket.cpp'
$game = Read-Source 'src/game.cpp'
$header = Read-Source 'src/database.h'
$transactionStart = $header.IndexOf('class DBTransaction' + "`n")
if ($transactionStart -lt 0) { $transactionStart = $header.IndexOf("class DBTransaction`r`n") }
$transaction = $header.Substring($transactionStart)
$transaction = $transaction.Substring(0,$transaction.IndexOf('#endif'))
$preamble = @'
#include <fmt/format.h>
#include <algorithm>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
enum MarketAction_t{MARKETACTION_BUY,MARKETACTION_SELL};
enum MarketOfferState_t{OFFERSTATE_EXPIRED,OFFERSTATE_CANCELLED,OFFERSTATE_ACCEPTED,OFFERSTATE_ACCEPTEDEX};
namespace Titan{template<class T>auto to_underlying(T v){return std::to_underlying(v);}}
struct MarketOfferEx{MarketOfferEx()=default;MarketOfferEx(MarketOfferEx&&)=default;MarketOfferEx(const MarketOfferEx&)=delete;MarketOfferEx& operator=(const MarketOfferEx&)=delete;MarketOfferEx& operator=(MarketOfferEx&&)=delete;uint32_t id=1,playerId=2,price=10,timestamp=100;uint16_t amount=10,itemId=100,counter=1;MarketAction_t type=MARKETACTION_BUY;std::string playerName;};
void CopyOffer(const MarketOfferEx& from,MarketOfferEx& to){to.id=from.id;to.playerId=from.playerId;to.price=from.price;to.timestamp=from.timestamp;to.amount=from.amount;to.itemId=from.itemId;to.counter=from.counter;to.type=from.type;to.playerName=from.playerName;}
MarketOfferEx CloneOffer(const MarketOfferEx& from){MarketOfferEx copy;CopyOffer(from,copy);return copy;}
using MarketOfferList=std::vector<MarketOfferEx>;
struct DBResult{MarketOfferEx offer;template<class T>T getNumber(const char* name)const{std::string_view n=name;if(n=="id")return offer.id;if(n=="player_id")return offer.playerId;if(n=="sale")return offer.type;if(n=="itemtype")return offer.itemId;if(n=="amount")return offer.amount;if(n=="price")return offer.price;if(n=="created")return offer.timestamp;return 0;}bool next(){return false;}};
using DBResult_ptr=std::shared_ptr<DBResult>;
struct Database{
 static Database& getInstance(){static Database db;return db;}
 MarketOfferEx row,saved_row;bool exists=true,saved_exists=true;std::string failure;int histories=0,saved_histories=0;uint64_t offline_credit=0,saved_credit=0;
 bool beginTransaction(){CopyOffer(row,saved_row);saved_exists=exists;saved_histories=histories;saved_credit=offline_credit;return true;}
 bool rollback(){CopyOffer(saved_row,row);exists=saved_exists;histories=saved_histories;offline_credit=saved_credit;return true;}bool commit(){return true;}
 DBResult_ptr storeQuery(const std::string&){return exists?std::make_shared<DBResult>(DBResult{CloneOffer(row)}):nullptr;}
 bool executeQuery(const std::string& q){
  if(not failure.empty() and q.find(failure)!=std::string::npos)return false;
  if(q.find("DELETE FROM `market_offers`")!=std::string::npos)exists=false;
  if(q.find("INSERT INTO `market_offers`")!=std::string::npos)exists=true;
  if(q.find("INSERT INTO `market_history`")!=std::string::npos)++histories;
  std::smatch match;
  if(q.find("UPDATE `market_offers`")!=std::string::npos and std::regex_search(q,match,std::regex("- ([0-9]+)")))row.amount-=std::stoi(match[1]);
  if(q.find("UPDATE `players`")!=std::string::npos and std::regex_search(q,match,std::regex("\\+ ([0-9]+)")))offline_credit+=std::stoull(match[1]);
  return true;
 }
};
struct Tasks{void addTask(const std::string& q){Database::getInstance().executeQuery(q);}}g_databaseTasks;
struct ConfigManager{enum{MARKET_OFFER_DURATION,MARKET_PREMIUM,MAX_MARKET_OFFERS_AT_A_TIME_PER_PLAYER};};
struct Config{int GetNumber(int key){return key==ConfigManager::MARKET_OFFER_DURATION?3600:0;}bool GetBoolean(int){return false;}}g_config;
namespace Zones{enum class ZoneFlag{NoTransaction};struct ZoneManager{static bool HasWorldFlag(int,ZoneFlag){return false;}};}
struct Item;struct Player;using ItemPtr=std::shared_ptr<Item>;using PlayerPtr=std::shared_ptr<Player>;using PlayerConstPtr=std::shared_ptr<const Player>;
struct ItemType{uint16_t wareId=100,charges=0;bool stackable=true;uint16_t getID()const{return 100;}};
struct Items{ItemType value;const ItemType& operator[](uint16_t)const{return value;}const ItemType& getItemType(uint16_t)const{return value;}};
struct Item{static Items items;uint16_t count=10;uint32_t owner=0;uint16_t getItemCount()const{return count;}uint16_t getID()const{return 100;}static ItemPtr CreateItem(uint16_t,uint16_t count){auto p=std::make_shared<Item>();p->count=count;return p;}};Items Item::items;
struct Container{ItemPtr owner=std::make_shared<Item>();ItemPtr getOwner(){return owner;}};
std::map<uint32_t,uint32_t> Delivered;
struct Player{explicit Player(std::nullptr_t){}uint32_t guid=0;uint64_t bankBalance=1000;uint64_t cash=0;bool offline=false;std::shared_ptr<Container> inbox=std::make_shared<Container>();
bool isInMarket()const{return true;}bool isPremium()const{return true;}bool isOffline()const{return offline;}int getPosition()const{return 0;}uint32_t getGUID()const{return guid;}uint32_t getAccount()const{return guid+10;}uint64_t getMoney()const{return cash;}auto getInbox()const{return inbox;}uint64_t getBankBalance()const{return bankBalance;}void setBankBalance(uint64_t v){bankBalance=v;}
void sendMarketLeave(){}void sendMarketEnter(){}void sendMarketBrowseItem(uint16_t,const MarketOfferList&,const MarketOfferList&){}void sendMarketCancelOffer(const MarketOfferEx&){}void sendMarketAcceptOffer(const MarketOfferEx&){}void onReceiveMail(){}
};
struct IOLoginData{static uint32_t getAccountIdByPlayerId(uint32_t guid){return guid+10;}static bool loadPlayerById(const PlayerPtr& p,uint32_t guid){if(not p)throw std::runtime_error("null offline buyer");p->guid=guid;p->inbox->owner->owner=guid;p->offline=true;return true;}static bool savePlayer(const PlayerPtr&){return true;}static void increaseBankBalance(uint32_t,uint64_t amount){Database::getInstance().offline_credit+=amount;}};
enum{INDEX_ANYWHERE=-1,FLAG_NOLIMIT=1,RETURNVALUE_NOERROR=0};
namespace BlackTek{struct ItemLocation{PlayerPtr player;ItemPtr containerItem;};}
struct Game{std::map<uint32_t,PlayerPtr> players;ItemPtr sellerItems=std::make_shared<Item>();PlayerPtr getPlayerByID(uint32_t id){return players.contains(id)?players[id]:nullptr;}PlayerPtr getPlayerByGUID(uint32_t id){return getPlayerByID(id);}
std::vector<ItemPtr> getMarketItemList(uint16_t,uint16_t amount,const PlayerPtr&){return sellerItems->count>=amount?std::vector<ItemPtr>{sellerItems}:std::vector<ItemPtr>{};}
int internalRemoveItem(const ItemPtr& p,int count=-1,bool test=false){if(not test)p->count-=count<0?p->count:count;return 0;}
int internalAddItem(BlackTek::ItemLocation to,const ItemPtr& item,int,uint32_t){Delivered[to.containerItem->owner]+=item->count;return 0;}
bool removeMoney(BlackTek::ItemLocation from,uint64_t amount){from.player->cash-=amount;return true;}
void playerCreateMarketOffer(uint32_t,uint8_t,uint16_t,uint16_t,uint32_t,bool);
void playerCancelMarketOffer(uint32_t,uint32_t,uint16_t);void playerAcceptMarketOffer(uint32_t,uint32_t,uint16_t,uint16_t);
}g_game;
class IOMarket{public:
static uint32_t getPlayerOfferCount(uint32_t){return 0;}static MarketOfferList getActiveOffers(MarketAction_t,uint16_t){return{};}
static MarketOfferEx getOfferByCounter(uint32_t,uint16_t){auto& db=Database::getInstance();auto offer=CloneOffer(db.row);if(not db.exists)offer.id=0;return offer;}
'@
$definitions = foreach ($name in @('createOffer','acceptOffer','deleteOffer','appendHistory','moveOfferToHistory')) {
    $returnType = if ($BaselineRef -and $name -ne 'moveOfferToHistory') { 'void' } else { 'bool' }
    $function = Get-Function $market "$returnType IOMarket::$name("
    $declaration = $function.Substring(0,$function.IndexOf('{')).Trim().Replace('IOMarket::','')
    if (-not $BaselineRef -and $name -eq 'acceptOffer') { $declaration=$declaration.Replace('bool creditOfflineSeller)','bool creditOfflineSeller = false)') }
    if (-not $BaselineRef -and $name -eq 'moveOfferToHistory') { $declaration=$declaration.Replace('MarketOfferEx* movedOffer)','MarketOfferEx* movedOffer = nullptr)') }
    $preamble += "`nstatic $declaration;`n"
    $function
}
$preamble += "`n};`n$transaction`n"
$definitions += Get-Function $market 'void IOMarket::processExpiredOffers('
# processExpiredOffers is exercised separately using a queued result snapshot.
$preamble = $preamble.Replace('class IOMarket{public:', 'class IOMarket{public: static void processExpiredOffers(const DBResult_ptr&,bool);')
foreach ($name in @('playerCreateMarketOffer','playerCancelMarketOffer','playerAcceptMarketOffer')) { $definitions += Get-Function $game "void Game::$name(" }
$tests = @'
void require(bool value){if(not value)throw std::runtime_error("assertion failed");}
PlayerPtr AddPlayer(uint32_t guid){auto p=std::make_shared<Player>(nullptr);p->guid=guid;p->inbox->owner->owner=guid;g_game.players[guid]=p;return p;}
void BuyRecipient(){auto seller=AddPlayer(1);AddPlayer(2);g_game.playerAcceptMarketOffer(1,0,1,2);require(Delivered[2]==2 and Delivered[1]==0 and seller->bankBalance==1020 and g_game.sellerItems->count==8);}
void OfflineBuyer(){auto seller=AddPlayer(1);g_game.playerAcceptMarketOffer(1,0,1,2);require(Delivered[2]==2 and seller->bankBalance==1020);}
void CancelFailure(){auto p=AddPlayer(2);Database::getInstance().failure="DELETE FROM `market_offers`";g_game.playerCancelMarketOffer(2,0,1);require(p->bankBalance==1000 and Database::getInstance().exists);}
void CancelHistoryFailure(){auto p=AddPlayer(2);Database::getInstance().failure="INSERT INTO `market_history`";g_game.playerCancelMarketOffer(2,0,1);require(p->bankBalance==1000 and Database::getInstance().exists);}
void AcceptFailure(){auto p=AddPlayer(1);AddPlayer(2);Database::getInstance().failure="UPDATE `market_offers`";g_game.playerAcceptMarketOffer(1,0,1,2);require(p->bankBalance==1000 and Delivered.empty() and g_game.sellerItems->count==10 and Database::getInstance().row.amount==10);}
void AcceptHistoryFailure(){auto p=AddPlayer(1);AddPlayer(2);Database::getInstance().failure="INSERT INTO `market_history`";g_game.playerAcceptMarketOffer(1,0,1,2);require(p->bankBalance==1000 and Delivered.empty() and g_game.sellerItems->count==10 and Database::getInstance().row.amount==10);}
void CreateBuyFailure(){auto p=AddPlayer(1);auto& db=Database::getInstance();db.exists=false;db.failure="INSERT INTO `market_offers`";g_game.playerCreateMarketOffer(1,MARKETACTION_BUY,100,2,10,false);require(p->bankBalance==1000 and not db.exists);}
void CreateSellFailure(){auto p=AddPlayer(1);auto& db=Database::getInstance();db.exists=false;db.failure="INSERT INTO `market_offers`";g_game.playerCreateMarketOffer(1,MARKETACTION_SELL,100,2,10,false);require(p->bankBalance==1000 and g_game.sellerItems->count==10 and not db.exists);}
void ExpiryCurrentAmount(){auto p=AddPlayer(2);auto& db=Database::getInstance();auto queued=std::make_shared<DBResult>(DBResult{CloneOffer(db.row)});db.row.amount=3;IOMarket::processExpiredOffers(queued,true);require(p->bankBalance==1030 and not db.exists);}
void OfflineSellerFailure(){auto p=AddPlayer(1);auto& db=Database::getInstance();db.row.type=MARKETACTION_SELL;db.failure="UPDATE `players`";g_game.playerAcceptMarketOffer(1,0,1,2);require(p->bankBalance==1000 and Delivered.empty() and db.row.amount==10);}
int main(int argc,char** argv){std::vector<void(*)()> tests={BuyRecipient,OfflineBuyer,CancelFailure,CancelHistoryFailure,AcceptFailure,AcceptHistoryFailure,CreateBuyFailure,CreateSellFailure,ExpiryCurrentAmount,OfflineSellerFailure};if(argc<2)return static_cast<int>(tests.size());try{tests.at(std::stoi(argv[1]))();return 0;}catch(const std::exception& e){std::cout<<e.what()<<std::endl;return 1;}}
'@
$generated = Join-Path $OutputDirectory 'market-regressions.cpp'
[IO.File]::WriteAllText($generated, ($preamble,($definitions -join "`n"),$tests -join "`n"))
$exe = Join-Path $OutputDirectory 'market-regressions.exe'
$env:PATH = (Split-Path $Compiler) + ';' + $env:PATH
& $Compiler -std=c++23 -O0 -g $generated -lfmt -o $exe
if ($LASTEXITCODE -ne 0) { throw 'Market regression compilation failed' }
$names = @('BuyRecipient','OfflineBuyer','CancelFailure','CancelHistoryFailure','AcceptFailure','AcceptHistoryFailure','CreateBuyFailure','CreateSellFailure','ExpiryCurrentAmount','OfflineSellerFailure')
$failed = 0
for ($i=0;$i -lt $names.Count;++$i) {
    & $exe $i
    $code=$LASTEXITCODE
    if($code -ne 0){$failed++}
    Write-Output "$($names[$i]): exit $code"
}
Write-Output "$($names.Count-$failed)/$($names.Count) passed"
if($failed){exit 1}
