#[doc = "FIXME: watchdog_timer.h が単体で bindgen 不可能だったので自前定義したもの"]
pub mod watchdog_timer {
    extern "C" {
        pub fn WDT_init();

        pub fn WDT_clear_wdt();
    }
}

#[doc = "FIXME: time_manager.h が単体で bindgen 不可能だったので自前定義したもの"]
pub mod time_manager {
    extern "C" {
        pub fn TMGR_init();

        pub fn TMGR_clear();

        pub fn TMGR_count_up_master_clock();
    }
}
