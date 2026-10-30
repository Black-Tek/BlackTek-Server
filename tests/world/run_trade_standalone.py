"""Exercise the complete production trade function with controlled move failures.

The move boundary tracks item ownership and can replace the moved stack pointer.
This is a control-flow regression harness, not a live Lua or client integration test.
"""
from pathlib import Path
import argparse
import json
import os
import subprocess


def extract_function(source, signature):
    start = source.index(signature)
    opening = source.index("{", start)
    depth, end = 1, opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end]


STUBS = r'''
#include <array>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
struct Player;struct Item;struct Container;
using PlayerPtr=std::shared_ptr<Player>;
using CreaturePtr=PlayerPtr;
using ItemPtr=std::shared_ptr<Item>;
namespace BlackTek {
struct ItemLocation {
    PlayerPtr player;ItemPtr containerItem;
    explicit operator bool()const{return player or containerItem;}
    friend bool operator==(const ItemLocation&,const ItemLocation&)=default;
};
namespace Console {template<class... T>void Error(T&&...){} }
}
enum ReturnValue {RETURNVALUE_NOERROR,RETURNVALUE_NOTENOUGHROOM,RETURNVALUE_NOTPOSSIBLE,RETURNVALUE_CANNOTTHROW};
enum TradeState {TRADE_NONE,TRADE_ACKNOWLEDGE,TRADE_INITIATED,TRADE_ACCEPT,TRADE_TRANSFER};
enum {INDEX_ANYWHERE=-1,CONST_SLOT_BACKPACK=3,MESSAGE_EVENT_ADVANCE=1,ON_TRADE_TRANSFER=1,ON_TRADE_CANCEL=2};
constexpr uint32_t FLAG_IGNOREAUTOSTACK=1,FLAG_NOLIMIT=2,FLAG_IGNORENOTMOVEABLE=4;
struct Position {};
struct Item {
    static inline std::array<int,100> items{};
    uint16_t id=1;uint32_t count=10;
    bool removed=false;int transfers=0,cancels=0;
    std::weak_ptr<Player> owner;
    std::weak_ptr<Item> parent;
    std::shared_ptr<Container> container;
    uint16_t getID()const{return id;}
    uint32_t getItemCount()const{return count;}
    bool isRemoved()const{return removed;}
    BlackTek::ItemLocation getLocation()const{return {.player=owner.lock(),.containerItem=parent.lock()};}
    void locate(const BlackTek::ItemLocation& where){owner=where.player;parent=where.containerItem;removed=false;}
    auto getContainer()const{return container;}
    void onTradeEvent(int kind,const PlayerPtr&){if(kind==ON_TRADE_TRANSFER)++transfers;else ++cancels;}
};
struct Container {
    ItemPtr owner;
    ItemPtr getOwner()const{return owner;}
    int32_t getItemIndex(const ItemPtr&)const{return 4;}
};
struct Player {
    uint32_t id=1;TradeState state=TRADE_INITIATED;
    PlayerPtr tradePartner;ItemPtr tradeItem;
    std::shared_ptr<Container> inbox=std::make_shared<Container>();
    int closes=0;std::vector<std::string> messages;
    TradeState getTradeState()const{return state;}
    void setTradeState(TradeState s){state=s;}
    Position getPosition()const{return {};}
    uint32_t getGUID()const{return id;}
    ItemPtr getInventoryItem(int)const{return nullptr;}
    int32_t getItemSlotIndex(const ItemPtr&)const{return 6;}
    auto getInbox()const{return inbox;}
    void sendTradeClose(){++closes;}
    void sendCancelMessage(ReturnValue){}
    void sendTextMessage(int,const std::string& text){messages.push_back(text);}
};
int getSlotType(int){return 0;}
struct Events {
    int accepts=0,completions=0;bool completedSuccess=false;
    bool eventPlayerOnTradeAccept(const PlayerPtr&,const PlayerPtr&,const ItemPtr&,const ItemPtr&){++accepts;return true;}
    void eventPlayerOnTradeCompleted(const PlayerPtr&,const PlayerPtr&,const ItemPtr&,const ItemPtr&,bool success){++completions;completedSuccess=success;}
}events;
Events* g_events=&events;
struct Game {
    PlayerPtr first=std::make_shared<Player>(),second=std::make_shared<Player>();
    ItemPtr firstItem=std::make_shared<Item>(),secondItem=std::make_shared<Item>(),movedItem;
    BlackTek::ItemLocation originalLocation;
    std::unordered_map<ItemPtr,uint32_t> tradeItems;
    int scenario=0,moves=0,boundaryFailures=0;
    void check(bool ok,const char* detail){if(not ok){++boundaryFailures;std::printf("boundary: %s\n",detail);}}
    PlayerPtr getPlayerByID(uint32_t id)const{return id==first->id?first:nullptr;}
    bool canThrowObjectTo(Position,Position,bool,bool)const{return true;}
    void internalCloseTrade(const PlayerPtr&,bool){}
    ReturnValue internalAddItem(BlackTek::ItemLocation,const ItemPtr&,int,uint32_t,bool){return RETURNVALUE_NOERROR;}
    ReturnValue internalRemoveItem(const ItemPtr&,uint32_t,bool){return RETURNVALUE_NOERROR;}
    std::string getTradeErrorDescription(ReturnValue,const ItemPtr&)const{return "trade failed";}
    ReturnValue internalMoveItem(BlackTek::ItemLocation from,BlackTek::ItemLocation to,int32_t index,
        ItemPtr item,uint32_t count,std::optional<std::reference_wrapper<ItemPtr>> output,uint32_t flags=0,
        CreaturePtr actor=nullptr,ItemPtr other=nullptr)
    {
        ++moves;
        check(item and not item->removed,"move must use the live item pointer");
        if(not item or item->removed)return RETURNVALUE_NOTPOSSIBLE;
        check(from==item->getLocation(),"move source must match current ownership");
        check(count==item->count,"move must preserve the full trade count");
        if(moves==1) {
            check(to.player==second,"first move must go to partner");
            if(scenario==1 or scenario==2) {
                movedItem=std::make_shared<Item>();movedItem->id=item->id;movedItem->count=count;
                item->removed=true;item->owner.reset();item->parent.reset();
            } else movedItem=item;
            movedItem->locate(to);
            if(output)output->get()=movedItem;
            return RETURNVALUE_NOERROR;
        }
        if(moves==2) {
            check(item==secondItem and to.player==first,"second move must transfer partner's own item");
            if(scenario!=0)return RETURNVALUE_NOTENOUGHROOM;
            item->locate(to);return RETURNVALUE_NOERROR;
        }
        constexpr uint32_t recovery=FLAG_NOLIMIT|FLAG_IGNOREAUTOSTACK|FLAG_IGNORENOTMOVEABLE;
        check(item==movedItem,"rollback must use actual moved pointer");
        check(flags==recovery,"rollback must use the full recovery flags");
        if(moves==3) {
            check(to==originalLocation,"rollback must return to the original location");
            check(index==(originalLocation.player?6:4),"rollback must preserve the source index");
            if(scenario==2)return RETURNVALUE_NOTPOSSIBLE;
        } else if(moves==4) {
            check(scenario==2,"inbox rescue only after normal rollback refusal");
            check(to.containerItem==first->inbox->owner and not to.player,"rescue must go to the original owner's inbox");
            check(index==INDEX_ANYWHERE,"inbox rescue must use any available slot");
        } else check(false,"unexpected extra move");
        item->locate(to);return RETURNVALUE_NOERROR;
    }
    void playerAcceptTrade(uint32_t);
};
'''

