use std::{env, num::NonZeroU32, sync::LazyLock, thread, time};

/// OBCT_STEP_IN_MSEC 相当
const STEP_IN_REALTIME: time::Duration = time::Duration::from_millis(1);

/// 実時間と同期する間隔 (step 数)．C2A の時刻はこの step 数ぶんまで実時間に先行する
const DEFAULT_SYNC_INTERVAL_IN_STEP: u32 = 1;

/// これ以上遅れたら，時間の回復は諦める (デバッガでの停止など)
const CATCH_UP_LIMIT: time::Duration = time::Duration::from_millis(100);

static SPEED_MULTIPLIER: LazyLock<f64> = LazyLock::new(|| {
    let multiplier = env::var("C2A_DEV_RUNTIME_SPEED")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or(1.0);

    if multiplier <= 0.0 {
        panic!(
            "C2A_DEV_RUNTIME_SPEED must be positive, got: {}",
            multiplier
        );
    }

    multiplier
});

static SYNC_INTERVAL_IN_STEP: LazyLock<u32> = LazyLock::new(|| {
    env::var("C2A_DEV_RUNTIME_SYNC_STEPS")
        .ok()
        .and_then(|v| v.parse::<NonZeroU32>().ok())
        .map(NonZeroU32::get)
        .unwrap_or(DEFAULT_SYNC_INTERVAL_IN_STEP)
});

static SYNC_INTERVAL: LazyLock<time::Duration> = LazyLock::new(|| {
    let interval = STEP_IN_REALTIME * *SYNC_INTERVAL_IN_STEP;
    interval.div_f64(*SPEED_MULTIPLIER)
});

pub fn c2a_init() {
    use c2a_core::*;

    unsafe {
        system::watchdog_timer::WDT_init();
        system::time_manager::TMGR_init(); // Time Manager
    }

    unsafe {
        C2A_core_init();
    }

    // TaskDispatcherでの大量のアノマリを避けるために、一度時刻を初期化する。
    unsafe {
        system::time_manager::TMGR_clear();
    }
    println!("C2A_init: TMGR_init done.");
}

pub fn c2a_main() {
    use c2a_core::*;

    // sleep の起床遅れが積み上がらないよう，目標時刻は前回の目標時刻から積算する
    let mut next_sync = time::Instant::now() + *SYNC_INTERVAL;

    loop {
        for _ in 0..*SYNC_INTERVAL_IN_STEP {
            unsafe {
                system::time_manager::TMGR_count_up_master_clock();
                C2A_core_main();
            }
        }

        let now = time::Instant::now();
        match next_sync.checked_duration_since(now) {
            Some(duration_to_sleep) => thread::sleep(duration_to_sleep),
            None if now - next_sync > CATCH_UP_LIMIT => next_sync = now,
            None => {}
        }
        next_sync += *SYNC_INTERVAL;
    }
}
