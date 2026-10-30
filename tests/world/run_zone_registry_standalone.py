"""Check verbatim zone iteration methods against callback registry mutations.

GCC checked iterators deterministically detect the original vector invalidation.
The pool boundary tracks live handles; a small real coroutine advances supervision
one cycle at a time. No game, scheduler thread, or Lua runtime is started.
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
#include <chrono>
#include <coroutine>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>
struct CoroTask {
    struct promise_type {
        CoroTask get_return_object(){return CoroTask{std::coroutine_handle<promise_type>::from_promise(*this)};}
        std::suspend_always initial_suspend()const noexcept{return {};}
        std::suspend_always final_suspend()const noexcept{return {};}
        void return_void()const noexcept{}
        void unhandled_exception()const{std::terminate();}
    };
    std::coroutine_handle<promise_type> handle;
    explicit CoroTask(std::coroutine_handle<promise_type> h):handle(h){}
    CoroTask(const CoroTask&)=delete;
    CoroTask(CoroTask&& other):handle(std::exchange(other.handle,{})){}
    ~CoroTask(){if(handle)handle.destroy();}
    void resume(){handle.resume();}
};
struct SleepFor {
    uint32_t duration;
    bool await_ready()const noexcept{return false;}
    void await_suspend(std::coroutine_handle<>)const noexcept{}
    void await_resume()const noexcept{}
};
namespace Zones {
using ZoneHandle=uint32_t;
enum class SpawnType {None,Monster};
struct Zone {
    SpawnType entity_type=SpawnType::Monster;
    bool active=false;
    int runs=0,activations=0;
    std::function<void()> onRun,onActivate;
    void Run(){++runs;if(onRun)onRun();}
    bool ActiveToday(std::chrono::weekday)const{return true;}
    bool Active()const{return active;}
    void Activate(){active=true;++activations;if(onActivate)onActivate();}
    void Deactivate(){active=false;}
};
struct Pool {
    std::map<ZoneHandle,Zone> live;
    Zone* TryGetMutable(ZoneHandle handle){auto it=live.find(handle);return it==live.end()?nullptr:&it->second;}
}zone_pool;
std::vector<ZoneHandle> zone_registry;
struct ZoneManager {static void ActivateAll();static CoroTask Supervise();};
'''

TESTS = r'''
}
int main(int argc,char** argv)
{
    if(argc!=2)return 255;
    using namespace Zones;
    const int scenario=std::stoi(argv[1]);
    const bool reload=scenario%2==1;
    const bool supervise=scenario>=2;
    zone_pool.live.try_emplace(1);zone_pool.live.try_emplace(2);
    zone_registry={1,2};zone_registry.shrink_to_fit();
    bool mutated=false;
    auto mutate=[&] {
        if(mutated)return;
        mutated=true;
        if(reload) {
            zone_registry.clear();
            zone_pool.live.erase(2);
        }
        zone_pool.live.try_emplace(3);
        zone_registry.push_back(3);
    };
    if(supervise)zone_pool.live.at(1).onActivate=mutate;
    else zone_pool.live.at(1).onRun=mutate;
    int failures=0;
    auto check=[&](bool ok,const char* message){if(not ok){++failures;std::printf("failure: %s\n",message);}};
    auto checkFirstCycle=[&] {
        check(mutated,"callback must mutate the live registry");
        const auto& first=zone_pool.live.at(1);
        const auto& added=zone_pool.live.at(3);
        check((supervise?first.activations:first.runs)==1,"first original zone executes once");
        check((supervise?added.activations:added.runs)==0,"new zone must wait until the next cycle");
        if(reload)check(not zone_pool.TryGetMutable(2),"deleted snapshot handle is no longer live");
        else {
            const auto& second=zone_pool.live.at(2);
            check((supervise?second.activations:second.runs)==1,"original second live zone must still execute");
        }
    };
    if(supervise) {
        auto task=ZoneManager::Supervise();
        task.resume();checkFirstCycle();
        task.resume();
        check(zone_pool.live.at(3).activations==1,"next supervision cycle activates the new zone");
    } else {
        ZoneManager::ActivateAll();checkFirstCycle();
        ZoneManager::ActivateAll();
        check(zone_pool.live.at(3).runs==1,"next activation cycle runs the new zone");
    }
    std::printf("case %d: %d failures\n",scenario,failures);
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
        source = subprocess.check_output(["git", "show", f"{args.baseline_ref}:src/zones.cpp"], cwd=repo, text=True)
    else:
        source = (repo / "src/zones.cpp").read_text()
    methods = [extract_function(source, signature) for signature in [
        "void ZoneManager::ActivateAll()", "CoroTask ZoneManager::Supervise()"]]
    args.output_directory.mkdir(parents=True, exist_ok=True)
    cpp = args.output_directory / "zone_registry_test.cpp"
    exe = args.output_directory / "zone_registry_test.exe"
    cpp.write_text(STUBS + "\n".join(methods) + TESTS)
    env = dict(os.environ, PATH=str(Path(args.compiler).resolve().parent) + os.pathsep + os.environ.get("PATH", ""))
    subprocess.run([args.compiler, "-std=c++20", "-O1", "-D_GLIBCXX_DEBUG", str(cpp), "-o", str(exe)], env=env, check=True)
    results = []
    for case, name in enumerate(["activate_add", "activate_reload", "supervise_add", "supervise_reload"]):
        result = subprocess.run([str(exe), str(case)], env=env, timeout=5, capture_output=True, text=True)
        results.append(dict(case=name, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr))
        print(name, result.returncode, result.stdout.strip(), result.stderr[-1200:].strip())
    (args.output_directory / "results.json").write_text(json.dumps(results, indent=2))
    failures = sum(result["returncode"] != 0 for result in results)
    print(f"{len(results)-failures}/{len(results)} production zone iteration cases passed")
    return bool(failures)


if __name__ == "__main__":
    raise SystemExit(main())
