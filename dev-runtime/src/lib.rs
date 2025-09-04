use std::{env, sync::LazyLock, thread, time};

const UPDATE_BATCH_SIZE: usize = 100;
const DEFAULT_UPDATE_BATCH_IN_REALTIME: time::Duration = time::Duration::from_millis(100);

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

static UPDATE_INTERVAL: LazyLock<time::Duration> = LazyLock::new(|| {
    let base_millis = DEFAULT_UPDATE_BATCH_IN_REALTIME.as_millis() as f64;
    let adjusted_millis = (base_millis / *SPEED_MULTIPLIER).max(1.0) as u64;
    time::Duration::from_millis(adjusted_millis)
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

    loop {
        let start = time::Instant::now();
        for _ in 0..UPDATE_BATCH_SIZE {
            unsafe {
                system::time_manager::TMGR_count_up_master_clock();
                C2A_core_main();
            }
        }
        let elapsed_time = start.elapsed();
        if elapsed_time < *UPDATE_INTERVAL {
            let duration_to_sleep = *UPDATE_INTERVAL - elapsed_time;
            thread::sleep(duration_to_sleep);
        }
        // FIXME: 時間がめっちゃ遅れてたときの時間回復は一旦しない
    }
}
