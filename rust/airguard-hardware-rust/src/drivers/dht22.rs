use esp_idf_svc::hal::gpio::{InputPin, OutputPin};
use crate::log_buffer;

pub fn read_dht22<P>(_pin: P) -> Result<(f32, f32), String>
where
    P: InputPin + OutputPin,
{
    critical_section::with(|_| {
        Err("DHT22 sensor read pending dht-sensor v0.2 API resolution".to_string())
    })
}

pub fn read_dht22_with_log<P>(label: &'static str, pin: P) -> Option<(f32, f32)>
where
    P: InputPin + OutputPin,
{
    match read_dht22(pin) {
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
