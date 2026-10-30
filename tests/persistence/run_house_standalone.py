"""Compile verbatim House methods with deterministic database failure boundaries.

This checks production control flow without a live database or world fixture.
Pass --baseline-ref with the original base commit to reproduce the failures.
"""
from pathlib import Path
import argparse
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
#include <cstdint>
#include <ctime>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#include <cstdio>
namespace fmt { template<class... T> std::string format(const char* text,T&&...){return text;} }
namespace BlackTek::Console::Database { template<class... T> void Error(T&&...){} }
struct Player; using PlayerPtr=std::shared_ptr<Player>;
struct Item; using ItemPtr=std::shared_ptr<Item>; using HouseTransferItemPtr=ItemPtr;
using ItemList=std::vector<ItemPtr>;
struct ItemLocation { ItemPtr containerItem; };
struct Container { ItemList items; ItemPtr getOwner() const; const ItemList& getItemList() const{return items;} };
struct Item {
    static ItemPtr CreateItem(int){return std::make_shared<Item>();}
    bool isPickupable()const{return true;}
    std::shared_ptr<Container> getContainer()const{return nullptr;}
    ItemLocation getLocation()const{return {};}
    uint32_t getItemCount()const{return 1;}
    void setText(const std::string&){}
};
ItemPtr Container::getOwner()const{return std::make_shared<Item>();}
struct Player {
    explicit Player(std::nullptr_t){}
    uint64_t balance=100;
    uint32_t getGUID()const{return 7;}
    uint64_t getBankBalance()const{return balance;}
    void setBankBalance(uint64_t b){balance=b;}
    std::shared_ptr<Container> getInbox()const{return std::make_shared<Container>();}
};
struct Creature { PlayerPtr getPlayer()const{return nullptr;} };
struct Creatures { std::vector<std::shared_ptr<Creature>> list; int size()const{return list.size();} const auto& getList()const{return list;} };
struct Tile { std::shared_ptr<ItemList> items=std::make_shared<ItemList>(); Creatures* getCreatures()const{return nullptr;} auto getItemList()const{return items;} };
struct Bed { int getSleeper()const{return 0;} void wakeUp(std::nullptr_t){} };
struct Door { void setAccessList(const char*){} };
struct Database {
    bool querySuccess=true; int queries=0;
    static Database& getInstance(){static Database db;return db;}
    bool executeQuery(const std::string&){++queries;return querySuccess;}
};
namespace IOLoginData {
    bool loadSuccess=true,saveSuccess=true; int saves=0; uint64_t savedBalance=0;
    bool loadPlayerById(const PlayerPtr&,uint32_t){return loadSuccess;}
    bool savePlayer(const PlayerPtr& p){++saves;savedBalance=p->balance;return saveSuccess;}
    std::string getNameByGuid(uint32_t){return "owner";}
    uint32_t getAccountIdByPlayerName(const std::string&){return 1;}
}
struct ConfigManager { enum {HOUSE_RENT_PERIOD}; std::string GetString(int)const{return "never";} } g_config;
std::string asLowerCaseString(std::string s){return s;}
constexpr int INDEX_ANYWHERE=-1,FLAG_NOLIMIT=1,RETURNVALUE_NOERROR=0,SUBOWNER_LIST=1,GUEST_LIST=2,ITEM_LETTER_STAMPED=100;
enum RentPeriod_t {RENTPERIOD_NEVER,RENTPERIOD_DAILY,RENTPERIOD_WEEKLY,RENTPERIOD_MONTHLY,RENTPERIOD_YEARLY};
struct Game {
    PlayerPtr online;
    bool moveSuccess=true; int moves=0;
    struct Map {struct Towns {bool getTown(uint32_t)const{return true;}}towns;}map;
    PlayerPtr getPlayerByGUID(uint32_t)const{return online;}
    int internalMoveItem(ItemLocation,ItemLocation,int,const ItemPtr&,uint32_t,std::nullopt_t,int){++moves;return moveSuccess?RETURNVALUE_NOERROR:1;}
    void internalAddItem(ItemLocation,const ItemPtr&,int,int){}
}g_game;
struct House {
    uint32_t id=1,owner=0,townId=1,ownerAccountId=0,rentWarnings=3,rent=10;
    bool isLoaded=false; time_t paidUntil=0;
    std::string ownerName;
    int accessChanges=0,kicks=0,doorUpdates=0;
    std::vector<std::shared_ptr<Tile>> houseTiles;
    std::vector<std::shared_ptr<Bed>> bedsList;
    std::vector<std::shared_ptr<Door>> doorSet;
    HouseTransferItemPtr transferItem;
    void setOwner(uint32_t,bool=true,const PlayerPtr&=nullptr);
    bool transferToDepot()const;
    bool transferToDepot(const PlayerPtr&)const;
    bool executeTransfer(const HouseTransferItemPtr&,const PlayerPtr&);
    void kickPlayer(std::nullptr_t,const PlayerPtr&){++kicks;}
    void setAccessList(int,const char*){++accessChanges;}
    void updateDoorDescription(){++doorUpdates;}
    uint32_t getOwner()const{return owner;}
    uint32_t getId()const{return id;}
    uint32_t getRent()const{return rent;}
    uint32_t getTownId()const{return townId;}
    time_t getPaidUntil()const{return paidUntil;}
    void setPaidUntil(time_t v){paidUntil=v;}
    uint32_t getPayRentWarnings()const{return rentWarnings;}
    void setPayRentWarnings(uint32_t v){rentWarnings=v;}
    std::string getName()const{return "test";}
};
struct Houses {std::map<int,House*> houseMap;void payHouses(RentPeriod_t)const;};
'''

TESTS = r'''
int main(int argc,char** argv)
{
    if(argc!=2)return 255;
    int test=std::stoi(argv[1]),failures=0;
    auto check=[&](bool ok){if(not ok)++failures;};
    auto& db=Database::getInstance();
    House house;
    if(test==0) {
        house.owner=42;
        auto tile=std::make_shared<Tile>();tile->items->push_back(std::make_shared<Item>());house.houseTiles.push_back(tile);
        db.querySuccess=false; house.setOwner(7);
        check(house.owner==42);check(not house.isLoaded);check(house.rentWarnings==3);check(house.accessChanges==0);check(g_game.moves==0);
    } else if(test==1) {
        house.owner=42;IOLoginData::loadSuccess=false;
        Houses houses;houses.houseMap.emplace(1,&house);houses.payHouses(RENTPERIOD_DAILY);
        check(house.owner==42);check(db.queries==0);check(house.paidUntil==0);check(house.rentWarnings==3);
    } else if(test==2) {
        house.owner=42;IOLoginData::saveSuccess=false;
        check(not house.transferToDepot());check(IOLoginData::saves==1);
    } else if(test==3) {
        house.owner=42;g_game.moveSuccess=false;
        auto tile=std::make_shared<Tile>();tile->items->push_back(std::make_shared<Item>());house.houseTiles.push_back(tile);
        check(not house.transferToDepot(std::make_shared<Player>(nullptr)));
    } else if(test==4) {
        house.owner=42;IOLoginData::loadSuccess=false;house.setOwner(0,false);
        check(house.owner==42);check(house.accessChanges==0);check(house.rentWarnings==3);check(house.kicks==0);
    } else if(test==5) {
        db.querySuccess=false;house.transferItem=std::make_shared<Item>();auto transfer=house.transferItem;
        check(not house.executeTransfer(transfer,std::make_shared<Player>(nullptr)));check(house.transferItem==transfer);
    } else if(test==6) {
        house.owner=42;Houses houses;houses.houseMap.emplace(1,&house);houses.payHouses(RENTPERIOD_DAILY);
        check(house.owner==42);check(house.paidUntil>0);check(house.rentWarnings==0);check(IOLoginData::saves==1);check(IOLoginData::savedBalance==90);
    }
    std::printf("case %d: %d failures\n",test,failures);
    return failures?1:0;
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
        source = subprocess.check_output(["git", "show", f"{args.baseline_ref}:src/house.cpp"], cwd=repo, text=True)
    else:
        source = (repo / "src/house.cpp").read_text()
    functions = [extract_function(source, signature) for signature in [
        "void House::setOwner(", "bool House::transferToDepot() const",
        "bool House::transferToDepot(const PlayerPtr& player) const",
        "bool House::executeTransfer(", "void Houses::payHouses("]]
    args.output_directory.mkdir(parents=True, exist_ok=True)
    cpp = args.output_directory / "house_failure_test.cpp"
    exe = args.output_directory / "house_failure_test.exe"
    cpp.write_text(STUBS.replace("const PlayerPtr&=nullptr", "const PlayerPtr& = nullptr") + "\n" + "\n".join(functions) + TESTS)
    env = dict(os.environ, PATH=str(Path(args.compiler).resolve().parent) + os.pathsep + os.environ.get("PATH", ""))
    subprocess.run([args.compiler, "-std=c++20", "-O1", str(cpp), "-o", str(exe)], env=env, check=True)
    failures = 0
    for case in range(7):
        result = subprocess.run([str(exe), str(case)], env=env, timeout=5)
        failures += result.returncode != 0
    print(f"{7-failures}/7 production House failure-boundary cases passed")
    return bool(failures)


if __name__ == "__main__":
    raise SystemExit(main())
