#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED




struct NodeItem{//use this item to sort node, dist: distance to specific landmark
    int nid;
    double dist;
    bool operator < (const NodeItem& nItem) const{
        return dist < nItem.dist;
    }


};



struct MovingObjItem{//use this item to sort moving object, aggValue: aggregate function value
    int oid;
    double aggValue;
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
