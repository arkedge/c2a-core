use std::{thread, time};

const UPDATE_BATCH_SIZE: usize = 100;
const UPDATE_BATCH_IN_REALTIME: time::Duration = time::Duration::from_millis(100);

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
        if elapsed_time < UPDATE_BATCH_IN_REALTIME {
            let duration_to_sleep = UPDATE_BATCH_IN_REALTIME - elapsed_time;
            thread::sleep(duration_to_sleep);
        }
        // FIXME: 時間がめっちゃ遅れてたときの時間回復は一旦しない
    }
}
