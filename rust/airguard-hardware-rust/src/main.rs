mod config;
mod drivers;
mod log_buffer;
mod scanner;
mod ws;

use esp_idf_hal::adc::{AdcChannelDriver, AdcDriver};
use esp_idf_hal::adc::attenuation;
use esp_idf_hal::gpio::{PinDriver, Pull};
use esp_idf_hal::peripherals::Peripherals;
use esp_idf_svc::eventloop::EspSystemEventLoop;
use esp_idf_svc::nvs::EspDefaultNvsPartition;
use esp_idf_svc::wifi::{BlockingWifi, ClientConfiguration, Configuration, EspWifi};

fn main() {
    esp_idf_svc::sys::link_patches();
    esp_idf_svc::log::EspLogger::initialize_default();

    let peripherals = Peripherals::take().unwrap();
    let sysloop = EspSystemEventLoop::take().unwrap();
    let nvs = EspDefaultNvsPartition::take().unwrap();

    let mut wifi = BlockingWifi::wrap(
        EspWifi::new(peripherals.modem, sysloop.clone(), Some(nvs)).unwrap(),
        sysloop,
    )
    .unwrap();

    wifi.set_configuration(&Configuration::Client(ClientConfiguration {
        ssid: config::SSID.try_into().unwrap(),
        password: config::PASSWORD.try_into().unwrap(),
        ..Default::default()
    }))
    .unwrap();

    wifi.start().unwrap();
    wifi.connect().unwrap();
    wifi.wait_netif_up().unwrap();
    log::info!("WiFi connected");

    let mut dht1 = PinDriver::input_output_od(peripherals.pins.gpio4, Pull::Floating).unwrap();
    let mut dht2 = PinDriver::input_output_od(peripherals.pins.gpio15, Pull::Floating).unwrap();
    let do1 = PinDriver::input(peripherals.pins.gpio19, Pull::Floating).unwrap();
    let do2 = PinDriver::input(peripherals.pins.gpio21, Pull::Floating).unwrap();
    let mut led = PinDriver::output(peripherals.pins.gpio2).unwrap();

    let mut adc = AdcDriver::new(peripherals.adc1, &Default::default()).unwrap();
    let mut ao1: AdcChannelDriver<{ attenuation::DB_11 }, _> =
        AdcChannelDriver::new(peripherals.pins.gpio34).unwrap();
    let mut ao2: AdcChannelDriver<{ attenuation::DB_11 }, _> =
        AdcChannelDriver::new(peripherals.pins.gpio35).unwrap();

    dht1.set_high().ok();
    dht2.set_high().ok();

    scanner::run_forever(&mut dht1, &mut dht2, &do1, &do2, &mut led, &mut adc, &mut ao1, &mut ao2);
}
