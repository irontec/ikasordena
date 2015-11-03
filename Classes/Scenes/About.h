//
//  About.h
//  Ikasesteka
//
//  Created by Sergio Garcia on 2/3/15.
//
//

#ifndef __IkasCocosGame__About__
#define __IkasCocosGame__About__

#include "cocos2d.h"
using namespace cocos2d;

#include "cocos-ext.h"
using namespace cocos2d::extension;


class About : public Layer, public TableViewDataSource//, public TableViewDelegate
{
public:
    virtual bool init();
    static Scene* createScene();
    CREATE_FUNC(About);
    
protected:
    void buttonPressed(Ref *sender);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
    
private:
    Label *labelAbout;
    TableView *tableViewAbout;
    
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx);
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(TableView *table);
};
#endif /* defined(__IkasCocosGame__About__) */
