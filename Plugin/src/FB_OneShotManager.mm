
/**********************************************************\
 Original Author: Georg Fritzsche
 
 Created:    Nov 6, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche and the Firebreath development team
 \**********************************************************/


#include "npapi.h"
#include "npfunctions.h"
#include "mthreads.h"
#include "FB_OneShotManager.h"
#import <Cocoa/Cocoa.h>
//#include <iostream>
//#include <fstream>
//#include <cassert>

@interface OneShotManagerHelper : NSObject {
    OneShotManager* manager_;
}

- (id)initWithManager:(OneShotManager *)manager;
- (void)dealloc;

- (void)disconnect;
- (void)doStuff:(id)obj;
@end

@implementation OneShotManagerHelper

- (id)initWithManager:(OneShotManager *)manager
{
    if ((self = [super init])) 
    {
        manager_ = manager;
    }
    
    return self;
}

- (void)dealloc
{
    manager_ = 0;
    [super dealloc];
}

- (void)disconnect
{
    manager_ = 0;
}

- (void)doStuff:(id)obj
{
    if (manager_) {
        manager_->shoot();
    }
}

@end

namespace 
{
    OneShotManagerHelper *getHelper(OneShotManager *manager)
    {
        static OneShotManagerHelper *helper = [[OneShotManagerHelper alloc] initWithManager:manager];
        return helper;
    }
}

OneShotManager::OneShotManager()
{
    
}

OneShotManager::~OneShotManager()
{
    [getHelper(this) disconnect];
}

void OneShotManager::push(void* npp, OneShotCallback sink)
{
    AutoLock lock(mLock);
    m_sinks.push(std::make_pair(npp, sink));
    [getHelper(this) performSelectorOnMainThread:@selector(doStuff:) withObject:nil waitUntilDone:NO];
}

void OneShotManager::clear(void* npp)
{
	// This and push must never be called at the same time; thus, the mutex prevents that
	// from happening.
        AutoLock lock(mLock);
        SinkQueue tmp;
	SinkPair cur;
	// Go through all items in the queue; add all but those
	// associated with npp to a temporary queue
	while (m_sinks.try_pop(cur)) {
                AsyncParam *param = (AsyncParam*) cur.first;
//		if (cur.first != npp) {
		if (param->npp != npp) {
			tmp.push(cur);
		} else {
                        AsyncParamFree(param);
                }
	}
	// Add the items we kept back into the queue
	while (tmp.try_pop(cur)) {
		m_sinks.push(cur);
	}
}

OneShotManager& OneShotManager::getInstance()
{
    static OneShotManager manager;
    return manager;
}

void OneShotManager::shoot()
{
    SinkPair val;
    while (m_sinks.try_pop(val))
    {
//        val.second(val.first, 0);
        val.second(val.first);
    }
}
