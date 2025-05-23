// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_CHROME_BROWSER_UI_MAIN_BROWSER_VIEW_WRANGLER_H_
#define IOS_CHROME_BROWSER_UI_MAIN_BROWSER_VIEW_WRANGLER_H_

#import <UIKit/UIKit.h>

#include "ios/chrome/app/application_mode.h"
#import "ios/chrome/browser/main/browser_provider_interface.h"

@protocol ApplicationCommands;
@protocol BrowsingDataCommands;
class Browser;
class ChromeBrowserState;
@class SceneState;
@class WrangledBrowser;

// Wrangler (a class in need of further refactoring) for handling the creation
// and ownership of Browser instances and their associated
// BrowserViewControllers.
@interface BrowserViewWrangler : NSObject <BrowserProviderInterface>

// Initialize a new instance of this class using `browserState` as the primary
// browser state for the tab models and BVCs.
// `sceneState` is the scene state that will be associated with any Browsers
// created.
// `applicationCommandEndpoint` and `browsingDataCommandEndpoint` are the
// objects that methods in the ApplicationCommands and BrowsingDataCommands
// protocol should be dispatched to.
- (instancetype)initWithBrowserState:(ChromeBrowserState*)browserState
                          sceneState:(SceneState*)sceneState
          applicationCommandEndpoint:
              (id<ApplicationCommands>)applicationCommandEndpoint
         browsingDataCommandEndpoint:
             (id<BrowsingDataCommands>)browsingDataCommandEndpoint
    NS_DESIGNATED_INITIALIZER;

- (instancetype)init NS_UNAVAILABLE;

// One interface must be designated as being the "current" interface.
// It's typically an error to assign this an interface which is neither of
// mainInterface` or `incognitoInterface`. The initial value of
// `currentInterface` is an implementation decision, but `mainInterface` is
// typical.
// Changing this value may or may not trigger actual UI changes, or may just be
// bookkeeping associated with UI changes handled elsewhere.
@property(nonatomic, weak) WrangledBrowser* currentInterface;
// The "main" (meaning non-incognito -- the nomenclature is legacy) interface.
// This interface's `incognito` property is expected to be NO.
@property(nonatomic, readonly) WrangledBrowser* mainInterface;
// The incognito interface. Its `incognito` property must be YES.
@property(nonatomic, readonly) WrangledBrowser* incognitoInterface;
// YES iff `incognitoInterface` is already created.
@property(nonatomic, assign, readonly) BOOL hasIncognitoInterface;

// Creates the main Browser used by the receiver, using the browser state
// it was configured with.
// Returns the created browser. The browser's internals, e.g.
// the dispatcher, can now be accessed. But createMainCoordinatorAndInterface
// should be called shortly after.
- (Browser*)createMainBrowser;

// Creates the main interface; until this
// method is called, the main and incognito interfaces will be nil. This should
// be done before the main interface is accessed, usually immediately after
// initialization.
// -createMainBrowser MUST be called before calling this method.
- (void)createMainCoordinatorAndInterface;

// Creates the inactive browser that stores all tabs from the main browser that
// have not been opened after a defined time. This browser is added to the main
// interface. -createMainBrowser and -createMainCoordinatorAndInterface MUST be
// called before calling this method.
- (void)createInactiveBrowser;

// Tells the receiver to clean up all the state that is tied to the incognito
// BrowserState. This method should be called before the incognito BrowserState
// is destroyed.
- (void)willDestroyIncognitoBrowserState;

// Tells the receiver to create all state that is tied to the incognito
// BrowserState. This method should be called after the incognito BrowserState
// has been created.
- (void)incognitoBrowserStateCreated;

// Tells the receiver to clean up prior to deallocation. It is an error for an
// instance of this class to deallocate without a call to this method first.
- (void)shutdown;

@end

#endif  // IOS_CHROME_BROWSER_UI_MAIN_BROWSER_VIEW_WRANGLER_H_
