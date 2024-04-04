use c2a_dev_runtime as c2a_runtime;

extern crate c2a_example_mobc_ccsds_kble;
extern crate c2a_uart_kble;
extern crate c2a_wdt_noop;

fn main() {
    c2a_runtime::c2a_init();
    c2a_runtime::c2a_main();
}
