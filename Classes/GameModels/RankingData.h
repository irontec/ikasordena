//
//  Ranking.h
//  IkasIkusi
//
//  Created by Asier Cortes on 24/4/15.
//
//

#ifndef IkasIkusi_RankingData_h
#define IkasIkusi_RankingData_h

#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

class RankingData : public Object
{
public:
    
    RankingData(){};
    ~RankingData(){};
    
    CC_SYNTHESIZE(string, m_users, user);
    CC_SYNTHESIZE(int, m_points, points);
//    CC_SYNTHESIZE(int, m_levels, levels);
};

#endif
