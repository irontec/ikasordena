//
//  Ranking.h
//  IkasIkusi
//
//  Created by Asier Cortes on 23/4/15.
//
//

#ifndef __IkasIkusi__Ranking__
#define __IkasIkusi__Ranking__

#include <stdio.h>
#include "../Constants/Constants.h"
#include "../Helpers/IkasAPI.h"
#include "../GameModels/RankingData.h"

#include "../../cocos2d/extensions/GUI/CCScrollView/CCTableView.h"

#include "cocos2d.h"
using namespace cocos2d;

class Ranking : public Layer, TableViewDataSource
{
public:
    virtual bool init();
    static Scene* createScene();
    
    void successRankingDataDownloaded(bool result, vector<RankingData*> data);
    void errorDataDownloaded(Ref* sender);

    
    CREATE_FUNC(Ranking);
    
protected:
    void switchSoundSettings(Ref* sender);
    void changeScene(Ref* sender);
    void openInfo(Ref* sender);
    
private:
//    Layer *_userRankingLayer, *_pointsRankingLayer, *_levelRankingLayer;
    Layer *_userRankingLayer, *_pointsRankingLayer;
    IkasAPI *_api;
    void showLoading(bool show);
    void checkNewData();
    void downloadLatestData(const IkasHandler& successCallback, const IkasHandler& errorCallback, const IkasValueHandler progressCallback);
    void createUsers(float dt);
    void returnHome(Ref* sender);
    
    vector<RankingData*> _rankingData;
//    vector<Label*> _users, _points, _levels;
    vector<Label*> _users, _points;
    Label * _labelLoading;
    int _totalLabels;
    
    Layer *buttonsLayer;
    TableView *_tableViewRanking;
    
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
};



#endif /* defined(__IkasIkusi__Ranking__) */
