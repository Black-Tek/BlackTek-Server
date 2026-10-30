param(
    [Parameter(Mandatory = $true)][string]$Compiler,
    [string]$OutputDirectory = (Join-Path $env:TEMP 'blacktek-item-tests'),
    [string]$BaselineRef = ''
)

# Extracts the production ownership, custom-stat, augment-removal and bed-access
# functions. Lightweight game boundaries make lifetime and reentrancy failures
# deterministic; this is not a full map, player or database integration test.
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
$source = Read-Source 'src/item.cpp'
$header = Read-Source 'src/item.h'
$shared = Read-Source 'src/sharedobject.h'
$shared = $shared.Substring($shared.IndexOf('class SharedObject;'))
$shared = $shared.Substring(0,$shared.IndexOf('#endif'))
$preamble = @'
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
'@
$preamble += "`n$shared`n"
$preamble += @'
class Item;using ItemPtr=std::shared_ptr<Item>;using ItemConstPtr=std::shared_ptr<const Item>;
enum class ContainerSubType{None};
struct ItemContainer{std::weak_ptr<Item> owner;ItemContainer(uint16_t,bool,bool,ContainerSubType){}};
using ContainerPtr=std::shared_ptr<ItemContainer>;using ContainerConstPtr=std::shared_ptr<const ItemContainer>;
namespace BlackTek{struct Augment{std::string name;explicit Augment(std::string n):name(std::move(n)){}std::string_view getName()const{return name;}
uint32_t attack_mod_count()const{return 1;}uint32_t def_mod_count()const{return 0;}uint32_t defense_mod_count()const{return 0;}uint32_t conversion_count()const{return 0;}uint32_t reform_count()const{return 0;}uint32_t damage_triggers()const{return 0;}uint32_t origin_triggers()const{return 0;}uint32_t creature_triggers()const{return 0;}uint32_t race_triggers()const{return 0;}uint32_t name_count()const{return 0;}};}
struct ItemEvents{std::function<void(const ItemPtr&,const std::shared_ptr<BlackTek::Augment>&)> callback;void fireRemoveAugment(const ItemPtr& p,const std::shared_ptr<BlackTek::Augment>& a){if(callback)callback(p,a);}} Events;auto* g_itemEvents=&Events;
struct Stat{bool add(uint32_t){return true;}bool remove(uint32_t){return true;}};
using StatRegistry=std::map<uint32_t,std::shared_ptr<Stat>>;
constexpr int ZONE_PROTECTION=1,HOUSE_OWNER=3;
struct Player{explicit Player(std::nullptr_t){}bool isPremium()const{return true;}int getZone()const{return ZONE_PROTECTION;}};
using PlayerPtr=std::shared_ptr<Player>;using PlayerConstPtr=std::shared_ptr<const Player>;
struct House{int getHouseAccessLevel(const PlayerConstPtr&)const{return 1;}};
struct IOLoginData{static bool loadPlayerById(const PlayerPtr& p,uint32_t){if(not p)throw std::runtime_error("null offline player");return true;}};
class Item:public SharedObject{
public:
 std::unique_ptr<ItemContainer> container;
 std::unique_ptr<StatRegistry> c_stats=std::make_unique<StatRegistry>();
 std::unique_ptr<std::vector<std::shared_ptr<BlackTek::Augment>>> augments=std::make_unique<std::vector<std::shared_ptr<BlackTek::Augment>>>();
 uint32_t attack_modifier_count=0,defense_modifier_count=0,conversion_modifier_count=0,reform_modifier_count=0,damage_modifiers_count=0,origin_modifiers_count=0,creature_modifiers_count=0,race_modifiers_count=0,named_modifiers_count=0;
 House* house=nullptr;uint32_t sleeperGUID=1;void updateHouse(){}
 void attachContainer(uint16_t size,bool unlocked,bool pagination,ContainerSubType subtype=ContainerSubType::None);
 ContainerPtr getContainer();ContainerConstPtr getContainer()const;
 const bool removeAugment(std::shared_ptr<BlackTek::Augment>&);const bool removeAugment(std::string_view);
 bool isAugmented()const{return not augments->empty();}bool canBedBeUsed(const PlayerConstPtr&)const;
'@
if ($header.Contains('ContainerPtr containerAlias;')) { $preamble += "`nContainerPtr containerAlias;`n" }
$preamble += (Get-Function $header 'bool increaseCustomStat(') + "`n" + (Get-Function $header 'bool decreaseCustomStat(') + "`n};`n"
$definitions = foreach ($signature in @('void Item::attachContainer(', 'ContainerPtr Item::getContainer(', 'ContainerConstPtr Item::getContainer(', 'const bool Item::removeAugment(std::shared_ptr', 'const bool Item::removeAugment(std::string_view', 'bool Item::canBedBeUsed(')) { Get-Function $source $signature }
$tests = @'
void require(bool v){if(not v)throw std::runtime_error("assertion failed");}
void ReleaseContainer(){std::weak_ptr<Item> itemWeak;std::weak_ptr<ItemContainer> containerWeak;{auto item=std::make_shared<Item>();item->attachContainer(8,true,false);itemWeak=item;containerWeak=item->getContainer();}require(itemWeak.expired() and containerWeak.expired());}
void HeldContainer(){auto item=std::make_shared<Item>();item->attachContainer(8,true,false);std::weak_ptr<Item> weak=item;auto container=item->getContainer();item.reset();require(not weak.expired() and container->owner.lock()==weak.lock());container.reset();require(weak.expired());}
void ConstContainer(){auto item=std::make_shared<Item>();item->attachContainer(8,true,false);ItemConstPtr immutable=item;auto container=immutable->getContainer();std::weak_ptr<Item> weak=item;item.reset();immutable.reset();require(not weak.expired());container.reset();require(weak.expired());}
void PlainItem(){auto item=std::make_shared<Item>();require(not item->getContainer());}
void MissingStatAdd(){Item item;item.c_stats->emplace(1,std::make_shared<Stat>());require(not item.increaseCustomStat(9,1));}
void MissingStatRemove(){Item item;item.c_stats->emplace(1,std::make_shared<Stat>());require(not item.decreaseCustomStat(9,1));}
void NullStat(){Item item;item.c_stats->emplace(1,nullptr);require(not item.increaseCustomStat(1,1) and not item.decreaseCustomStat(1,1));}
void AugmentCallbackAfterRemoval(){auto item=std::make_shared<Item>();auto aug=std::make_shared<BlackTek::Augment>("test");item->augments->push_back(aug);item->attack_modifier_count=1;bool called=false;Events.callback=[&](const ItemPtr& p,const auto&){called=true;require(p->augments->empty());};require(item->removeAugment("test"));require(called and item->attack_modifier_count==0);}
void AugmentNestedRemoval(){auto item=std::make_shared<Item>();item->augments->push_back(std::make_shared<BlackTek::Augment>("first"));item->augments->push_back(std::make_shared<BlackTek::Augment>("second"));item->attack_modifier_count=2;Events.callback=[](const ItemPtr& p,const auto& a){if(a->name=="first")p->removeAugment("second");};require(item->removeAugment("first"));require(item->augments->empty() and item->attack_modifier_count==0);}
void OfflineBed(){auto item=std::make_shared<Item>();House house;item->house=&house;require(item->canBedBeUsed(std::make_shared<Player>(nullptr)));}
int main(int argc,char** argv){std::vector<void(*)()> tests={ReleaseContainer,HeldContainer,ConstContainer,PlainItem,MissingStatAdd,MissingStatRemove,NullStat,AugmentCallbackAfterRemoval,AugmentNestedRemoval,OfflineBed};if(argc<2)return static_cast<int>(tests.size());try{tests.at(std::stoi(argv[1]))();return 0;}catch(const std::exception& e){std::cout<<e.what()<<std::endl;return 1;}}
'@
$generated = Join-Path $OutputDirectory 'item-regressions.cpp'
[IO.File]::WriteAllText($generated, ($preamble,($definitions -join "`n"),$tests -join "`n"))
$exe = Join-Path $OutputDirectory 'item-regressions.exe'
$env:PATH = (Split-Path $Compiler) + ';' + $env:PATH
& $Compiler -std=c++23 -O0 -g -D_GLIBCXX_DEBUG $generated -o $exe
if ($LASTEXITCODE -ne 0) { throw 'Item regression compilation failed' }
$names = @('ReleaseContainer','HeldContainer','ConstContainer','PlainItem','MissingStatAdd','MissingStatRemove','NullStat','AugmentCallbackAfterRemoval','AugmentNestedRemoval','OfflineBed')
$failed = 0
for ($i=0;$i -lt $names.Count;++$i) {
    & $exe $i
    $code=$LASTEXITCODE
    if($code -ne 0){$failed++}
    Write-Output "$($names[$i]): exit $code"
}
Write-Output "$($names.Count-$failed)/$($names.Count) passed"
if($failed){exit 1}
