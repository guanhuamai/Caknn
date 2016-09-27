#include "RoadNetwork.h"



size_t Edge::getOppositeNode(size_t nid){
    if(nid == snid){
        return enid;
    }
    return snid;
}

bool Edge::pasteMovObjs(std::vector<MovingObj> movOjbs){

    for(int i = 0; i < movOjbs.size(); i++){
        if(movObjsID.find(movOjbs[i].id) == movObjsID.end()){
            movObjsID.insert(movOjbs[i].id);
        }else{
            return false;
        }
    }
    return true;
}



bool Edge::eraseMovObjs(std::vector<MovingObj> movOjbs){
    for(size_t i = 0; i < movOjbs.size(); i++){
        std::set<size_t>::iterator itr = this->movObjsID.find(movOjbs[i].id);
        if(itr != this->movObjsID.end()){
            this->movObjsID.erase(itr);
        }else{
            return false;
        }
    }
    return true;
}
