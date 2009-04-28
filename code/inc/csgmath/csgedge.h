#ifndef _CSGEDGE_H
#define _CSGEDGE_H

#include "util/nstl.h"

struct edge_t {
    edge_t(long pa, long pb) : cnt(1) 
	{        
        this->set(pa, pb);
    }
    void set(long pa, long pb) {
        if (pa < pb) {
            this->a = pa; this->b = pb;
        } else {
            this->a = pb; this->b = pa;
        }
    }
    long a, b;
    long cnt;
};

bool operator==(const edge_t& x, const edge_t& y) {
    return (x.a == y.a) && (x.b == y.b);
}

bool operator < (const edge_t& x, const edge_t& y) {
    if (x.cnt == y.cnt)
        return x.a < y.a;
    else 
        return x.cnt < y.cnt;
}

class edges_v : public std::vector<edge_t> {
public:
    edges_v() : in_begin(false), open(false) {};
    void Begin() {
        assert(!this->in_begin && "error: edges not completed.");
        this->in_begin = true;
        this->open = false;
        this->clear();
    }

    void Add(const edge_t& e) {
        assert(this->in_begin && "error: edges not started.");
        
        std::vector<edge_t>::iterator i = std::find(this->begin(), this->end(), e);
        
        if (i != this->end()) {
            i->cnt++;
        } else {
            this->push_back(e);            
        }
    }

    void Add(long a, long b) {
        this->Add(edge_t(a, b));
    }    

    void End() {
        assert(this->in_begin && "error: edges not started.");
        this->in_begin = false;
        std::sort(this->begin(), this->end());
        for (std::vector<edge_t>::iterator i = this->begin(); i != this->end();i++) {
            if (i->cnt == 1) {
                this->open = true;
                break;
            }
        }
    }

    bool Open() const {
        assert(!this->in_begin && "error: edges not completed.");
        return this->open;
    }
protected:
    bool open;    
    bool in_begin;
};


#endif //_CSGEDGE_H