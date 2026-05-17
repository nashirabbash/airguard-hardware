use serde::Serialize;
use tungstenite::Message;
use esp_idf_hal::delay::FreeRtos;
use crate::log_buffer;

#[derive(Serialize)]
pub struct SensorPayload {
    pub device_id: &'static str,
    pub temp1: f32,
    pub hum1: f32,
    pub temp2: f32,
    pub hum2: f32,
    pub mq1_value: u16,
    pub mq2_value: u16,
}

fn send_payload(url: &str, payload: &SensorPayload) -> Result<(), tungstenite::Error> {
    let json = serde_json::to_string(payload).expect("serialize");
    let (mut socket, _) = tungstenite::connect(url)?;
    socket.send(Message::Text(json.into()))?;
    socket.close(None)?;
    Ok(())
}

pub fn send_with_retry(url: &str, payload: &SensorPayload) {
    for attempt in 1u8..=3 {
        match send_payload(url, payload) {
            Ok(()) => {
                log_buffer::log_ok("WS", "sent");
                return;
            }
            Err(_) => {
                log_buffer::log_fail("WS", &format!("attempt {}", attempt));
                if attempt < 3 {
                    FreeRtos::delay_ms(1000);
                }
            }
        }
    }
    unsafe { esp_idf_svc::sys::esp_restart() };
}
