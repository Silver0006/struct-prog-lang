#include <nanobind/nanobind.h>

int addwow(int first, int second){
    return first+second;
}

// m is the module object
NB_MODULE(parserCPP, m){
    m.def("addwow", &addwow);
}
