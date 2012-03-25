#include "../include/hashFunctions.hpp"

 unsigned int hash(const char *str) {
   unsigned int hash = 0;
   int c;

   while (c = *str++)
   hash = c + (hash << 6) + (hash << 16) - hash;
   return hash;
}         
