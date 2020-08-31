#import "brave/ios/web/brave_webmain_runner.h"

#include "base/i18n/icu_util.h"
#include "base/logging.h"
#include "base/macros.h"
#include "ios/web/init/web_main_loop.h"
#include "ios/web/public/init/ios_global_state.h"
#include "ios/web/public/navigation/url_schemes.h"
#import "ios/web/public/web_client.h"
#include "ios/web/web_thread_impl.h"
#include "mojo/core/embedder/embedder.h"
#include "ui/base/ui_base_paths.h"

#if !defined(__has_feature) || !__has_feature(objc_arc)
#error "This file requires ARC support."
#endif

namespace web {

class BraveWebMainRunnerImpl : public WebMainRunner {
 public:
  BraveWebMainRunnerImpl()
      : is_initialized_(false),
        is_shutdown_(false),
        completed_basic_startup_(false),
        delegate_(nullptr) {}

  ~BraveWebMainRunnerImpl() override {
    if (is_initialized_ && !is_shutdown_) {
      ShutDown();
    }
  }

  int Initialize(WebMainParams params) override {
    ////////////////////////////////////////////////////////////////////////
    // ContentMainRunnerImpl::Initialize()
    //
    is_initialized_ = true;
    delegate_ = params.delegate;

    //Added by Brandon to prevent freezing when multiple ThreadPools are created.
    //There can only be a single thread-pool ever.. however, the class is NOT a singleton!
    //And it ends up deallocating a global variable which calls "join" in the destructor.. thereby causing a freeze!
    if (!base::ThreadPoolInstance::Get()) {
        ios_global_state::CreateParams create_params;
        create_params.install_at_exit_manager = params.register_exit_manager;
        create_params.argc = params.argc;
        create_params.argv = params.argv;
        ios_global_state::Create(create_params);
    }
      
    web::WebThreadImpl::CreateTaskExecutor();

    if (delegate_) {
      delegate_->BasicStartupComplete();
    }
    completed_basic_startup_ = true;

    mojo::core::Init();

    // TODO(crbug.com/965894): Should we instead require that all embedders call
    // SetWebClient()?
    if (!GetWebClient())
      SetWebClient(&empty_web_client_);

    RegisterWebSchemes();
    ui::RegisterPathProvider();

    CHECK(base::i18n::InitializeICU());

    ////////////////////////////////////////////////////////////
    //  BrowserMainRunnerImpl::Initialize()

    main_loop_.reset(new WebMainLoop());
    main_loop_->Init();
    main_loop_->EarlyInitialization();
    main_loop_->MainMessageLoopStart();
    main_loop_->CreateStartupTasks();
      fprintf(stderr, "STARTUP TASKS\n");
    int result_code = main_loop_->GetResultCode();
      fprintf(stderr, "RESULT CODE\n");
    if (result_code > 0)
      return result_code;

    // Return -1 to indicate no early termination.
    return -1;
  }

  void ShutDown() override {
    ////////////////////////////////////////////////////////////////////
    // BrowserMainRunner::Shutdown()
    //
    DCHECK(is_initialized_);
    DCHECK(!is_shutdown_);
    main_loop_->ShutdownThreadsAndCleanUp();
    main_loop_.reset(nullptr);

    ////////////////////////////////////////////////////////////////////
    // ContentMainRunner::Shutdown()
    //
    if (completed_basic_startup_ && delegate_) {
      delegate_->ProcessExiting();
    }

    ios_global_state::DestroyAtExitManager();

    delegate_ = nullptr;
    is_shutdown_ = true;
  }

 protected:
  // True if we have started to initialize the runner.
  bool is_initialized_;

  // True if the runner has been shut down.
  bool is_shutdown_;

  // True if basic startup was completed.
  bool completed_basic_startup_;

  // The delegate will outlive this object.
  WebMainDelegate* delegate_;

  // Used if the embedder doesn't set one.
  WebClient empty_web_client_;

  std::unique_ptr<WebMainLoop> main_loop_;

  DISALLOW_COPY_AND_ASSIGN(BraveWebMainRunnerImpl);
};

// static
WebMainRunner* BraveWebMainRunner::Create() {
  return new BraveWebMainRunnerImpl();
}

}
