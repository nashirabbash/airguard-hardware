use esp_idf_hal::adc::{AdcChannel, AdcChannelDriver, AdcDriver, AdcUnit};
use esp_idf_hal::adc::attenuation;
use esp_idf_hal::delay::FreeRtos;
use esp_idf_hal::gpio::{Input, InputOutput, Output, PinDriver};
use esp_idf_hal::sys::adc_atten_t;
use crate::{config, drivers, ws};

pub fn run_forever<ADCU, const ATT: adc_atten_t, AO1, AO2>(
    dht1: &mut PinDriver<'_, InputOutput>,
    dht2: &mut PinDriver<'_, InputOutput>,
    do1: &PinDriver<'_, Input>,
    do2: &PinDriver<'_, Input>,
    led: &mut PinDriver<'_, Output>,
    adc: &mut AdcDriver<'_, ADCU>,
    ao1: &mut AdcChannelDriver<'_, ATT, AO1>,
    ao2: &mut AdcChannelDriver<'_, ATT, AO2>,
) -> !
where
    ADCU: AdcUnit,
    AO1: AdcChannel<AdcUnit = ADCU>,
    AO2: AdcChannel<AdcUnit = ADCU>,
{
    let mut delay = FreeRtos;
    let mut led_state = false;

    loop {
        let r1 = drivers::dht22::read_dht22_with_log("DHT22 #1", &mut delay, dht1);
        FreeRtos::delay_ms(2000);
        let r2 = drivers::dht22::read_dht22_with_log("DHT22 #2", &mut delay, dht2);

        let mq1 = drivers::mq135::check_mq135("MQ135 #1", do1, adc, ao1);
        let mq2 = drivers::mq135::check_mq135("MQ135 #2", do2, adc, ao2);

        ws::send_with_retry(config::WS_URL, &ws::SensorPayload {
            device_id: config::DEVICE_ID,
            temp1:  r1.map(|(t, _)| t).unwrap_or(0.0),
            hum1:   r1.map(|(_, h)| h).unwrap_or(0.0),
            temp2:  r2.map(|(t, _)| t).unwrap_or(0.0),
            hum2:   r2.map(|(_, h)| h).unwrap_or(0.0),
            mq1_value: mq1.unwrap_or(0),
            mq2_value: mq2.unwrap_or(0),
        });

        led_state = !led_state;
        if led_state { led.set_high().ok(); } else { led.set_low().ok(); }

        FreeRtos::delay_ms(config::SCAN_INTERVAL_MS as u32);
    }
}
