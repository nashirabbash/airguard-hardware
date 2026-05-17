use dht_sensor::{dht22, DhtReading, Delay, InputOutputPin};
use crate::log_buffer;

pub fn read_dht22<P, E>(delay: &mut dyn Delay, pin: &mut P) -> Result<(f32, f32), String>
where
    P: InputOutputPin<E>,
    E: core::fmt::Debug,
{
    critical_section::with(|_| {
        dht22::Reading::read(delay, pin)
            .map(|r| (r.temperature, r.relative_humidity))
            .map_err(|e| format!("{:?}", e))
    })
}

pub fn read_dht22_with_log<P, E>(label: &'static str, delay: &mut dyn Delay, pin: &mut P) -> Option<(f32, f32)>
where
    P: InputOutputPin<E>,
    E: core::fmt::Debug,
{
    match read_dht22(delay, pin) {
        Ok((temp, hum)) => {
            log_buffer::log_ok(label, &format!("temp={:.1} hum={:.1}", temp, hum));
            Some((temp, hum))
        }
        Err(e) => {
            log_buffer::log_fail(label, &e);
            None
        }
    }
}
