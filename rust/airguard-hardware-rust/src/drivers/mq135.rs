use esp_idf_hal::adc::{AdcDriver, AdcChannelDriver, AdcChannel, AdcUnit};
use esp_idf_hal::gpio::{Input, PinDriver};
use esp_idf_hal::delay::FreeRtos;
use esp_idf_hal::sys::adc_atten_t;
use crate::log_buffer;

pub fn check_mq135<'a, const A: adc_atten_t, ADCU, C>(
    label: &'static str,
    do_pin: &PinDriver<'_, Input>,
    adc: &mut AdcDriver<'a, ADCU>,
    channel: &mut AdcChannelDriver<'_, A, C>,
) -> Option<u16>
where
    ADCU: AdcUnit,
    C: AdcChannel<AdcUnit = ADCU>,
{
    let mut readings = [0u16; 10];
    for i in 0..10 {
        readings[i] = adc.read(channel).unwrap_or(0);
        if i < 9 {
            FreeRtos::delay_ms(100);
        }
    }

    let all_same = readings.windows(2).all(|w| w[0] == w[1]);
    if all_same {
        log_buffer::log_fail(label, "disconnected");
        return None;
    }

    let alarm = do_pin.is_high();
    log_buffer::log_ok(label, &format!("value={} alarm={}", readings[9], alarm));
    Some(readings[9])
}
