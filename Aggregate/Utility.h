#include <limits>

#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#define DBL_ERROR 0.0001
#define DBL_MAX std::numeric_limits<double>::max()

struct NodeItem{//use this item to sort node, dist: distance to specific landmark
    size_t nid;
    double dist;

    NodeItem(size_t nid, double dist){
        this->nid = nid;
        this->dist = dist;
    }

    bool operator < (const NodeItem& nItem) const{
        return dist < nItem.dist;
    }



};



struct MovingObjItem{//use this item to sort moving object, aggValue: aggregate function value
    size_t oid;
    double aggValue;

    MovingObjItem(){
        this->oid = (size_t) -1;
        this->aggValue = DBL_MAX;
    }

    MovingObjItem(size_t oid, double aggValue){
        this->oid = oid;
        this->aggValue = aggValue;
    }

    bool operator < (const MovingObjItem& objItem) const{
        if(aggValue > objItem.aggValue){//dangenrous: use aggValue as key, easy to lose item in set, please syncronize 'rslts' and 'movingObjs' carefully;
            return true;
        }else if(aggValue > objItem.aggValue){
            return false;
        }
        return oid < objItem.oid;
    }
};

#endif // UTILITY_H_INCLUDED
