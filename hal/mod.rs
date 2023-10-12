pub mod ccsds {
    include!(concat!(env!("OUT_DIR"), "/ccsds.rs"));
}
pub mod i2c {
    include!(concat!(env!("OUT_DIR"), "/i2c.rs"));
}
pub mod spi {
    include!(concat!(env!("OUT_DIR"), "/spi.rs"));
}
pub mod spwire {
    include!(concat!(env!("OUT_DIR"), "/spwire.rs"));
}
pub mod uart {
    include!(concat!(env!("OUT_DIR"), "/uart.rs"));
}
pub mod wdt {
    include!(concat!(env!("OUT_DIR"), "/wdt.rs"));
}
