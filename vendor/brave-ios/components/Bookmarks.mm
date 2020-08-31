#include "brave/vendor/brave-ios/components/Bookmarks.h"

#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "base/strings/sys_string_conversions.h"
#include "brave/vendor/brave-ios/components/bookmarks/bookmarks_api.h"
#include "brave/vendor/brave-ios/components/brave_sync/brave_sync_service.h"
#include "brave/vendor/brave-ios/components/browser_state/chrome_browser_state.h"

@interface BookmarksAPI()
{
    bookmarks::BookmarksAPI* api_;
}
@end

@implementation BookmarksAPI
- (instancetype)init:(bookmarks::BookmarksAPI *)api {
    if ((self = [super init])) {
        api_ = api;
    }
    return self;
}

- (void)dealloc {
    api_ = nullptr;
}

- (void)createWithParentId:(NSUInteger)parentId index:(NSUInteger)index title:(NSString *)title url:(NSURL *)url {

    base::string16 title_;
    base::UTF8ToUTF16([title UTF8String], [title length], &title_);

//    GURL url_ = parsedUrl(url.absoluteString.UTF8String);
//    api_->Create(parentId, index, title_, url_);
}
@end


@interface BookmarksService()
{
    std::unique_ptr<ChromeBrowserState> browser_state_;
    std::unique_ptr<BraveSyncService> sync_service_;
}
@end

@implementation BookmarksService
- (instancetype)init {
    if ((self = [super init])) {
        // TODO(bridiver)
        browser_state_ = std::make_unique<ChromeBrowserState>(
            base::FilePath(kIOSChromeInitialBrowserState));
        sync_service_ = std::make_unique<BraveSyncService>(
            browser_state_.get());
    }
    return self;
}

- (void)dealloc {
    sync_service_.reset();
}

- (BookmarksAPI *)create {
    return [[BookmarksAPI alloc] init:sync_service_->bookmarks_api()];
}
@end
