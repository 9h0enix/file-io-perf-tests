#include "MicroStats.h"
#include <random>
#include <cassert>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

//#include "../memorymap/MMapFile.h"

template<typename Fn>
void testRunner(uint64_t iterations, Fn&& fn, MicroStats<8> & stats)
{
    uint64_t it = iterations;
    uint64_t total{0};
    
    uint64_t numitems = 100;
    std::uniform_int_distribution<uint64_t> d(0, numitems * iterations);
    std::mt19937 gen;
    std::vector<uint64_t> erand(numitems);

    while(iterations--){
        //generate 
        for(int i = 0; i < numitems; i++){
            erand[i] = d(gen);
        }

        uint64_t t0 = __rdtsc();

        fn(erand);

        uint64_t t1 = __rdtsc();

        stats.add((t1-t0));
    }
}

void testFwrite(uint64_t iterations)
{
    std::cout << __func__ << std::endl;
    char * tmp = tmpnam(nullptr);
    assert(tmp != nullptr);

    FILE * fh = fopen(tmp, "w");
    assert(fh != nullptr);

    auto fn = [&](std::vector<uint64_t> & data){
        uint64_t sz = 0;
        uint64_t numitems = data.size();
        do{
            sz += fwrite(data.data(), sizeof(uint64_t), (numitems - sz), fh);
        }while(sz != data.size());
    };

    //testfwrite(fh);
    MicroStats<8> stats;
    testRunner(iterations, fn, stats);

    std::cout << stats << std::endl;
    std::cout << "Average Cycles : " << stats.average() << std::endl;

    int rc = fclose(fh);

    rc = remove(tmp);
    assert(rc == 0);

}

void FDWrite(uint64_t iterations)
{
    std::cout << __func__ << std::endl;
    char * tmp = tmpnam(nullptr);
    assert(tmp != nullptr);

    int fd = open(tmp,  O_WRONLY | O_CREAT | O_TRUNC);
    assert(fd > 0);

    auto fn = [&](std::vector<uint64_t> & data){
        ssize_t sz = 0;
        sz += write(fd, data.data(), data.size());
    };

        //testfwrite(fh);
    MicroStats<8> stats;
    testRunner(iterations, fn, stats);

    std::cout << stats << std::endl;
    std::cout << "Average Cycles : " << stats.average() << std::endl;

    int rc = close(fd);
    assert(rc == 0);

    rc = remove(tmp);
    assert(rc == 0);
}

// void MMap(uint64_t iterations) {
//     std::cout << __func__ << std::endl;
//     char shmname[4096];
//     ::sprintf(shmname, "mmtest-%d", ::getpid());

//     MMapFile mmap;
//     bool rc = mmap.init(shmname, iterations * 100 * sizeof(uint64_t));
//     assert(rc);

//     ssize_t consumed{0};
//     auto fn = [&](std::vector<uint64_t> &data) {
//       void *ptr = mmap.data();
//       char *dst = (char *)ptr + consumed;
//       ssize_t sz = data.size() * sizeof(uint64_t);
//       memcpy(dst, data.data(), sz);
//       consumed += sz;
//     };

//     MicroStats<8> stats;
//     testRunner(iterations, fn, stats);

//     std::cout << stats << std::endl;
//     std::cout << "Average Cycles : " << stats.average() << std::endl;
// }

int main()
{    
    testFwrite(50000);
    std::cout << std::endl;
    FDWrite(50000);
    std::cout << std::endl;
    //MMap(50000);
}