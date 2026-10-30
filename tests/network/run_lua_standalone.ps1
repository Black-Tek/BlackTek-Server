param(
    [Parameter(Mandatory = $true)][string]$Compiler,
    [string]$OutputDirectory = (Join-Path $env:TEMP 'blacktek-lua-tests'),
    [string]$BaselineRef = ''
)

# Compiles the production binding functions against a real LuaJIT VM. Game, SQL,
# curl and XML lifetime boundaries are deterministic doubles. No requests are sent.
# This checks binding behavior, not full-server or live-service integration.
$ErrorActionPreference = 'Stop'
$repository = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$prefix = Split-Path (Split-Path $Compiler)
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
$source = Read-Source 'src/luascript.cpp'
$header = Read-Source 'src/luascript.h'
$preamble = @'
#include <lua.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <ctime>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <utility>
static lua_State* luaState;
static int32_t scriptEnvIndex = -1;
static int Errors = 0;
template<class... T> void reportErrorFunc(lua_State*, T&&...) { ++Errors; }
enum { LUA_ERROR_ITEM_NOT_FOUND, LUA_ERROR_CREATURE_NOT_FOUND, LUA_ERROR_AREA_NOT_FOUND };
const char* getErrorDesc(int) { return "missing"; }
template<class T> T getNumber(lua_State* L, int arg) { return static_cast<T>(lua_tonumber(L,arg)); }
template<class T> T getNumber(lua_State* L, int arg,T fallback) { return lua_isnoneornil(L,arg) ? fallback : getNumber<T>(L,arg); }
bool isNumber(lua_State* L,int arg) { return lua_isnumber(L,arg); }
bool isString(lua_State* L,int arg) { return lua_isstring(L,arg); }
bool isBoolean(lua_State* L,int arg) { return lua_isboolean(L,arg); }
bool isUserdata(lua_State* L,int arg) { return lua_isuserdata(L,arg); }
std::string getString(lua_State* L,int arg) { size_t len=0;const auto value=lua_tolstring(L,arg,&len);return value?std::string(value,len):std::string{}; }
void pushString(lua_State* L,const std::string& s) { lua_pushlstring(L,s.data(),s.size()); }
void pushBoolean(lua_State* L,bool value) { lua_pushboolean(L,value); }
template<class T> std::shared_ptr<T>& getSharedPtr(lua_State* L,int arg) { return *static_cast<std::shared_ptr<T>*>(lua_touserdata(L,arg)); }
template<class T> T** getRawUserdata(lua_State* L,int arg) { return static_cast<T**>(lua_touserdata(L,arg)); }
template<class T> T* getUserdata(lua_State* L,int arg) { auto p=getRawUserdata<T>(L,arg); return p?*p:nullptr; }
template<class T> void pushUserdata(lua_State* L,T* p) { *static_cast<T**>(lua_newuserdata(L,sizeof(T*)))=p; }
template<class T> void pushSharedPtr(lua_State* L,const std::shared_ptr<T>& p) { new(lua_newuserdata(L,sizeof(p))) std::shared_ptr<T>(p); }
void setMetatable(lua_State* L,int arg,const std::string& name) { if(arg<0) arg=lua_gettop(L)+arg+1;luaL_getmetatable(L,name.c_str());lua_setmetatable(L,arg); }
namespace BlackTek::Console::Script { template<class...T> void Warn(T&&...){} template<class...T> void Error(T&&...){} }
namespace BlackTek::Console::Net { template<class...T> void Warn(T&&...){} }
struct Database {
  static Database& getInstance(){static Database db;return db;}
  std::string escapeBlob(const char* p,uint32_t len){if(len>4) throw std::runtime_error("out of bounds blob read"); return std::string(p,len);}
};
struct Item; using ItemPtr=std::shared_ptr<Item>;
struct Container { bool isHoldingItem(const ItemPtr&) const { if(this==nullptr) throw std::runtime_error("null container");return true; } };
struct ItemType { bool stackable=true; };
struct ItemTypes { ItemType operator[](uint16_t)const{return{};} uint16_t getItemIdByName(const std::string&)const{return 2;} };
struct Item {
  static ItemTypes items; uint16_t count=10,id=1; bool removed=false; std::shared_ptr<Container> container;
  bool isStackable()const{return true;} bool isRemoved()const{return removed;} bool isAugmented()const{return false;}
  uint16_t getItemCount()const{return count;} uint16_t getID()const{return id;} uint16_t getSubType()const{return count;}
  void setItemCount(uint16_t n){count=n;} void setScriptDetached(){} auto getContainer()const{return container;}
  ItemPtr clone()const{return std::make_shared<Item>(*this);}
}; ItemTypes Item::items;
void setItemMetatable(lua_State* L,int arg,const ItemPtr&) {setMetatable(L,arg,"Item");}
struct ScriptEnvironment {uint32_t addItem(const ItemPtr&){return 1;} void removeItemByUID(uint32_t){} void insertItem(uint32_t,const ItemPtr&){} void addTempItem(const ItemPtr&){} template<class T>void setScriptId(int,T*){} };
ScriptEnvironment* getScriptEnv(){static ScriptEnvironment e;return &e;}
struct CURL { std::string payload; }; struct curl_slist{};
enum {CURLOPT_URL,CURLOPT_POSTFIELDS,CURLOPT_HTTPHEADER,CURLOPT_CONNECTTIMEOUT,CURLOPT_TIMEOUT,CURLE_OK};
int Handles=0,Headers=0,Requests=0;std::string LastPayload;
CURL* curl_easy_init(){++Handles;return new CURL;}
void curl_easy_cleanup(CURL* p){--Handles;delete p;}
curl_slist* curl_slist_append(curl_slist*,const char*){++Headers;return new curl_slist;}
void curl_slist_free_all(curl_slist* p){--Headers;delete p;}
template<class T> void curl_easy_setopt(CURL* p,int option,T value){if constexpr(std::is_convertible_v<T,const char*>){if(option==CURLOPT_POSTFIELDS)p->payload=value;}}
int curl_easy_perform(CURL* p){++Requests;LastPayload=p->payload;return CURLE_OK;}
const char* curl_easy_strerror(int){return "error";}
enum DiscordMessageType { MESSAGE_NORMAL,MESSAGE_ERROR,MESSAGE_LOG,MESSAGE_INFO };
struct Game { ItemPtr replacement; CURL shared; CURL* curl=&shared; ItemPtr transformItem(const ItemPtr& p,uint16_t id,int32_t count){if(replacement)return replacement;p->id=id;p->count=count;return p;} }g_game;
template<class T> auto createTask(T task){return task;}
struct Boss { template<class T>void addTask(T task){task();} }g_utility_boss;
enum class StatModifierType {None,Multiply,Divide,Add,Subtract};
struct StandardStatMod {StatModifierType type;uint32_t value;};
struct StandardStat {uint32_t id,initial,maximum;bool scale;int modifications=0;
StandardStat(uint32_t id,uint32_t initial,uint32_t maximum,bool scale=true):id(id),initial(initial),maximum(maximum),scale(scale){}
uint32_t current()const{return initial;}uint32_t max()const{return maximum;}
void addModifier(const StandardStatMod&){++modifications;}void addModifier(StatModifierType,uint32_t){++modifications;}
bool removeModifier(const StandardStatMod&){--modifications;return true;}bool removeModifier(StatModifierType,uint32_t){--modifications;return true;}
};
struct Party {bool leader=true;void disband(){leader=false;}bool getLeader()const{return leader;} };
namespace pugi {
struct xml_attribute { bool valid=true;explicit operator bool()const{return valid;}const char* value()const{return "value";} };
struct xml_node {std::weak_ptr<int> owner;std::string label;
  xml_node()=default; xml_node(std::weak_ptr<int> p,std::string s):owner(p),label(std::move(s)){}
  void check()const{if(owner.expired())throw std::runtime_error("XML document use after free");}
  explicit operator bool()const{return not owner.expired();}
  xml_attribute attribute(const char*)const{check();return{};}const char* name()const{check();return label.c_str();}
  xml_node first_child()const{check();return {owner,"child"};}xml_node next_sibling()const{check();return {owner,"sibling"};}
};
struct xml_document {std::shared_ptr<int> owner=std::make_shared<int>(0);bool load_file(const char*){return true;}xml_node child(const char*){return{owner,"root"};}};
}
template<class...T>void printXMLError(T&&...){}
struct Creature {};using CreaturePtr=std::shared_ptr<Creature>;
CreaturePtr Target=std::make_shared<Creature>();
CreaturePtr getCreature(lua_State* L,int arg){return lua_tointeger(L,arg)==1?Target:nullptr;}
struct Position{};Position getPosition(lua_State*,int){return{};}
using CombatType_t=uint16_t;constexpr uint8_t CONST_ME_NONE=0;
int32_t normal_random(int32_t min,int32_t){return min;}
namespace BlackTek {
struct AreaCombat{};
struct Combat {
 enum Origin:uint8_t{None,Condition,Spell,Melee};enum DamageType:uint16_t{Physical=1,Healing=128};
 enum Config{Aggressive,HealthTarget,BlockedByArmor,BlockedByDefense};
 uint16_t type;uint32_t amount;Origin origin=None;uint8_t effect=0;bool flags[4]{};bool area=false,executed=false,struck=false;
 void SetImpactEffect(uint8_t v){effect=v;}void setOrigin(Origin v){origin=v;}void SetConfig(Config c,bool v=true){flags[c]=v;}
 void setArea(std::unique_ptr<AreaCombat>){area=true;}void execute(const CreaturePtr&,Position){executed=true;}
 void strike_target(const CreaturePtr&,const CreaturePtr&){struck=true;}
};
struct Registry{std::shared_ptr<Combat> last;auto Create(uint16_t type,uint32_t amount){last=std::make_shared<Combat>();last->type=type;last->amount=amount;return last;}}g_combat_registry;
}
struct LuaEnvironment{BlackTek::AreaCombat area;const auto* getAreaObject(uint32_t id){return id==1?&area:nullptr;}}g_luaEnvironment;
using PlayerPtr=CreaturePtr;using BlockType_t=uint8_t;
struct LuaScriptInterface{
 bool reserveScriptEnv(){return true;}ScriptEnvironment* getScriptEnv(){return ::getScriptEnv();}lua_State* getLuaState(){return luaState;}void pushFunction(int){}void callVoidFunction(int){}
 static void pushItem(lua_State* L,const ItemPtr& p){::pushSharedPtr(L,p);}template<class T>static void pushSharedPtr(lua_State* L,const std::shared_ptr<T>& p){::pushSharedPtr(L,p);}
 static void setMetatable(lua_State* L,int arg,const char* name){::setMetatable(L,arg,name);}static void setCreatureMetatable(lua_State*,int,const CreaturePtr&){}
 static void pushBoolean(lua_State* L,bool b){::pushBoolean(L,b);}
};
namespace BlackTek::Console{template<class...T>void Error(T&&...){}}
struct ItemEvent{LuaScriptInterface* scriptInterface;int scriptId=1;void executeAttack(const ItemPtr&,const PlayerPtr&,const CreaturePtr&,BlockType_t,CombatType_t,BlackTek::Combat::Origin,bool,bool)const;};
enum{GLOBALEVENT_TIMER};
struct GlobalEvent {int interval=123;time_t next=0;void setInterval(int v){interval=v;}void setNextExecution(time_t v){next=v;}void setEventType(int){}std::string getName()const{return "test";}};
std::vector<std::string> explodeString(const std::string& s,const char*){std::vector<std::string> out;size_t start=0;for(size_t i=0;i<=s.size();++i){if(i==s.size()or s[i]==':'){out.push_back(s.substr(start,i-start));start=i+1;}}return out;}
std::vector<int32_t> vectorAtoi(const std::vector<std::string>& values){std::vector<int32_t> out;for(const auto& s:values)out.push_back(std::stoi(s));return out;}
'@
$boolean = (Get-Function $header 'static bool getBoolean(lua_State* L, int32_t arg)') + "`n" + (Get-Function $header 'static bool getBoolean(lua_State* L, int32_t arg, bool defaultValue)')
$cache = @'
template<class T> concept SharedPtr = requires(T value) { value.get(); };
static int spCacheRef = LUA_NOREF;
int lua_rawgetp(lua_State* L,int index,const void* key){if(index<0)index=lua_gettop(L)+index+1;lua_pushlightuserdata(L,const_cast<void*>(key));lua_rawget(L,index);return lua_type(L,-1);}
void lua_rawsetp(lua_State* L,int index,const void* key){if(index<0)index=lua_gettop(L)+index+1;lua_pushlightuserdata(L,const_cast<void*>(key));lua_insert(L,-2);lua_rawset(L,index);}
void* lua_newuserdatauv(lua_State* L,size_t size,int){return lua_newuserdata(L,size);}
'@
$cache += "`n" + (Get-Function $header 'static void initSharedPtrCache(') + "`n" + (Get-Function $header 'template <SharedPtr T>')
$preamble = $preamble.Replace('template<class T> void pushSharedPtr(lua_State* L,const std::shared_ptr<T>& p) { new(lua_newuserdata(L,sizeof(p))) std::shared_ptr<T>(p); }', $cache)
$reserve = Get-Function $header 'static bool reserveScriptEnv('
$functions = @('luaDatabaseEscapeBlob','luaIsType','luaItemSplit','luaItemTransform','luaContainerHasItem','luaStatCreate','luaStatAddModifier','luaStatRemoveModifier','luaPartyDisband','luaGameSendDiscordWebhook','luaCreateXmlDocument','luaDeleteXmlDocument','luaXmlDocumentChild','luaDeleteXmlNode','luaXmlNodeAttribute','luaXmlNodeName','luaXmlNodeFirstChild','luaXmlNodeNextSibling','luaDoAreaCombat','luaDoTargetCombat','luaGlobalEventTime')
$definitions = foreach ($name in $functions) { (Get-Function $source "int LuaScriptInterface::$name(").Replace('LuaScriptInterface::','') }
$definitions += (Get-Function $source 'void LuaScriptInterface::setWeakMetatable(').Replace('LuaScriptInterface::','')
$definitions += Get-Function (Read-Source 'src/itemevents.cpp') 'void ItemEvent::executeAttack('
$xmlWrapper = ''
if (-not $BaselineRef) {
    $start = $source.IndexOf('namespace BlackTek::Script')
    $end = $source.IndexOf('int LuaScriptInterface::luaCreateXmlDocument(', $start)
    $xmlWrapper = $source.Substring($start,$end-$start)
}
$tests = @'
void require(bool condition){if(not condition)throw std::runtime_error("assertion failed");}
void clear(){lua_settop(luaState,0);}
void pushItem(const ItemPtr& p){pushSharedPtr(luaState,p);}
void makeClass(const char* name,const char* base=nullptr){
 lua_newtable(luaState);lua_setglobal(luaState,name);
 lua_getglobal(luaState,name);lua_newtable(luaState);if(base){lua_getglobal(luaState,base);lua_setfield(luaState,-2,"__index");}lua_setmetatable(luaState,-2);lua_pop(luaState,1);
 luaL_newmetatable(luaState,name);lua_getglobal(luaState,name);lua_setfield(luaState,-2,"__index");lua_pop(luaState,1);
}
void classObject(const char* name){lua_newuserdata(luaState,8);setMetatable(luaState,-1,name);}
void BooleanFalse(){lua_pushboolean(luaState,false);require(not getBoolean(luaState,1,true));}
void BooleanMissing(){require(getBoolean(luaState,1,true));lua_pushnil(luaState);require(getBoolean(luaState,1,true));}
void EnvironmentOverflow(){for(int i=0;i<16;++i)require(reserveScriptEnv());require(not reserveScriptEnv());require(scriptEnvIndex==15);--scriptEnvIndex;require(reserveScriptEnv());require(scriptEnvIndex==15);}
void UnrelatedTypes(){makeClass("Item");makeClass("Player");classObject("Item");classObject("Player");luaIsType(luaState);require(not lua_toboolean(luaState,-1));}
void InheritedTypes(){makeClass("Item");makeClass("Container","Item");classObject("Container");lua_getglobal(luaState,"Item");luaIsType(luaState);require(lua_toboolean(luaState,-1));}
void InvalidType(){lua_pushnil(luaState);lua_pushnumber(luaState,1);luaIsType(luaState);require(not lua_toboolean(luaState,-1));}
void BlobTooLong(){lua_pushstring(luaState,"test");lua_pushnumber(luaState,100);luaDatabaseEscapeBlob(luaState);require(lua_isboolean(luaState,-1) and not lua_toboolean(luaState,-1));}
void BlobPrefix(){lua_pushstring(luaState,"test");lua_pushnumber(luaState,2);luaDatabaseEscapeBlob(luaState);require(getString(luaState,-1)=="te");}
void SplitOwnership(){auto p=std::make_shared<Item>();pushItem(p);lua_pushnumber(luaState,2);luaItemSplit(luaState);auto& stored=getSharedPtr<Item>(luaState,1);require(not p.owner_before(stored) and not stored.owner_before(p));}
void TransformHandle(){auto p=std::make_shared<Item>();g_game.replacement=std::make_shared<Item>();g_game.replacement->id=2;pushItem(p);lua_pushnumber(luaState,2);luaItemTransform(luaState);require(getSharedPtr<Item>(luaState,1)==g_game.replacement);}
void NullContainer(){pushItem(std::make_shared<Item>());pushItem(std::make_shared<Item>());luaContainerHasItem(luaState);require(lua_isnil(luaState,-1));}
void StatNumeric(){lua_pushnil(luaState);lua_pushnumber(luaState,7);lua_pushnumber(luaState,100);lua_pushnumber(luaState,30);lua_pushboolean(luaState,false);luaStatCreate(luaState);const auto p=getSharedPtr<StandardStat>(luaState,-1);require(p and p->current()==30 and p->max()==100 and not p->scale);}
void StatZero(){lua_pushnil(luaState);lua_pushnumber(luaState,7);lua_pushnumber(luaState,100);luaStatCreate(luaState);require(getSharedPtr<StandardStat>(luaState,-1)->current()==0);}
void StatModifierNumber(){auto p=std::make_shared<StandardStat>(1,30,100);pushSharedPtr(luaState,p);lua_pushnumber(luaState,3);lua_pushnumber(luaState,4);luaStatAddModifier(luaState);require(p->modifications==1 and lua_toboolean(luaState,-1));}
void StatModifierObject(){auto p=std::make_shared<StandardStat>(1,30,100);pushSharedPtr(luaState,p);pushSharedPtr(luaState,std::make_shared<StandardStatMod>());luaStatAddModifier(luaState);require(p->modifications==1 and lua_toboolean(luaState,-1));}
void PartyRelease(){auto p=std::make_shared<Party>();pushSharedPtr(luaState,p);luaPartyDisband(luaState);require(p.use_count()==1);}
void WebhookEscaping(){lua_pushstring(luaState,"https://example.invalid");lua_pushnumber(luaState,0);lua_pushstring(luaState,"line\n\"quote\"\\end");luaGameSendDiscordWebhook(luaState);require(Requests==1);require(LastPayload.find("line\\u000a\\\"quote\\\"\\\\end")!=std::string::npos);require(Handles==0 and Headers==0);}
void WebhookInvalidType(){lua_pushstring(luaState,"https://example.invalid");lua_pushnumber(luaState,77);lua_pushstring(luaState,"test");luaGameSendDiscordWebhook(luaState);require(Requests==0 and lua_isboolean(luaState,-1) and not lua_toboolean(luaState,-1));require(Handles==0 and Headers==0);}
void XmlChildLifetime(){
 lua_pushnil(luaState);lua_pushstring(luaState,"fixture.xml");luaCreateXmlDocument(luaState);lua_replace(luaState,1);lua_settop(luaState,1);lua_pushstring(luaState,"root");luaXmlDocumentChild(luaState);int node=luaL_ref(luaState,LUA_REGISTRYINDEX);lua_settop(luaState,1);luaDeleteXmlDocument(luaState);clear();lua_rawgeti(luaState,LUA_REGISTRYINDEX,node);luaXmlNodeFirstChild(luaState);lua_replace(luaState,1);lua_settop(luaState,1);luaXmlNodeName(luaState);require(getString(luaState,-1)=="child");
}
void StatRemoveNumber(){auto p=std::make_shared<StandardStat>(1,30,100);p->modifications=1;pushSharedPtr(luaState,p);lua_pushnumber(luaState,3);lua_pushnumber(luaState,4);luaStatRemoveModifier(luaState);require(p->modifications==0 and lua_toboolean(luaState,-1));}
void WeakMetatableReload(){makeClass("Condition");classObject("Condition");setWeakMetatable(luaState,-1,"Condition");lua_close(luaState);luaState=luaL_newstate();makeClass("Condition");classObject("Condition");setWeakMetatable(luaState,-1,"Condition");require(lua_getmetatable(luaState,-1)!=0);}
void AreaArguments(uint32_t area=1){lua_pushnumber(luaState,0);lua_pushnumber(luaState,128);lua_newtable(luaState);lua_pushnumber(luaState,area);lua_pushnumber(luaState,-2147483648.0);lua_pushnumber(luaState,-2147483648.0);lua_pushnumber(luaState,9);lua_pushnumber(luaState,3);}
void AreaExecutes(){AreaArguments();luaDoAreaCombat(luaState);const auto c=BlackTek::g_combat_registry.last;require(c and c->executed and c->area and c->amount==2147483648u and c->origin==BlackTek::Combat::Melee and c->flags[BlackTek::Combat::HealthTarget] and not c->flags[BlackTek::Combat::Aggressive] and lua_isboolean(luaState,-1));}
void AreaInvalid(){AreaArguments(999);luaDoAreaCombat(luaState);require(not BlackTek::g_combat_registry.last and lua_isboolean(luaState,-1) and not lua_toboolean(luaState,-1));}
void TargetHealing(){lua_pushnumber(luaState,0);lua_pushnumber(luaState,1);lua_pushnumber(luaState,128);lua_pushnumber(luaState,10);lua_pushnumber(luaState,10);lua_pushnumber(luaState,9);lua_pushnumber(luaState,3);luaDoTargetCombat(luaState);const auto c=BlackTek::g_combat_registry.last;require(c and c->struck and c->flags[BlackTek::Combat::HealthTarget] and not c->flags[BlackTek::Combat::Aggressive] and c->origin==BlackTek::Combat::Melee);}
void InvalidEventTime(){GlobalEvent event;pushUserdata(luaState,&event);lua_pushstring(luaState,"bad");luaGlobalEventTime(luaState);require(not lua_toboolean(luaState,-1) and event.interval==123);}
void AtomicEventTime(){GlobalEvent event;pushUserdata(luaState,&event);lua_pushstring(luaState,"12:99");luaGlobalEventTime(luaState);require(not lua_toboolean(luaState,-1) and event.interval==123);}
void ItemEventDamageType(){LuaScriptInterface interface;ItemEvent event{&interface};event.executeAttack(std::make_shared<Item>(),Target,Target,0,2048,BlackTek::Combat::Melee,false,false);require(lua_tointeger(luaState,5)==2048);}
void CachedItemReplacement(){auto original=std::make_shared<Item>();auto replacement=std::make_shared<Item>();pushSharedPtr(luaState,original);getSharedPtr<Item>(luaState,-1)=replacement;pushSharedPtr(luaState,original);require(getSharedPtr<Item>(luaState,-1)==original);}
int main(int argc,char** argv){
 luaState=luaL_newstate();luaL_openlibs(luaState);initSharedPtrCache(luaState);
 std::vector<void(*)()> tests={BooleanFalse,BooleanMissing,EnvironmentOverflow,UnrelatedTypes,InheritedTypes,InvalidType,BlobTooLong,BlobPrefix,SplitOwnership,TransformHandle,NullContainer,StatNumeric,StatZero,StatModifierNumber,StatModifierObject,PartyRelease,WebhookEscaping,WebhookInvalidType,XmlChildLifetime,StatRemoveNumber,WeakMetatableReload,AreaExecutes,AreaInvalid,TargetHealing,InvalidEventTime,AtomicEventTime,ItemEventDamageType,CachedItemReplacement};
 if(argc<2)return static_cast<int>(tests.size());
 try{tests.at(std::stoi(argv[1]))();std::_Exit(0);}catch(const std::exception& error){std::cout<<error.what()<<std::endl;std::_Exit(1);}
}
'@
$generated = Join-Path $OutputDirectory 'lua-regressions.cpp'
[IO.File]::WriteAllText($generated, ($preamble,$boolean,$reserve,$xmlWrapper,($definitions -join "`n"),$tests -join "`n"))
$exe = Join-Path $OutputDirectory 'lua-regressions.exe'
$env:PATH = (Split-Path $Compiler) + ';' + $env:PATH
& $Compiler -std=c++23 -O0 -g -I (Join-Path $prefix 'include/luajit-2.1') $generated '-lluajit-5.1' -lfmt -o $exe
if ($LASTEXITCODE -ne 0) { throw 'Lua regression compilation failed' }
$names = @('BooleanFalse','BooleanMissing','EnvironmentOverflow','UnrelatedTypes','InheritedTypes','InvalidType','BlobTooLong','BlobPrefix','SplitOwnership','TransformHandle','NullContainer','StatNumeric','StatZero','StatModifierNumber','StatModifierObject','PartyRelease','WebhookEscaping','WebhookInvalidType','XmlChildLifetime','StatRemoveNumber','WeakMetatableReload','AreaExecutes','AreaInvalid','TargetHealing','InvalidEventTime','AtomicEventTime','ItemEventDamageType','CachedItemReplacement')
$failed = 0
for ($i=0;$i -lt $names.Count;++$i) {
    & $exe $i
    $code=$LASTEXITCODE
    if($code -ne 0){$failed++}
    Write-Output "$($names[$i]): exit $code"
}
Write-Output "$($names.Count-$failed)/$($names.Count) passed"
if($failed){exit 1}