TESTS = r'''
int main(int argc,char** argv)
{
    if(argc!=2)return 255;
    Game game;game.scenario=std::stoi(argv[1]);
    auto a=game.first,b=game.second;
    a->id=1;b->id=2;a->state=TRADE_INITIATED;b->state=TRADE_ACCEPT;
    a->tradePartner=b;b->tradePartner=a;
    a->tradeItem=game.firstItem;b->tradeItem=game.secondItem;
    game.firstItem->id=1;game.secondItem->id=2;
    game.firstItem->locate({.player=a});game.secondItem->locate({.player=b});
    a->inbox->owner=std::make_shared<Item>();a->inbox->owner->id=90;a->inbox->owner->locate({.player=a});
    ItemPtr bag;
    if(game.scenario==2) {
        bag=std::make_shared<Item>();bag->container=std::make_shared<Container>();bag->locate({.player=a});
        game.firstItem->locate({.containerItem=bag});
    }
    game.originalLocation=game.firstItem->getLocation();
    game.tradeItems.emplace(game.firstItem,1);game.tradeItems.emplace(game.secondItem,2);
    if(game.scenario==3)b->tradeItem=nullptr;
    game.playerAcceptTrade(1);
    auto check=[&](bool ok,const char* detail){game.check(ok,detail);};
    if(game.scenario==3) {
        check(game.moves==0 and events.accepts==0 and events.completions==0,"missing offer must be rejected before any callbacks or moves");
        check(a->state==TRADE_INITIATED,"missing offer must not advance the trade state");
    } else {
        check(events.completions==1,"one completion callback required");
        check(a->state==TRADE_NONE and b->state==TRADE_NONE,"trade states must close");
        check(not a->tradeItem and not b->tradeItem and not a->tradePartner and not b->tradePartner,"trade handles must clear");
        check(game.tradeItems.empty(),"trade locks must release");
        check(a->closes==1 and b->closes==1,"both players receive trade close");
        check(game.movedItem and game.movedItem->count==10 and game.secondItem->count==10,"no item count loss");
        if(game.scenario==0) {
            check(game.moves==2 and events.completedSuccess,"successful trade completes successfully");
            check(game.movedItem->getLocation().player==b and game.secondItem->getLocation().player==a,"successful trade swaps owners");
            check(game.firstItem->transfers==1 and game.secondItem->transfers==1,"successful trade emits both transfer callbacks");
            check(game.firstItem->cancels==0 and game.secondItem->cancels==0,"successful trade has no cancellation callbacks");
        } else {
            check(not events.completedSuccess,"second move refusal must report unsuccessful completion");
            check(game.firstItem->transfers==0 and game.secondItem->transfers==0 and game.movedItem->transfers==0,"failed trade must not emit transfer callbacks");
            check(game.firstItem->cancels==1 and game.secondItem->cancels==1,"failed trade cancels both offers");
            check(game.secondItem->getLocation().player==b,"partner keeps its original item");
            check(game.firstItem->removed and not game.movedItem->removed,"replaced original stack must not duplicate the recovered item");
            if(game.scenario==1) {
                check(game.moves==3 and game.movedItem->getLocation()==game.originalLocation,"normal rollback restores original owner and slot");
            } else {
                check(game.moves==4 and game.movedItem->getLocation().containerItem==a->inbox->owner,"failed normal rollback rescues the item into its owner's inbox");
                bool notified=false;for(const auto& msg:a->messages)if(msg.find("inbox")!=std::string::npos)notified=true;
                check(notified,"inbox recovery informs the original owner");
            }
        }
    }
    a->tradePartner.reset();b->tradePartner.reset();
    std::printf("case %d: %d failures\n",game.scenario,game.boundaryFailures);
    return game.boundaryFailures?1:0;
}
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", required=True)
    parser.add_argument("--output-directory", required=True, type=Path)
    parser.add_argument("--baseline-ref")
    args = parser.parse_args()
    repo = Path(__file__).resolve().parents[2]
    if args.baseline_ref:
        source = subprocess.check_output(["git", "show", f"{args.baseline_ref}:src/game.cpp"], cwd=repo, text=True)
    else:
        source = (repo / "src/game.cpp").read_text()
    function = extract_function(source, "void Game::playerAcceptTrade(")
    args.output_directory.mkdir(parents=True, exist_ok=True)
    cpp = args.output_directory / "trade_failure_test.cpp"
    exe = args.output_directory / "trade_failure_test.exe"
    cpp.write_text(STUBS + function + TESTS)
    env = dict(os.environ, PATH=str(Path(args.compiler).resolve().parent) + os.pathsep + os.environ.get("PATH", ""))
    subprocess.run([args.compiler, "-std=c++20", "-O1", str(cpp), "-o", str(exe)], env=env, check=True)
    results = []
    for case, name in enumerate(["successful_trade", "second_move_refused_rollback", "rollback_refused_inbox_recovery", "missing_offer"]):
        result = subprocess.run([str(exe), str(case)], env=env, timeout=5, capture_output=True, text=True)
        results.append(dict(case=name, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr))
        print(name, result.returncode, result.stdout.strip())
    (args.output_directory / "results.json").write_text(json.dumps(results, indent=2))
    failures = sum(result["returncode"] != 0 for result in results)
    print(f"{len(results)-failures}/{len(results)} production trade cases passed")
    return bool(failures)


if __name__ == "__main__":
    raise SystemExit(main())
