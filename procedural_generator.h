#pragma once
#include  <cstdint>

namespace  Procedural_Components {

//    every tile and object needs a seed
    struct Seed {
        uint32_t seed;
    };
}


namespace Procedural_Generation {

    uint32_t Create_Initial_Seed(uint32_t x, uint32_t y)
    {
        return (x & 0xFFFF) << 16 | (y & 0xFFFF);
    }

    uint32_t Generate_Random_Number(uint32_t &seed)
    {
        seed += 0xe120fc15;
        uint32_t tmp;
        tmp = (uint32_t)seed * 0x4a39b70d;
        uint32_t m1 = (tmp >> 16) ^ tmp;
        tmp = (uint32_t)m1 * 0x12fad5c9;
        uint32_t m2 = (tmp >> 16) ^ tmp;
        return m2;
    }

    int Random_Int(int min, int max, Procedural_Components::Seed seed)
    {
        return (Generate_Random_Number(seed.seed) % (max - min)) + min;
    }

    double Random_Int(double min, double max, Procedural_Components::Seed seed)
    {
        return ((double)Generate_Random_Number(seed.seed) / (double)(0x7FFFFFFF) * (max - min)) + min;
    }


};